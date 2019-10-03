@echo Compiling bio_test.s into bio_test.nes...
del bio_test.o
del bio_test.nes
del bio_test_nsf.o
del bio_test.nsf
cc65\bin\ca65 bio_test.s
cc65\bin\ld65 -C swap_test.cfg -o bio_test.nes bio_test.o -m bio_test_map.txt
cc65\bin\ca65 bio_test.s -o bio_test_nsf.o -D NSF
cc65\bin\ld65 -C swap_test_nsf.cfg -o bio_test.nsf bio_test_nsf.o -m bio_test_map_nsf.txt
@pause