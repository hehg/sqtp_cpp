#include "sq_unit_test.h"
#include "log/sq_logger.h"
#include "text/csv_file.h"
#include <iostream>
using namespace sq;


SQ_TEST(test_csv_reader_case1)
{
    csv_file_reader reader;
    reader.open("./test.csv");
    while (reader.read_row())
    {
       std::cout<< reader["name"].as_string()<<",";
       std::cout<< reader["age"].as_int()<<",";
       std::cout<< reader["sex"].as_int()<<"\n";
    }
    
}
