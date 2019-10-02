=================================================================
 NESticle                                 Version x.xx (xx/xx/xx)
 by Bloodlust Software
=================================================================

Email: bldlust@maelstrom.net

Death to the defiler.

Do not send any files without asking first.

Do not ask about SNESticle (or anything else you want emulated).

---------------------
     What it is
---------------------

NESticle is a DOS/Win95 based emulator for the Nintendo
Entertainment System written in C++/Assembly.

It is freeware and it is not to be distributed with game
ROMs and it is not to be modified or sold.

A majority of the NES technical information used to create this
came from Marat Fayzullin's (fms@freeflight.com) great NES.DOC.

Sound information was provided by AJG, Fanwen, Mikael Iushin and Y0SHi.

---------------------
     Revisions
---------------------

Version x.xx:
 - ????

---------------------
    What it does
---------------------

NESticle emulates the basic NES hardware, along with several of
the memory mapping chips used by various games. 

What's emulated:

   -6502 CPU
   -PPU
   -VRAM/VROM
   -Horizontal/Vertical mirroring
   -Scrolling
   -8x8 and 8x16 Sprites
   -Dual joypads
   -iNES Memory mappers fully/partially supported: 
      0,1,2,3,4,5,6,7,8,9,11,15,19,22,23,32
   -Battery backed RAM
   -Sound emulation

Other neat features:

   -Multiple save states
   -Joystick/"GrIP" support
   -Network play (Win95 exe only)
   -"Friendly" GUI
   -VESA/DirectDraw support
   -Fullscreen 256x224 VGA mode (DOS only)
   -Save .pcx snapshots
   -Editable pattern tables (CHR/tile/vrom)
   -iNES single file cart format
   -Wave output loggable
   -Editable palette (I dont have the correct colors)


---------------------
 What it doesn't do
---------------------

Lightgun

Mapper/compatibility problems

Palette changes during frame (W&W, Startropics)


---------------------
  What you need
---------------------

A 486 or Pentium running DOS or Windows 95/NT.

The DOS version requires DOS4GW, available separately.

For the Windows version you MUST HAVE DirectX Version 3.0.
It's available somewhere on Microsoft's labyrinth of a website.
At least 16MB of memory for the Win95 version.

NESticle can take advantage of VESA 2.0 support if it exists.
If it does not exist, NESticle will resort to only the
video modes 320x200, 256x224, 256x240, 256x256.
256x256 has scanlines.
You need about 8MB of memory for the DOS version.

The DOS version supports true Soundblasters ONLY.
The Win95 version utilizes DirectSound for digital output.

---------------------
 How to work it
---------------------

Load up a .NES rom from the File/Load ROM.
Reset the rom from CPU/Reset.

If you're running in a high-res mode (>320x240), the NES
game screen will appear in a window. You can maximize the window
with the "SQUARE" button in the title bar. You close the window with
the X button.

You can toggle the GUI on and off by pressing ESC.

You change the input devices from the Settings/RedefineInput menu(s).
No matter what the settings are, Enter/Tab are always Start/Select in order
to accomodate 2 button joysticks. Enter/Tab only work if the game window
has input focus (click on it) or the gui is disabled.

F5/F7 saves and loads your games. Essentially it stores the entire NES state
in a state file. Pressing the keyboard keys 0-9 (not the numeric keypad ones)
will change the current state slot for subsequent saves/loads:
   0 = "xxxx.STA"        (xxxx is the rom file name)
   1 = "xxxx.ST1"
   2 = "xxxx.ST2"
     ... etc
The default slot whenever a rom is loaded is 0 (ie "xxxx.STA")
With statebackuping on, when saving, the old state file is renamed to 
'xxxx.bak'. You'll have to manually rename the backup state file to
use it again.

You can record movies with CPU/NES Movie/Record. This will create
a NES Movie file (*.nsm) which contains the NES state along with
recorded input.

To edit patterns, View/Pattern tables, then click on a pattern and edit it.
Right clicking on a pattern will change its attribute (palette), to aid
in drawing. If you wish, you can save the changes with File/Write VROM, and the
VROM will be written directly to the .NES file, so beware.


---------------------
  ROMS and stuff
---------------------

NESticle currently uses the iNES single file cart format (*.NES).
The ROMs must be stored in the same dir as the .exe.

Here's a description of a sample cart 'ZELDA' and some possible extensions:

 - ZELDA.NES
    The actual ROM/VROM data preceded by a 16-byte header

 - ZELDA.HDR
    An alternate 16-byte header file. If NESticle encounters this in the dir
    of the ROM, it will use this header instead of the one present in the
    NES file.

 - ZELDA.SAV
    8K file representing the contents of the battery backed memory used
    in some games. This file is automagically saved and loaded as needed.

 - ZELDA.ST?
    It's the 'state' file used by saved games.

 - ZELDA.NSM
    Recorded NES Movie.

 - ZELDA.PAT
    Text file containing ROM patches


---------------------
ROM patching/GameGenie
---------------------

I finally deciphered GameGenie codes with some basic info provided
by Benzene of Digital Emutations (demu@wspice.com). No thanks to Galoob.

NESticle supports GameGenie codes and general patching of the
CPU ROM address space ($8000-$FFFF).

Bring up the ROM patching dialog with F6 or from the CPU menu.
There are two edit fields in the dialog, the Name and the Code.
The Code field is where the actual code goes (eg APZLGG). 
The Name field lets you type in a description of the code (eg Megajump),
if no name is entered then the code string is used as the name.
Once you've filled these in, clicking "Add" (or pressing enter) will 
add it to the list of patches. To activate or deactivate the patch, 
doubleclick on the code name in the listbox or click on "Toggle".
"Remove" deletes the patch completely. "Save" writes all the patches
to the text file <romname>.pat, this file is decribed below.

NESticle accepts either GameGenie codes or raw patches.
GameGenie codes can be either 6 or 8 digits and
use the letters:
    A P Z L G I T Y   E O X U K S V N
Deciphering the GameGenie code is a mess and I won't
go into it here. NESticle internally converts them to a raw format.

The raw patches have two formats that coorespond to both
types of Game Genie codes:

        <$addr>:<$val>          (6 letter GG code)
        <$addr>?<$key>:<$val>   (8 letter GG code)

<$addr> is a 15-bit hex address within $8000-$FFFF of 
          the CPU address space (the rom area).
<$val> is the 8-bit hex value to be patched at that address.
<$key> is an 8-bit hex number that MUST be present at that
          ROM location before the patch will be active.

Examples:
 10A4:10    patches CPU address $90A4 with the hex value $10.
 05A1?5B:FF patches CPU address $85A1 with the hex value $FF if 
             and only if $85A1 currently contains $5B

NESticle can save a .pat file that contains all the patches
created during a game. This file can be modified using a
text editor. Each line of the .pat file looks like this:

          [+]<code> [<name>]

[+]    if a '+' is prefixed to the code, then it will be automatically
        activated when the game is run
<code> is the patch itself (GG 6/8 or raw format).
<name> is the text description of the code, if no name is supplied
        then the code is used as the name

Note: If more than one patch modifies the same address (and have compatible
keys) then only one can be active at once. Activating one will deactivate
the others (ie they are mutually exclusive).



---------------------
   Input Devices
---------------------

NESticle supports analog joysticks, 4-button gamepads,
the Gravis GrIP gameport and keyboard input.

Some general things:
 - There are two input devices, one each for NES controller 1 and 2.
 - Change the input devices with Settings/RedefineInput
 - Enter/Tab will simulate Start/Select on NES controller 1, ONLY when
    the GUI is disabled or the NES game window has input focus (click on it)
 - You can remap the buttons for a joystick input device with the 
    command line option '-remapbut' or from the "Remap buttons" dialog...
    Simply click on the NES button type and then press the joystick button 
    you wish to use. Pressing any key will set the NES button to "None".
 - You can redefine the keys for a Keyboard input device with the 
    Redefine Keys dialog, or with the '-setkey' command line option

Under DOS:
 - Joystick 1 or 2 refer to 2-button analog joysticks only!
 - "Gamepad" refers to a 4-button analog joypad (or joystick)
   The default button mapping for "Gamepad" conforms to that
   for a Gravis Gamepad (ie, 0 2 1 3), but can be changed
 - Due to hardware limits, only one 4-button device can be connected
 - You can interactively calibrate the joystick with the "Calibrate" 
   button. Just center and swirl. The joystick thresholds can be also
   set with the '-joythresh' command line option.
 - In order to enable GrIP support, you MUST copy the "grip.gll" file
    to NESticle's startup directory. NESticle will load the driver
    if it finds it and the GrIP devices in slot 1 or 2 can be used.
 - For the MS Sidewinder to work in digital mode under Win95, you must
    have v1.5 of the MS Sidewinder software and you must press the
    mode button a few times.

Under Win95:
 - Joystick 1 and 2 refer to joystick device 1 and 2 as defined under
    Win95's Control Panel/Joystick and can be calibrated from there
 - Gamepad refers to joystick device 1 (with the default Gravis Gamepad mapping)
 - Under Win95 there is no practical limit to the number of buttons
    per joystick device, and any can be remapped

GrIP support was added via Gravis's SDK, however it doesn't seem to 
work with other devices besides the GrIP Gameport. I dont know why.

---------------------
Command line options
---------------------

DOS & WIN95:
------------

@<filename>     : Parses the file <filename> for command line options

-res <xw> <yw>  : Sets the resolution to xw,yw on startup.
                : Use this if 256x224 doesn't work or you get a
                : "Unable to init DDraw: invalid mode" error

-setinput <num> <type>
                : Sets input device <num> to <type>
                   ex. '-setinput 1 GRAVIS'
-remapbut <device> <b> <a> <sel> <start>
                : Remaps buttons for <device>
                : <device> can be JOY1, JOY2 or GRAVIS
                : <b>,<a>,<sel>,<start> are the joystick button #'s for each NES button
                : A setting of '255' will disable the NES button
                : The old 'swapbut' is now obselete, instead use "-remapbut JOY1 1 0"
-setkey <device> <ul> <u> <ur> <l> <r> <dl> <d> <dr> <b> <a> <sel> <start>
                : Sets the keyboard scancodes for <device>
                : <device> can be KEY1 or KEY2
-setjoythresh <num>  <left> <right> <up> <down>
                : Sets the joystick threshold for analog joystick <num>
                : left,right,up,down define the joystick's "dead-zone"

-vsync <ON/OFF> : Wait for the vertical sync to draw frames (see Performance)
-frameskip <x>  : Sets frameskip (1-8 or "AUTO" for autoskipping)
-hidegui        : Hide GUI on startup
-nomsgpopup     : Disables the last message from popping up on screen

-load <rom>     : Load <rom>
-run <rom>      : Load & Run <rom>
-restore <rom>  : Load & Run & Restore state for <rom>

-sndrate <rate> : Sets the sound sampling rate (8000-44100) default=22050
-sndblksize <s> : Sets the size of the mixing block (in samples) default=256
-nosound        : Bypasses the initialization of any sound hardware
-force8bit      : Forces 8-bit sound output (even if 16-bit exists)

-romdir <dir>   : Sets the startup dir for .NES roms
-savedir <dir>  : Sets the directory for .SAV and .STA files
-pcxdir <dir>   : Sets the directory for saving snapshots
-logdir <dir>   : Sets the directory for logging and debug files
-patchdir <dir> : Sets the directory for .PAT files

-swmode <x>     : Sets the MS Sidewinder polling mode
                : 0=Interrupts disabled, multiple gameport resets  (default)
                : 1=Interrupts disabled, single gameport reset
                : 2=Interrupts enabled, multiple gameport resets
                : 3=Interrupts enabled, single gameport reset

DOS only:
---------
-? or -h        : Display command line help
-disablelfn     : Disables long file name support
-novesa         : Don't use VESA extensions
-linear         : Force linear video mode
-banked         : Force banked video mode

-sb   <port> <irq> <dma8>  : Forces 8-bit soundblaster settings (ex -sb 220 7 1)
-sb16 <port> <irq> <dma16> : Forces 16-bit soundblaster settings (ex -sb16 220 5 5)
      (if soundblaster settings are not supplied, they will
        be extracted from the BLASTER environment variable)

-rdtsc          : Enable the use of the Pentium RDTSC profiling instruction
-fpucopy        : Use the FPU for mem->vidmem copies (pentium only)


"NESticle.ini" and "NEStcl95.ini" are two automatically generated
configuration files which are parsed before the command line
itself. You can manually edit the constants in this file if
you wish, but any additional commands or options added to it
will be overwritten the next time you run NESticle.

The two files "NESticle.cmd" and "NEStcl95.cmd" are user-editable
files for command line options and will be appended to the
ini files when they are parsed.

---------------------
     Contact
---------------------

bldlust@maelstrom.net
http://???

---------------------
  Acknowledgements
---------------------

Marat Fayzullin for his priceless NES.DOC and iNES emulator.

Y0SHi for taking the time to maintain his 'nestech.doc'

AJG, Y0SHi, Marat, Mikael Iushin and FanWen for 
supplying valuable information on NES sound.

Thanks to Robert William Grubbs for writing his document
 on reading the MS Sidewinder digitally in DOS. 

All the drippy folk at dripware software.

Additional info:
 FanWen (yangfanw@ms4.hinet.net)
 Neill Corlett
 Andrew Davie (adavie@comcen.com.au)
 Matthew J Richey (mr6v+@andrew.cmu.edu)
 Benzene (demu@wspice.com)
 Neil Bradley (neil@synthcom.com)
 Jens Christian Restemeier
 Benny Boola Hill
 Barkley, DeceiverX, Loopy, Vectriff, Zophar

Scrotum-art and testing by digger.


---------------------
     Disclaimer
---------------------

NESticle Copyright 1997 Bloodlust Software
Nintendo Entertainment System (NES) is a registered trademark of Nintendo.
Game Genie is a trademark of Galoob.
Neither Bloodlust Software nor the author are affiliated with Nintendo
 nor advocate the piracy of NES games.
NESticle is freeware and can be distributed freely as long as it is not
 modified and ROMs are not packaged with the program.
When you use this software you do so at your own risk. The author is 
 not responsible for any loss or damage resulting from the use or 
 misuse of this software.
If you do not agree with these terms then NESticle is not for you.
