
#ifndef SHIFTREGISTER_20080314_H_
#define SHIFTREGISTER_20080314_H_

#include <climits>

template <class T>
class ShiftRegister {
public:
	//-------------------------------------------------------------------
	// Name: ShiftRegister
	//-------------------------------------------------------------------
	ShiftRegister(T value = 0) : data_(value) {
	}

	//-------------------------------------------------------------------
	// Name: load
	//-------------------------------------------------------------------
	void load(T value) {
		data_ = value;
	}

	//-------------------------------------------------------------------
	// Name: read
	//-------------------------------------------------------------------
	T read() {
		const T ret = (data_ & 0x1);
		data_ >>= 1;
		return ret;
	}

private:
	T data_;
};

#endif
