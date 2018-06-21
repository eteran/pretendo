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
		qt/PulseAudio.cc
	)
	
	set(INCLUDES ${INCLUDES} ${PULSEAUDIO_INCLUDE_DIR})
	set(LIBRARIES ${LIBRARIES} ${PULSEAUDIO_LIBRARY} -lpulse-simple)	

else()
	set(SOURCE_FILES ${SOURCE_FILES}
		qt/NullAudio.cc
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
	APU.cc
	APU.h
	BitField.h
	Cart.cc
	Cart.h
	circular_buffer.h
	Config.cc
	Config.h
	Controller.cc
	Controller.h
	CPU.cc
	CPU.h
	DMC.cc
	DMC.h
	Envelope.cc
	Envelope.h
	Input.cc
	Input.h
	LengthCounter.cc
	LengthCounter.h
	LFSR.h
	LinearCounter.cc
	LinearCounter.h
	Mapper.cc
	Mapper.h
	MemoryMappedFile.cc
	MemoryMappedFile.h
	NES.cc
	NES.h
	Noise.cc
	Noise.h
	Palette.cc
	Palette.h
	PPU.cc
	PPU.h
	Reset.h
	ShiftRegister.h
	Square.h
	Sweep.h
	Timer.cc
	Timer.h
	Triangle.cc
	Triangle.h
	VideoInterface.h
	VRAMBank.h

	qt/About.cc
	qt/About.h
	qt/AudioViewer.cc
	qt/AudioViewer.h
	qt/glext.h
	qt/main.cc
	qt/NullAudio.h
	qt/Preferences.cc
	qt/Preferences.h
	qt/Pretendo.cc
	qt/Pretendo.h
	qt/pretendo.qrc
	qt/PulseAudio.h
	qt/QtVideo.cc
	qt/QtVideo.h
	qt/SortFilterProxyModel.cc
	qt/SortFilterProxyModel.h

	mappers/Bandai.cc
	mappers/Bandai.h
	mappers/BNROM.cc
	mappers/BNROM.h
	mappers/Mapper000.cc
	mappers/Mapper000.h
	mappers/Mapper001.cc
	mappers/Mapper001.h
	mappers/Mapper002.cc
	mappers/Mapper002.h
	mappers/Mapper003.cc
	mappers/Mapper003.h
	mappers/Mapper004.cc
	mappers/Mapper004.h
	mappers/Mapper005.cc
	mappers/Mapper005.h
	mappers/Mapper007.cc
	mappers/Mapper007.h
	mappers/Mapper009.cc
	mappers/Mapper009.h
	mappers/Mapper010.cc
	mappers/Mapper010.h
	mappers/Mapper011.cc
	mappers/Mapper011.h
	mappers/Mapper013.cc
	mappers/Mapper013.h
	mappers/Mapper015.cc
	mappers/Mapper015.h
	mappers/Mapper016.cc
	mappers/Mapper016.h
	mappers/Mapper018.cc
	mappers/Mapper018.h
	mappers/Mapper019.cc
	mappers/Mapper019.h
	mappers/Mapper021.cc
	mappers/Mapper021.h
	mappers/Mapper022.cc
	mappers/Mapper022.h
	mappers/Mapper023.cc
	mappers/Mapper023.h
	mappers/Mapper024.cc
	mappers/Mapper024.h
	mappers/Mapper025.cc
	mappers/Mapper025.h
	mappers/Mapper026.cc
	mappers/Mapper026.h
	mappers/Mapper027.cc
	mappers/Mapper027.h
	mappers/Mapper032.cc
	mappers/Mapper032.h
	mappers/Mapper033.cc
	mappers/Mapper033.h
	mappers/Mapper034.cc
	mappers/Mapper034.h
	mappers/Mapper040.cc
	mappers/Mapper040.h
	mappers/Mapper041.cc
	mappers/Mapper041.h
	mappers/Mapper043.cc
	mappers/Mapper043.h
	mappers/Mapper046.cc
	mappers/Mapper046.h
	mappers/Mapper048.cc
	mappers/Mapper048.h
	mappers/Mapper050.cc
	mappers/Mapper050.h
	mappers/Mapper057.cc
	mappers/Mapper057.h
	mappers/Mapper058.cc
	mappers/Mapper058.h
	mappers/Mapper061.cc
	mappers/Mapper061.h
	mappers/Mapper064.cc
	mappers/Mapper064.h
	mappers/Mapper066.cc
	mappers/Mapper066.h
	mappers/Mapper068.cc
	mappers/Mapper068.h
	mappers/Mapper069.cc
	mappers/Mapper069.h
	mappers/Mapper071.cc
	mappers/Mapper071.h
	mappers/Mapper075.cc
	mappers/Mapper075.h
	mappers/Mapper076.cc
	mappers/Mapper076.h
	mappers/Mapper079.cc
	mappers/Mapper079.h
	mappers/Mapper086.cc
	mappers/Mapper086.h
	mappers/Mapper088.cc
	mappers/Mapper088.h
	mappers/Mapper090.cc
	mappers/Mapper090.h
	mappers/Mapper113.cc
	mappers/Mapper113.h
	mappers/Mapper118.cc
	mappers/Mapper118.h
	mappers/Mapper144.cc
	mappers/Mapper144.h
	mappers/Mapper159.cc
	mappers/Mapper159.h
	mappers/Mapper163.cc
	mappers/Mapper163.h
	mappers/Mapper180.cc
	mappers/Mapper180.h
	mappers/Mapper201.cc
	mappers/Mapper201.h
	mappers/Mapper206.cc
	mappers/Mapper206.h
	mappers/Mapper218.cc
	mappers/Mapper218.h
	mappers/Mapper226.cc
	mappers/Mapper226.h
	mappers/Mapper228.cc
	mappers/Mapper228.h
	mappers/Mapper231.cc
	mappers/Mapper231.h
	mappers/Mapper232.cc
	mappers/Mapper232.h
	mappers/Mapper240.cc
	mappers/Mapper240.h
	mappers/Mapper241.cc
	mappers/Mapper241.h
	mappers/Mapper242.cc
	mappers/Mapper242.h
	mappers/Mapper243.cc
	mappers/Mapper243.h
	mappers/Mapper245.cc
	mappers/Mapper245.h
	mappers/MMC3.cc
	mappers/MMC3.h
	mappers/NINA-001.cc
	mappers/NINA-001.h
	mappers/VRC2.cc
	mappers/VRC2.h
	mappers/VRC4.cc
	mappers/VRC4.h
	mappers/VRC6.cc
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
