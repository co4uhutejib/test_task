cmake_minimum_required(VERSION 3.7)

project(server)

find_package(Threads)

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++14" )

add_executable(${PROJECT_NAME} "server.cpp"
	"../common/tcp_listener.cpp"
	"../common/udp_connection.cpp"
	"../common/udp_listener.cpp"
	"../common/logger.cpp"
	"udp_session.cpp"
	"data_handler.cpp"
)

target_link_libraries(server ${CMAKE_THREAD_LIBS_INIT})
