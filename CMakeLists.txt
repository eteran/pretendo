cmake_minimum_required (VERSION 3.0)
project (pretendo CXX)


if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "RelWithDebInfo" CACHE STRING "Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif()

option(ENABLE_AUDIO "BUild with audio support" OFF)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if(ENABLE_AUDIO)

	find_package(PulseAudio REQUIRED)

	add_definitions(-DPULSE_AUDIO_SOUND)

	set(SOURCE_FILES ${SOURCE_FILES}
		qt/PulseAudio.cpp
	)
	
	set(INCLUDES ${INCLUDES} ${PULSEAUDIO_INCLUDE_DIR})
	set(LIBRARIES ${LIBRARIES} ${PULSEAUDIO_LIBRARY} -lpulse-simple)	

else()
	set(SOURCE_FILES ${SOURCE_FILES}
		qt/NullAudio.cpp
	)
endif()


if(ENABLE_PROFILING)
	set(CMAKE_CXX_FLAGS        "${CMAKE_CXX_FLAGS} -pg")
	set(CMAKE_C_FLAGS          "${CMAKE_C_FLAGS} -pg")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
endif()

if(ENABLE_LTO)
	set(CMAKE_CXX_FLAGS        "${CMAKE_CXX_FLAGS} -flto")
	set(CMAKE_C_FLAGS          "${CMAKE_C_FLAGS} -flto")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -flto")
endif()

find_package(Qt5 5.4.0 REQUIRED Widgets OpenGL)
find_package(Boost 1.35 COMPONENTS system filesystem REQUIRED)

find_package(OpenGL REQUIRED)


set(UI_FILES ${UI_FILES}
	qt/Pretendo.ui
	qt/Preferences.ui
	qt/About.ui
	qt/AudioViewer.ui
)

set(SOURCE_FILES ${SOURCE_FILES}
	APU.cpp
	APU.h
	BitField.h
	Cart.cpp
	Cart.h
	circular_buffer.h
	Config.cpp
	Config.h
	Controller.cpp
	Controller.h
	CPU.cpp
	CPU.h
	DMC.cpp
	DMC.h
	Envelope.cpp
	Envelope.h
	Input.cpp
	Input.h
	LengthCounter.cpp
	LengthCounter.h
	LFSR.h
	LinearCounter.cpp
	LinearCounter.h
	Mapper.cpp
	Mapper.h
	MemoryMappedFile.cpp
	MemoryMappedFile.h
	NES.cpp
	NES.h
	Noise.cpp
	Noise.h
	Palette.cpp
	Palette.h
	PPU.cpp
	PPU.h
	Reset.h
	ShiftRegister.h
	Square.h
	Sweep.h
	Timer.cpp
	Timer.h
	Triangle.cpp
	Triangle.h
	VideoInterface.h
	VRAMBank.h

	qt/About.cpp
	qt/About.h
	qt/AudioViewer.cpp
	qt/AudioViewer.h
	qt/glext.h
	qt/main.cpp
	qt/NullAudio.h
	qt/Preferences.cpp
	qt/Preferences.h
	qt/Pretendo.cpp
	qt/Pretendo.h
	qt/pretendo.qrc
	qt/PulseAudio.h
	qt/QtVideo.cpp
	qt/QtVideo.h
	qt/SortFilterProxyModel.cpp
	qt/SortFilterProxyModel.h
)

add_executable(pretendo ${SOURCE_FILES})

target_include_directories(pretendo PUBLIC
	${PROJECT_SOURCE_DIR}/qt
	${PROJECT_SOURCE_DIR}/p6502
	${PROJECT_SOURCE_DIR}/libines2/include
	${Boost_INCLUDE_DIR}
	${INCLUDES}
	${OPENGL_INCLUDE_DIRS}
)

target_link_libraries(pretendo LINK_PUBLIC
	Qt5::Widgets Qt5::OpenGL
	${OPENGL_LIBRARIES}
	${Boost_LIBRARIES}
	${LIBRARIES}
	ines2
	p6502
	$<TARGET_OBJECTS:mappers>
)

set_property(TARGET pretendo PROPERTY CXX_STANDARD 14)
set_property(TARGET pretendo PROPERTY CXX_EXTENSIONS OFF)

add_subdirectory(libines2/lib)
add_subdirectory(p6502)
add_subdirectory(mappers)
