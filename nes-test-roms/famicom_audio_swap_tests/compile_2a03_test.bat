@echo Compiling 2a03_test.s into 2a03_test.nes...
del 2a03_test.o
del 2a03_test.nes
del 2a03_test_nsf.o
del 2a03_test.nsf
cc65\bin\ca65 2a03_test.s
cc65\bin\ld65 -C swap_test.cfg -o 2a03_test.nes 2a03_test.o -m 2a03_test_map.txt
cc65\bin\ca65 2a03_test.s -o 2a03_test_nsf.o -D NSF
cc65\bin\ld65 -C swap_test_nsf.cfg -o 2a03_test.nsf 2a03_test_nsf.o -m 2a03_test_map_nsf.txt
@pause