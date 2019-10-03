@echo Compiling fds_test_4.s into fds_test_4.nes...
del fds_test_4.o
del fds_test_4.nes
del fds_test_4_nsf.o
del fds_test_4.nsf
cc65\bin\ca65 fds_test_4.s
cc65\bin\ld65 -C swap_test.cfg -o fds_test_4.nes fds_test_4.o -m fds_test_4_map.txt
cc65\bin\ca65 fds_test_4.s -o fds_test_4_nsf.o -D NSF
cc65\bin\ld65 -C swap_test_nsf.cfg -o fds_test_4.nsf fds_test_4_nsf.o -m fds_test_4_map_nsf.txt
@pause