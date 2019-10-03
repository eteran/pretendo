@echo Compiling n163_vol.s into n163_vol.nes...
del n163_vol.o
del n163_vol.nes
del n163_vol_nsf.o
del n163_vol.nsf
cc65\bin\ca65 n163_vol.s
cc65\bin\ld65 -C swap_test.cfg -o n163_vol.nes n163_vol.o -m n163_vol_map.txt
cc65\bin\ca65 n163_vol.s -o n163_vol_nsf.o -D NSF
cc65\bin\ld65 -C swap_test_nsf.cfg -o n163_vol.nsf n163_vol_nsf.o -m n163_vol_map_nsf.txt
@pause