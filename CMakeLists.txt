cmake_minimum_required(VERSION 3.1.0)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

project(NFMATCH)

add_executable(NFMATCH optimization.cpp)
set_property(TARGET NFMATCH PROPERTY CXX_STANDARD 11)

install(
	TARGETS

	NFMATCH

	RUNTIME DESTINATION
	${CMAKE_INSTALL_PREFIX}/bin
)