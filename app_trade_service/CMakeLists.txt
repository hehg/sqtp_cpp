#cmake_minimum_required (VERSION 2.8)

set (app_name "app_trade_service")
set (api_name "sq_ts_api")
set (cli_name "cli")

include (${PROJECT_SOURCE_DIR}/head.cmake)

include_directories(${PROJECT_SOURCE_DIR}/libsqtp/include)
include_directories(${PROJECT_SOURCE_DIR}/libsqbz/include)
include_directories(${PROJECT_SOURCE_DIR}/liboffer/include)
include_directories(${PROJECT_SOURCE_DIR}/libframe/include)
#server
aux_source_directory(./src/ SRC_FILES)
aux_source_directory(./src/base SRC_FILES)
aux_source_directory(./src/gate_server SRC_FILES)
aux_source_directory(./src/md_server SRC_FILES)
aux_source_directory(./src/offer_server SRC_FILES)
aux_source_directory(./src/query_server SRC_FILES)
aux_source_directory(./src/trade_server SRC_FILES)
aux_source_directory(./src/data_server SRC_FILES)

#api
aux_source_directory(./src/api API_SRC_FILES)

#cli
aux_source_directory(./src/cli CLI_SRC_FILES)

link_directories(${PROJECT_SOURCE_DIR}/lib)
link_directories(${PROJECT_SOURCE_DIR}/lib/third)


set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/${app_name}/bin/)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/${app_name}/bin)

add_executable(${app_name}  ${SRC_FILES} )

add_library(${api_name} SHARED ${API_SRC_FILES} )
add_executable(${cli_name} ${CLI_SRC_FILES})

add_dependencies(${app_name} sqtp sqbz)
add_dependencies(${api_name} sqtp sqbz)
add_dependencies(${cli_name} ${api_name})
TARGET_LINK_LIBRARIES(${app_name} sqtp sqbz )
TARGET_LINK_LIBRARIES(${api_name} sqtp sqbz )
target_link_libraries(${cli_name} ${api_name})

