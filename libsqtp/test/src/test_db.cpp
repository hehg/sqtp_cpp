

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

#define REG_FD(s, t, f)                             \
  {                                                 \
    #f, #t, sizeof(t), sizeof(s::f), offsetof(s, f) \
  }

struct table_person
{
  void static init_desc()
  {
    static struct_reflect table_des;
    table_des.m_name = "table_person";
    table_des.m_fields =
        {
            REG_FD(table_person, int, id),
            REG_FD(table_person, char, name),
            REG_FD(table_person, char, phone),
        };
    s_desc_map[table_des.m_name] = (&table_des);
  }
  static struct_reflect &get_desc()
  {
    auto it = s_desc_map.find("table_person");
    return *it->second;
  }

  int id;
  byte_array<12> name;
  byte_array<32> phone;
};
#if defined(SUPPORT_DB_SQLITE)
SQ_TEST(test_sq_sqlite_adapter)
{
  sq_sqlite_adapter db;
  int ret = db.connect("./test_sqlite.db", "", "", "");
  SQ_EXPECT_EQ(ret, 0);

  ret = db.exec_sql("CREATE TABLE IF NOT EXISTS company ( \
   ID INT PRIMARY KEY     NOT NULL, \
   NAME           TEXT    NOT NULL, \
   AGE            INT     NOT NULL, \
   ADDRESS        CHAR(50), \
   SALARY         REAL ) ",nullptr);

   //insert data
  
  for (int i = 0; i < 5; i++)
  {
    std::vector<db_bind_info> values;
    db_bind_info f;
    f.type = field_type_t::type_int32;
    f.bind_data.int_data = i;
    values.push_back(f);

    char name[21];
    f.type = field_type_t::type_char_array;
    sprintf(name, "name_%d", i);
    f.bind_data.text_data = name;
    values.push_back(f);

    f.type = field_type_t::type_int16;
    f.bind_data.int_data = 10 + i;
    values.push_back(f);

    f.type = field_type_t::type_char_array;
    f.bind_data.text_data = "address";
    values.push_back(f);

    f.type = field_type_t::type_float32;
    f.bind_data.double_data = 10 + i;
    values.push_back(f);

    ret = db.exec_sql("insert into company values(?,?,?,?,?)", values,
                      [](const db_result_table &ret)
                      {
                        cout << "insert ok\n";
                      });
    values.clear();
    cout << "ret=" << ret << std::endl;
  }

  db.disconnect();
}
SQ_TEST(test_sq_orm_sqlite)
{

  sq_orm<sq_sqlite_db> orm;
  SQ_LOG_LEVEL(level_enum::log_debug);
  table_person::init_desc();

  orm.connect("test_db.sqlite", nullptr, nullptr, nullptr);

  table_person person;
  person.id = 1;
  person.name = "hehong";
  person.phone = "123456789";

  // orm.create_table<table_person>();
  // orm.insert<table_person>(&person);

  orm.disconnect();
}
#endif

#ifdef SUPPORT_DB_MONGO
SQ_TEST(test_sq_mongodb_adapter)
{

  sq_mongo_adapter::db_init();

  sq::sq_mongo_adapter db;
  db.connect("test", "40.104.183.14:27017", nullptr, nullptr);
}
#endif