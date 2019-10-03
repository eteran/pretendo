@echo Compiling n163_test.s into n163_test.nes...
del n163_test.o
del n163_test.nes
del n163_test_nsf.o
del n163_test.nsf
cc65\bin\ca65 n163_test.s
cc65\bin\ld65 -C swap_test.cfg -o n163_test.nes n163_test.o -m n163_test_map.txt
cc65\bin\ca65 n163_test.s -o n163_test_nsf.o -D NSF
cc65\bin\ld65 -C swap_test_nsf.cfg -o n163_test.nsf n163_test_nsf.o -m n163_test_map_nsf.txt
@pause