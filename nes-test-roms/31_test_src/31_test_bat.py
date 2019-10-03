for i in range(0,256):
    print("cc65\\bin\\ca65 31_test.s -D BANK=$%02X -g -o temp\\31_test_%02X.o" % (i,i))
    if (i == 0):
        print("@IF ERRORLEVEL 1 GOTO badbuild")
print()

for i in range(0,256):
    print("cc65\\bin\\ld65 -o temp\\31_test_%02X.bin -C 31_test.cfg temp\\31_test_%02X.o" % (i,i))
    if (i == 0):
        print("@IF ERRORLEVEL 1 GOTO badbuild")
print()

for i in range(0,64):
    a = i * 4
    b = a+1
    c = a+2
    d = a+3
    print("copy /b temp\\31_test_%02X.bin + temp\\31_test_%02X.bin + temp\\31_test_%02X.bin + temp\\31_test_%02X.bin   temp\\31_test_%02X_%02X.bin" % (a,b,c,d,a,d))
print()

for i in range(0,16):
    a = i * 16
    b = a+4
    c = a+8
    d = a+12
    print("copy /b temp\\31_test_%02X_%02X.bin + temp\\31_test_%02X_%02X.bin + temp\\31_test_%02X_%02X.bin + temp\\31_test_%02X_%02X.bin   temp\\31_test_%02X_%02X.bin" % (a,a+3,b,b+3,c,c+3,d,d+3,a,d+3))
print()

for i in range(0,4):
    a = i * 64
    b = a + 16
    c = a + 32
    d = a + 48
    print("copy /b temp\\31_test_%02X_%02X.bin + temp\\31_test_%02X_%02X.bin + temp\\31_test_%02X_%02X.bin + temp\\31_test_%02X_%02X.bin   temp\\31_test_%02X_%02X.bin" % (a,a+15,b,b+15,c,c+15,d,d+15,a,d+15))
