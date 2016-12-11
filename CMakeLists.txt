cmake_minimum_required (VERSION 3.0)
project (pretendo)

SET(AUDIO "None" CACHE STRING "Audio Subsystem To Use")
SET_PROPERTY(CACHE AUDIO PROPERTY STRINGS None QSound Pulse ASound) 

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if(${AUDIO} STREQUAL "QSound")
	
	add_definitions(-DQT_SOUND)
	
	set(SOURCE_FILES ${SOURCE_FILES}
		qt/QtAudio.cc 
		qt/AudioBuffer.cc
	)
elseif(${AUDIO} STREQUAL "ASound")
	
	find_package(ALSA REQUIRED)
	
	add_definitions(-DALSA_SOUND)

	set(SOURCE_FILES ${SOURCE_FILES}
		qt/AlsaAudio.cc
	)
	
	set(INCLUDES ${INCLUDES} ${ALSA_INCLUDE_DIR})
	set(LIBRARIES ${LIBRARIES} ${ALSA_LIBRARY})
	
elseif(${AUDIO} STREQUAL "Pulse")

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

find_package(Qt4 4.6.0 REQUIRED QtCore QtGui QtOpenGL QtMultimedia)
find_package(Boost 1.35 REQUIRED)
find_package(OpenGL REQUIRED)

include(${QT_USE_FILE})

include_directories(
	qt
	p6502
	libines2/include
	${Boost_INCLUDE_DIR}
	${INCLUDES}
	${OPENGL_INCLUDE_DIRS}
	
)

set(UI_FILES ${UI_FILES}
	qt/Pretendo.ui
	qt/Preferences.ui
	qt/About.ui
	qt/AudioViewer.ui
)

set(SOURCE_FILES ${SOURCE_FILES}
	APU.cc 
	Cart.cc 
	Config.cc 
	Controller.cc 
	DMC.cc 
	Envelope.cc 
	Input.cc 
	LengthCounter.cc 
	LinearCounter.cc 
	Mapper.cc 
	NES.cc 
	Noise.cc 
	PPU.cc 
	Palette.cc 
	Timer.cc 
	Triangle.cc 
	mappers/BNROM.cc 
	mappers/Bandai.cc 
	mappers/MMC3.cc 
	mappers/Mapper000.cc 
	mappers/Mapper001.cc 
	mappers/Mapper002.cc 
	mappers/Mapper003.cc 
	mappers/Mapper004.cc 
	mappers/Mapper005.cc 
	mappers/Mapper007.cc 
	mappers/Mapper009.cc 
	mappers/Mapper010.cc 
	mappers/Mapper011.cc 
	mappers/Mapper013.cc 
	mappers/Mapper015.cc 
	mappers/Mapper016.cc 
	mappers/Mapper018.cc 
	mappers/Mapper019.cc 
	mappers/Mapper021.cc 
	mappers/Mapper022.cc 
	mappers/Mapper023.cc 
	mappers/Mapper024.cc 
	mappers/Mapper025.cc 
	mappers/Mapper026.cc 
	mappers/Mapper027.cc 
	mappers/Mapper032.cc 
	mappers/Mapper033.cc 
	mappers/Mapper034.cc 
	mappers/Mapper040.cc 
	mappers/Mapper041.cc 
	mappers/Mapper043.cc 
	mappers/Mapper046.cc 
	mappers/Mapper048.cc 
	mappers/Mapper050.cc 
	mappers/Mapper057.cc 
	mappers/Mapper058.cc 
	mappers/Mapper061.cc 
	mappers/Mapper064.cc 
	mappers/Mapper066.cc 
	mappers/Mapper068.cc 
	mappers/Mapper069.cc 
	mappers/Mapper071.cc 
	mappers/Mapper075.cc 
	mappers/Mapper076.cc 
	mappers/Mapper079.cc 
	mappers/Mapper086.cc 
	mappers/Mapper088.cc 
	mappers/Mapper090.cc 
	mappers/Mapper113.cc 
	mappers/Mapper118.cc 
	mappers/Mapper144.cc 
	mappers/Mapper159.cc 
	mappers/Mapper163.cc 
	mappers/Mapper180.cc 
	mappers/Mapper201.cc 
	mappers/Mapper206.cc 
	mappers/Mapper218.cc 
	mappers/Mapper226.cc 
	mappers/Mapper228.cc 
	mappers/Mapper231.cc 
	mappers/Mapper232.cc 
	mappers/Mapper240.cc 
	mappers/Mapper241.cc 
	mappers/Mapper242.cc 
	mappers/Mapper243.cc 
	mappers/Mapper245.cc 
	mappers/NINA-001.cc 
	mappers/VRC2.cc 
	mappers/VRC4.cc 
	mappers/VRC6.cc 
	p6502/P6502.cc
	p6502/CPU.cc
	qt/About.cc 
	qt/AudioViewer.cc 
	qt/Preferences.cc 
	qt/Pretendo.cc 
	qt/QtVideo.cc 
	qt/SortFilterProxyModel.cc 
	qt/main.cc
	qt/pretendo.qrc
)

add_executable(pretendo ${SOURCE_FILES})

target_link_libraries(pretendo LINK_PUBLIC
	Qt4::QtGui Qt4::QtOpenGL Qt4::QtMultimedia
	${OPENGL_LIBRARIES}
	ines2
	${BOOST_LIBRARIES}
	${LIBRARIES}
)

set_property(TARGET pretendo PROPERTY CXX_STANDARD 11)

add_subdirectory(libines2/lib)
