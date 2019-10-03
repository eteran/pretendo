@del fdsirqtests.o
@del fdsirqtests.fds
@del fdsirqtests.bin
@echo.
@echo Compiling...
cc65\bin\ca65 fdsirqtests.s -g -o fdsirqtests.o
@IF ERRORLEVEL 1 GOTO failure
@echo.
@echo Linking...
cc65\bin\ld65 -o fdsirqtests.fds -C fdsirqtests.cfg fdsirqtests.o -m fdsirqtests.map.txt --dbgfile fdsirqtests.dbg
@IF ERRORLEVEL 1 GOTO failure
@echo.
@echo Success!
@pause
@GOTO endbuild
:failure
@echo.
@echo Build error!
@pause
:endbuild
