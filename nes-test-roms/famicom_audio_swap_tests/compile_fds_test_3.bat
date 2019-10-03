@echo Compiling fds_test_3.s into fds_test_3.nes...
del fds_test_3.o
del fds_test_3.nes
del fds_test_3_nsf.o
del fds_test_3.nsf
cc65\bin\ca65 fds_test_3.s
cc65\bin\ld65 -C swap_test.cfg -o fds_test_3.nes fds_test_3.o -m fds_test_3_map.txt
cc65\bin\ca65 fds_test_3.s -o fds_test_3_nsf.o -D NSF
cc65\bin\ld65 -C swap_test_nsf.cfg -o fds_test_3.nsf fds_test_3_nsf.o -m fds_test_3_map_nsf.txt
@pause