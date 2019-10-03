@echo Compiling vrc6_test_2.s into vrc6_test_2.nes...
del vrc6_test_2.o
del vrc6_test_2.nes
del vrc6_test_2_nsf.o
del vrc6_test_2.nsf
cc65\bin\ca65 vrc6_test_2.s
cc65\bin\ld65 -C swap_test.cfg -o vrc6_test_2.nes vrc6_test_2.o -m vrc6_test_2_map.txt
cc65\bin\ca65 vrc6_test_2.s -o vrc6_test_2_nsf.o -D NSF
cc65\bin\ld65 -C swap_test_nsf.cfg -o vrc6_test_2.nsf vrc6_test_2_nsf.o -m vrc6_test_2_map_nsf.txt
@pause