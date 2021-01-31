cmake_minimum_required(VERSION 3.1.0)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

project(dfa)

add_executable(dfa optimization.cpp)
set_property(TARGET dfa PROPERTY CXX_STANDARD 11)

install(
	TARGETS

	dfa

	RUNTIME DESTINATION
	${CMAKE_INSTALL_PREFIX}/bin
)