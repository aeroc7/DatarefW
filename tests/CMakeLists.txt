cmake_minimum_required(VERSION 3.1)
project(dataref_tests)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++11 -fPIC -g -O2")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} --std=c99 -fPIC -g -O2")

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU") #GCC
	set(WARN_LIST_COMMON " \
		-Werror \
		-Wall \
		-Wextra \
		-Wpedantic \
		-Wformat=2 \
		-Wduplicated-cond \
		-Wfloat-equal \
		-Wshadow \
		-Wlogical-not-parentheses \
		-Wnull-dereference \
		-Wcast-align \
		-Wmisleading-indentation \
		-Wduplicated-branches \
		-Wlogical-op")

	set(WARN_LIST_CXX "${WARN_LIST_COMMON} \
		-Wnon-virtual-dtor \
		-Wold-style-cast \
		-Woverloaded-virtual")

	set(WARN_LIST_C "${WARN_LIST_COMMON}")

	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${WARN_LIST_CXX} -fno-gnu-unique")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${WARN_LIST_C}")
endif()

add_definitions(-DXPLM200=1 -DXPLM210=1 -DXPLM300=1 -DXPLM301=1)

if (WIN32)
	add_definitions(-DAPL=0 -DIBM=1 -DLIN=0)
    set(BIN_OUTPUT_DR "win_x64")
elseif(APPLE)
	add_definitions(-DAPL=1 -DIBM=0 -DLIN=0)
    set(BIN_OUTPUT_DR "mac_x64")
else()
	add_definitions(-DAPL=0 -DIBM=0 -DLIN=1)
    set(BIN_OUTPUT_DR "lin_x64")
endif()

if (NOT EXISTS ${CMAKE_SOURCE_DIR}/SDK)
    message(FATAL_ERROR "An unzipped X-Plane SDK folder needs to be predent in the base CMake directory.")
endif()

include_directories(
    "${CMAKE_SOURCE_DIR}/../"
    "${CMAKE_SOURCE_DIR}/SDK/CHeaders/XPLM"
)

add_library(dataref_tests SHARED
	${CMAKE_CURRENT_LIST_DIR}/test.cpp)

if (WIN32)
    target_link_libraries(dataref_tests
    -static
    -static-libstdc++
    -static-libgcc
    )
elseif(APPLE)
    target_link_libraries(dataref_tests
    -static-libstdc++
    -static-libgcc
    )
else()
    target_link_libraries(dataref_tests
    -static-libstdc++
    -static-libgcc
    )
endif()

set_target_properties(dataref_tests PROPERTIES PREFIX "")
set_target_properties(dataref_tests PROPERTIES SUFFIX "")
set_target_properties(dataref_tests PROPERTIES OUTPUT_NAME "DatarefTest.xpl")

set_target_properties(dataref_tests PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../bin/${BIN_OUTPUT_DR}/" )
set_target_properties(dataref_tests PROPERTIES
	LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../bin/${BIN_OUTPUT_DR}/" )