
#ifndef MAPPER026_20080314_H_
#define MAPPER026_20080314_H_

#include "Mapper.h"

class Mapper26 : public Mapper {
public:
	Mapper26();
	virtual ~Mapper26();

public:
	std::string name() const;

public:
	virtual void write_8(uint16_t address, uint8_t value);
	virtual void write_9(uint16_t address, uint8_t value);
	virtual void write_a(uint16_t address, uint8_t value);
	virtual void write_b(uint16_t address, uint8_t value);
	virtual void write_c(uint16_t address, uint8_t value);
	virtual void write_d(uint16_t address, uint8_t value);
	virtual void write_e(uint16_t address, uint8_t value);
	virtual void write_f(uint16_t address, uint8_t value);

public:
	virtual void cpu_sync();

private:
	Mapper *mapper_;
};

#endif
