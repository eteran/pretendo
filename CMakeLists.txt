cmake_minimum_required (VERSION 3.15)
project (pretendo CXX)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

option(ENABLE_AUDIO         "Build with audio support" OFF)
option(ENABLE_PROFILING     "Enable profiling")
option(ENABLE_ASAN          "Enable address santiziers")
option(ENABLE_USAN          "Enable undefined santiziers")
option(ENABLE_MSAN          "Enable memory santiziers")
option(ENABLE_TSAN          "Enable thread santiziers")
option(ENABLE_LTO           "Enable Link Time Optimization")
option(ENABLE_STL_DEBUG     "Enable STL container debugging")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(ENABLE_STL_DEBUG)
	add_definitions(-D_GLIBCXX_DEBUG)
	add_definitions(-D_GLIBCXX_DEBUG_PEDANTIC)
endif()

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -pedantic -Wunused")
endif()

if(ENABLE_PROFILING)
	set(CMAKE_CXX_FLAGS        "${CMAKE_CXX_FLAGS}        -pg -no-pie")
	set(CMAKE_C_FLAGS          "${CMAKE_C_FLAGS}          -pg -no-pie")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
endif()

if(ENABLE_ASAN)
	set(CMAKE_CXX_FLAGS        "${CMAKE_CXX_FLAGS}        -fsanitize=address") # -fsanitize-address-use-after-scope
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
	add_definitions(-D_GLIBCXX_SANITIZE_VECTOR)
endif()

if(ENABLE_USAN)
	set(CMAKE_CXX_FLAGS        "${CMAKE_CXX_FLAGS}        -fsanitize=undefined")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=undefined")
endif()

if(ENABLE_TSAN)
	set(CMAKE_CXX_FLAGS        "${CMAKE_CXX_FLAGS}        -fsanitize=thread")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=thread")
endif()

if(ENABLE_MSAN)
	set(CMAKE_CXX_FLAGS        "${CMAKE_CXX_FLAGS}        -fsanitize=memory")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=memory")
endif()

if(ENABLE_LTO)
	set(CMAKE_CXX_FLAGS        "${CMAKE_CXX_FLAGS}        -flto")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto")
	set(CMAKE_AR               "gcc-ar")
	set(CMAKE_RANLIB           "gcc-ranlib")
endif()

add_subdirectory(src)
add_subdirectory(libines2/lib)
