
#include <unordered_map>
#include "sq_unit_test.h"
#include "log/sq_logger.h"
#include "container/byte_array.h"
#include "db/sq_enable_db.h"
#include "db/sq_orm.h"
#include "db/db_adapter/mongodb/sq_mongo_adapter.h"
#include "db/db_adapter/sqlite/sq_sqlite_adapter.h"
#include "db/db_adapter/mysql/sq_mysql_adapter.h"
#include "fmt/format.h"

using namespace sq;

#ifdef SUPPORT_DB_MYSQL

SQ_TEST(test_sq_mysql_adapter)
{
  sq_mysql_adapter::db_init();
  sq_mysql_adapter db;
  int ret = db.connect("monitor", "mysql://47.104.183.14:3306", "hehong", "hehong");
  SQ_EXPECT_EQ(ret, 0);

  ret = db.exec_sql("CREATE TABLE IF NOT EXISTS company ( \
   ID INT PRIMARY KEY     NOT NULL, \
   NAME           TEXT    NOT NULL, \
   AGE            INT     NOT NULL, \
   ADDRESS        CHAR(50), \
   SALARY         REAL ) ",nullptr);

   //insert data
  #if 0
   ret= db.exec_sql("INSERT INTO company (ID,NAME,AGE,ADDRESS,SALARY) \
   VALUES (1, 'Paul', 32, 'California', 20000.00 )",[](const db_result_table&result){
    printf("effect rows=%d\n",result.effect_rows_);
   });
   SQ_EXPECT_EQ(ret, 0);
   ret = db.exec_sql("INSERT INTO company (ID,NAME,AGE,ADDRESS,SALARY) \
   VALUES (2, 'Allen', 25, 'Texas', 15000.00 )",nullptr);

   SQ_EXPECT_EQ(ret, 0);


#endif

  //query data
  ret=db.exec_sql("select * from company",[](const db_result_table&result){
    for(const auto&row : result.rows_){
      printf("id=%d,name=%s,age=%d,address=%s,salary=%f\n",
      row.as_int(0),
      row.as_string(1).c_str(),
      row.as_int(2),
      row.as_string(3).c_str(),
      row.as_double(4));
    }
});
  

  db.disconnect();
}

#endif