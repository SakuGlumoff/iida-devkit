cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(app
	LANGUAGES C CXX
)

target_sources(app PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/src/main.c
)

target_include_directories(app PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/src/include
)
