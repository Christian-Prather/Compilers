cmake_minimum_required(VERSION 3.1.0)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

project(NFAMATCH)

add_executable(NFAMATCH optimization.cpp)
set_property(TARGET NFAMATCH PROPERTY CXX_STANDARD 11)

install(
	TARGETS

	NFAMATCH

	RUNTIME DESTINATION
	${CMAKE_INSTALL_PREFIX}/bin
)