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
find_package(OpenGL REQUIRED)

add_executable(pretendo
	${SOURCE_FILES}

	Apu.cpp
	Apu.h
	BitField.h
	Cart.cpp
	Cart.h
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
	SaveState.cpp
	SaveState.h
	ShiftRegister.h
	Square.h
	Sweep.h
	Timer.cpp
	Timer.h
	Triangle.cpp
	Triangle.h
	VRAMBank.h
	circular_buffer.h
	Bus.cpp
	Bus.h

	address_modes.h
	address_modes/absolute.h
	address_modes/absolute_x.h
	address_modes/absolute_y.h
	address_modes/accumulator.h
	address_modes/immediate.h
	address_modes/implied.h
	address_modes/indexed_indirect.h
	address_modes/indirect.h
	address_modes/indirect_indexed.h
	address_modes/relative.h
	address_modes/stack.h
	address_modes/zero_page.h
	address_modes/zero_page_x.h
	address_modes/zero_page_y.h
	memory.h
	opcodes.h
	opcodes/adc.h
	opcodes/and.h
	opcodes/asl.h
	opcodes/bcc.h
	opcodes/bcs.h
	opcodes/beq.h
	opcodes/bit.h
	opcodes/bmi.h
	opcodes/bne.h
	opcodes/bpl.h
	opcodes/branch.h
	opcodes/bvc.h
	opcodes/bvs.h
	opcodes/clc.h
	opcodes/cld.h
	opcodes/cli.h
	opcodes/clv.h
	opcodes/cmp.h
	opcodes/compare.h
	opcodes/cpx.h
	opcodes/cpy.h
	opcodes/dec.h
	opcodes/dex.h
	opcodes/dey.h
	opcodes/eor.h
	opcodes/inc.h
	opcodes/inx.h
	opcodes/iny.h
	opcodes/jmp.h
	opcodes/jsr.h
	opcodes/lda.h
	opcodes/ldx.h
	opcodes/ldy.h
	opcodes/lsr.h
	opcodes/nop.h
	opcodes/ora.h
	opcodes/pha.h
	opcodes/php.h
	opcodes/pla.h
	opcodes/plp.h
	opcodes/rol.h
	opcodes/ror.h
	opcodes/rti.h
	opcodes/rts.h
	opcodes/sbc.h
	opcodes/sec.h
	opcodes/sed.h
	opcodes/sei.h
	opcodes/special/brk.h
	opcodes/special/irq.h
	opcodes/special/jam.h
	opcodes/special/jsr.h
	opcodes/special/nmi.h
	opcodes/special/rst.h
	opcodes/special/rti.h
	opcodes/special/rts.h
	opcodes/sta.h
	opcodes/stx.h
	opcodes/sty.h
	opcodes/tax.h
	opcodes/tay.h
	opcodes/tsx.h
	opcodes/txa.h
	opcodes/txs.h
	opcodes/tya.h
	opcodes/unofficial/aac.h
	opcodes/unofficial/aax.h
	opcodes/unofficial/arr.h
	opcodes/unofficial/asr.h
	opcodes/unofficial/axa.h
	opcodes/unofficial/axs.h
	opcodes/unofficial/dcp.h
	opcodes/unofficial/isc.h
	opcodes/unofficial/lar.h
	opcodes/unofficial/lax.h
	opcodes/unofficial/rla.h
	opcodes/unofficial/rra.h
	opcodes/unofficial/slo.h
	opcodes/unofficial/sre.h
	opcodes/unofficial/sxa.h
	opcodes/unofficial/sya.h
	opcodes/unofficial/xaa.h
	opcodes/unofficial/xas.h

	qt/About.cpp
	qt/About.h
	qt/AudioViewer.cpp
	qt/AudioViewer.h
	qt/FilesystemModel.cpp
	qt/FilesystemModel.h
	qt/NullAudio.h
	qt/Preferences.cpp
	qt/Preferences.h
	qt/Pretendo.cpp
	qt/Pretendo.h
	qt/PulseAudio.h
	qt/QtVideo.cpp
	qt/QtVideo.h
	qt/Settings.cpp
	qt/Settings.h
	qt/SortFilterProxyModel.cpp
	qt/SortFilterProxyModel.h
	qt/glext.h
	qt/main.cpp
	qt/pretendo.qrc

	qt/Pretendo.ui
	qt/Preferences.ui
	qt/About.ui
	qt/AudioViewer.ui
)

add_dependencies(pretendo mappers)

target_include_directories(pretendo PRIVATE
	${CMAKE_CURRENT_SOURCE_DIR}/qt
	${INCLUDES}
)

target_link_libraries(pretendo LINK_PUBLIC
	Qt5::Widgets
	Qt5::OpenGL
	OpenGL::GL
	${LIBRARIES}
	iNES2
	-lpthread
	$<TARGET_OBJECTS:mappers>
)

set_target_properties(pretendo PROPERTIES
    CXX_STANDARD 17
    CXX_EXTENSIONS OFF
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_subdirectory(mappers)
