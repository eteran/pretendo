cmake_minimum_required(VERSION 3.0)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library (p6502
	P6502.cc
)

target_include_directories (p6502 PUBLIC
	${CMAKE_CURRENT_SOURCE_DIR} 
	${PROJECT_SOURCE_DIR}
)

target_link_libraries(p6502 LINK_PUBLIC
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -pedantic -Wunused -Wuseless-cast")

set_property(TARGET p6502 PROPERTY CXX_STANDARD 11)
set_property(TARGET p6502 PROPERTY CXX_EXTENSIONS OFF)
