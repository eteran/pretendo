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
	
    opcodes/adc.h
    opcodes/branch.h
    opcodes/ldy.h
    opcodes/sed.h
    opcodes/bcs.h
    opcodes/compare.h
    opcodes/sec.h
    opcodes/tsx.h
    opcodes/asl.h
    opcodes/txs.h
    opcodes/cpy.h
    opcodes/clc.h
    opcodes/unofficial/aac.h
    opcodes/unofficial/sxa.h
    opcodes/unofficial/lax.h
    opcodes/unofficial/rla.h
    opcodes/unofficial/arr.h
    opcodes/unofficial/xaa.h
    opcodes/unofficial/aax.h
    opcodes/unofficial/sya.h
    opcodes/unofficial/asr.h
    opcodes/unofficial/axs.h
    opcodes/unofficial/isc.h
    opcodes/unofficial/slo.h
    opcodes/unofficial/axa.h
    opcodes/unofficial/sre.h
    opcodes/unofficial/lar.h
    opcodes/unofficial/rra.h
    opcodes/unofficial/xas.h
    opcodes/unofficial/dcp.h
    opcodes/lsr.h
    opcodes/bcc.h
    opcodes/stx.h
    opcodes/txa.h
    opcodes/lda.h
    opcodes/cld.h
    opcodes/bmi.h
    opcodes/tya.h
    opcodes/dey.h
    opcodes/pha.h
    opcodes/pla.h
    opcodes/ora.h
    opcodes/rts.h
    opcodes/rti.h
    opcodes/and.h
    opcodes/php.h
    opcodes/bne.h
    opcodes/tax.h
    opcodes/sty.h
    opcodes/bvs.h
    opcodes/bit.h
    opcodes/jsr.h
    opcodes/plp.h
    opcodes/cpx.h
    opcodes/dex.h
    opcodes/sta.h
    opcodes/tay.h
    opcodes/clv.h
    opcodes/special/irq.h
    opcodes/special/brk.h
    opcodes/special/jam.h
    opcodes/special/rst.h
    opcodes/special/rts.h
    opcodes/special/rti.h
    opcodes/special/nmi.h
    opcodes/special/jsr.h
    opcodes/sei.h
    opcodes/cli.h
    opcodes/nop.h
    opcodes/jmp.h
    opcodes/cmp.h
    opcodes/inc.h
    opcodes/beq.h
    opcodes/dec.h
    opcodes/ror.h
    opcodes/iny.h
    opcodes/sbc.h
    opcodes/inx.h
    opcodes/eor.h
    opcodes/bvc.h
    opcodes/ldx.h
    opcodes/bpl.h
    opcodes/rol.h
    opcodes.h
    memory.h
    address_modes/absolute_x.h
    address_modes/indexed_indirect.h
    address_modes/zero_page_y.h
    address_modes/stack.h
    address_modes/absolute_y.h
    address_modes/indirect.h
    address_modes/implied.h
    address_modes/indirect_indexed.h
    address_modes/absolute.h
    address_modes/zero_page_x.h
    address_modes/zero_page.h
    address_modes/relative.h
    address_modes/accumulator.h
    address_modes/immediate.h
    address_modes.h
	Apu.cpp
	Apu.h
	BitField.h
    Bus.cpp
    Bus.h
	Cart.cpp
	Cart.h
	circular_buffer.h
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
	VRAMBank.h

	qt/Settings.cpp
	qt/Settings.h
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
	qt/FilesystemModel.cpp
	qt/FilesystemModel.h
	
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
	-lpthread
	$<TARGET_OBJECTS:mappers>
)

set_property(TARGET pretendo PROPERTY CXX_STANDARD 14)
set_property(TARGET pretendo PROPERTY CXX_EXTENSIONS OFF)
set_property(TARGET pretendo PROPERTY RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")
