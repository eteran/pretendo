
#ifndef BITFIELD_H_
#define BITFIELD_H_

#include <cstdint>
#include <cstddef>

using std::uint8_t;
using std::uint16_t;
using std::uint32_t;
using std::uint64_t;

namespace {

template <size_t LastBit>
struct MinimumTypeHelper;

template <> struct MinimumTypeHelper<1>  { typedef uint8_t type; };
template <> struct MinimumTypeHelper<2>  { typedef uint8_t type; };
template <> struct MinimumTypeHelper<3>  { typedef uint8_t type; };
template <> struct MinimumTypeHelper<4>  { typedef uint8_t type; };
template <> struct MinimumTypeHelper<5>  { typedef uint8_t type; };
template <> struct MinimumTypeHelper<6>  { typedef uint8_t type; };
template <> struct MinimumTypeHelper<7>  { typedef uint8_t type; };
template <> struct MinimumTypeHelper<8>  { typedef uint8_t type; };

template <> struct MinimumTypeHelper<9>  { typedef uint16_t type; };
template <> struct MinimumTypeHelper<10> { typedef uint16_t type; };
template <> struct MinimumTypeHelper<11> { typedef uint16_t type; };
template <> struct MinimumTypeHelper<12> { typedef uint16_t type; };
template <> struct MinimumTypeHelper<13> { typedef uint16_t type; };
template <> struct MinimumTypeHelper<14> { typedef uint16_t type; };
template <> struct MinimumTypeHelper<15> { typedef uint16_t type; };
template <> struct MinimumTypeHelper<16> { typedef uint16_t type; };

template <> struct MinimumTypeHelper<17> { typedef uint32_t type; };
template <> struct MinimumTypeHelper<18> { typedef uint32_t type; };
template <> struct MinimumTypeHelper<19> { typedef uint32_t type; };
template <> struct MinimumTypeHelper<20> { typedef uint32_t type; };
template <> struct MinimumTypeHelper<21> { typedef uint32_t type; };
template <> struct MinimumTypeHelper<22> { typedef uint32_t type; };
template <> struct MinimumTypeHelper<23> { typedef uint32_t type; };
template <> struct MinimumTypeHelper<24> { typedef uint32_t type; };
template <> struct MinimumTypeHelper<25> { typedef uint32_t type; };
template <> struct MinimumTypeHelper<26> { typedef uint32_t type; };
template <> struct MinimumTypeHelper<27> { typedef uint32_t type; };
template <> struct MinimumTypeHelper<28> { typedef uint32_t type; };
template <> struct MinimumTypeHelper<29> { typedef uint32_t type; };
template <> struct MinimumTypeHelper<30> { typedef uint32_t type; };
template <> struct MinimumTypeHelper<31> { typedef uint32_t type; };
template <> struct MinimumTypeHelper<32> { typedef uint32_t type; };

template <> struct MinimumTypeHelper<33> { typedef uint64_t type; };
template <> struct MinimumTypeHelper<34> { typedef uint64_t type; };
template <> struct MinimumTypeHelper<35> { typedef uint64_t type; };
template <> struct MinimumTypeHelper<36> { typedef uint64_t type; };
template <> struct MinimumTypeHelper<37> { typedef uint64_t type; };
template <> struct MinimumTypeHelper<38> { typedef uint64_t type; };
template <> struct MinimumTypeHelper<39> { typedef uint64_t type; };
template <> struct MinimumTypeHelper<40> { typedef uint64_t type; };
template <> struct MinimumTypeHelper<41> { typedef uint64_t type; };
template <> struct MinimumTypeHelper<42> { typedef uint64_t type; };
template <> struct MinimumTypeHelper<43> { typedef uint64_t type; };
template <> struct MinimumTypeHelper<44> { typedef uint64_t type; };
template <> struct MinimumTypeHelper<45> { typedef uint64_t type; };
template <> struct MinimumTypeHelper<46> { typedef uint64_t type; };
template <> struct MinimumTypeHelper<47> { typedef uint64_t type; };
template <> struct MinimumTypeHelper<48> { typedef uint64_t type; };
template <> struct MinimumTypeHelper<49> { typedef uint64_t type; };
template <> struct MinimumTypeHelper<50> { typedef uint64_t type; };
template <> struct MinimumTypeHelper<51> { typedef uint64_t type; };
template <> struct MinimumTypeHelper<52> { typedef uint64_t type; };
template <> struct MinimumTypeHelper<53> { typedef uint64_t type; };
template <> struct MinimumTypeHelper<54> { typedef uint64_t type; };
template <> struct MinimumTypeHelper<55> { typedef uint64_t type; };
template <> struct MinimumTypeHelper<56> { typedef uint64_t type; };
template <> struct MinimumTypeHelper<57> { typedef uint64_t type; };
template <> struct MinimumTypeHelper<58> { typedef uint64_t type; };
template <> struct MinimumTypeHelper<59> { typedef uint64_t type; };
template <> struct MinimumTypeHelper<60> { typedef uint64_t type; };
template <> struct MinimumTypeHelper<61> { typedef uint64_t type; };
template <> struct MinimumTypeHelper<62> { typedef uint64_t type; };
template <> struct MinimumTypeHelper<63> { typedef uint64_t type; };
template <> struct MinimumTypeHelper<64> { typedef uint64_t type; };

}

template <size_t Index, size_t Bits = 1>
class BitField {
private:
	enum {
		Mask = (1u << Bits) - 1u
	};

	typedef typename MinimumTypeHelper<Index + Bits>::type T;
public:
	template <class T2>
	BitField &operator=(T2 value) {
		value_ = (value_ & ~(Mask << Index)) | ((value & Mask) << Index);
		return *this;
	}

	operator T() const     { return (value_ >> Index) & Mask; }
	BitField &operator++() { return *this = *this + 1; }
	T operator++(int)      { T r = *this; ++*this; return r; }
	BitField &operator--() { return *this = *this - 1; }
	T operator--(int)      { T r = *this; ++*this; return r; }

private:
	T value_;
};


template <size_t Index>
class BitField<Index, 1> {
private:
	enum {
		Bits = 1,
		Mask = 0x01
	};

	typedef typename MinimumTypeHelper<Index + Bits>::type T;


public:
	BitField &operator=(bool value) {
		value_ = (value_ & ~(Mask << Index)) | (value << Index);
		return *this;
	}

	operator bool() const  { return (value_ >> Index) & Mask; }
	
private:
	T value_;
};

#endif
