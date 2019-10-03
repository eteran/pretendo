@echo Compiling 5b_test.s into 5b_test.nes...
del 5b_test.o
del 5b_test.nes
del 5b_test_nsf.o
del 5b_test.nsf
cc65\bin\ca65 5b_test.s
cc65\bin\ld65 -C swap_test.cfg -o 5b_test.nes 5b_test.o -m 5b_test_map.txt
cc65\bin\ca65 5b_test.s -o 5b_test_nsf.o -D NSF
cc65\bin\ld65 -C swap_test_nsf.cfg -o 5b_test.nsf 5b_test_nsf.o -m 5b_test_map_nsf.txt
@pause