

include (${PROJECT_SOURCE_DIR}/head.cmake)


include_directories(${PROJECT_SOURCE_DIR}/libsqtp/include)
include_directories(${PROJECT_SOURCE_DIR}/libprotocol/include)
include_directories(${PROJECT_SOURCE_DIR}/libbz/include)
include_directories(${PROJECT_SOURCE_DIR}/liboffer/include)


set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
LINK_LIBRARIES(sqtp)

add_executable( echo_tcp_client ./src/echo_tcp_client.cpp )
add_executable( echo_tcp_server ./src/echo_tcp_server.cpp )
add_executable( echo_ws_server ./src/echo_ws_server.cpp )
add_executable( echo_ws_client ./src/echo_ws_client.cpp )
add_executable( echo_udp_sender ./src/echo_udp_sender.cpp)
add_executable( echo_udp_recver ./src/echo_udp_recver.cpp)


LINK_LIBRARIES(pthread)

