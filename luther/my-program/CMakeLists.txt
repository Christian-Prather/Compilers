cmake_minimum_required(VERSION 3.1.0)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

project(LUTHER)

add_executable(LUTHER luther.cpp)
set_property(TARGET LUTHER PROPERTY CXX_STANDARD 11)

install(
	TARGETS

	LUTHER

	RUNTIME DESTINATION
	${CMAKE_INSTALL_PREFIX}/bin
)