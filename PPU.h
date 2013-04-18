
#ifndef PPU_20080314_H_
#define PPU_20080314_H_

#include "Reset.h"
#include "VRAMBank.h"
#include <boost/cstdint.hpp>
#include <boost/noncopyable.hpp>

using boost::uint8_t;
using boost::uint16_t;
using boost::uint32_t;
using boost::uint64_t;

class Mapper;

struct scanline {
};

struct scanline_vblank : scanline {
	explicit scanline_vblank(int l) : line(l) {
	}
	int line;
};

struct scanline_prerender : scanline {
};

struct scanline_postrender : scanline {
};

struct scanline_render : scanline {
	explicit scanline_render(uint8_t *p) : buffer(p) {
	}
	uint8_t *const buffer;
};

class PPU : public boost::noncopyable {
public:
	class SpriteEntry {
	public:
		uint8_t  pattern[2];
		uint8_t  sprite_bytes[4];
	public:
		bool horizontal_flip() const;
		bool is_background() const;
		bool is_sprite_zero() const;
		bool vertical_flip() const;
		uint8_t index() const;
		uint8_t palette() const;
		uint8_t x() const;
		uint8_t y() const;
	};

public:
	PPU();
	~PPU();

public:
	void reset(nes::RESET reset_type);

public:
	static const uint8_t mirror_single_low  = 0x00;
	static const uint8_t mirror_single_high = 0x55;
	static const uint8_t mirror_vertical    = 0x44;
	static const uint8_t mirror_horizontal  = 0x50;
	static const uint8_t mirror_4screen     = 0xe4;

public:
	void write2000(uint8_t value);
	void write2001(uint8_t value);
	void write2002(uint8_t value);
	void write2003(uint8_t value);
	void write2004(uint8_t value);
	void write2005(uint8_t value);
	void write2006(uint8_t value);
	void write2007(uint8_t value);

public:
	uint8_t read2000();
	uint8_t read2001();
	uint8_t read2002();
	uint8_t read2003();
	uint8_t read2004();
	uint8_t read2005();
	uint8_t read2006();
	uint8_t read2007();

public:
	void sprite_dma(uint8_t value);

public:
	void write_vram(uint16_t address, uint8_t value);
	uint8_t read_vram(uint16_t address);

public:
	uint8_t color_intensity() const;
	void end_frame();
	void execute_scanline(const scanline_vblank &target);
	void execute_scanline(const scanline_prerender &target);
	void execute_scanline(const scanline_postrender &target);
	void execute_scanline(const scanline_render &target);
	void set_mirroring(uint8_t mir);
	void set_vram_bank(uint8_t bank, uint8_t *p, bool writeable);
	void start_frame();
	void unset_vram_bank(uint8_t bank);

	uint64_t cycle_count() const {
		return ppu_cycle_;
	}

private:
	bool background_enabled() const;
	bool nmi_on_vblank() const;
	bool screen_enabled() const;
	bool sprites_enabled() const;
	uint8_t select_blank_pixel() const;
	uint8_t select_pixel(uint8_t index);
	void clock_x();
	void clock_y();
	void enter_vblank();
	void execute_cycle(const scanline_prerender &target);
	void execute_cycle(const scanline_render &target);
	void execute_cycle(const scanline_postrender &target);
	void execute_cycle(const scanline_vblank &target);
	void exit_vblank();
	void open_background_attribute();
	void open_tile_index();
	void read_background_attribute();
	void read_tile_index();
	void render_pixel(uint8_t *dest_buffer);
	void update_shift_registers_idle();
	void update_shift_registers_render();
	void update_x_scroll();
	void update_sprite_registers();
	void update_vram_address();
	void evaluate_sprites_even();
	void evaluate_sprites_odd();
	int clock_cpu();

private:
	template <int Size>
	void evaluate_sprites();

	template <int Size, class Pattern>
	void read_sprite_pattern();

	template <int Size, class Pattern>
	void open_sprite_pattern();

	template <class Pattern>
	void read_background_pattern();

	template <class Pattern>
	void open_background_pattern();

	template <int Size, class Pattern>
	uint16_t sprite_pattern_address(uint8_t index, uint8_t sprite_line) const;

private:
	uint8_t      nametables_[4 * 0x400]; // nametable and attribute table data
									     // 4 nametables, each $03c0 bytes in
									     // size plus 4 corresponding $40 byte
									     // attribute tables
									     // even though the real thing only has 2
									     // tables, we currently simulate 4 for
									     // simplicity
	VRAMBank     vram_banks_[0x10];
	uint8_t      sprite_ram_[0x100];
	SpriteEntry  sprite_data_[8];
	uint8_t      palette_[0x20];
	uint64_t     ppu_cycle_;
	uint64_t     ppu_read_2002_cycle_;
	uint16_t     next_ppu_fetch_address_;
	uint16_t     attribute_queue_[2];
	uint16_t     pattern_queue_[2];
	uint16_t     background_pattern_table_;
	uint16_t     nametable_;	         // loopy's "t"
	uint16_t     sprite_pattern_table_;
	uint16_t     vram_address_;          // loopy's "v"
	uint16_t     hpos_;                  // pixel counter
	uint16_t     vpos_;                  // scanline counter
	uint8_t      next_pattern_[2];
	uint8_t      address_increment_;
	uint8_t      color_intensity_;
	uint8_t      latch_;
	uint8_t      next_attribute_;
	uint8_t      next_tile_index_;
	uint8_t      register_2000_;
	uint8_t      register_2001_;
	uint8_t      register_2007_buffer_;
	uint8_t      sprite_address_;
	uint8_t      sprite_data_index_;
	uint8_t      sprite_size_;
	uint8_t      status_;
	uint8_t      tile_offset_;           // loopy's "x"
	uint8_t      sprite_buffer_;
	bool         background_clipping_;
	bool         background_visible_;
	bool         greyscale_;
	bool         nmi_on_vblank_;
	bool         odd_frame_;
	bool         rendering_;
	bool         sprite_clipping_;
	bool         sprite_init_;
	bool         sprite_zero_found_next_;
	bool         sprite_zero_found_curr_;
	bool         sprites_visible_;
	bool         write_latch_;
	bool         write_block_;

public:
	bool         show_sprites_;
};

#endif
