@echo Compiling 2a03_level.s into 2a03_level.nes...
del 2a03_level.o
del 2a03_level.nes
del 2a03_level_nsf.o
del 2a03_level.nsf
cc65\bin\ca65 2a03_level.s
cc65\bin\ld65 -C swap_test.cfg -o 2a03_level.nes 2a03_level.o -m 2a03_level_map.txt
cc65\bin\ca65 2a03_level.s -o 2a03_level_nsf.o -D NSF
cc65\bin\ld65 -C swap_test_nsf.cfg -o 2a03_level.nsf 2a03_level_nsf.o -m 2a03_level_map_nsf.txt
@pause