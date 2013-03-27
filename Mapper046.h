
#ifndef MAPPER46_460080314_H_
#define MAPPER46_460080314_H_

#include "Mapper.h"

class Mapper46 : public Mapper {
public:
	Mapper46();

public:
	virtual std::string name() const;

public:
	virtual void write_6(uint16_t address, uint8_t value);
	virtual void write_7(uint16_t address, uint8_t value);
	virtual void write_8(uint16_t address, uint8_t value);
	virtual void write_9(uint16_t address, uint8_t value);
	virtual void write_a(uint16_t address, uint8_t value);
	virtual void write_b(uint16_t address, uint8_t value);
	virtual void write_c(uint16_t address, uint8_t value);
	virtual void write_d(uint16_t address, uint8_t value);
	virtual void write_e(uint16_t address, uint8_t value);
	virtual void write_f(uint16_t address, uint8_t value);

private:
	void write67(uint16_t address, uint8_t value);
	void write89ABCDEF(uint16_t address, uint8_t value);

private:
	uint8_t prg_;
	uint8_t chr_;
};

#endif
