
#ifndef BITFIELD_H__20140702_H_
#define BITFIELD_H__20140702_H_

#include <cstdint>
#include <cstddef>

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;

namespace {

template <size_t LastBit>
struct bitfield_helper;

template <> struct bitfield_helper<1>  { typedef uint8_t type; };
template <> struct bitfield_helper<2>  { typedef uint8_t type; };
template <> struct bitfield_helper<3>  { typedef uint8_t type; };
template <> struct bitfield_helper<4>  { typedef uint8_t type; };
template <> struct bitfield_helper<5>  { typedef uint8_t type; };
template <> struct bitfield_helper<6>  { typedef uint8_t type; };
template <> struct bitfield_helper<7>  { typedef uint8_t type; };
template <> struct bitfield_helper<8>  { typedef uint8_t type; };

template <> struct bitfield_helper<9>  { typedef uint16_t type; };
template <> struct bitfield_helper<10> { typedef uint16_t type; };
template <> struct bitfield_helper<11> { typedef uint16_t type; };
template <> struct bitfield_helper<12> { typedef uint16_t type; };
template <> struct bitfield_helper<13> { typedef uint16_t type; };
template <> struct bitfield_helper<14> { typedef uint16_t type; };
template <> struct bitfield_helper<15> { typedef uint16_t type; };
template <> struct bitfield_helper<16> { typedef uint16_t type; };

template <> struct bitfield_helper<17> { typedef uint32_t type; };
template <> struct bitfield_helper<18> { typedef uint32_t type; };
template <> struct bitfield_helper<19> { typedef uint32_t type; };
template <> struct bitfield_helper<20> { typedef uint32_t type; };
template <> struct bitfield_helper<21> { typedef uint32_t type; };
template <> struct bitfield_helper<22> { typedef uint32_t type; };
template <> struct bitfield_helper<23> { typedef uint32_t type; };
template <> struct bitfield_helper<24> { typedef uint32_t type; };
template <> struct bitfield_helper<25> { typedef uint32_t type; };
template <> struct bitfield_helper<26> { typedef uint32_t type; };
template <> struct bitfield_helper<27> { typedef uint32_t type; };
template <> struct bitfield_helper<28> { typedef uint32_t type; };
template <> struct bitfield_helper<29> { typedef uint32_t type; };
template <> struct bitfield_helper<30> { typedef uint32_t type; };
template <> struct bitfield_helper<31> { typedef uint32_t type; };
template <> struct bitfield_helper<32> { typedef uint32_t type; };

template <> struct bitfield_helper<33> { typedef uint64_t type; };
template <> struct bitfield_helper<34> { typedef uint64_t type; };
template <> struct bitfield_helper<35> { typedef uint64_t type; };
template <> struct bitfield_helper<36> { typedef uint64_t type; };
template <> struct bitfield_helper<37> { typedef uint64_t type; };
template <> struct bitfield_helper<38> { typedef uint64_t type; };
template <> struct bitfield_helper<39> { typedef uint64_t type; };
template <> struct bitfield_helper<40> { typedef uint64_t type; };
template <> struct bitfield_helper<41> { typedef uint64_t type; };
template <> struct bitfield_helper<42> { typedef uint64_t type; };
template <> struct bitfield_helper<43> { typedef uint64_t type; };
template <> struct bitfield_helper<44> { typedef uint64_t type; };
template <> struct bitfield_helper<45> { typedef uint64_t type; };
template <> struct bitfield_helper<46> { typedef uint64_t type; };
template <> struct bitfield_helper<47> { typedef uint64_t type; };
template <> struct bitfield_helper<48> { typedef uint64_t type; };
template <> struct bitfield_helper<49> { typedef uint64_t type; };
template <> struct bitfield_helper<50> { typedef uint64_t type; };
template <> struct bitfield_helper<51> { typedef uint64_t type; };
template <> struct bitfield_helper<52> { typedef uint64_t type; };
template <> struct bitfield_helper<53> { typedef uint64_t type; };
template <> struct bitfield_helper<54> { typedef uint64_t type; };
template <> struct bitfield_helper<55> { typedef uint64_t type; };
template <> struct bitfield_helper<56> { typedef uint64_t type; };
template <> struct bitfield_helper<57> { typedef uint64_t type; };
template <> struct bitfield_helper<58> { typedef uint64_t type; };
template <> struct bitfield_helper<59> { typedef uint64_t type; };
template <> struct bitfield_helper<60> { typedef uint64_t type; };
template <> struct bitfield_helper<61> { typedef uint64_t type; };
template <> struct bitfield_helper<62> { typedef uint64_t type; };
template <> struct bitfield_helper<63> { typedef uint64_t type; };
template <> struct bitfield_helper<64> { typedef uint64_t type; };

}

template <size_t Index, size_t Bits = 1>
class bitfield {
private:
	enum {
		Mask = (1u << Bits) - 1u
	};

	typedef typename bitfield_helper<Index + Bits>::type T;
public:
	template <class T2>
	bitfield &operator=(T2 value) {
		value_ = (value_ & ~(Mask << Index)) | ((value & Mask) << Index);
		return *this;
	}

	operator T() const             { return (value_ >> Index) & Mask; }
	explicit operator bool() const { return value_ & (Mask << Index); }
	bitfield &operator++()         { return *this = *this + 1; }
	T operator++(int)              { T r = *this; ++*this; return r; }
	bitfield &operator--()         { return *this = *this - 1; }
	T operator--(int)              { T r = *this; ++*this; return r; }

private:
	T value_;
};


template <size_t Index>
class bitfield<Index, 1> {
private:
	enum {
		Bits = 1,
		Mask = 0x01
	};

	typedef typename bitfield_helper<Index + Bits>::type T;


public:
	bitfield &operator=(bool value) {
		value_ = (value_ & ~(Mask << Index)) | (value << Index);
		return *this;
	}

	operator bool() const { return value_ & (Mask << Index); }
	
private:
	T value_;
};

#endif
