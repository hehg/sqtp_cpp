cmake_minimum_required (VERSION 3.1)

project(app_sqbus)
set (root_dir ${PROJECT_SOURCE_DIR}/../)


include (${root_dir}/head.cmake)
include_directories(${root_dir}/libsqtp/include)
include_directories(${PROJECT_SOURCE_DIR}/src/)

add_subdirectory(./src/api)
aux_source_directory(./src/server SERVER_SRC_FILES)

set (CLIENT_S_SRC_FILES ./src/client/cli_s.cpp)
set (CLIENT_R_SRC_FILES ./src/client/cli_r.cpp)


link_directories(${root_dir}/lib)
link_directories(${root_dir}/lib/third)


set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/)

add_executable(app_sqbus_server  ${SERVER_SRC_FILES} )
add_executable(app_sqbus_cli_r  ${CLIENT_R_SRC_FILES} )
add_executable(app_sqbus_cli_s  ${CLIENT_S_SRC_FILES} )


TARGET_LINK_LIBRARIES(app_sqbus_server sqtp )
TARGET_LINK_LIBRARIES(app_sqbus_cli_r bus_api sqtp )
TARGET_LINK_LIBRARIES(app_sqbus_cli_s bus_api sqtp )

