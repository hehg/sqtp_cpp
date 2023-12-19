
option(DEFINE_GCOVER        "gcover mode"          off)
option(SUPPORT_DB_SQLITE    "support sqlite3 db"   off)
option(SUPPORT_DB_MYSQL     "support mysql db"     off)
option(SUPPORT_DB_MONGO     "support mongodb"      off)
option(SUPPORT_DB_REDIS     "support redis"        on)
option(SUPPORT_READLINE     "support readline lib" off)

if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "Build type not set - defaulting to Release")
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build from: Debug Release RelWithDebInfo MinSizeRel Coverage." FORCE)
endif()

if(DEFINE_GCOVER)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage ")
endif()


if (SUPPORT_DB_SQLITE)
    add_definitions(-DSUPPORT_DB_SQLITE)
endif()

if (SUPPORT_DB_MYSQL)
    add_definitions(-DSUPPORT_DB_MYSQL)
endif()

if (SUPPORT_DB_REDIS)
    add_definitions(-DSUPPORT_DB_REDIS)
endif()
if (SUPPORT_READLINE)
    add_definitions(-DSUPPORT_READLINE)
endif()
#message(STATUS "build type = ${CMAKE_BUILD_TYPE}")
#message(STATUS "OS =${CMAKE_SYSTEM_NAME}")

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_definitions(-DWINDOWS)
    set(CMAKE_CXX_STANDARD 17)
    if(MSVC)
        add_compile_options(/utf-8 -D_CRT_SECURE_NO_WARNINGS)

        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/lib)
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/lib)
        #set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/lib)
        set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/lib)
        set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/lib)
        #set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_SOURCE_DIR}/bin)

    endif()

elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -g -fPIC -Wl,-Bsymbolic ")
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS}  -O0 ")
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS} -O2 ")
    add_definitions(-DLINUX)
    LINK_LIBRARIES(pthread dl)
elseif(CMAKE_SYSTEM_NAME MATCHES "Darwin")
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17 -g -fPIC ")
    SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG} -O0 ")
    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS}  -O2")
    add_definitions(-DMAC)
    add_definitions(-D__APPLE__)
    include_directories("/usr/local/Cellar/openssl@1.1/1.1.1d/include/")
    include_directories("/usr/local/include/")
    #for mongodb
    include_directories("/usr/local/include/mongocxx/v_noabi/")
    include_directories("/usr/local/include/bsoncxx/v_noabi")
    link_directories("/usr/local/lib/")
    #for mysql
    include_directories("/usr/local/Cellar/mariadb-connector-c/3.3.5_1/include/mariadb/")
    link_directories("/usr/local/Cellar/mariadb-connector-c/3.3.5_1/lib/mariadb")

ENDIF(CMAKE_SYSTEM_NAME MATCHES "Windows")

include_directories("${PROJECT_SOURCE_DIR}/../libsqtp/third/")
include_directories("${PROJECT_SOURCE_DIR}/third/hredis/")

link_directories(${PROJECT_SOURCE_DIR}/lib)
link_directories(${PROJECT_SOURCE_DIR}/lib/third)


#set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
#set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
#set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

#set(CMAKE_VERBOSE_MAKEFILE ON)
#LINK_LIBRARIES(pthread)
#LINK_LIBRARIES(iconv)
if (SUPPORT_DB_MYSQL)
    link_libraries(mariadb)
endif()

if (SUPPORT_READLINE)
    link_libraries(readline)
endif()