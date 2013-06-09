
#ifndef SHIFTREGISTER_20080314_H_
#define SHIFTREGISTER_20080314_H_

template <class T>
class ShiftRegister {
public:
	//-------------------------------------------------------------------
	// Name: ShiftRegister
	//-------------------------------------------------------------------
	explicit ShiftRegister(T value = 0) : data_(value) {
	}
	
	//-------------------------------------------------------------------
	// Name: ShiftRegister
	//-------------------------------------------------------------------
	ShiftRegister(const ShiftRegister &other) : data_(other.data_) {
	
	}
	
	//-------------------------------------------------------------------
	// Name: operator=
	//-------------------------------------------------------------------
	ShiftRegister &operator=(const ShiftRegister &rhs) {
		data_ = rhs.data_;
		return *this;
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
