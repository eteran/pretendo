@echo Compiling fds_test.s into fds_test.nes...
del fds_test.o
del fds_test.nes
del fds_test_nsf.o
del fds_test.nsf
cc65\bin\ca65 fds_test.s
cc65\bin\ld65 -C swap_test.cfg -o fds_test.nes fds_test.o -m fds_test_map.txt
cc65\bin\ca65 fds_test.s -o fds_test_nsf.o -D NSF
cc65\bin\ld65 -C swap_test_nsf.cfg -o fds_test.nsf fds_test_nsf.o -m fds_test_map_nsf.txt
@pause