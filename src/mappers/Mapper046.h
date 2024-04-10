
#ifndef MAPPER046_460080314_H_
#define MAPPER046_460080314_H_

#include "Mapper.h"

class Mapper46 final : public Mapper {
public:
	Mapper46();

public:
	std::string name() const override;

public:
	void write_6(uint_least16_t address, uint8_t value) override;
	void write_7(uint_least16_t address, uint8_t value) override;
	void write_8(uint_least16_t address, uint8_t value) override;
	void write_9(uint_least16_t address, uint8_t value) override;
	void write_a(uint_least16_t address, uint8_t value) override;
	void write_b(uint_least16_t address, uint8_t value) override;
	void write_c(uint_least16_t address, uint8_t value) override;
	void write_d(uint_least16_t address, uint8_t value) override;
	void write_e(uint_least16_t address, uint8_t value) override;
	void write_f(uint_least16_t address, uint8_t value) override;

private:
	void write67(uint_least16_t address, uint8_t value);
	void write89ABCDEF(uint_least16_t address, uint8_t value);

private:
	uint8_t prg_ = 0;
	uint8_t chr_ = 0;
};

#endif
