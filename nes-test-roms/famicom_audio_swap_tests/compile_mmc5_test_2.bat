@echo Compiling mmc5_test_2.s into mmc5_test_2.nes...
del mmc5_test_2.o
del mmc5_test_2.nes
del mmc5_test_2_nsf.o
del mmc5_test_2.nsf
cc65\bin\ca65 mmc5_test_2.s
cc65\bin\ld65 -C swap_test.cfg -o mmc5_test_2.nes mmc5_test_2.o -m mmc5_test_2_map.txt
cc65\bin\ca65 mmc5_test_2.s -o mmc5_test_2_nsf.o -D NSF
cc65\bin\ld65 -C swap_test_nsf.cfg -o mmc5_test_2.nsf mmc5_test_2_nsf.o -m mmc5_test_2_map_nsf.txt
@pause