
#ifndef SPRITE_20080314_H_
#define SPRITE_20080314_H_

#include <boost/cstdint.hpp>

using boost::uint8_t;
using boost::uint16_t;
using boost::uint32_t;
using boost::uint64_t;

class Sprite {
private:
	uint8_t y_coord_;
	uint8_t index_;
	uint8_t attributes_;
	uint8_t x_coord_;

public:
	bool vertical_flip() const   { return (attributes_ & 0x80) != 0; }
	bool horizontal_flip() const { return (attributes_ & 0x40) != 0; }
	bool priority() const        { return (attributes_ & 0x20) != 0; }
	uint8_t color() const        { return (attributes_ & 0x03); }
	uint8_t y_coord() const      { return y_coord_; }
	uint8_t x_coord() const      { return x_coord_; }
	uint8_t index() const        { return index_; }
};

#endif
