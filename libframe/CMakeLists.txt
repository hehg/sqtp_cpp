
set (app_name "frame")

include (../head.cmake)

add_definitions(-DUSE_SQ_DLL_EXPORT)
include_directories(./include)
include_directories(${PROJECT_SOURCE_DIR}/libsqtp/include)
include_directories(${PROJECT_SOURCE_DIR}/libsqbz/include)
include_directories(${PROJECT_SOURCE_DIR}/liboffer/include)



if(CMAKE_SYSTEM_NAME MATCHES "Windows")
	FILE(GLOB SRC_FILES "./src/*.cpp" "./include/*.h")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_SOURCE_DIR}/lib/)
else()
	aux_source_directory(./src SRC_FILES)
    set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib/)
endif(CMAKE_SYSTEM_NAME MATCHES "Windows")




add_library(${app_name} SHARED ${SRC_FILES} )

TARGET_LINK_LIBRARIES(${app_name} sqbz sqtp   )

#add_custom_command(TARGET ${app_name} 
#	POST_BUILD 
 # COMMAND echo "Generating log.txt file..."
 # COMMAND copy ${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}*.dll ${PROJECT_SOURCE_DIR}/app_client_ui/bin/

 #)

