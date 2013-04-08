
TEMPLATE = app
TARGET   = pretendo
QT      += opengl

# Input
HEADERS += \
		APU.h \
		BNROM.h \
		Bandai.h \
		Cart.h \
		Controller.h \
		DMC.h \
		Input.h \
		LengthCounter.h \
		MMC3.h \
		Mapper.h \
		Mapper000.h \
		Mapper001.h \
		Mapper002.h \
		Mapper003.h \
		Mapper004.h \
		Mapper005.h \
		Mapper007.h \
		Mapper009.h \
		Mapper010.h \
		Mapper011.h \
		Mapper013.h \
		Mapper015.h \
		Mapper016.h \
		Mapper018.h \
		Mapper019.h \
		Mapper021.h \
		Mapper022.h \
		Mapper023.h \
		Mapper024.h \
		Mapper025.h \
		Mapper026.h \
		Mapper027.h \
		Mapper032.h \
		Mapper033.h \
		Mapper034.h \
		Mapper040.h \
		Mapper041.h \
		Mapper043.h \
		Mapper046.h \
		Mapper048.h \
		Mapper050.h \
		Mapper057.h \
		Mapper058.h \
		Mapper061.h \
		Mapper064.h \
		Mapper066.h \
		Mapper068.h \
		Mapper069.h \
		Mapper071.h \
		Mapper075.h \
		Mapper076.h \
		Mapper079.h \
		Mapper086.h \
		Mapper090.h \
		Mapper113.h \
		Mapper118.h \
		Mapper144.h \
		Mapper159.h \
		Mapper163.h \
		Mapper180.h \
		Mapper201.h \
		Mapper206.h \
		Mapper218.h \
		Mapper226.h \
		Mapper228.h \
		Mapper231.h \
		Mapper232.h \
		Mapper240.h \
		Mapper241.h \
		Mapper242.h \
		Mapper243.h \
		Mapper245.h \
		NES.h \
		NINA-001.h \
		NTSC.h \
		Noise.h \
		PPU.h \
		Palette.h \
		Sprite.h \
		Square.h \
		Triangle.h \
		VRAMBank.h \
		VRC2.h \
		VRC4.h \
		VRC6.h \
		libunif/ines_convert.h \
		libunif/lib_ines.h \
		libunif/lib_unif.h \
		libunif/load_ines.h \
		libunif/load_unif.h \
		libunif/std_func.h \
		libunif/unif_crc32.h \
		libunif/unif_types.h \
		qt/NullAudio.h \
		qt/Pretendo.h \
		qt/QtVideo.h \
		qt/SortFilterProxyModel.h

FORMS += qt/Pretendo.ui

SOURCES += \
		APU.cc \
		BNROM.cc \
		Bandai.cc \
		Cart.cc \
		Controller.cc \
		DMC.cc \
		Input.cc \
		LengthCounter.cc \
		MMC3.cc \
		Mapper.cc \
		Mapper000.cc \
		Mapper001.cc \
		Mapper002.cc \
		Mapper003.cc \
		Mapper004.cc \
		Mapper005.cc \
		Mapper007.cc \
		Mapper009.cc \
		Mapper010.cc \
		Mapper011.cc \
		Mapper013.cc \
		Mapper015.cc \
		Mapper016.cc \
		Mapper018.cc \
		Mapper019.cc \
		Mapper021.cc \
		Mapper022.cc \
		Mapper023.cc \
		Mapper024.cc \
		Mapper025.cc \
		Mapper026.cc \
		Mapper027.cc \
		Mapper032.cc \
		Mapper033.cc \
		Mapper034.cc \
		Mapper040.cc \
		Mapper041.cc \
		Mapper043.cc \
		Mapper046.cc \
		Mapper048.cc \
		Mapper050.cc \
		Mapper057.cc \
		Mapper058.cc \
		Mapper061.cc \
		Mapper064.cc \
		Mapper066.cc \
		Mapper068.cc \
		Mapper069.cc \
		Mapper071.cc \
		Mapper075.cc \
		Mapper076.cc \
		Mapper079.cc \
		Mapper086.cc \
		Mapper090.cc \
		Mapper113.cc \
		Mapper118.cc \
		Mapper144.cc \
		Mapper159.cc \
		Mapper163.cc \
		Mapper180.cc \
		Mapper201.cc \
		Mapper206.cc \
		Mapper218.cc \
		Mapper226.cc \
		Mapper228.cc \
		Mapper231.cc \
		Mapper232.cc \
		Mapper240.cc \
		Mapper241.cc \
		Mapper242.cc \
		Mapper243.cc \
		Mapper245.cc \
		NES.cc \
		NINA-001.cc \
		NTSC.cc \
		Noise.cc \
		PPU.cc \
		Palette.cc \
		Square.cc \
		Triangle.cc \
		VRC2.cc \
		VRC4.cc \
		VRC6.cc \
		libunif/ines_convert.c \
		libunif/lib_ines.c \
		libunif/lib_unif.c \
		libunif/load_ines.c \
		libunif/load_unif.c \
		libunif/std_func.c \
		libunif/unif_crc32.c \
		qt/main.cc \
		qt/NullAudio.cc \
		qt/Pretendo.cc \
		qt/QtVideo.cc \
		qt/SortFilterProxyModel.cc

RESOURCES += pretendo.qrc

bender {
	DEFINES     += USE_BENDER
	DEFINES     += FAST_CPU
	DEPENDPATH  += bender
	INCLUDEPATH += bender
	
	HEADERS     +=      \
		bender/CPU.h    \
		bender/bender.h \
		
	SOURCES     +=    \
		bender/CPU.cc \
		
        QMAKE_EXTRA_COMPILERS += nasmproc
        ASM_SRCS += bender.asm revbits.asm
        nasmproc.output   = ${OBJECTS_DIR}/${QMAKE_FILE_BASE}.o
        nasmproc.commands = nasm -f elf ${QMAKE_FILE_NAME} -o ${OBJECTS_DIR}/${QMAKE_FILE_BASE}.o
        nasmproc.input    = ASM_SRCS
		
} else {
        DEPENDPATH  += p6502
        INCLUDEPATH += p6502

        HEADERS += \
			p6502/address_modes/zero_page_x.h \
			p6502/address_modes/absolute_x.h \
			p6502/address_modes/zero_page_y.h \
			p6502/address_modes/relative.h \
			p6502/address_modes/indirect_indexed.h \
			p6502/address_modes/stack.h \
			p6502/address_modes/implied.h \
			p6502/address_modes/immediate.h \
			p6502/address_modes/accumulator.h \
			p6502/address_modes/zero_page.h \
			p6502/address_modes/indirect.h \
			p6502/address_modes/absolute_y.h \
			p6502/address_modes/absolute.h \
			p6502/address_modes/indexed_indirect.h \
			p6502/CPU.h \
			p6502/memory.h \
			p6502/opcodes.h \
			p6502/address_modes.h \
			p6502/opcodes/brk.h \
			p6502/opcodes/tsx.h \
			p6502/opcodes/lsr.h \
			p6502/opcodes/ldx.h \
			p6502/opcodes/sta.h \
			p6502/opcodes/bvs.h \
			p6502/opcodes/dey.h \
			p6502/opcodes/ldy.h \
			p6502/opcodes/unofficial/asr.h \
			p6502/opcodes/unofficial/aac.h \
			p6502/opcodes/unofficial/arr.h \
			p6502/opcodes/unofficial/axa.h \
			p6502/opcodes/unofficial/axs.h \
			p6502/opcodes/unofficial/dcp.h \
			p6502/opcodes/unofficial/isc.h \
			p6502/opcodes/unofficial/lar.h \
			p6502/opcodes/unofficial/lax.h \
			p6502/opcodes/unofficial/rla.h \
			p6502/opcodes/unofficial/rra.h \
			p6502/opcodes/unofficial/aax.h \
			p6502/opcodes/unofficial/slo.h \
			p6502/opcodes/unofficial/sre.h \
			p6502/opcodes/unofficial/sxa.h \
			p6502/opcodes/unofficial/sya.h \
			p6502/opcodes/unofficial/xaa.h \
			p6502/opcodes/unofficial/xas.h \
			p6502/opcodes/asl.h \
			p6502/opcodes/txs.h \
			p6502/opcodes/clc.h \
			p6502/opcodes/bcs.h \
			p6502/opcodes/sei.h \
			p6502/opcodes/iny.h \
			p6502/opcodes/bmi.h \
			p6502/opcodes/bit.h \
			p6502/opcodes/lda.h \
			p6502/opcodes/cli.h \
			p6502/opcodes/rti.h \
			p6502/opcodes/stx.h \
			p6502/opcodes/pha.h \
			p6502/opcodes/bpl.h \
			p6502/opcodes/branch.h \
			p6502/opcodes/sed.h \
			p6502/opcodes/ora.h \
			p6502/opcodes/beq.h \
			p6502/opcodes/cpx.h \
			p6502/opcodes/bcc.h \
			p6502/opcodes/adc.h \
			p6502/opcodes/bvc.h \
			p6502/opcodes/clv.h \
			p6502/opcodes/plp.h \
			p6502/opcodes/inc.h \
			p6502/opcodes/sec.h \
			p6502/opcodes/inx.h \
			p6502/opcodes/rts.h \
			p6502/opcodes/sty.h \
			p6502/opcodes/tya.h \
			p6502/opcodes/bne.h \
			p6502/opcodes/txa.h \
			p6502/opcodes/cld.h \
			p6502/opcodes/eor.h \
			p6502/opcodes/jmp.h \
			p6502/opcodes/ror.h \
			p6502/opcodes/compare.h \
			p6502/opcodes/dex.h \
			p6502/opcodes/and.h \
			p6502/opcodes/cpy.h \
			p6502/opcodes/tay.h \
			p6502/opcodes/cmp.h \
			p6502/opcodes/nop.h \
			p6502/opcodes/rol.h \
			p6502/opcodes/dec.h \
			p6502/opcodes/sbc.h \
			p6502/opcodes/jsr.h \
			p6502/opcodes/tax.h \
			p6502/opcodes/pla.h \
			p6502/opcodes/php.h \
			p6502/P6502.h

        SOURCES += \
			p6502/P6502.cc \
			p6502/CPU.cc

}

asound {
	DEFINES += ENABLE_SOUND 
	
	SOURCES += qt/AlsaAudio.cc
	HEADERS += qt/AlsaAudio.h
	
	LIBS    += -lasound
}

qsound {
	QT      += multimedia
	DEFINES += ENABLE_SOUND USE_QAUDIO
	
	SOURCES += qt/QtAudio.cc qt/AudioBuffer.cc
	HEADERS += qt/QtAudio.h  qt/AudioBuffer.h
}


linux-* {
	DEFINES += Linux
	INCLUDEPATH += qt
	#INCLUDEPATH += /usr/include/QtMultimediaKit
	#INCLUDEPATH += /usr/include/QtMobility
}

win32-* {
    INCLUDEPATH += "C:\\Program Files\\boost\\boost_1_51"
}

#QMAKE_CC   = clang   -ansi -pedantic -Wno-long-long
#QMAKE_CXX  = clang++ -ansi -pedantic -Wno-long-long
#QMAKE_LINK = clang++

*-g++*{
	QMAKE_CFLAGS_DEBUG     += -g3 -W -Wall
	QMAKE_CXXFLAGS_DEBUG   += -g3 -W -Wall
	QMAKE_LFLAGS_DEBUG     += -g3 -W -Wall

	QMAKE_CFLAGS_RELEASE   += -march=native -g3 -W -Wall
	QMAKE_CXXFLAGS_RELEASE += -march=native -g3 -W -Wall
	QMAKE_LFLAGS_RELEASE   += -march=native -g3
}

CONFIG(debug, debug|release) {
	OBJECTS_DIR = $${OUT_PWD}/.debug-shared/obj
	MOC_DIR     = $${OUT_PWD}/.debug-shared/moc
	RCC_DIR     = $${OUT_PWD}/.debug-shared/rcc
	UI_DIR      = $${OUT_PWD}/.debug-shared/uic
	DEFINES    += QT_SHAREDPOINTER_TRACK_POINTERS
}

CONFIG(release, debug|release) {
	OBJECTS_DIR = $${OUT_PWD}/.release-shared/obj
	MOC_DIR     = $${OUT_PWD}/.release-shared/moc
	RCC_DIR     = $${OUT_PWD}/.release-shared/rcc
	UI_DIR      = $${OUT_PWD}/.release-shared/uic
	DEFINES += NDEBUG
}
