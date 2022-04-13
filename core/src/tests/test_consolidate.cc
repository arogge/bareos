/*
   BAREOS® - Backup Archiving REcovery Open Sourced

   Copyright (C) 2018-2022 Bareos GmbH & Co. KG

   This program is Free Software; you can redistribute it and/or
   modify it under the terms of version three of the GNU Affero General Public
   License as published by the Free Software Foundation and included
   in the file LICENSE.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Affero General Public License for more details.

   You should have received a copy of the GNU Affero General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.
*/
#define _BDB_PRIV_INTERFACE_

#if defined(HAVE_MINGW)
#  include "include/bareos.h"
#  include "gtest/gtest.h"
#else
#  include "gtest/gtest.h"
#  include "include/bareos.h"
#endif

#include "dird/consolidate.h"
#include "dird/jcr_private.h"
#include "dird/dird_conf.h"
#include "dird/dird_globals.h"
#include "lib/parse_conf.h"

#include <vector>
#include <string>
#include <regex>

namespace directordaemon {
bool DoReloadConfig() { return false; }
}  // namespace directordaemon

using directordaemon::DoConsolidate;
using directordaemon::JobResource;
using directordaemon::my_config;
using directordaemon::R_JOB;
using directordaemon::R_LAST;
using ResultSet = std::vector<std::vector<std::string>>;

class TestDb : public BareosDb {
  struct InternalState {
    std::regex query_pattern;
    int affected_rows{0};
    ResultSet rs;

    bool MatchQuery(const std::string& query)
    {
      return std::regex_match(query, query_pattern);
    }
    InternalState() = default;
    InternalState(std::string query, int rows)
        : query_pattern(
            std::regex{query, std::regex::extended | std::regex::icase})
        , affected_rows(rows)
    {
    }
    InternalState(std::string query, ResultSet&& data)
        : query_pattern(
            std::regex{query, std::regex::extended | std::regex::icase})
        , rs(std::move(data))
    {
    }
  };

 public:
  TestDb() : current_state(query_states.begin()) { OpenDatabase(nullptr); }
  ~TestDb() { CloseDatabase(nullptr); }

  template <typename... Args> void AddQuery(Args... args)
  {
    query_states.emplace_back(std::forward<Args>(args)...);
    current_state = query_states.begin();  // iterator may be invalidated!
  }

  bool AllQueriesConsumed() {
    return next(current_state) == query_states.end();
  }

 private:
  const char* db_error = "No error handling. Just test DB adapter.";

  std::vector<InternalState> query_states{InternalState{}};
  std::vector<InternalState>::iterator current_state;
  ResultSet::iterator rs_iter;

  const char* data_row[1000];

  bool MatchQueryAndAdvance(const std::string& query)
  {
    auto next_state = next(current_state);
    if (next_state == query_states.end()) {
      Dmsg0(5, "TestDb: at end of queries\n");
      return false;
    } else if (next_state->MatchQuery(query)) {
      Dmsg0(5, "TestDb: advancing to next state\n");
      ++current_state;
      rs_iter = current_state->rs.begin();
      return true;
    } else {
      Dmsg0(5, "TestDb: next query does not match!\n");
      return false;
    }
  }

  virtual bool OpenDatabase(JobControlRecord*) override
  {
    cmd = GetPoolMemory(PM_EMSG);
    errmsg = GetPoolMemory(PM_EMSG);
    if (int errstat = RwlInit(&lock_); errstat != 0) {
      Mmsg0(errmsg, _("Unable to initialize DB lock."));
      return false;
    }
    ASSERT(RwlIsInit(&lock_));
    connected_ = true;
    return true;
  }
  virtual void CloseDatabase(JobControlRecord*) override
  {
    if (RwlIsInit(&lock_)) { RwlDestroy(&lock_); }
    FreePoolMemory(errmsg);
    FreePoolMemory(cmd);
  }
  virtual bool ValidateConnection() override { return true; }
  virtual void StartTransaction(JobControlRecord*) override {}
  virtual void EndTransaction(JobControlRecord*) override {}
  virtual bool SqlCopyStart(const std::string&,
                            const std::vector<std::string>&) override
  {
    return true;
  }
  virtual bool SqlCopyInsert(const std::vector<DatabaseField>&) override
  {
    return true;
  }
  virtual bool SqlCopyEnd() override { return true; }

  virtual void SqlFieldSeek(int) override {}
  virtual int SqlNumFields() override { return 0; }
  virtual void SqlFreeResult() override {}
  virtual SQL_ROW SqlFetchRow() override
  {
    if (rs_iter == current_state->rs.end()) { return NULL; }
    int i = 0;
    for (auto& col : *rs_iter) { data_row[i++] = col.c_str(); }
    ++rs_iter;
    return data_row;
  }
  virtual bool SqlQueryWithoutHandler(const char* query, int) override
  {
    return MatchQueryAndAdvance(query);
  }
  virtual bool SqlQueryWithHandler(const char* query,
                                   DB_RESULT_HANDLER* ResultHandler,
                                   void* ctx) override
  {
    if (MatchQueryAndAdvance(query)) {
      /* here we call the handler for each row in the resultset */
      ASSERT(0);
      return true;
    }
    return false;
  }
  virtual const char* sql_strerror() override { return db_error; }
  virtual void SqlDataSeek(int) override {}
  virtual int SqlAffectedRows() override
  {
    return current_state->affected_rows;
  }
  virtual uint64_t SqlInsertAutokeyRecord(const char*, const char*) override
  {
    return 0;
  }
  virtual SQL_FIELD* SqlFetchField() override { return NULL; }
  virtual bool SqlFieldIsNotNull(int) override { return true; }
  virtual bool SqlFieldIsNumeric(int) override { return true; }
  virtual bool SqlBatchStartFileTable(JobControlRecord*) override
  {
    return true;
  }
  virtual bool SqlBatchEndFileTable(JobControlRecord*, const char*) override
  {
    return true;
  }
  virtual bool SqlBatchInsertFileTable(JobControlRecord*,
                                       AttributesDbRecord*) override
  {
    return true;
  }
};

TEST(consolidate, no_ai_jobs)
{
  InitMsg(NULL, NULL);
  debug_level = 100;

  JobControlRecord jcr;
  JobControlRecordPrivate jcr_private;
  jcr.impl = &jcr_private;

  jcr.JobId = 1000;
  jcr.impl->jr.JobLevel = 'F';
  strcpy(jcr.Job, "ConsolidateJob.xyz");

  JobResource job;
  PoolMem job_res_name{"Consolidate Job"};
  job.resource_name_ = job_res_name.c_str();

  ConfigurationParser cp;
  ResourceTable res_table[R_LAST];
  res_table[R_JOB].name = "Job";
  cp.resources_ = res_table;
  BareosResource* res_head[R_LAST];
  cp.res_head_ = res_head;
  cp.AppendToResourcesChain(&job, R_JOB);

  my_config = &cp;
  jcr.impl->res.job = &job;

  TestDb db;
  jcr.db = &db;

  db.AddQuery(
      "UPDATE Job SET "
      "JobStatus='T',Level='F',EndTime='.*',ClientId=0,JobBytes=0,ReadBytes=0,"
      "JobFiles=0,JobErrors=0,VolSessionId=0,VolSessionTime=0,PoolId=0,"
      "FileSetId=0,JobTDate=.*,RealEndTime='.*',PriorJobId=0,HasBase=0,"
      "PurgedFiles=0 WHERE JobId=1000",
      1);
  db.AddQuery(
      "SELECT "
      "VolSessionId,VolSessionTime,PoolId,StartTime,EndTime,JobFiles,JobBytes,"
      "JobTDate,Job,JobStatus,Type,Level,ClientId,Name,PriorJobId,RealEndTime,"
      "JobId,FileSetId,SchedTime,RealEndTime,ReadBytes,HasBase,PurgedFiles "
      "FROM Job WHERE JobId=1000",
      ResultSet{{"0",
                 "0",
                 "0",
                 "2021-10-23 00:11:22",
                 "2021-10-23 00:22:33",
                 "0",
                 "0",
                 "1234567890",
                 "Consolidate Job.xyz",
                 "T",
                 "C",
                 "F",
                 "0",
                 "Consolidate Job",
                 "0",
                 "2021-10-23 00:22:44",
                 "1000",
                 "0",
                 "2021-10-23 00:11:00",
                 "2021-10-23 00:22:44",
                 "0",
                 "0",
                 "0"}});

  EXPECT_TRUE(DoConsolidate(&jcr));
  EXPECT_TRUE(db.AllQueriesConsumed());
  // Avoid memory leak
  FreeAndNullPoolMemory(jcr.impl->fname);
}
