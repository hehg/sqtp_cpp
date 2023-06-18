cmake_minimum_required(VERSION 3.2)
project(sqtp)

option(build_test "build  test code" on)

add_subdirectory(libsqtp)
add_subdirectory(libsqbz)
add_subdirectory(liboffer)
add_subdirectory(libframe)


#test
if(build_test)
    add_subdirectory(libsqbz/test)
    add_subdirectory(libsqtp/test)
endif()


