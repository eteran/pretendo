
TEMPLATE = app
TARGET   = pretendo
QT      += opengl

HEADERS += \
		APU.h \
		mappers/BNROM.h \
		mappers/Bandai.h \
		Cart.h \
		ConfigManager.h \
		Controller.h \
		DMC.h \
		Envelope.h \
		Sweep.h \
		Input.h \
		LengthCounter.h \
		LinearCounter.h \
		mappers/MMC3.h \
		Mapper.h \
		mappers/Mapper000.h \
		mappers/Mapper001.h \
		mappers/Mapper002.h \
		mappers/Mapper003.h \
		mappers/Mapper004.h \
		mappers/Mapper005.h \
		mappers/Mapper007.h \
		mappers/Mapper009.h \
		mappers/Mapper010.h \
		mappers/Mapper011.h \
		mappers/Mapper013.h \
		mappers/Mapper015.h \
		mappers/Mapper016.h \
		mappers/Mapper018.h \
		mappers/Mapper019.h \
		mappers/Mapper021.h \
		mappers/Mapper022.h \
		mappers/Mapper023.h \
		mappers/Mapper024.h \
		mappers/Mapper025.h \
		mappers/Mapper026.h \
		mappers/Mapper027.h \
		mappers/Mapper032.h \
		mappers/Mapper033.h \
		mappers/Mapper034.h \
		mappers/Mapper040.h \
		mappers/Mapper041.h \
		mappers/Mapper043.h \
		mappers/Mapper046.h \
		mappers/Mapper048.h \
		mappers/Mapper050.h \
		mappers/Mapper057.h \
		mappers/Mapper058.h \
		mappers/Mapper061.h \
		mappers/Mapper064.h \
		mappers/Mapper066.h \
		mappers/Mapper068.h \
		mappers/Mapper069.h \
		mappers/Mapper071.h \
		mappers/Mapper075.h \
		mappers/Mapper076.h \
		mappers/Mapper079.h \
		mappers/Mapper086.h \
		mappers/Mapper090.h \
		mappers/Mapper113.h \
		mappers/Mapper118.h \
		mappers/Mapper144.h \
		mappers/Mapper159.h \
		mappers/Mapper163.h \
		mappers/Mapper180.h \
		mappers/Mapper201.h \
		mappers/Mapper206.h \
		mappers/Mapper218.h \
		mappers/Mapper226.h \
		mappers/Mapper228.h \
		mappers/Mapper231.h \
		mappers/Mapper232.h \
		mappers/Mapper240.h \
		mappers/Mapper241.h \
		mappers/Mapper242.h \
		mappers/Mapper243.h \
		mappers/Mapper245.h \
		NES.h \
		mappers/NINA-001.h \
		Noise.h \
		PPU.h \
		Palette.h \
		Sprite.h \
		Square.h \
		Triangle.h \
		VRAMBank.h \
		Timer.h \
		mappers/VRC2.h \
		mappers/VRC4.h \
		mappers/VRC6.h \
		libunif/ines_convert.h \
		libunif/lib_ines.h \
		libunif/lib_unif.h \
		libunif/load_ines.h \
		libunif/load_unif.h \
		libunif/std_func.h \
		libunif/unif_crc32.h \
		libunif/unif_types.h

FORMS += qt/Pretendo.ui qt/Preferences.ui qt/About.ui

SOURCES += \
		APU.cc \
		mappers/BNROM.cc \
		mappers/Bandai.cc \
		Cart.cc \
		ConfigManager.cc \
		Controller.cc \
		DMC.cc \
		Envelope.cc \
		Sweep.cc \
		Input.cc \
		LengthCounter.cc \
		LinearCounter.cc \
		mappers/MMC3.cc \
		Mapper.cc \
		mappers/Mapper000.cc \
		mappers/Mapper001.cc \
		mappers/Mapper002.cc \
		mappers/Mapper003.cc \
		mappers/Mapper004.cc \
		mappers/Mapper005.cc \
		mappers/Mapper007.cc \
		mappers/Mapper009.cc \
		mappers/Mapper010.cc \
		mappers/Mapper011.cc \
		mappers/Mapper013.cc \
		mappers/Mapper015.cc \
		mappers/Mapper016.cc \
		mappers/Mapper018.cc \
		mappers/Mapper019.cc \
		mappers/Mapper021.cc \
		mappers/Mapper022.cc \
		mappers/Mapper023.cc \
		mappers/Mapper024.cc \
		mappers/Mapper025.cc \
		mappers/Mapper026.cc \
		mappers/Mapper027.cc \
		mappers/Mapper032.cc \
		mappers/Mapper033.cc \
		mappers/Mapper034.cc \
		mappers/Mapper040.cc \
		mappers/Mapper041.cc \
		mappers/Mapper043.cc \
		mappers/Mapper046.cc \
		mappers/Mapper048.cc \
		mappers/Mapper050.cc \
		mappers/Mapper057.cc \
		mappers/Mapper058.cc \
		mappers/Mapper061.cc \
		mappers/Mapper064.cc \
		mappers/Mapper066.cc \
		mappers/Mapper068.cc \
		mappers/Mapper069.cc \
		mappers/Mapper071.cc \
		mappers/Mapper075.cc \
		mappers/Mapper076.cc \
		mappers/Mapper079.cc \
		mappers/Mapper086.cc \
		mappers/Mapper090.cc \
		mappers/Mapper113.cc \
		mappers/Mapper118.cc \
		mappers/Mapper144.cc \
		mappers/Mapper159.cc \
		mappers/Mapper163.cc \
		mappers/Mapper180.cc \
		mappers/Mapper201.cc \
		mappers/Mapper206.cc \
		mappers/Mapper218.cc \
		mappers/Mapper226.cc \
		mappers/Mapper228.cc \
		mappers/Mapper231.cc \
		mappers/Mapper232.cc \
		mappers/Mapper240.cc \
		mappers/Mapper241.cc \
		mappers/Mapper242.cc \
		mappers/Mapper243.cc \
		mappers/Mapper245.cc \
		NES.cc \
		mappers/NINA-001.cc \
		Noise.cc \
		PPU.cc \
		Palette.cc \
		Square.cc \
		Triangle.cc \
		Timer.cc \
		mappers/VRC2.cc \
		mappers/VRC4.cc \
		mappers/VRC6.cc \
		libunif/ines_convert.c \
		libunif/lib_ines.c \
		libunif/lib_unif.c \
		libunif/load_ines.c \
		libunif/load_unif.c \
		libunif/std_func.c \
		libunif/unif_crc32.c 

RESOURCES += qt/pretendo.qrc

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
	DEFINES += ALSA_SOUND 
	
	SOURCES += qt/AlsaAudio.cc
	HEADERS += qt/AlsaAudio.h
	
	LIBS    += -lasound
	
	SOURCES -= qt/NullAudio.cc
	HEADERS -= qt/NullAudio.h	
}

sdlsound {
	DEFINES += SDL_SOUND
	
	SOURCES += qt/SDLAudio.cc
	HEADERS += qt/SDLAudio.h
	
	LIBS    += -lSDL
	
	SOURCES -= qt/NullAudio.cc
	HEADERS -= qt/NullAudio.h
}

qsound {
	QT      += multimedia
	DEFINES += QT_SOUND
	
	SOURCES += qt/QtAudio.cc qt/AudioBuffer.cc
	HEADERS += qt/QtAudio.h  qt/AudioBuffer.h
	
	SOURCES -= qt/NullAudio.cc
	HEADERS -= qt/NullAudio.h
}

linux-* {
	DEFINES += Linux
	INCLUDEPATH += qt

	SOURCES += \
		qt/main.cc \
		qt/NullAudio.cc \
		qt/Pretendo.cc \
		qt/QtVideo.cc \
		qt/SortFilterProxyModel.cc \
		qt/Preferences.cc \
		qt/About.cc
		
	HEADERS += \
		qt/NullAudio.h \
		qt/Pretendo.h \
		qt/QtVideo.h \
		qt/SortFilterProxyModel.h \
		qt/Preferences.h \
		qt/About.h
}

win32-* {
    INCLUDEPATH += "C:\\Program Files\\boost\\boost_1_51"
}

#QMAKE_CC   = clang   -ansi -pedantic -Wno-long-long
#QMAKE_CXX  = clang++ -ansi -pedantic -Wno-long-long
#QMAKE_LINK = clang++

*-g++* {
	QMAKE_CFLAGS_DEBUG     += -g3 -W -Wall
	QMAKE_CXXFLAGS_DEBUG   += -g3 -W -Wall
	QMAKE_LFLAGS_DEBUG     += -g3 -W -Wall

	QMAKE_CFLAGS_RELEASE   += -march=native -g3 -W -Wall -O3
	QMAKE_CXXFLAGS_RELEASE += -march=native -g3 -W -Wall -O3
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
