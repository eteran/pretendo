@echo Compiling fds_test_2.s into fds_test_2.nes...
del fds_test_2.o
del fds_test_2.nes
del fds_test_2_nsf.o
del fds_test_2.nsf
cc65\bin\ca65 fds_test_2.s
cc65\bin\ld65 -C swap_test.cfg -o fds_test_2.nes fds_test_2.o -m fds_test_2_map.txt
cc65\bin\ca65 fds_test_2.s -o fds_test_2_nsf.o -D NSF
cc65\bin\ld65 -C swap_test_nsf.cfg -o fds_test_2.nsf fds_test_2_nsf.o -m fds_test_2_map_nsf.txt
@pause