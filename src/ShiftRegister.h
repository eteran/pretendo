
#ifndef SHIFT_REGISTER_20080314_H_
#define SHIFT_REGISTER_20080314_H_

template <class T>
class ShiftRegister {
public:
	//-------------------------------------------------------------------
	// Name: ShiftRegister
	//-------------------------------------------------------------------
	explicit ShiftRegister(T value = 0) : data_(value) {
	}

    ShiftRegister(const ShiftRegister &other)           = default;
    ShiftRegister &operator=(const ShiftRegister &rhs)  = default;

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
		const T ret = value();
		clock();
		return ret;
	}

	//-------------------------------------------------------------------
	// Name: value
	//-------------------------------------------------------------------
	T value() const {
		return (data_ & 0x1);
	}

	//-------------------------------------------------------------------
	// Name: clock
	//-------------------------------------------------------------------
	void clock() {
		data_ >>= 1;
	}

private:
	T data_;
};

#endif
