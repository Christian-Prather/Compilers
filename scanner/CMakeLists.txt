cmake_minimum_required(VERSION 3.1.0)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

project(Scanner)

add_executable(Scanner scanner.cpp)
set_property(TARGET Scanner PROPERTY CXX_STANDARD 11)

install(
	TARGETS

	Scanner

	RUNTIME DESTINATION
	${CMAKE_INSTALL_PREFIX}/bin
)