
#ifndef MAPPER016_20080314_H_
#define MAPPER016_20080314_H_

#include "Mapper.h"

class Mapper16 : public Mapper {
public:
	Mapper16();
	virtual ~Mapper16();

public:
	virtual std::string name() const;

public:
	virtual uint8_t read_6(uint16_t address);
	virtual uint8_t read_7(uint16_t address);

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

public:
	virtual void cpu_sync();

private:
	Mapper *mapper_;

};

#endif
