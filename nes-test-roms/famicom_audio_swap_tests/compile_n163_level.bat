@echo Compiling n163_level.s into n163_level.nes...
del n163_level.o
del n163_level.nes
del n163_level_nsf.o
del n163_level.nsf
cc65\bin\ca65 n163_level.s
cc65\bin\ld65 -C swap_test.cfg -o n163_level.nes n163_level.o -m n163_level_map.txt
cc65\bin\ca65 n163_level.s -o n163_level_nsf.o -D NSF
cc65\bin\ld65 -C swap_test_nsf.cfg -o n163_level.nsf n163_level_nsf.o -m n163_level_map_nsf.txt
@pause