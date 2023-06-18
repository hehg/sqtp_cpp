
set (app_name "sqbz")

include (../head.cmake)

include_directories(./include)
include_directories(${PROJECT_SOURCE_DIR}/libsqtp/include)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	FILE(GLOB SRC_FILES "./src/*.cpp" "./include/*.h")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/lib/)
else()
	aux_source_directory(./src SRC_FILES)
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/)
endif(CMAKE_SYSTEM_NAME MATCHES "Windows")

add_library(${app_name} STATIC ${SRC_FILES} )


