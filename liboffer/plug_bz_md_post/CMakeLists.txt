
set (app_name "plug_bz_md_post")

include (${PROJECT_SOURCE_DIR}/head.cmake)

include_directories(${PROJECT_SOURCE_DIR}/libsqtp/include)
include_directories(${PROJECT_SOURCE_DIR}/libsqbz/include)
include_directories(${PROJECT_SOURCE_DIR}/liboffer/include)

aux_source_directory(./ SRC_FILES)

link_directories(${PROJECT_SOURCE_DIR}/lib)
link_directories(${PROJECT_SOURCE_DIR}/lib/third)

set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/plug)

add_library(${app_name} SHARED ${SRC_FILES} ../src/plug_base.cpp )

TARGET_LINK_LIBRARIES(${app_name} sqbz sqtp )
