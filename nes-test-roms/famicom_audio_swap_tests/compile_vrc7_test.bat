@echo Compiling vrc7_test.s into vrc7_test.nes...
del vrc7_test.o
del vrc7_test.nes
del vrc7_test_nsf.o
del vrc7_test.nsf
cc65\bin\ca65 vrc7_test.s
cc65\bin\ld65 -C swap_test.cfg -o vrc7_test.nes vrc7_test.o -m vrc7_test_map.txt
cc65\bin\ca65 vrc7_test.s -o vrc7_test_nsf.o -D NSF
cc65\bin\ld65 -C swap_test_nsf.cfg -o vrc7_test.nsf vrc7_test_nsf.o -m vrc7_test_map_nsf.txt
@pause