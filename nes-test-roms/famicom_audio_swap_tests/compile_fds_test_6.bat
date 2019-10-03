@echo Compiling fds_test_6.s into fds_test_6.nes...
del fds_test_6.o
del fds_test_6.nes
del fds_test_6_nsf.o
del fds_test_6.nsf
cc65\bin\ca65 fds_test_6.s
cc65\bin\ld65 -C swap_test.cfg -o fds_test_6.nes fds_test_6.o -m fds_test_6_map.txt
cc65\bin\ca65 fds_test_6.s -o fds_test_6_nsf.o -D NSF
cc65\bin\ld65 -C swap_test_nsf.cfg -o fds_test_6.nsf fds_test_6_nsf.o -m fds_test_6_map_nsf.txt
@pause