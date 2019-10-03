@echo Compiling fds_test_5.s into fds_test_5.nes...
del fds_test_5.o
del fds_test_5.nes
del fds_test_5_nsf.o
del fds_test_5.nsf
cc65\bin\ca65 fds_test_5.s
cc65\bin\ld65 -C swap_test.cfg -o fds_test_5.nes fds_test_5.o -m fds_test_5_map.txt
cc65\bin\ca65 fds_test_5.s -o fds_test_5_nsf.o -D NSF
cc65\bin\ld65 -C swap_test_nsf.cfg -o fds_test_5.nsf fds_test_5_nsf.o -m fds_test_5_map_nsf.txt
@pause