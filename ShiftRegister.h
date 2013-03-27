
#ifndef SHIFTREGISTER_20080314_H_
#define SHIFTREGISTER_20080314_H_

#include <climits>

template <class T>
class ShiftRegister {
private:
	static const T HIGH_BIT = 1 << ((sizeof(T) * CHAR_BIT) - 1);

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

		// apparently reading after it is empty yields a set bit
		data_ = (data_ >> 1) | HIGH_BIT;

		return ret;
	}

	//-------------------------------------------------------------------
	// Name: clear
	//-------------------------------------------------------------------
	void clear() {
		data_ = 0;
	}

	//-------------------------------------------------------------------
	// Name: operator|=
	//-------------------------------------------------------------------
	T operator|=(T value) {
		data_ |= value;
		return data_;
	}

private:
	T data_;
};

#endif
