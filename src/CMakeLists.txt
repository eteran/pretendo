cmake_minimum_required (VERSION 3.0)

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

find_package(Qt5 5.4.0 REQUIRED Widgets OpenGL)
find_package(Boost 1.35 COMPONENTS system filesystem REQUIRED)
find_package(OpenGL REQUIRED)

add_executable(pretendo 
	${SOURCE_FILES}
	
	Apu.cpp
	Apu.h
	BitField.h
    Bus.cpp
    Bus.h
	Cart.cpp
	Cart.h
	circular_buffer.h
	Config.cpp
	Config.h
	Controller.cpp
	Controller.h
	Cpu.cpp
	Cpu.h
	Dmc.cpp
	Dmc.h
	Envelope.cpp
	Envelope.h
	Input.cpp
	Input.h
	LengthCounter.cpp
	LengthCounter.h
	Lfsr.h
	LinearCounter.cpp
	LinearCounter.h
	Mapper.cpp
	Mapper.h
	MemoryMappedFile.cpp
	MemoryMappedFile.h
	Nes.cpp
	Nes.h
	Noise.cpp
	Noise.h
	Palette.cpp
	Palette.h
	Ppu.cpp
	Ppu.h
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

	qt/Pretendo.ui
	qt/Preferences.ui
	qt/About.ui
	qt/AudioViewer.ui
)

add_dependencies(pretendo mappers)

target_include_directories(pretendo PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/qt
	${Boost_INCLUDE_DIR}
	${INCLUDES}
)

target_link_libraries(pretendo LINK_PUBLIC
	Qt5::Widgets
	Qt5::OpenGL
	OpenGL::GL
	${Boost_LIBRARIES}
	${LIBRARIES}
	iNES2
	p6502
	$<TARGET_OBJECTS:mappers>
)

set_property(TARGET pretendo PROPERTY CXX_STANDARD 14)
set_property(TARGET pretendo PROPERTY CXX_EXTENSIONS OFF)
set_property(TARGET pretendo PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
