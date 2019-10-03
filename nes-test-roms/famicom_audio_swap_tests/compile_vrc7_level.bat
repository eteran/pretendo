@echo Compiling vrc7_level.s into vrc7_level.nes...
del vrc7_level.o
del vrc7_level.nes
del vrc7_level_nsf.o
del vrc7_level.nsf
cc65\bin\ca65 vrc7_level.s
cc65\bin\ld65 -C swap_test.cfg -o vrc7_level.nes vrc7_level.o -m vrc7_level_map.txt
cc65\bin\ca65 vrc7_level.s -o vrc7_level_nsf.o -D NSF
cc65\bin\ld65 -C swap_test_nsf.cfg -o vrc7_level.nsf vrc7_level_nsf.o -m vrc7_level_map_nsf.txt
@pause