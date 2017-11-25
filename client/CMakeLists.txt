cmake_minimum_required(VERSION 3.7)

project(client)

find_package(Threads)

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++14" )

add_executable(${PROJECT_NAME} "client.cpp"
	"../common/tcp_connection.cpp"
	"../common/udp_connection.cpp"
	"../common/udp_listener.cpp"
	"../common/logger.cpp"
)

target_link_libraries(client ${CMAKE_THREAD_LIBS_INIT})
