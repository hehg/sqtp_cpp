
set (app_name "test_sqtp")

include (${PROJECT_SOURCE_DIR}/head.cmake)


include_directories(${PROJECT_SOURCE_DIR}/libsqtp/include)
include_directories(${PROJECT_SOURCE_DIR}/libprotocol/include)
include_directories(${PROJECT_SOURCE_DIR}/libbz/include)
include_directories(${PROJECT_SOURCE_DIR}/liboffer/include)

aux_source_directory(./src SRC_FILES)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_executable(${app_name}  ${SRC_FILES} )


TARGET_LINK_LIBRARIES(${app_name} sqtp  )
TARGET_LINK_LIBRARIES(${app_name} pthread)

if(SUPPORT_DB_MONGO)
    target_link_libraries(${app_name} mongocxx)
endif()

if (LINUX)
    TARGET_LINK_LIBRARIES(${app_name} rt)
endif()
