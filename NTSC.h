
#ifndef NTSC_20080314_H_
#define NTSC_20080314_H_

// this is simpler and easier to work with, and the rounding we do later with the value
// make it effectivly a rounded up to int anyway
// having it be a struct instead of a namespace makes this usable in a template as well
// which is a nice way to deal with things
struct NTSC {
	static const int VFIELD_FREQUENCY = 60;
};

struct PAL {
	static const int VFIELD_FREQUENCY = 50;
};


#endif
