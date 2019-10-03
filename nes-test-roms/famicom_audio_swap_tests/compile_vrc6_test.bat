@echo Compiling vrc6_test.s into vrc6_test.nes...
del vrc6_test.o
del vrc6_test.nes
del vrc6_test_nsf.o
del vrc6_test.nsf
cc65\bin\ca65 vrc6_test.s
cc65\bin\ld65 -C swap_test.cfg -o vrc6_test.nes vrc6_test.o -m vrc6_test_map.txt
cc65\bin\ca65 vrc6_test.s -o vrc6_test_nsf.o -D NSF
cc65\bin\ld65 -C swap_test_nsf.cfg -o vrc6_test.nsf vrc6_test_nsf.o -m vrc6_test_map_nsf.txt
@pause