@echo Compiling fds_test_7.s into fds_test_7.nes...
del fds_test_7.o
del fds_test_7.nes
del fds_test_7_nsf.o
del fds_test_7.nsf
cc65\bin\ca65 fds_test_7.s
cc65\bin\ld65 -C swap_test.cfg -o fds_test_7.nes fds_test_7.o -m fds_test_7_map.txt
cc65\bin\ca65 fds_test_7.s -o fds_test_7_nsf.o -D NSF
cc65\bin\ld65 -C swap_test_nsf.cfg -o fds_test_7.nsf fds_test_7_nsf.o -m fds_test_7_map_nsf.txt
@pause