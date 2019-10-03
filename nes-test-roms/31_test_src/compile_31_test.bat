md temp
del temp\31_test*.o
del temp\31_test*.nes

cc65\bin\ca65 31_test_header.s -D BANKS=1  -g -o temp\31_test_header_16.o
@IF ERRORLEVEL 1 GOTO badbuild
cc65\bin\ca65 31_test_header.s -D BANKS=2  -g -o temp\31_test_header_32.o
cc65\bin\ca65 31_test_header.s -D BANKS=4  -g -o temp\31_test_header_64.o
cc65\bin\ca65 31_test_header.s -D BANKS=8  -g -o temp\31_test_header_128.o
cc65\bin\ca65 31_test_header.s -D BANKS=16 -g -o temp\31_test_header_256.o
cc65\bin\ca65 31_test_header.s -D BANKS=32 -g -o temp\31_test_header_512.o
cc65\bin\ca65 31_test_header.s -D BANKS=64 -g -o temp\31_test_header_1024.o

cc65\bin\ld65 -o temp\31_test_header_16.bin   -C header.cfg temp\31_test_header_16.o
@IF ERRORLEVEL 1 GOTO badbuild
cc65\bin\ld65 -o temp\31_test_header_32.bin   -C header.cfg temp\31_test_header_32.o
cc65\bin\ld65 -o temp\31_test_header_64.bin   -C header.cfg temp\31_test_header_64.o
cc65\bin\ld65 -o temp\31_test_header_128.bin  -C header.cfg temp\31_test_header_128.o
cc65\bin\ld65 -o temp\31_test_header_256.bin  -C header.cfg temp\31_test_header_256.o
cc65\bin\ld65 -o temp\31_test_header_512.bin  -C header.cfg temp\31_test_header_512.o
cc65\bin\ld65 -o temp\31_test_header_1024.bin -C header.cfg temp\31_test_header_1024.o

REM generated code (see 31_test_bat.py)

cc65\bin\ca65 31_test.s -D BANK=$00 -g -o temp\31_test_00.o
@IF ERRORLEVEL 1 GOTO badbuild
cc65\bin\ca65 31_test.s -D BANK=$01 -g -o temp\31_test_01.o
cc65\bin\ca65 31_test.s -D BANK=$02 -g -o temp\31_test_02.o
cc65\bin\ca65 31_test.s -D BANK=$03 -g -o temp\31_test_03.o
cc65\bin\ca65 31_test.s -D BANK=$04 -g -o temp\31_test_04.o
cc65\bin\ca65 31_test.s -D BANK=$05 -g -o temp\31_test_05.o
cc65\bin\ca65 31_test.s -D BANK=$06 -g -o temp\31_test_06.o
cc65\bin\ca65 31_test.s -D BANK=$07 -g -o temp\31_test_07.o
cc65\bin\ca65 31_test.s -D BANK=$08 -g -o temp\31_test_08.o
cc65\bin\ca65 31_test.s -D BANK=$09 -g -o temp\31_test_09.o
cc65\bin\ca65 31_test.s -D BANK=$0A -g -o temp\31_test_0A.o
cc65\bin\ca65 31_test.s -D BANK=$0B -g -o temp\31_test_0B.o
cc65\bin\ca65 31_test.s -D BANK=$0C -g -o temp\31_test_0C.o
cc65\bin\ca65 31_test.s -D BANK=$0D -g -o temp\31_test_0D.o
cc65\bin\ca65 31_test.s -D BANK=$0E -g -o temp\31_test_0E.o
cc65\bin\ca65 31_test.s -D BANK=$0F -g -o temp\31_test_0F.o
cc65\bin\ca65 31_test.s -D BANK=$10 -g -o temp\31_test_10.o
cc65\bin\ca65 31_test.s -D BANK=$11 -g -o temp\31_test_11.o
cc65\bin\ca65 31_test.s -D BANK=$12 -g -o temp\31_test_12.o
cc65\bin\ca65 31_test.s -D BANK=$13 -g -o temp\31_test_13.o
cc65\bin\ca65 31_test.s -D BANK=$14 -g -o temp\31_test_14.o
cc65\bin\ca65 31_test.s -D BANK=$15 -g -o temp\31_test_15.o
cc65\bin\ca65 31_test.s -D BANK=$16 -g -o temp\31_test_16.o
cc65\bin\ca65 31_test.s -D BANK=$17 -g -o temp\31_test_17.o
cc65\bin\ca65 31_test.s -D BANK=$18 -g -o temp\31_test_18.o
cc65\bin\ca65 31_test.s -D BANK=$19 -g -o temp\31_test_19.o
cc65\bin\ca65 31_test.s -D BANK=$1A -g -o temp\31_test_1A.o
cc65\bin\ca65 31_test.s -D BANK=$1B -g -o temp\31_test_1B.o
cc65\bin\ca65 31_test.s -D BANK=$1C -g -o temp\31_test_1C.o
cc65\bin\ca65 31_test.s -D BANK=$1D -g -o temp\31_test_1D.o
cc65\bin\ca65 31_test.s -D BANK=$1E -g -o temp\31_test_1E.o
cc65\bin\ca65 31_test.s -D BANK=$1F -g -o temp\31_test_1F.o
cc65\bin\ca65 31_test.s -D BANK=$20 -g -o temp\31_test_20.o
cc65\bin\ca65 31_test.s -D BANK=$21 -g -o temp\31_test_21.o
cc65\bin\ca65 31_test.s -D BANK=$22 -g -o temp\31_test_22.o
cc65\bin\ca65 31_test.s -D BANK=$23 -g -o temp\31_test_23.o
cc65\bin\ca65 31_test.s -D BANK=$24 -g -o temp\31_test_24.o
cc65\bin\ca65 31_test.s -D BANK=$25 -g -o temp\31_test_25.o
cc65\bin\ca65 31_test.s -D BANK=$26 -g -o temp\31_test_26.o
cc65\bin\ca65 31_test.s -D BANK=$27 -g -o temp\31_test_27.o
cc65\bin\ca65 31_test.s -D BANK=$28 -g -o temp\31_test_28.o
cc65\bin\ca65 31_test.s -D BANK=$29 -g -o temp\31_test_29.o
cc65\bin\ca65 31_test.s -D BANK=$2A -g -o temp\31_test_2A.o
cc65\bin\ca65 31_test.s -D BANK=$2B -g -o temp\31_test_2B.o
cc65\bin\ca65 31_test.s -D BANK=$2C -g -o temp\31_test_2C.o
cc65\bin\ca65 31_test.s -D BANK=$2D -g -o temp\31_test_2D.o
cc65\bin\ca65 31_test.s -D BANK=$2E -g -o temp\31_test_2E.o
cc65\bin\ca65 31_test.s -D BANK=$2F -g -o temp\31_test_2F.o
cc65\bin\ca65 31_test.s -D BANK=$30 -g -o temp\31_test_30.o
cc65\bin\ca65 31_test.s -D BANK=$31 -g -o temp\31_test_31.o
cc65\bin\ca65 31_test.s -D BANK=$32 -g -o temp\31_test_32.o
cc65\bin\ca65 31_test.s -D BANK=$33 -g -o temp\31_test_33.o
cc65\bin\ca65 31_test.s -D BANK=$34 -g -o temp\31_test_34.o
cc65\bin\ca65 31_test.s -D BANK=$35 -g -o temp\31_test_35.o
cc65\bin\ca65 31_test.s -D BANK=$36 -g -o temp\31_test_36.o
cc65\bin\ca65 31_test.s -D BANK=$37 -g -o temp\31_test_37.o
cc65\bin\ca65 31_test.s -D BANK=$38 -g -o temp\31_test_38.o
cc65\bin\ca65 31_test.s -D BANK=$39 -g -o temp\31_test_39.o
cc65\bin\ca65 31_test.s -D BANK=$3A -g -o temp\31_test_3A.o
cc65\bin\ca65 31_test.s -D BANK=$3B -g -o temp\31_test_3B.o
cc65\bin\ca65 31_test.s -D BANK=$3C -g -o temp\31_test_3C.o
cc65\bin\ca65 31_test.s -D BANK=$3D -g -o temp\31_test_3D.o
cc65\bin\ca65 31_test.s -D BANK=$3E -g -o temp\31_test_3E.o
cc65\bin\ca65 31_test.s -D BANK=$3F -g -o temp\31_test_3F.o
cc65\bin\ca65 31_test.s -D BANK=$40 -g -o temp\31_test_40.o
cc65\bin\ca65 31_test.s -D BANK=$41 -g -o temp\31_test_41.o
cc65\bin\ca65 31_test.s -D BANK=$42 -g -o temp\31_test_42.o
cc65\bin\ca65 31_test.s -D BANK=$43 -g -o temp\31_test_43.o
cc65\bin\ca65 31_test.s -D BANK=$44 -g -o temp\31_test_44.o
cc65\bin\ca65 31_test.s -D BANK=$45 -g -o temp\31_test_45.o
cc65\bin\ca65 31_test.s -D BANK=$46 -g -o temp\31_test_46.o
cc65\bin\ca65 31_test.s -D BANK=$47 -g -o temp\31_test_47.o
cc65\bin\ca65 31_test.s -D BANK=$48 -g -o temp\31_test_48.o
cc65\bin\ca65 31_test.s -D BANK=$49 -g -o temp\31_test_49.o
cc65\bin\ca65 31_test.s -D BANK=$4A -g -o temp\31_test_4A.o
cc65\bin\ca65 31_test.s -D BANK=$4B -g -o temp\31_test_4B.o
cc65\bin\ca65 31_test.s -D BANK=$4C -g -o temp\31_test_4C.o
cc65\bin\ca65 31_test.s -D BANK=$4D -g -o temp\31_test_4D.o
cc65\bin\ca65 31_test.s -D BANK=$4E -g -o temp\31_test_4E.o
cc65\bin\ca65 31_test.s -D BANK=$4F -g -o temp\31_test_4F.o
cc65\bin\ca65 31_test.s -D BANK=$50 -g -o temp\31_test_50.o
cc65\bin\ca65 31_test.s -D BANK=$51 -g -o temp\31_test_51.o
cc65\bin\ca65 31_test.s -D BANK=$52 -g -o temp\31_test_52.o
cc65\bin\ca65 31_test.s -D BANK=$53 -g -o temp\31_test_53.o
cc65\bin\ca65 31_test.s -D BANK=$54 -g -o temp\31_test_54.o
cc65\bin\ca65 31_test.s -D BANK=$55 -g -o temp\31_test_55.o
cc65\bin\ca65 31_test.s -D BANK=$56 -g -o temp\31_test_56.o
cc65\bin\ca65 31_test.s -D BANK=$57 -g -o temp\31_test_57.o
cc65\bin\ca65 31_test.s -D BANK=$58 -g -o temp\31_test_58.o
cc65\bin\ca65 31_test.s -D BANK=$59 -g -o temp\31_test_59.o
cc65\bin\ca65 31_test.s -D BANK=$5A -g -o temp\31_test_5A.o
cc65\bin\ca65 31_test.s -D BANK=$5B -g -o temp\31_test_5B.o
cc65\bin\ca65 31_test.s -D BANK=$5C -g -o temp\31_test_5C.o
cc65\bin\ca65 31_test.s -D BANK=$5D -g -o temp\31_test_5D.o
cc65\bin\ca65 31_test.s -D BANK=$5E -g -o temp\31_test_5E.o
cc65\bin\ca65 31_test.s -D BANK=$5F -g -o temp\31_test_5F.o
cc65\bin\ca65 31_test.s -D BANK=$60 -g -o temp\31_test_60.o
cc65\bin\ca65 31_test.s -D BANK=$61 -g -o temp\31_test_61.o
cc65\bin\ca65 31_test.s -D BANK=$62 -g -o temp\31_test_62.o
cc65\bin\ca65 31_test.s -D BANK=$63 -g -o temp\31_test_63.o
cc65\bin\ca65 31_test.s -D BANK=$64 -g -o temp\31_test_64.o
cc65\bin\ca65 31_test.s -D BANK=$65 -g -o temp\31_test_65.o
cc65\bin\ca65 31_test.s -D BANK=$66 -g -o temp\31_test_66.o
cc65\bin\ca65 31_test.s -D BANK=$67 -g -o temp\31_test_67.o
cc65\bin\ca65 31_test.s -D BANK=$68 -g -o temp\31_test_68.o
cc65\bin\ca65 31_test.s -D BANK=$69 -g -o temp\31_test_69.o
cc65\bin\ca65 31_test.s -D BANK=$6A -g -o temp\31_test_6A.o
cc65\bin\ca65 31_test.s -D BANK=$6B -g -o temp\31_test_6B.o
cc65\bin\ca65 31_test.s -D BANK=$6C -g -o temp\31_test_6C.o
cc65\bin\ca65 31_test.s -D BANK=$6D -g -o temp\31_test_6D.o
cc65\bin\ca65 31_test.s -D BANK=$6E -g -o temp\31_test_6E.o
cc65\bin\ca65 31_test.s -D BANK=$6F -g -o temp\31_test_6F.o
cc65\bin\ca65 31_test.s -D BANK=$70 -g -o temp\31_test_70.o
cc65\bin\ca65 31_test.s -D BANK=$71 -g -o temp\31_test_71.o
cc65\bin\ca65 31_test.s -D BANK=$72 -g -o temp\31_test_72.o
cc65\bin\ca65 31_test.s -D BANK=$73 -g -o temp\31_test_73.o
cc65\bin\ca65 31_test.s -D BANK=$74 -g -o temp\31_test_74.o
cc65\bin\ca65 31_test.s -D BANK=$75 -g -o temp\31_test_75.o
cc65\bin\ca65 31_test.s -D BANK=$76 -g -o temp\31_test_76.o
cc65\bin\ca65 31_test.s -D BANK=$77 -g -o temp\31_test_77.o
cc65\bin\ca65 31_test.s -D BANK=$78 -g -o temp\31_test_78.o
cc65\bin\ca65 31_test.s -D BANK=$79 -g -o temp\31_test_79.o
cc65\bin\ca65 31_test.s -D BANK=$7A -g -o temp\31_test_7A.o
cc65\bin\ca65 31_test.s -D BANK=$7B -g -o temp\31_test_7B.o
cc65\bin\ca65 31_test.s -D BANK=$7C -g -o temp\31_test_7C.o
cc65\bin\ca65 31_test.s -D BANK=$7D -g -o temp\31_test_7D.o
cc65\bin\ca65 31_test.s -D BANK=$7E -g -o temp\31_test_7E.o
cc65\bin\ca65 31_test.s -D BANK=$7F -g -o temp\31_test_7F.o
cc65\bin\ca65 31_test.s -D BANK=$80 -g -o temp\31_test_80.o
cc65\bin\ca65 31_test.s -D BANK=$81 -g -o temp\31_test_81.o
cc65\bin\ca65 31_test.s -D BANK=$82 -g -o temp\31_test_82.o
cc65\bin\ca65 31_test.s -D BANK=$83 -g -o temp\31_test_83.o
cc65\bin\ca65 31_test.s -D BANK=$84 -g -o temp\31_test_84.o
cc65\bin\ca65 31_test.s -D BANK=$85 -g -o temp\31_test_85.o
cc65\bin\ca65 31_test.s -D BANK=$86 -g -o temp\31_test_86.o
cc65\bin\ca65 31_test.s -D BANK=$87 -g -o temp\31_test_87.o
cc65\bin\ca65 31_test.s -D BANK=$88 -g -o temp\31_test_88.o
cc65\bin\ca65 31_test.s -D BANK=$89 -g -o temp\31_test_89.o
cc65\bin\ca65 31_test.s -D BANK=$8A -g -o temp\31_test_8A.o
cc65\bin\ca65 31_test.s -D BANK=$8B -g -o temp\31_test_8B.o
cc65\bin\ca65 31_test.s -D BANK=$8C -g -o temp\31_test_8C.o
cc65\bin\ca65 31_test.s -D BANK=$8D -g -o temp\31_test_8D.o
cc65\bin\ca65 31_test.s -D BANK=$8E -g -o temp\31_test_8E.o
cc65\bin\ca65 31_test.s -D BANK=$8F -g -o temp\31_test_8F.o
cc65\bin\ca65 31_test.s -D BANK=$90 -g -o temp\31_test_90.o
cc65\bin\ca65 31_test.s -D BANK=$91 -g -o temp\31_test_91.o
cc65\bin\ca65 31_test.s -D BANK=$92 -g -o temp\31_test_92.o
cc65\bin\ca65 31_test.s -D BANK=$93 -g -o temp\31_test_93.o
cc65\bin\ca65 31_test.s -D BANK=$94 -g -o temp\31_test_94.o
cc65\bin\ca65 31_test.s -D BANK=$95 -g -o temp\31_test_95.o
cc65\bin\ca65 31_test.s -D BANK=$96 -g -o temp\31_test_96.o
cc65\bin\ca65 31_test.s -D BANK=$97 -g -o temp\31_test_97.o
cc65\bin\ca65 31_test.s -D BANK=$98 -g -o temp\31_test_98.o
cc65\bin\ca65 31_test.s -D BANK=$99 -g -o temp\31_test_99.o
cc65\bin\ca65 31_test.s -D BANK=$9A -g -o temp\31_test_9A.o
cc65\bin\ca65 31_test.s -D BANK=$9B -g -o temp\31_test_9B.o
cc65\bin\ca65 31_test.s -D BANK=$9C -g -o temp\31_test_9C.o
cc65\bin\ca65 31_test.s -D BANK=$9D -g -o temp\31_test_9D.o
cc65\bin\ca65 31_test.s -D BANK=$9E -g -o temp\31_test_9E.o
cc65\bin\ca65 31_test.s -D BANK=$9F -g -o temp\31_test_9F.o
cc65\bin\ca65 31_test.s -D BANK=$A0 -g -o temp\31_test_A0.o
cc65\bin\ca65 31_test.s -D BANK=$A1 -g -o temp\31_test_A1.o
cc65\bin\ca65 31_test.s -D BANK=$A2 -g -o temp\31_test_A2.o
cc65\bin\ca65 31_test.s -D BANK=$A3 -g -o temp\31_test_A3.o
cc65\bin\ca65 31_test.s -D BANK=$A4 -g -o temp\31_test_A4.o
cc65\bin\ca65 31_test.s -D BANK=$A5 -g -o temp\31_test_A5.o
cc65\bin\ca65 31_test.s -D BANK=$A6 -g -o temp\31_test_A6.o
cc65\bin\ca65 31_test.s -D BANK=$A7 -g -o temp\31_test_A7.o
cc65\bin\ca65 31_test.s -D BANK=$A8 -g -o temp\31_test_A8.o
cc65\bin\ca65 31_test.s -D BANK=$A9 -g -o temp\31_test_A9.o
cc65\bin\ca65 31_test.s -D BANK=$AA -g -o temp\31_test_AA.o
cc65\bin\ca65 31_test.s -D BANK=$AB -g -o temp\31_test_AB.o
cc65\bin\ca65 31_test.s -D BANK=$AC -g -o temp\31_test_AC.o
cc65\bin\ca65 31_test.s -D BANK=$AD -g -o temp\31_test_AD.o
cc65\bin\ca65 31_test.s -D BANK=$AE -g -o temp\31_test_AE.o
cc65\bin\ca65 31_test.s -D BANK=$AF -g -o temp\31_test_AF.o
cc65\bin\ca65 31_test.s -D BANK=$B0 -g -o temp\31_test_B0.o
cc65\bin\ca65 31_test.s -D BANK=$B1 -g -o temp\31_test_B1.o
cc65\bin\ca65 31_test.s -D BANK=$B2 -g -o temp\31_test_B2.o
cc65\bin\ca65 31_test.s -D BANK=$B3 -g -o temp\31_test_B3.o
cc65\bin\ca65 31_test.s -D BANK=$B4 -g -o temp\31_test_B4.o
cc65\bin\ca65 31_test.s -D BANK=$B5 -g -o temp\31_test_B5.o
cc65\bin\ca65 31_test.s -D BANK=$B6 -g -o temp\31_test_B6.o
cc65\bin\ca65 31_test.s -D BANK=$B7 -g -o temp\31_test_B7.o
cc65\bin\ca65 31_test.s -D BANK=$B8 -g -o temp\31_test_B8.o
cc65\bin\ca65 31_test.s -D BANK=$B9 -g -o temp\31_test_B9.o
cc65\bin\ca65 31_test.s -D BANK=$BA -g -o temp\31_test_BA.o
cc65\bin\ca65 31_test.s -D BANK=$BB -g -o temp\31_test_BB.o
cc65\bin\ca65 31_test.s -D BANK=$BC -g -o temp\31_test_BC.o
cc65\bin\ca65 31_test.s -D BANK=$BD -g -o temp\31_test_BD.o
cc65\bin\ca65 31_test.s -D BANK=$BE -g -o temp\31_test_BE.o
cc65\bin\ca65 31_test.s -D BANK=$BF -g -o temp\31_test_BF.o
cc65\bin\ca65 31_test.s -D BANK=$C0 -g -o temp\31_test_C0.o
cc65\bin\ca65 31_test.s -D BANK=$C1 -g -o temp\31_test_C1.o
cc65\bin\ca65 31_test.s -D BANK=$C2 -g -o temp\31_test_C2.o
cc65\bin\ca65 31_test.s -D BANK=$C3 -g -o temp\31_test_C3.o
cc65\bin\ca65 31_test.s -D BANK=$C4 -g -o temp\31_test_C4.o
cc65\bin\ca65 31_test.s -D BANK=$C5 -g -o temp\31_test_C5.o
cc65\bin\ca65 31_test.s -D BANK=$C6 -g -o temp\31_test_C6.o
cc65\bin\ca65 31_test.s -D BANK=$C7 -g -o temp\31_test_C7.o
cc65\bin\ca65 31_test.s -D BANK=$C8 -g -o temp\31_test_C8.o
cc65\bin\ca65 31_test.s -D BANK=$C9 -g -o temp\31_test_C9.o
cc65\bin\ca65 31_test.s -D BANK=$CA -g -o temp\31_test_CA.o
cc65\bin\ca65 31_test.s -D BANK=$CB -g -o temp\31_test_CB.o
cc65\bin\ca65 31_test.s -D BANK=$CC -g -o temp\31_test_CC.o
cc65\bin\ca65 31_test.s -D BANK=$CD -g -o temp\31_test_CD.o
cc65\bin\ca65 31_test.s -D BANK=$CE -g -o temp\31_test_CE.o
cc65\bin\ca65 31_test.s -D BANK=$CF -g -o temp\31_test_CF.o
cc65\bin\ca65 31_test.s -D BANK=$D0 -g -o temp\31_test_D0.o
cc65\bin\ca65 31_test.s -D BANK=$D1 -g -o temp\31_test_D1.o
cc65\bin\ca65 31_test.s -D BANK=$D2 -g -o temp\31_test_D2.o
cc65\bin\ca65 31_test.s -D BANK=$D3 -g -o temp\31_test_D3.o
cc65\bin\ca65 31_test.s -D BANK=$D4 -g -o temp\31_test_D4.o
cc65\bin\ca65 31_test.s -D BANK=$D5 -g -o temp\31_test_D5.o
cc65\bin\ca65 31_test.s -D BANK=$D6 -g -o temp\31_test_D6.o
cc65\bin\ca65 31_test.s -D BANK=$D7 -g -o temp\31_test_D7.o
cc65\bin\ca65 31_test.s -D BANK=$D8 -g -o temp\31_test_D8.o
cc65\bin\ca65 31_test.s -D BANK=$D9 -g -o temp\31_test_D9.o
cc65\bin\ca65 31_test.s -D BANK=$DA -g -o temp\31_test_DA.o
cc65\bin\ca65 31_test.s -D BANK=$DB -g -o temp\31_test_DB.o
cc65\bin\ca65 31_test.s -D BANK=$DC -g -o temp\31_test_DC.o
cc65\bin\ca65 31_test.s -D BANK=$DD -g -o temp\31_test_DD.o
cc65\bin\ca65 31_test.s -D BANK=$DE -g -o temp\31_test_DE.o
cc65\bin\ca65 31_test.s -D BANK=$DF -g -o temp\31_test_DF.o
cc65\bin\ca65 31_test.s -D BANK=$E0 -g -o temp\31_test_E0.o
cc65\bin\ca65 31_test.s -D BANK=$E1 -g -o temp\31_test_E1.o
cc65\bin\ca65 31_test.s -D BANK=$E2 -g -o temp\31_test_E2.o
cc65\bin\ca65 31_test.s -D BANK=$E3 -g -o temp\31_test_E3.o
cc65\bin\ca65 31_test.s -D BANK=$E4 -g -o temp\31_test_E4.o
cc65\bin\ca65 31_test.s -D BANK=$E5 -g -o temp\31_test_E5.o
cc65\bin\ca65 31_test.s -D BANK=$E6 -g -o temp\31_test_E6.o
cc65\bin\ca65 31_test.s -D BANK=$E7 -g -o temp\31_test_E7.o
cc65\bin\ca65 31_test.s -D BANK=$E8 -g -o temp\31_test_E8.o
cc65\bin\ca65 31_test.s -D BANK=$E9 -g -o temp\31_test_E9.o
cc65\bin\ca65 31_test.s -D BANK=$EA -g -o temp\31_test_EA.o
cc65\bin\ca65 31_test.s -D BANK=$EB -g -o temp\31_test_EB.o
cc65\bin\ca65 31_test.s -D BANK=$EC -g -o temp\31_test_EC.o
cc65\bin\ca65 31_test.s -D BANK=$ED -g -o temp\31_test_ED.o
cc65\bin\ca65 31_test.s -D BANK=$EE -g -o temp\31_test_EE.o
cc65\bin\ca65 31_test.s -D BANK=$EF -g -o temp\31_test_EF.o
cc65\bin\ca65 31_test.s -D BANK=$F0 -g -o temp\31_test_F0.o
cc65\bin\ca65 31_test.s -D BANK=$F1 -g -o temp\31_test_F1.o
cc65\bin\ca65 31_test.s -D BANK=$F2 -g -o temp\31_test_F2.o
cc65\bin\ca65 31_test.s -D BANK=$F3 -g -o temp\31_test_F3.o
cc65\bin\ca65 31_test.s -D BANK=$F4 -g -o temp\31_test_F4.o
cc65\bin\ca65 31_test.s -D BANK=$F5 -g -o temp\31_test_F5.o
cc65\bin\ca65 31_test.s -D BANK=$F6 -g -o temp\31_test_F6.o
cc65\bin\ca65 31_test.s -D BANK=$F7 -g -o temp\31_test_F7.o
cc65\bin\ca65 31_test.s -D BANK=$F8 -g -o temp\31_test_F8.o
cc65\bin\ca65 31_test.s -D BANK=$F9 -g -o temp\31_test_F9.o
cc65\bin\ca65 31_test.s -D BANK=$FA -g -o temp\31_test_FA.o
cc65\bin\ca65 31_test.s -D BANK=$FB -g -o temp\31_test_FB.o
cc65\bin\ca65 31_test.s -D BANK=$FC -g -o temp\31_test_FC.o
cc65\bin\ca65 31_test.s -D BANK=$FD -g -o temp\31_test_FD.o
cc65\bin\ca65 31_test.s -D BANK=$FE -g -o temp\31_test_FE.o
cc65\bin\ca65 31_test.s -D BANK=$FF -g -o temp\31_test_FF.o

cc65\bin\ld65 -o temp\31_test_00.bin -C 31_test.cfg temp\31_test_00.o
@IF ERRORLEVEL 1 GOTO badbuild
cc65\bin\ld65 -o temp\31_test_01.bin -C 31_test.cfg temp\31_test_01.o
cc65\bin\ld65 -o temp\31_test_02.bin -C 31_test.cfg temp\31_test_02.o
cc65\bin\ld65 -o temp\31_test_03.bin -C 31_test.cfg temp\31_test_03.o
cc65\bin\ld65 -o temp\31_test_04.bin -C 31_test.cfg temp\31_test_04.o
cc65\bin\ld65 -o temp\31_test_05.bin -C 31_test.cfg temp\31_test_05.o
cc65\bin\ld65 -o temp\31_test_06.bin -C 31_test.cfg temp\31_test_06.o
cc65\bin\ld65 -o temp\31_test_07.bin -C 31_test.cfg temp\31_test_07.o
cc65\bin\ld65 -o temp\31_test_08.bin -C 31_test.cfg temp\31_test_08.o
cc65\bin\ld65 -o temp\31_test_09.bin -C 31_test.cfg temp\31_test_09.o
cc65\bin\ld65 -o temp\31_test_0A.bin -C 31_test.cfg temp\31_test_0A.o
cc65\bin\ld65 -o temp\31_test_0B.bin -C 31_test.cfg temp\31_test_0B.o
cc65\bin\ld65 -o temp\31_test_0C.bin -C 31_test.cfg temp\31_test_0C.o
cc65\bin\ld65 -o temp\31_test_0D.bin -C 31_test.cfg temp\31_test_0D.o
cc65\bin\ld65 -o temp\31_test_0E.bin -C 31_test.cfg temp\31_test_0E.o
cc65\bin\ld65 -o temp\31_test_0F.bin -C 31_test.cfg temp\31_test_0F.o
cc65\bin\ld65 -o temp\31_test_10.bin -C 31_test.cfg temp\31_test_10.o
cc65\bin\ld65 -o temp\31_test_11.bin -C 31_test.cfg temp\31_test_11.o
cc65\bin\ld65 -o temp\31_test_12.bin -C 31_test.cfg temp\31_test_12.o
cc65\bin\ld65 -o temp\31_test_13.bin -C 31_test.cfg temp\31_test_13.o
cc65\bin\ld65 -o temp\31_test_14.bin -C 31_test.cfg temp\31_test_14.o
cc65\bin\ld65 -o temp\31_test_15.bin -C 31_test.cfg temp\31_test_15.o
cc65\bin\ld65 -o temp\31_test_16.bin -C 31_test.cfg temp\31_test_16.o
cc65\bin\ld65 -o temp\31_test_17.bin -C 31_test.cfg temp\31_test_17.o
cc65\bin\ld65 -o temp\31_test_18.bin -C 31_test.cfg temp\31_test_18.o
cc65\bin\ld65 -o temp\31_test_19.bin -C 31_test.cfg temp\31_test_19.o
cc65\bin\ld65 -o temp\31_test_1A.bin -C 31_test.cfg temp\31_test_1A.o
cc65\bin\ld65 -o temp\31_test_1B.bin -C 31_test.cfg temp\31_test_1B.o
cc65\bin\ld65 -o temp\31_test_1C.bin -C 31_test.cfg temp\31_test_1C.o
cc65\bin\ld65 -o temp\31_test_1D.bin -C 31_test.cfg temp\31_test_1D.o
cc65\bin\ld65 -o temp\31_test_1E.bin -C 31_test.cfg temp\31_test_1E.o
cc65\bin\ld65 -o temp\31_test_1F.bin -C 31_test.cfg temp\31_test_1F.o
cc65\bin\ld65 -o temp\31_test_20.bin -C 31_test.cfg temp\31_test_20.o
cc65\bin\ld65 -o temp\31_test_21.bin -C 31_test.cfg temp\31_test_21.o
cc65\bin\ld65 -o temp\31_test_22.bin -C 31_test.cfg temp\31_test_22.o
cc65\bin\ld65 -o temp\31_test_23.bin -C 31_test.cfg temp\31_test_23.o
cc65\bin\ld65 -o temp\31_test_24.bin -C 31_test.cfg temp\31_test_24.o
cc65\bin\ld65 -o temp\31_test_25.bin -C 31_test.cfg temp\31_test_25.o
cc65\bin\ld65 -o temp\31_test_26.bin -C 31_test.cfg temp\31_test_26.o
cc65\bin\ld65 -o temp\31_test_27.bin -C 31_test.cfg temp\31_test_27.o
cc65\bin\ld65 -o temp\31_test_28.bin -C 31_test.cfg temp\31_test_28.o
cc65\bin\ld65 -o temp\31_test_29.bin -C 31_test.cfg temp\31_test_29.o
cc65\bin\ld65 -o temp\31_test_2A.bin -C 31_test.cfg temp\31_test_2A.o
cc65\bin\ld65 -o temp\31_test_2B.bin -C 31_test.cfg temp\31_test_2B.o
cc65\bin\ld65 -o temp\31_test_2C.bin -C 31_test.cfg temp\31_test_2C.o
cc65\bin\ld65 -o temp\31_test_2D.bin -C 31_test.cfg temp\31_test_2D.o
cc65\bin\ld65 -o temp\31_test_2E.bin -C 31_test.cfg temp\31_test_2E.o
cc65\bin\ld65 -o temp\31_test_2F.bin -C 31_test.cfg temp\31_test_2F.o
cc65\bin\ld65 -o temp\31_test_30.bin -C 31_test.cfg temp\31_test_30.o
cc65\bin\ld65 -o temp\31_test_31.bin -C 31_test.cfg temp\31_test_31.o
cc65\bin\ld65 -o temp\31_test_32.bin -C 31_test.cfg temp\31_test_32.o
cc65\bin\ld65 -o temp\31_test_33.bin -C 31_test.cfg temp\31_test_33.o
cc65\bin\ld65 -o temp\31_test_34.bin -C 31_test.cfg temp\31_test_34.o
cc65\bin\ld65 -o temp\31_test_35.bin -C 31_test.cfg temp\31_test_35.o
cc65\bin\ld65 -o temp\31_test_36.bin -C 31_test.cfg temp\31_test_36.o
cc65\bin\ld65 -o temp\31_test_37.bin -C 31_test.cfg temp\31_test_37.o
cc65\bin\ld65 -o temp\31_test_38.bin -C 31_test.cfg temp\31_test_38.o
cc65\bin\ld65 -o temp\31_test_39.bin -C 31_test.cfg temp\31_test_39.o
cc65\bin\ld65 -o temp\31_test_3A.bin -C 31_test.cfg temp\31_test_3A.o
cc65\bin\ld65 -o temp\31_test_3B.bin -C 31_test.cfg temp\31_test_3B.o
cc65\bin\ld65 -o temp\31_test_3C.bin -C 31_test.cfg temp\31_test_3C.o
cc65\bin\ld65 -o temp\31_test_3D.bin -C 31_test.cfg temp\31_test_3D.o
cc65\bin\ld65 -o temp\31_test_3E.bin -C 31_test.cfg temp\31_test_3E.o
cc65\bin\ld65 -o temp\31_test_3F.bin -C 31_test.cfg temp\31_test_3F.o
cc65\bin\ld65 -o temp\31_test_40.bin -C 31_test.cfg temp\31_test_40.o
cc65\bin\ld65 -o temp\31_test_41.bin -C 31_test.cfg temp\31_test_41.o
cc65\bin\ld65 -o temp\31_test_42.bin -C 31_test.cfg temp\31_test_42.o
cc65\bin\ld65 -o temp\31_test_43.bin -C 31_test.cfg temp\31_test_43.o
cc65\bin\ld65 -o temp\31_test_44.bin -C 31_test.cfg temp\31_test_44.o
cc65\bin\ld65 -o temp\31_test_45.bin -C 31_test.cfg temp\31_test_45.o
cc65\bin\ld65 -o temp\31_test_46.bin -C 31_test.cfg temp\31_test_46.o
cc65\bin\ld65 -o temp\31_test_47.bin -C 31_test.cfg temp\31_test_47.o
cc65\bin\ld65 -o temp\31_test_48.bin -C 31_test.cfg temp\31_test_48.o
cc65\bin\ld65 -o temp\31_test_49.bin -C 31_test.cfg temp\31_test_49.o
cc65\bin\ld65 -o temp\31_test_4A.bin -C 31_test.cfg temp\31_test_4A.o
cc65\bin\ld65 -o temp\31_test_4B.bin -C 31_test.cfg temp\31_test_4B.o
cc65\bin\ld65 -o temp\31_test_4C.bin -C 31_test.cfg temp\31_test_4C.o
cc65\bin\ld65 -o temp\31_test_4D.bin -C 31_test.cfg temp\31_test_4D.o
cc65\bin\ld65 -o temp\31_test_4E.bin -C 31_test.cfg temp\31_test_4E.o
cc65\bin\ld65 -o temp\31_test_4F.bin -C 31_test.cfg temp\31_test_4F.o
cc65\bin\ld65 -o temp\31_test_50.bin -C 31_test.cfg temp\31_test_50.o
cc65\bin\ld65 -o temp\31_test_51.bin -C 31_test.cfg temp\31_test_51.o
cc65\bin\ld65 -o temp\31_test_52.bin -C 31_test.cfg temp\31_test_52.o
cc65\bin\ld65 -o temp\31_test_53.bin -C 31_test.cfg temp\31_test_53.o
cc65\bin\ld65 -o temp\31_test_54.bin -C 31_test.cfg temp\31_test_54.o
cc65\bin\ld65 -o temp\31_test_55.bin -C 31_test.cfg temp\31_test_55.o
cc65\bin\ld65 -o temp\31_test_56.bin -C 31_test.cfg temp\31_test_56.o
cc65\bin\ld65 -o temp\31_test_57.bin -C 31_test.cfg temp\31_test_57.o
cc65\bin\ld65 -o temp\31_test_58.bin -C 31_test.cfg temp\31_test_58.o
cc65\bin\ld65 -o temp\31_test_59.bin -C 31_test.cfg temp\31_test_59.o
cc65\bin\ld65 -o temp\31_test_5A.bin -C 31_test.cfg temp\31_test_5A.o
cc65\bin\ld65 -o temp\31_test_5B.bin -C 31_test.cfg temp\31_test_5B.o
cc65\bin\ld65 -o temp\31_test_5C.bin -C 31_test.cfg temp\31_test_5C.o
cc65\bin\ld65 -o temp\31_test_5D.bin -C 31_test.cfg temp\31_test_5D.o
cc65\bin\ld65 -o temp\31_test_5E.bin -C 31_test.cfg temp\31_test_5E.o
cc65\bin\ld65 -o temp\31_test_5F.bin -C 31_test.cfg temp\31_test_5F.o
cc65\bin\ld65 -o temp\31_test_60.bin -C 31_test.cfg temp\31_test_60.o
cc65\bin\ld65 -o temp\31_test_61.bin -C 31_test.cfg temp\31_test_61.o
cc65\bin\ld65 -o temp\31_test_62.bin -C 31_test.cfg temp\31_test_62.o
cc65\bin\ld65 -o temp\31_test_63.bin -C 31_test.cfg temp\31_test_63.o
cc65\bin\ld65 -o temp\31_test_64.bin -C 31_test.cfg temp\31_test_64.o
cc65\bin\ld65 -o temp\31_test_65.bin -C 31_test.cfg temp\31_test_65.o
cc65\bin\ld65 -o temp\31_test_66.bin -C 31_test.cfg temp\31_test_66.o
cc65\bin\ld65 -o temp\31_test_67.bin -C 31_test.cfg temp\31_test_67.o
cc65\bin\ld65 -o temp\31_test_68.bin -C 31_test.cfg temp\31_test_68.o
cc65\bin\ld65 -o temp\31_test_69.bin -C 31_test.cfg temp\31_test_69.o
cc65\bin\ld65 -o temp\31_test_6A.bin -C 31_test.cfg temp\31_test_6A.o
cc65\bin\ld65 -o temp\31_test_6B.bin -C 31_test.cfg temp\31_test_6B.o
cc65\bin\ld65 -o temp\31_test_6C.bin -C 31_test.cfg temp\31_test_6C.o
cc65\bin\ld65 -o temp\31_test_6D.bin -C 31_test.cfg temp\31_test_6D.o
cc65\bin\ld65 -o temp\31_test_6E.bin -C 31_test.cfg temp\31_test_6E.o
cc65\bin\ld65 -o temp\31_test_6F.bin -C 31_test.cfg temp\31_test_6F.o
cc65\bin\ld65 -o temp\31_test_70.bin -C 31_test.cfg temp\31_test_70.o
cc65\bin\ld65 -o temp\31_test_71.bin -C 31_test.cfg temp\31_test_71.o
cc65\bin\ld65 -o temp\31_test_72.bin -C 31_test.cfg temp\31_test_72.o
cc65\bin\ld65 -o temp\31_test_73.bin -C 31_test.cfg temp\31_test_73.o
cc65\bin\ld65 -o temp\31_test_74.bin -C 31_test.cfg temp\31_test_74.o
cc65\bin\ld65 -o temp\31_test_75.bin -C 31_test.cfg temp\31_test_75.o
cc65\bin\ld65 -o temp\31_test_76.bin -C 31_test.cfg temp\31_test_76.o
cc65\bin\ld65 -o temp\31_test_77.bin -C 31_test.cfg temp\31_test_77.o
cc65\bin\ld65 -o temp\31_test_78.bin -C 31_test.cfg temp\31_test_78.o
cc65\bin\ld65 -o temp\31_test_79.bin -C 31_test.cfg temp\31_test_79.o
cc65\bin\ld65 -o temp\31_test_7A.bin -C 31_test.cfg temp\31_test_7A.o
cc65\bin\ld65 -o temp\31_test_7B.bin -C 31_test.cfg temp\31_test_7B.o
cc65\bin\ld65 -o temp\31_test_7C.bin -C 31_test.cfg temp\31_test_7C.o
cc65\bin\ld65 -o temp\31_test_7D.bin -C 31_test.cfg temp\31_test_7D.o
cc65\bin\ld65 -o temp\31_test_7E.bin -C 31_test.cfg temp\31_test_7E.o
cc65\bin\ld65 -o temp\31_test_7F.bin -C 31_test.cfg temp\31_test_7F.o
cc65\bin\ld65 -o temp\31_test_80.bin -C 31_test.cfg temp\31_test_80.o
cc65\bin\ld65 -o temp\31_test_81.bin -C 31_test.cfg temp\31_test_81.o
cc65\bin\ld65 -o temp\31_test_82.bin -C 31_test.cfg temp\31_test_82.o
cc65\bin\ld65 -o temp\31_test_83.bin -C 31_test.cfg temp\31_test_83.o
cc65\bin\ld65 -o temp\31_test_84.bin -C 31_test.cfg temp\31_test_84.o
cc65\bin\ld65 -o temp\31_test_85.bin -C 31_test.cfg temp\31_test_85.o
cc65\bin\ld65 -o temp\31_test_86.bin -C 31_test.cfg temp\31_test_86.o
cc65\bin\ld65 -o temp\31_test_87.bin -C 31_test.cfg temp\31_test_87.o
cc65\bin\ld65 -o temp\31_test_88.bin -C 31_test.cfg temp\31_test_88.o
cc65\bin\ld65 -o temp\31_test_89.bin -C 31_test.cfg temp\31_test_89.o
cc65\bin\ld65 -o temp\31_test_8A.bin -C 31_test.cfg temp\31_test_8A.o
cc65\bin\ld65 -o temp\31_test_8B.bin -C 31_test.cfg temp\31_test_8B.o
cc65\bin\ld65 -o temp\31_test_8C.bin -C 31_test.cfg temp\31_test_8C.o
cc65\bin\ld65 -o temp\31_test_8D.bin -C 31_test.cfg temp\31_test_8D.o
cc65\bin\ld65 -o temp\31_test_8E.bin -C 31_test.cfg temp\31_test_8E.o
cc65\bin\ld65 -o temp\31_test_8F.bin -C 31_test.cfg temp\31_test_8F.o
cc65\bin\ld65 -o temp\31_test_90.bin -C 31_test.cfg temp\31_test_90.o
cc65\bin\ld65 -o temp\31_test_91.bin -C 31_test.cfg temp\31_test_91.o
cc65\bin\ld65 -o temp\31_test_92.bin -C 31_test.cfg temp\31_test_92.o
cc65\bin\ld65 -o temp\31_test_93.bin -C 31_test.cfg temp\31_test_93.o
cc65\bin\ld65 -o temp\31_test_94.bin -C 31_test.cfg temp\31_test_94.o
cc65\bin\ld65 -o temp\31_test_95.bin -C 31_test.cfg temp\31_test_95.o
cc65\bin\ld65 -o temp\31_test_96.bin -C 31_test.cfg temp\31_test_96.o
cc65\bin\ld65 -o temp\31_test_97.bin -C 31_test.cfg temp\31_test_97.o
cc65\bin\ld65 -o temp\31_test_98.bin -C 31_test.cfg temp\31_test_98.o
cc65\bin\ld65 -o temp\31_test_99.bin -C 31_test.cfg temp\31_test_99.o
cc65\bin\ld65 -o temp\31_test_9A.bin -C 31_test.cfg temp\31_test_9A.o
cc65\bin\ld65 -o temp\31_test_9B.bin -C 31_test.cfg temp\31_test_9B.o
cc65\bin\ld65 -o temp\31_test_9C.bin -C 31_test.cfg temp\31_test_9C.o
cc65\bin\ld65 -o temp\31_test_9D.bin -C 31_test.cfg temp\31_test_9D.o
cc65\bin\ld65 -o temp\31_test_9E.bin -C 31_test.cfg temp\31_test_9E.o
cc65\bin\ld65 -o temp\31_test_9F.bin -C 31_test.cfg temp\31_test_9F.o
cc65\bin\ld65 -o temp\31_test_A0.bin -C 31_test.cfg temp\31_test_A0.o
cc65\bin\ld65 -o temp\31_test_A1.bin -C 31_test.cfg temp\31_test_A1.o
cc65\bin\ld65 -o temp\31_test_A2.bin -C 31_test.cfg temp\31_test_A2.o
cc65\bin\ld65 -o temp\31_test_A3.bin -C 31_test.cfg temp\31_test_A3.o
cc65\bin\ld65 -o temp\31_test_A4.bin -C 31_test.cfg temp\31_test_A4.o
cc65\bin\ld65 -o temp\31_test_A5.bin -C 31_test.cfg temp\31_test_A5.o
cc65\bin\ld65 -o temp\31_test_A6.bin -C 31_test.cfg temp\31_test_A6.o
cc65\bin\ld65 -o temp\31_test_A7.bin -C 31_test.cfg temp\31_test_A7.o
cc65\bin\ld65 -o temp\31_test_A8.bin -C 31_test.cfg temp\31_test_A8.o
cc65\bin\ld65 -o temp\31_test_A9.bin -C 31_test.cfg temp\31_test_A9.o
cc65\bin\ld65 -o temp\31_test_AA.bin -C 31_test.cfg temp\31_test_AA.o
cc65\bin\ld65 -o temp\31_test_AB.bin -C 31_test.cfg temp\31_test_AB.o
cc65\bin\ld65 -o temp\31_test_AC.bin -C 31_test.cfg temp\31_test_AC.o
cc65\bin\ld65 -o temp\31_test_AD.bin -C 31_test.cfg temp\31_test_AD.o
cc65\bin\ld65 -o temp\31_test_AE.bin -C 31_test.cfg temp\31_test_AE.o
cc65\bin\ld65 -o temp\31_test_AF.bin -C 31_test.cfg temp\31_test_AF.o
cc65\bin\ld65 -o temp\31_test_B0.bin -C 31_test.cfg temp\31_test_B0.o
cc65\bin\ld65 -o temp\31_test_B1.bin -C 31_test.cfg temp\31_test_B1.o
cc65\bin\ld65 -o temp\31_test_B2.bin -C 31_test.cfg temp\31_test_B2.o
cc65\bin\ld65 -o temp\31_test_B3.bin -C 31_test.cfg temp\31_test_B3.o
cc65\bin\ld65 -o temp\31_test_B4.bin -C 31_test.cfg temp\31_test_B4.o
cc65\bin\ld65 -o temp\31_test_B5.bin -C 31_test.cfg temp\31_test_B5.o
cc65\bin\ld65 -o temp\31_test_B6.bin -C 31_test.cfg temp\31_test_B6.o
cc65\bin\ld65 -o temp\31_test_B7.bin -C 31_test.cfg temp\31_test_B7.o
cc65\bin\ld65 -o temp\31_test_B8.bin -C 31_test.cfg temp\31_test_B8.o
cc65\bin\ld65 -o temp\31_test_B9.bin -C 31_test.cfg temp\31_test_B9.o
cc65\bin\ld65 -o temp\31_test_BA.bin -C 31_test.cfg temp\31_test_BA.o
cc65\bin\ld65 -o temp\31_test_BB.bin -C 31_test.cfg temp\31_test_BB.o
cc65\bin\ld65 -o temp\31_test_BC.bin -C 31_test.cfg temp\31_test_BC.o
cc65\bin\ld65 -o temp\31_test_BD.bin -C 31_test.cfg temp\31_test_BD.o
cc65\bin\ld65 -o temp\31_test_BE.bin -C 31_test.cfg temp\31_test_BE.o
cc65\bin\ld65 -o temp\31_test_BF.bin -C 31_test.cfg temp\31_test_BF.o
cc65\bin\ld65 -o temp\31_test_C0.bin -C 31_test.cfg temp\31_test_C0.o
cc65\bin\ld65 -o temp\31_test_C1.bin -C 31_test.cfg temp\31_test_C1.o
cc65\bin\ld65 -o temp\31_test_C2.bin -C 31_test.cfg temp\31_test_C2.o
cc65\bin\ld65 -o temp\31_test_C3.bin -C 31_test.cfg temp\31_test_C3.o
cc65\bin\ld65 -o temp\31_test_C4.bin -C 31_test.cfg temp\31_test_C4.o
cc65\bin\ld65 -o temp\31_test_C5.bin -C 31_test.cfg temp\31_test_C5.o
cc65\bin\ld65 -o temp\31_test_C6.bin -C 31_test.cfg temp\31_test_C6.o
cc65\bin\ld65 -o temp\31_test_C7.bin -C 31_test.cfg temp\31_test_C7.o
cc65\bin\ld65 -o temp\31_test_C8.bin -C 31_test.cfg temp\31_test_C8.o
cc65\bin\ld65 -o temp\31_test_C9.bin -C 31_test.cfg temp\31_test_C9.o
cc65\bin\ld65 -o temp\31_test_CA.bin -C 31_test.cfg temp\31_test_CA.o
cc65\bin\ld65 -o temp\31_test_CB.bin -C 31_test.cfg temp\31_test_CB.o
cc65\bin\ld65 -o temp\31_test_CC.bin -C 31_test.cfg temp\31_test_CC.o
cc65\bin\ld65 -o temp\31_test_CD.bin -C 31_test.cfg temp\31_test_CD.o
cc65\bin\ld65 -o temp\31_test_CE.bin -C 31_test.cfg temp\31_test_CE.o
cc65\bin\ld65 -o temp\31_test_CF.bin -C 31_test.cfg temp\31_test_CF.o
cc65\bin\ld65 -o temp\31_test_D0.bin -C 31_test.cfg temp\31_test_D0.o
cc65\bin\ld65 -o temp\31_test_D1.bin -C 31_test.cfg temp\31_test_D1.o
cc65\bin\ld65 -o temp\31_test_D2.bin -C 31_test.cfg temp\31_test_D2.o
cc65\bin\ld65 -o temp\31_test_D3.bin -C 31_test.cfg temp\31_test_D3.o
cc65\bin\ld65 -o temp\31_test_D4.bin -C 31_test.cfg temp\31_test_D4.o
cc65\bin\ld65 -o temp\31_test_D5.bin -C 31_test.cfg temp\31_test_D5.o
cc65\bin\ld65 -o temp\31_test_D6.bin -C 31_test.cfg temp\31_test_D6.o
cc65\bin\ld65 -o temp\31_test_D7.bin -C 31_test.cfg temp\31_test_D7.o
cc65\bin\ld65 -o temp\31_test_D8.bin -C 31_test.cfg temp\31_test_D8.o
cc65\bin\ld65 -o temp\31_test_D9.bin -C 31_test.cfg temp\31_test_D9.o
cc65\bin\ld65 -o temp\31_test_DA.bin -C 31_test.cfg temp\31_test_DA.o
cc65\bin\ld65 -o temp\31_test_DB.bin -C 31_test.cfg temp\31_test_DB.o
cc65\bin\ld65 -o temp\31_test_DC.bin -C 31_test.cfg temp\31_test_DC.o
cc65\bin\ld65 -o temp\31_test_DD.bin -C 31_test.cfg temp\31_test_DD.o
cc65\bin\ld65 -o temp\31_test_DE.bin -C 31_test.cfg temp\31_test_DE.o
cc65\bin\ld65 -o temp\31_test_DF.bin -C 31_test.cfg temp\31_test_DF.o
cc65\bin\ld65 -o temp\31_test_E0.bin -C 31_test.cfg temp\31_test_E0.o
cc65\bin\ld65 -o temp\31_test_E1.bin -C 31_test.cfg temp\31_test_E1.o
cc65\bin\ld65 -o temp\31_test_E2.bin -C 31_test.cfg temp\31_test_E2.o
cc65\bin\ld65 -o temp\31_test_E3.bin -C 31_test.cfg temp\31_test_E3.o
cc65\bin\ld65 -o temp\31_test_E4.bin -C 31_test.cfg temp\31_test_E4.o
cc65\bin\ld65 -o temp\31_test_E5.bin -C 31_test.cfg temp\31_test_E5.o
cc65\bin\ld65 -o temp\31_test_E6.bin -C 31_test.cfg temp\31_test_E6.o
cc65\bin\ld65 -o temp\31_test_E7.bin -C 31_test.cfg temp\31_test_E7.o
cc65\bin\ld65 -o temp\31_test_E8.bin -C 31_test.cfg temp\31_test_E8.o
cc65\bin\ld65 -o temp\31_test_E9.bin -C 31_test.cfg temp\31_test_E9.o
cc65\bin\ld65 -o temp\31_test_EA.bin -C 31_test.cfg temp\31_test_EA.o
cc65\bin\ld65 -o temp\31_test_EB.bin -C 31_test.cfg temp\31_test_EB.o
cc65\bin\ld65 -o temp\31_test_EC.bin -C 31_test.cfg temp\31_test_EC.o
cc65\bin\ld65 -o temp\31_test_ED.bin -C 31_test.cfg temp\31_test_ED.o
cc65\bin\ld65 -o temp\31_test_EE.bin -C 31_test.cfg temp\31_test_EE.o
cc65\bin\ld65 -o temp\31_test_EF.bin -C 31_test.cfg temp\31_test_EF.o
cc65\bin\ld65 -o temp\31_test_F0.bin -C 31_test.cfg temp\31_test_F0.o
cc65\bin\ld65 -o temp\31_test_F1.bin -C 31_test.cfg temp\31_test_F1.o
cc65\bin\ld65 -o temp\31_test_F2.bin -C 31_test.cfg temp\31_test_F2.o
cc65\bin\ld65 -o temp\31_test_F3.bin -C 31_test.cfg temp\31_test_F3.o
cc65\bin\ld65 -o temp\31_test_F4.bin -C 31_test.cfg temp\31_test_F4.o
cc65\bin\ld65 -o temp\31_test_F5.bin -C 31_test.cfg temp\31_test_F5.o
cc65\bin\ld65 -o temp\31_test_F6.bin -C 31_test.cfg temp\31_test_F6.o
cc65\bin\ld65 -o temp\31_test_F7.bin -C 31_test.cfg temp\31_test_F7.o
cc65\bin\ld65 -o temp\31_test_F8.bin -C 31_test.cfg temp\31_test_F8.o
cc65\bin\ld65 -o temp\31_test_F9.bin -C 31_test.cfg temp\31_test_F9.o
cc65\bin\ld65 -o temp\31_test_FA.bin -C 31_test.cfg temp\31_test_FA.o
cc65\bin\ld65 -o temp\31_test_FB.bin -C 31_test.cfg temp\31_test_FB.o
cc65\bin\ld65 -o temp\31_test_FC.bin -C 31_test.cfg temp\31_test_FC.o
cc65\bin\ld65 -o temp\31_test_FD.bin -C 31_test.cfg temp\31_test_FD.o
cc65\bin\ld65 -o temp\31_test_FE.bin -C 31_test.cfg temp\31_test_FE.o
cc65\bin\ld65 -o temp\31_test_FF.bin -C 31_test.cfg temp\31_test_FF.o

copy /b temp\31_test_00.bin + temp\31_test_01.bin + temp\31_test_02.bin + temp\31_test_03.bin   temp\31_test_00_03.bin
copy /b temp\31_test_04.bin + temp\31_test_05.bin + temp\31_test_06.bin + temp\31_test_07.bin   temp\31_test_04_07.bin
copy /b temp\31_test_08.bin + temp\31_test_09.bin + temp\31_test_0A.bin + temp\31_test_0B.bin   temp\31_test_08_0B.bin
copy /b temp\31_test_0C.bin + temp\31_test_0D.bin + temp\31_test_0E.bin + temp\31_test_0F.bin   temp\31_test_0C_0F.bin
copy /b temp\31_test_10.bin + temp\31_test_11.bin + temp\31_test_12.bin + temp\31_test_13.bin   temp\31_test_10_13.bin
copy /b temp\31_test_14.bin + temp\31_test_15.bin + temp\31_test_16.bin + temp\31_test_17.bin   temp\31_test_14_17.bin
copy /b temp\31_test_18.bin + temp\31_test_19.bin + temp\31_test_1A.bin + temp\31_test_1B.bin   temp\31_test_18_1B.bin
copy /b temp\31_test_1C.bin + temp\31_test_1D.bin + temp\31_test_1E.bin + temp\31_test_1F.bin   temp\31_test_1C_1F.bin
copy /b temp\31_test_20.bin + temp\31_test_21.bin + temp\31_test_22.bin + temp\31_test_23.bin   temp\31_test_20_23.bin
copy /b temp\31_test_24.bin + temp\31_test_25.bin + temp\31_test_26.bin + temp\31_test_27.bin   temp\31_test_24_27.bin
copy /b temp\31_test_28.bin + temp\31_test_29.bin + temp\31_test_2A.bin + temp\31_test_2B.bin   temp\31_test_28_2B.bin
copy /b temp\31_test_2C.bin + temp\31_test_2D.bin + temp\31_test_2E.bin + temp\31_test_2F.bin   temp\31_test_2C_2F.bin
copy /b temp\31_test_30.bin + temp\31_test_31.bin + temp\31_test_32.bin + temp\31_test_33.bin   temp\31_test_30_33.bin
copy /b temp\31_test_34.bin + temp\31_test_35.bin + temp\31_test_36.bin + temp\31_test_37.bin   temp\31_test_34_37.bin
copy /b temp\31_test_38.bin + temp\31_test_39.bin + temp\31_test_3A.bin + temp\31_test_3B.bin   temp\31_test_38_3B.bin
copy /b temp\31_test_3C.bin + temp\31_test_3D.bin + temp\31_test_3E.bin + temp\31_test_3F.bin   temp\31_test_3C_3F.bin
copy /b temp\31_test_40.bin + temp\31_test_41.bin + temp\31_test_42.bin + temp\31_test_43.bin   temp\31_test_40_43.bin
copy /b temp\31_test_44.bin + temp\31_test_45.bin + temp\31_test_46.bin + temp\31_test_47.bin   temp\31_test_44_47.bin
copy /b temp\31_test_48.bin + temp\31_test_49.bin + temp\31_test_4A.bin + temp\31_test_4B.bin   temp\31_test_48_4B.bin
copy /b temp\31_test_4C.bin + temp\31_test_4D.bin + temp\31_test_4E.bin + temp\31_test_4F.bin   temp\31_test_4C_4F.bin
copy /b temp\31_test_50.bin + temp\31_test_51.bin + temp\31_test_52.bin + temp\31_test_53.bin   temp\31_test_50_53.bin
copy /b temp\31_test_54.bin + temp\31_test_55.bin + temp\31_test_56.bin + temp\31_test_57.bin   temp\31_test_54_57.bin
copy /b temp\31_test_58.bin + temp\31_test_59.bin + temp\31_test_5A.bin + temp\31_test_5B.bin   temp\31_test_58_5B.bin
copy /b temp\31_test_5C.bin + temp\31_test_5D.bin + temp\31_test_5E.bin + temp\31_test_5F.bin   temp\31_test_5C_5F.bin
copy /b temp\31_test_60.bin + temp\31_test_61.bin + temp\31_test_62.bin + temp\31_test_63.bin   temp\31_test_60_63.bin
copy /b temp\31_test_64.bin + temp\31_test_65.bin + temp\31_test_66.bin + temp\31_test_67.bin   temp\31_test_64_67.bin
copy /b temp\31_test_68.bin + temp\31_test_69.bin + temp\31_test_6A.bin + temp\31_test_6B.bin   temp\31_test_68_6B.bin
copy /b temp\31_test_6C.bin + temp\31_test_6D.bin + temp\31_test_6E.bin + temp\31_test_6F.bin   temp\31_test_6C_6F.bin
copy /b temp\31_test_70.bin + temp\31_test_71.bin + temp\31_test_72.bin + temp\31_test_73.bin   temp\31_test_70_73.bin
copy /b temp\31_test_74.bin + temp\31_test_75.bin + temp\31_test_76.bin + temp\31_test_77.bin   temp\31_test_74_77.bin
copy /b temp\31_test_78.bin + temp\31_test_79.bin + temp\31_test_7A.bin + temp\31_test_7B.bin   temp\31_test_78_7B.bin
copy /b temp\31_test_7C.bin + temp\31_test_7D.bin + temp\31_test_7E.bin + temp\31_test_7F.bin   temp\31_test_7C_7F.bin
copy /b temp\31_test_80.bin + temp\31_test_81.bin + temp\31_test_82.bin + temp\31_test_83.bin   temp\31_test_80_83.bin
copy /b temp\31_test_84.bin + temp\31_test_85.bin + temp\31_test_86.bin + temp\31_test_87.bin   temp\31_test_84_87.bin
copy /b temp\31_test_88.bin + temp\31_test_89.bin + temp\31_test_8A.bin + temp\31_test_8B.bin   temp\31_test_88_8B.bin
copy /b temp\31_test_8C.bin + temp\31_test_8D.bin + temp\31_test_8E.bin + temp\31_test_8F.bin   temp\31_test_8C_8F.bin
copy /b temp\31_test_90.bin + temp\31_test_91.bin + temp\31_test_92.bin + temp\31_test_93.bin   temp\31_test_90_93.bin
copy /b temp\31_test_94.bin + temp\31_test_95.bin + temp\31_test_96.bin + temp\31_test_97.bin   temp\31_test_94_97.bin
copy /b temp\31_test_98.bin + temp\31_test_99.bin + temp\31_test_9A.bin + temp\31_test_9B.bin   temp\31_test_98_9B.bin
copy /b temp\31_test_9C.bin + temp\31_test_9D.bin + temp\31_test_9E.bin + temp\31_test_9F.bin   temp\31_test_9C_9F.bin
copy /b temp\31_test_A0.bin + temp\31_test_A1.bin + temp\31_test_A2.bin + temp\31_test_A3.bin   temp\31_test_A0_A3.bin
copy /b temp\31_test_A4.bin + temp\31_test_A5.bin + temp\31_test_A6.bin + temp\31_test_A7.bin   temp\31_test_A4_A7.bin
copy /b temp\31_test_A8.bin + temp\31_test_A9.bin + temp\31_test_AA.bin + temp\31_test_AB.bin   temp\31_test_A8_AB.bin
copy /b temp\31_test_AC.bin + temp\31_test_AD.bin + temp\31_test_AE.bin + temp\31_test_AF.bin   temp\31_test_AC_AF.bin
copy /b temp\31_test_B0.bin + temp\31_test_B1.bin + temp\31_test_B2.bin + temp\31_test_B3.bin   temp\31_test_B0_B3.bin
copy /b temp\31_test_B4.bin + temp\31_test_B5.bin + temp\31_test_B6.bin + temp\31_test_B7.bin   temp\31_test_B4_B7.bin
copy /b temp\31_test_B8.bin + temp\31_test_B9.bin + temp\31_test_BA.bin + temp\31_test_BB.bin   temp\31_test_B8_BB.bin
copy /b temp\31_test_BC.bin + temp\31_test_BD.bin + temp\31_test_BE.bin + temp\31_test_BF.bin   temp\31_test_BC_BF.bin
copy /b temp\31_test_C0.bin + temp\31_test_C1.bin + temp\31_test_C2.bin + temp\31_test_C3.bin   temp\31_test_C0_C3.bin
copy /b temp\31_test_C4.bin + temp\31_test_C5.bin + temp\31_test_C6.bin + temp\31_test_C7.bin   temp\31_test_C4_C7.bin
copy /b temp\31_test_C8.bin + temp\31_test_C9.bin + temp\31_test_CA.bin + temp\31_test_CB.bin   temp\31_test_C8_CB.bin
copy /b temp\31_test_CC.bin + temp\31_test_CD.bin + temp\31_test_CE.bin + temp\31_test_CF.bin   temp\31_test_CC_CF.bin
copy /b temp\31_test_D0.bin + temp\31_test_D1.bin + temp\31_test_D2.bin + temp\31_test_D3.bin   temp\31_test_D0_D3.bin
copy /b temp\31_test_D4.bin + temp\31_test_D5.bin + temp\31_test_D6.bin + temp\31_test_D7.bin   temp\31_test_D4_D7.bin
copy /b temp\31_test_D8.bin + temp\31_test_D9.bin + temp\31_test_DA.bin + temp\31_test_DB.bin   temp\31_test_D8_DB.bin
copy /b temp\31_test_DC.bin + temp\31_test_DD.bin + temp\31_test_DE.bin + temp\31_test_DF.bin   temp\31_test_DC_DF.bin
copy /b temp\31_test_E0.bin + temp\31_test_E1.bin + temp\31_test_E2.bin + temp\31_test_E3.bin   temp\31_test_E0_E3.bin
copy /b temp\31_test_E4.bin + temp\31_test_E5.bin + temp\31_test_E6.bin + temp\31_test_E7.bin   temp\31_test_E4_E7.bin
copy /b temp\31_test_E8.bin + temp\31_test_E9.bin + temp\31_test_EA.bin + temp\31_test_EB.bin   temp\31_test_E8_EB.bin
copy /b temp\31_test_EC.bin + temp\31_test_ED.bin + temp\31_test_EE.bin + temp\31_test_EF.bin   temp\31_test_EC_EF.bin
copy /b temp\31_test_F0.bin + temp\31_test_F1.bin + temp\31_test_F2.bin + temp\31_test_F3.bin   temp\31_test_F0_F3.bin
copy /b temp\31_test_F4.bin + temp\31_test_F5.bin + temp\31_test_F6.bin + temp\31_test_F7.bin   temp\31_test_F4_F7.bin
copy /b temp\31_test_F8.bin + temp\31_test_F9.bin + temp\31_test_FA.bin + temp\31_test_FB.bin   temp\31_test_F8_FB.bin
copy /b temp\31_test_FC.bin + temp\31_test_FD.bin + temp\31_test_FE.bin + temp\31_test_FF.bin   temp\31_test_FC_FF.bin

copy /b temp\31_test_00_03.bin + temp\31_test_04_07.bin + temp\31_test_08_0B.bin + temp\31_test_0C_0F.bin   temp\31_test_00_0F.bin
copy /b temp\31_test_10_13.bin + temp\31_test_14_17.bin + temp\31_test_18_1B.bin + temp\31_test_1C_1F.bin   temp\31_test_10_1F.bin
copy /b temp\31_test_20_23.bin + temp\31_test_24_27.bin + temp\31_test_28_2B.bin + temp\31_test_2C_2F.bin   temp\31_test_20_2F.bin
copy /b temp\31_test_30_33.bin + temp\31_test_34_37.bin + temp\31_test_38_3B.bin + temp\31_test_3C_3F.bin   temp\31_test_30_3F.bin
copy /b temp\31_test_40_43.bin + temp\31_test_44_47.bin + temp\31_test_48_4B.bin + temp\31_test_4C_4F.bin   temp\31_test_40_4F.bin
copy /b temp\31_test_50_53.bin + temp\31_test_54_57.bin + temp\31_test_58_5B.bin + temp\31_test_5C_5F.bin   temp\31_test_50_5F.bin
copy /b temp\31_test_60_63.bin + temp\31_test_64_67.bin + temp\31_test_68_6B.bin + temp\31_test_6C_6F.bin   temp\31_test_60_6F.bin
copy /b temp\31_test_70_73.bin + temp\31_test_74_77.bin + temp\31_test_78_7B.bin + temp\31_test_7C_7F.bin   temp\31_test_70_7F.bin
copy /b temp\31_test_80_83.bin + temp\31_test_84_87.bin + temp\31_test_88_8B.bin + temp\31_test_8C_8F.bin   temp\31_test_80_8F.bin
copy /b temp\31_test_90_93.bin + temp\31_test_94_97.bin + temp\31_test_98_9B.bin + temp\31_test_9C_9F.bin   temp\31_test_90_9F.bin
copy /b temp\31_test_A0_A3.bin + temp\31_test_A4_A7.bin + temp\31_test_A8_AB.bin + temp\31_test_AC_AF.bin   temp\31_test_A0_AF.bin
copy /b temp\31_test_B0_B3.bin + temp\31_test_B4_B7.bin + temp\31_test_B8_BB.bin + temp\31_test_BC_BF.bin   temp\31_test_B0_BF.bin
copy /b temp\31_test_C0_C3.bin + temp\31_test_C4_C7.bin + temp\31_test_C8_CB.bin + temp\31_test_CC_CF.bin   temp\31_test_C0_CF.bin
copy /b temp\31_test_D0_D3.bin + temp\31_test_D4_D7.bin + temp\31_test_D8_DB.bin + temp\31_test_DC_DF.bin   temp\31_test_D0_DF.bin
copy /b temp\31_test_E0_E3.bin + temp\31_test_E4_E7.bin + temp\31_test_E8_EB.bin + temp\31_test_EC_EF.bin   temp\31_test_E0_EF.bin
copy /b temp\31_test_F0_F3.bin + temp\31_test_F4_F7.bin + temp\31_test_F8_FB.bin + temp\31_test_FC_FF.bin   temp\31_test_F0_FF.bin

copy /b temp\31_test_00_0F.bin + temp\31_test_10_1F.bin + temp\31_test_20_2F.bin + temp\31_test_30_3F.bin   temp\31_test_00_3F.bin
copy /b temp\31_test_40_4F.bin + temp\31_test_50_5F.bin + temp\31_test_60_6F.bin + temp\31_test_70_7F.bin   temp\31_test_40_7F.bin
copy /b temp\31_test_80_8F.bin + temp\31_test_90_9F.bin + temp\31_test_A0_AF.bin + temp\31_test_B0_BF.bin   temp\31_test_80_BF.bin
copy /b temp\31_test_C0_CF.bin + temp\31_test_D0_DF.bin + temp\31_test_E0_EF.bin + temp\31_test_F0_FF.bin   temp\31_test_C0_FF.bin

REM end of generated code

copy /b temp\31_test_00_03.bin + temp\31_test_04_07.bin   temp\31_test_00_07.bin
copy /b temp\31_test_00_0F.bin + temp\31_test_10_1F.bin   temp\31_test_00_1F.bin
copy /b temp\31_test_00_3F.bin + temp\31_test_40_7F.bin   temp\31_test_00_7F.bin
copy /b temp\31_test_00_3F.bin + temp\31_test_40_7F.bin + temp\31_test_80_BF.bin + temp\31_test_C0_FF.bin   temp\31_test_00_FF.bin

copy /b temp\31_test_header_16.bin   + temp\31_test_00_03.bin + test.chr + test.chr   temp\31_test_16.nes
copy /b temp\31_test_header_32.bin   + temp\31_test_00_07.bin + test.chr + test.chr   temp\31_test_32.nes
copy /b temp\31_test_header_64.bin   + temp\31_test_00_0F.bin + test.chr + test.chr   temp\31_test_64.nes
copy /b temp\31_test_header_128.bin  + temp\31_test_00_1F.bin + test.chr + test.chr   temp\31_test_128.nes
copy /b temp\31_test_header_256.bin  + temp\31_test_00_3F.bin + test.chr + test.chr   temp\31_test_256.nes
copy /b temp\31_test_header_512.bin  + temp\31_test_00_7F.bin + test.chr + test.chr   temp\31_test_512.nes
copy /b temp\31_test_header_1024.bin + temp\31_test_00_FF.bin + test.chr + test.chr   temp\31_test_1024.nes

@echo.
@echo.
@echo Build complete and successful!
@IF NOT "%1"=="" GOTO endbuild
@pause
@GOTO endbuild

:badbuild
@echo.
@echo.
@echo Build error!
@IF NOT "%1"=="" GOTO endbuild
@pause
:endbuild
