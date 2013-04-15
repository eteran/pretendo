
#ifndef _OPTIMISED_COPIES_H_
#define _OPTIMISED_COPIES_H_

extern "C" {
	void mmx2_copy (uint8 *dest, uint8 *src, size_t size);
	void mmx2_copy (uint8 *dest, uint8 *src, size_t size);
	void sse_copy (uint8 *dest, uint8 *src, size_t size);
}


#endif // _OPTIMISED_COPIES_
