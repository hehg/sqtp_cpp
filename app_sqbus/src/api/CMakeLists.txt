
link_directories(${root_dir}/lib)

aux_source_directory(./ SRC_FILES)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin/)

add_library(bus_api SHARED ${SRC_FILES})

TARGET_LINK_LIBRARIES(bus_api sqtp )