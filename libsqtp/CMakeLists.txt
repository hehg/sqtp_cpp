#cmake_minimum_required (VERSION 2.8)

set (app_name "sqtp")

include (../head.cmake)


include_directories(./include)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	FILE(GLOB_RECURSE SRC_FILES "./src/*.cpp" "./include/*.h")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/lib/)
else()
	aux_source_directory(./src SRC_FILES)
	aux_source_directory(./src/base SRC_FILES)
	aux_source_directory(./src/container SRC_FILES)
	aux_source_directory(./src/crypto SRC_FILES)
	aux_source_directory(./src/db SRC_FILES)
	aux_source_directory(./src/db/db_adapter SRC_FILES)

	aux_source_directory(./src/db/db_adapter/mysql SRC_FILES)

	if(SUPPORT_DB_SQLITE)
		aux_source_directory(./src/db/db_adapter/sqlite SRC_FILES)
	endif()
	
	aux_source_directory(./src/db/db_adapter/mongodb SRC_FILES)
	aux_source_directory(./src/db/db_adapter/redis SRC_FILES)

	aux_source_directory(./src/http SRC_FILES)
	aux_source_directory(./src/log SRC_FILES)
	aux_source_directory(./src/math SRC_FILES)
	aux_source_directory(./src/mq SRC_FILES)
	aux_source_directory(./src/net SRC_FILES)
	aux_source_directory(./src/os SRC_FILES)
	aux_source_directory(./src/pack SRC_FILES)
	aux_source_directory(./src/store SRC_FILES)
	aux_source_directory(./src/sync SRC_FILES)
	aux_source_directory(./src/text SRC_FILES)
	aux_source_directory(./src/time SRC_FILES)
	aux_source_directory(./src/plug SRC_FILES)
	aux_source_directory(./src/error SRC_FILES)
	aux_source_directory(./src/cmd_line SRC_FILES)

    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/)
endif(CMAKE_SYSTEM_NAME MATCHES "Windows")

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

add_library(${app_name} STATIC ${SRC_FILES} )
