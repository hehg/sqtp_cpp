
set (app_name "plug_td_sim_by_quot")

include (${PROJECT_SOURCE_DIR}/head.cmake)

add_definitions(-DUSE_SQ_DLL_EXPORT)

include_directories(${PROJECT_SOURCE_DIR}/libsqtp/include)
include_directories(${PROJECT_SOURCE_DIR}/libsqbz/include)
include_directories(${PROJECT_SOURCE_DIR}/liboffer/include)

aux_source_directory(./ SRC_FILES)

link_directories(${PROJECT_SOURCE_DIR}/lib)
link_directories(${PROJECT_SOURCE_DIR}/lib/third)
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}//lib/plug)
else()
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/plug)
endif(CMAKE_SYSTEM_NAME MATCHES "Windows")


add_library(${app_name} SHARED ${SRC_FILES} ../src/plug_base.cpp )

TARGET_LINK_LIBRARIES(${app_name} sqbz sqtp )
