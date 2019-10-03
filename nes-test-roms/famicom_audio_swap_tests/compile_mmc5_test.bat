@echo Compiling mmc5_test.s into mmc5_test.nes...
del mmc5_test.o
del mmc5_test.nes
del mmc5_test_nsf.o
del mmc5_test.nsf
cc65\bin\ca65 mmc5_test.s
cc65\bin\ld65 -C swap_test.cfg -o mmc5_test.nes mmc5_test.o -m mmc5_test_map.txt
cc65\bin\ca65 mmc5_test.s -o mmc5_test_nsf.o -D NSF
cc65\bin\ld65 -C swap_test_nsf.cfg -o mmc5_test.nsf mmc5_test_nsf.o -m mmc5_test_map_nsf.txt
@pause