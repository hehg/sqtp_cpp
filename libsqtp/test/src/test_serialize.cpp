#include "sq_unit_test.h"
#include <iostream>
#include "container/byte_array.h"
#include "pack/struct_reflect.h"
using namespace sq;

#define REG_FD(s,t,f)     {#f, #t, sizeof(t), sizeof(s::f), offsetof(s, f)}

struct Person
{
  void static init_desc()
  {
    static struct_reflect table_des;
    table_des.m_name = "Person";
    table_des.m_fields =
        {
            REG_FD(Person, int32_t, id),
            REG_FD(Person, char, name),
            REG_FD(Person, char, phone),
            REG_FD(Person, double, iban),
            REG_FD(Person, float, iban2),
        };
    s_desc_map[table_des.m_name] = (&table_des);
  }
  static struct_reflect &get_desc()
  {
    auto it = s_desc_map.find("Person");
    if(it==s_desc_map.end()){
        init_desc();
        it = s_desc_map.find("Person");
    }
    return *it->second;
  }

  void to_string(std::ostream &of){
        get_desc().to_string(this,of);
  }

  int32_t id;
  byte_array<12> name;
  byte_array<32> phone;
   double iban;
  float iban2;
};

SQ_TEST(test_serialize_case1)
{
  Person person;
  person.id = 1;
  person.name = byte_array<12>("Abdu'l-Bahá");
  person.phone = byte_array<32>("0123456789");
  person.iban = -1.0;
  person.iban2 = -1.0;
  person.to_string(std::cout);
}
