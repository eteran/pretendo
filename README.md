# Pretendo

Pretendo is a Nintendo Entertainment System (NES) emulator written in C++17 with a Qt6 interface,
developed by Evan Teran and Eli Dayan. This codebase is a rewrite of an earlier version, undertaken
to focus on emulation accuracy ahead of feature count and release cadence.

The CPU, PPU, and APU are each modeled one clock cycle at a time. The PPU holds the master clock and
steps the other two components, so the timing relationship between them follows the hardware instead
of a per-component schedule. CPU instructions are built as a cross product of opcode definitions and
addressing modes, and each addressing mode spells out the bus traffic of every cycle it takes,
including the dummy reads and writes that real hardware performs.

There are no prebuilt binaries. Build from source with the steps in [Build](#build).

## Features

- Per-cycle emulation of the 2A03 CPU, the 2C02 PPU, and the APU.
- 63 iNES mappers, including MMC1 through MMC5 and the Konami VRC family.
- iNES 1.0 and NES 2.0 ROM images, read through the
  [libines2](https://github.com/eteran/libines2) library. Gzip-compressed `.nes.gz` images work when
  zlib is available at build time.
- Battery-backed cartridge SRAM, saved automatically.
- Integer window scaling at 1x, 2x, 3x, and 4x.
- NTSC palette synthesis with adjustable hue, saturation, contrast, brightness, and gamma, applied
  across all eight color-emphasis settings.
- A pattern table viewer and an oscilloscope view of the mixed APU output.
- PNG screenshots at the native 256x240 resolution.
- Optional audio output through Qt Multimedia.

### Supported mappers

The name in the second column is what Pretendo reports on the console when it loads a cartridge.

<details>
<summary>63 supported iNES mappers</summary>

| Mapper | Reported name |
| ---: | --- |
| 0 | NROM |
| 1 | Nintendo MMC1 |
| 2 | UxROM |
| 3 | CNROM |
| 4 | MMC3A, MMC3B, or MMC6, detected at load time |
| 5 | Nintendo MMC5 |
| 7 | AxROM |
| 9 | PxROM (Nintendo MMC2) |
| 10 | FxROM (Nintendo MMC4) |
| 11 | Color Dreams |
| 13 | NES-CPROM |
| 15 | 100-in-1 |
| 16 | Bandai FCG |
| 18 | JALECO SS88006 |
| 19 | Namcot 106 |
| 21 | VRC4a |
| 22 | VRC2a |
| 23 | VRC2b |
| 24 | VRC6a |
| 25 | VRC2c |
| 26 | VRC6b |
| 27 | VRC4a |
| 32 | Irem G-101 |
| 33 | Taito TC019 |
| 34 | NINA-001 or BNROM, detected at load time |
| 40 | SMB2j Pirate |
| 41 | Caltron 6-in-1 |
| 43 | X-in-1 |
| 46 | Rumblestation 15-in-1 |
| 48 | iNES 48 (Taito) |
| 50 | SMB2j Pirate |
| 57 | Mapper 57 (Pirate) |
| 58 | iNES 58 |
| 61 | iNES 61 |
| 64 | Tengen RAMBO-1 |
| 66 | GxROM |
| 68 | Sunsoft Mapper #4 |
| 69 | Sunsoft 5B/FME-7 |
| 71 | Camerica/Codemasters |
| 75 | VRC1 |
| 76 | iNES 76 |
| 79 | NINA-003/NINA-006 |
| 86 | JALECO-JF-13 |
| 88 | iNES 88 |
| 90 | iNES 90 |
| 113 | iNES 113 |
| 118 | TxSROM |
| 144 | Color Dreams (50282) |
| 159 | Bandai FCG |
| 163 | iNES 163 |
| 180 | UNROM/Crazy Climber |
| 201 | iNES 201 |
| 206 | Tengen MIMIC-1/Namcot 118 |
| 218 | iNES 218 |
| 226 | 76-in-1 |
| 228 | Action 52 |
| 231 | 20-in-1 |
| 232 | Camerica/Codemasters |
| 240 | iNES #240 |
| 241 | iNES 241 |
| 242 | iNES 242 |
| 243 | Sachen |
| 245 | iNES 245 (MMC3 Clone) |

</details>

When a cartridge uses a mapper that Pretendo does not implement, it prints
`unsupported mapper hardware - iNES number: N` and the emulator does not start.

## Requirements

| Dependency | Version | Notes |
| --- | --- | --- |
| CMake | 3.15 or later | |
| C++ compiler | C++17 | GCC receives extra warning flags; `ENABLE_LTO` and `ENABLE_STL_DEBUG` are GCC and libstdc++ specific. |
| Qt6 | 6.5.0 or later | Widgets, OpenGL, and OpenGLWidgets. |
| OpenGL | 2.1 | Requested as a double-buffered context. |
| Qt6 Multimedia | 6.5.0 or later | Required only when you build with `ENABLE_AUDIO=ON`. |
| zlib | any | Optional. Without it, Pretendo cannot read `.nes.gz` images. |

## Build

The `libines2` submodule is required. Configuration fails without it, because `src/Cart.h` includes
`iNES/Rom.h`.

1. Clone the repository with its submodule:

   ```sh
   git clone --recurse-submodules https://github.com/eteran/pretendo.git
   cd pretendo
   ```

   If you already cloned without `--recurse-submodules`, run
   `git submodule update --init --recursive`.

2. Configure the build:

   ```sh
   cmake -S . -B build
   ```

   `CMAKE_BUILD_TYPE` defaults to `RelWithDebInfo`.

3. Compile:

   ```sh
   cmake --build build -j
   ```

The binary is written to `build/pretendo`.

### Build options

Pass any of these to the configure step as `-DOPTION=ON`. All default to off.

| Option | Effect |
| --- | --- |
| `ENABLE_AUDIO` | Build audio output. Replaces `qt/NullAudio.cpp` with `qt/Audio.cpp` and requires Qt6 Multimedia. Without it, Pretendo runs silently. |
| `ENABLE_ASAN` | Build with AddressSanitizer. |
| `ENABLE_USAN` | Build with UndefinedBehaviorSanitizer. |
| `ENABLE_MSAN` | Build with MemorySanitizer. |
| `ENABLE_TSAN` | Build with ThreadSanitizer. |
| `ENABLE_LTO` | Build with link-time optimization. Uses `gcc-ar` and `gcc-ranlib`. |
| `ENABLE_PROFILING` | Build with `gprof` instrumentation. |
| `ENABLE_STL_DEBUG` | Build with libstdc++ container debugging. |

To add a source file, list it in `src/CMakeLists.txt` or `src/mappers/CMakeLists.txt`. The build
does not glob.

## Run

To load and start a cartridge, pass its path:

```sh
./build/pretendo path/to/rom.nes
```

Starting Pretendo without a path opens a browser listing every `.nes` file under `romDirectory`.
Double-click an entry to load and run it. Use the **Filter** box to narrow the list.

`romDirectory` defaults to your home directory and has no control in the interface. To change it,
edit `config.ini` while Pretendo is closed. The file is written on exit, so changes you make while
it is running are overwritten.

Pretendo stores `config.ini` in a `pretendo` subdirectory of the location Qt reports for generic
configuration data:

| Platform | Configuration file |
| --- | --- |
| Linux | `~/.config/pretendo/config.ini` |
| macOS | `~/Library/Preferences/pretendo/config.ini` |
| Windows | `%LOCALAPPDATA%\pretendo\config.ini` |

The same file holds `zoomFactor` (`2` by default) and `showSprites` (`true` by default).

### Command-line options

| Option | Description |
| --- | --- |
| `--fps <NUM>` | Target frame rate. The default is 60. The emulation thread paces itself against this figure. |
| `--test-frames <NUM>` | Run for `NUM` frames, write a screenshot, and exit. |
| `--test-config <FILE>` | Run the screenshot regression suite described by `FILE` and exit. The exit status is 1 if any comparison mismatched. Cannot be combined with `--test-frames`. |
| `--help` | Print usage and exit. |

Give at most one ROM path, and give it last. Pretendo treats any argument that follows it as an
error.

### Controls

Pretendo reads the keyboard for player 1. The bindings are fixed at compile time in
`src/qt/Pretendo.cpp`.

| NES button | Key |
| --- | --- |
| D-pad | Arrow keys |
| A | `X` |
| B | `Z` |
| Select | `A` |
| Start | `S` |

Emulation is controlled from the **CPU** menu: **Run** (`F5`), **Pause** (`Ctrl+P`), **Reset**
(`Ctrl+R`), and **Hard Reset**. **Stop** ends emulation and returns to the ROM browser.

### Cartridge saves

Games with battery-backed SRAM save automatically. Pretendo memory-maps a `.sav` file named after
the ROM's hash into a `pretendo` subdirectory of the location Qt reports for generic cache data, so
there is nothing to trigger by hand.

| Platform | Save directory |
| --- | --- |
| Linux | `~/.cache/pretendo/` |
| macOS | `~/Library/Caches/pretendo/` |
| Windows | `%LOCALAPPDATA%\cache\pretendo\` |

Pretendo does not implement save states.

### View options

The **View** menu holds the window and inspection options: **Zoom** (1x, 2x, 3x, or 4x),
**Show Sprites**, **Take Screenshot**, **Pattern Table Viewer**, and **Audio Viewer**.

Screenshots are written to the working directory as
`pretendo-<rom hash>-<rom name>.png`, always at 256x240 regardless of the zoom setting.

**Preferences** holds five sliders that shape the NTSC palette: hue, saturation, contrast,
brightness, and gamma. Changes take effect as soon as you click **OK**, and **Restore Defaults**
returns all five to their built-in values. Pretendo does not save palette settings between runs.

## Documentation

`doc/` holds the hardware references this emulator is written against: `doc/cpu/`, `doc/ppu/`,
`doc/apu/`, and one file per mapper number under `doc/mappers/`. Consult them before you change
timing behavior.

---

Nintendo Entertainment System and NES are trademarks of Nintendo. Pretendo is not affiliated with or
endorsed by Nintendo.
