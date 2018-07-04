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

	mappers/Bandai.cpp
	mappers/Bandai.h
	mappers/BNROM.cpp
	mappers/BNROM.h
	mappers/Mapper000.cpp
	mappers/Mapper000.h
	mappers/Mapper001.cpp
	mappers/Mapper001.h
	mappers/Mapper002.cpp
	mappers/Mapper002.h
	mappers/Mapper003.cpp
	mappers/Mapper003.h
	mappers/Mapper004.cpp
	mappers/Mapper004.h
	mappers/Mapper005.cpp
	mappers/Mapper005.h
	mappers/Mapper007.cpp
	mappers/Mapper007.h
	mappers/Mapper009.cpp
	mappers/Mapper009.h
	mappers/Mapper010.cpp
	mappers/Mapper010.h
	mappers/Mapper011.cpp
	mappers/Mapper011.h
	mappers/Mapper013.cpp
	mappers/Mapper013.h
	mappers/Mapper015.cpp
	mappers/Mapper015.h
	mappers/Mapper016.cpp
	mappers/Mapper016.h
	mappers/Mapper018.cpp
	mappers/Mapper018.h
	mappers/Mapper019.cpp
	mappers/Mapper019.h
	mappers/Mapper021.cpp
	mappers/Mapper021.h
	mappers/Mapper022.cpp
	mappers/Mapper022.h
	mappers/Mapper023.cpp
	mappers/Mapper023.h
	mappers/Mapper024.cpp
	mappers/Mapper024.h
	mappers/Mapper025.cpp
	mappers/Mapper025.h
	mappers/Mapper026.cpp
	mappers/Mapper026.h
	mappers/Mapper027.cpp
	mappers/Mapper027.h
	mappers/Mapper032.cpp
	mappers/Mapper032.h
	mappers/Mapper033.cpp
	mappers/Mapper033.h
	mappers/Mapper034.cpp
	mappers/Mapper034.h
	mappers/Mapper040.cpp
	mappers/Mapper040.h
	mappers/Mapper041.cpp
	mappers/Mapper041.h
	mappers/Mapper043.cpp
	mappers/Mapper043.h
	mappers/Mapper046.cpp
	mappers/Mapper046.h
	mappers/Mapper048.cpp
	mappers/Mapper048.h
	mappers/Mapper050.cpp
	mappers/Mapper050.h
	mappers/Mapper057.cpp
	mappers/Mapper057.h
	mappers/Mapper058.cpp
	mappers/Mapper058.h
	mappers/Mapper061.cpp
	mappers/Mapper061.h
	mappers/Mapper064.cpp
	mappers/Mapper064.h
	mappers/Mapper066.cpp
	mappers/Mapper066.h
	mappers/Mapper068.cpp
	mappers/Mapper068.h
	mappers/Mapper069.cpp
	mappers/Mapper069.h
	mappers/Mapper071.cpp
	mappers/Mapper071.h
	mappers/Mapper075.cpp
	mappers/Mapper075.h
	mappers/Mapper076.cpp
	mappers/Mapper076.h
	mappers/Mapper079.cpp
	mappers/Mapper079.h
	mappers/Mapper086.cpp
	mappers/Mapper086.h
	mappers/Mapper088.cpp
	mappers/Mapper088.h
	mappers/Mapper090.cpp
	mappers/Mapper090.h
	mappers/Mapper113.cpp
	mappers/Mapper113.h
	mappers/Mapper118.cpp
	mappers/Mapper118.h
	mappers/Mapper144.cpp
	mappers/Mapper144.h
	mappers/Mapper159.cpp
	mappers/Mapper159.h
	mappers/Mapper163.cpp
	mappers/Mapper163.h
	mappers/Mapper180.cpp
	mappers/Mapper180.h
	mappers/Mapper201.cpp
	mappers/Mapper201.h
	mappers/Mapper206.cpp
	mappers/Mapper206.h
	mappers/Mapper218.cpp
	mappers/Mapper218.h
	mappers/Mapper226.cpp
	mappers/Mapper226.h
	mappers/Mapper228.cpp
	mappers/Mapper228.h
	mappers/Mapper231.cpp
	mappers/Mapper231.h
	mappers/Mapper232.cpp
	mappers/Mapper232.h
	mappers/Mapper240.cpp
	mappers/Mapper240.h
	mappers/Mapper241.cpp
	mappers/Mapper241.h
	mappers/Mapper242.cpp
	mappers/Mapper242.h
	mappers/Mapper243.cpp
	mappers/Mapper243.h
	mappers/Mapper245.cpp
	mappers/Mapper245.h
	mappers/MMC3.cpp
	mappers/MMC3.h
	mappers/NINA-001.cpp
	mappers/NINA-001.h
	mappers/VRC2.cpp
	mappers/VRC2.h
	mappers/VRC4.cpp
	mappers/VRC4.h
	mappers/VRC6.cpp
	mappers/VRC6.h
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
)

set_property(TARGET pretendo PROPERTY CXX_STANDARD 14)
set_property(TARGET pretendo PROPERTY CXX_EXTENSIONS OFF)

add_subdirectory(libines2/lib)
add_subdirectory(p6502)
