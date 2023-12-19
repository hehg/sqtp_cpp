
set (app_name "test_sqbz")

include (../../head.cmake)


include_directories(${PROJECT_SOURCE_DIR}/libsqtp/include)
include_directories(${PROJECT_SOURCE_DIR}/libsqbz/include)
aux_source_directory(./src SRC_FILES)

set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_executable(${app_name}  ${SRC_FILES} )


TARGET_LINK_LIBRARIES(${app_name} sqbz sqtp  )

if (LINUX)
    TARGET_LINK_LIBRARIES(${app_name} rt)
endif()
