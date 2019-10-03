@echo Compiling 5b_level.s into 5b_level.nes...
del 5b_level.o
del 5b_level.nes
del 5b_level_nsf.o
del 5b_level.nsf
cc65\bin\ca65 5b_level.s
cc65\bin\ld65 -C swap_test.cfg -o 5b_level.nes 5b_level.o -m 5b_level_map.txt
cc65\bin\ca65 5b_level.s -o 5b_level_nsf.o -D NSF
cc65\bin\ld65 -C swap_test_nsf.cfg -o 5b_level.nsf 5b_level_nsf.o -m 5b_level_map_nsf.txt
@pause