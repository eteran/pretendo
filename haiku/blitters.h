/* $Header: /var/cvsroot/pretendo/os/beos/blitters.h,v 1.3 2005/05/27 18:34:18 eteran Exp $ */
// optimised blitters for BeOS

#ifndef _BE_BLITTERS_H_
#define _BE_BLITTERS_H_

extern "C" {
void blit_windowed_dirty_mmx (uint8 *src, 
							  uint8 *dirty, 
							  uint8 *dst, 
							  size_t size, 
							  int32 bpp);
							  
void blit_2x_mmx (uint8 *dest, uint8 *source, size_t size);

void blit_2x_dirty_mmx (uint8 *dest, 
						uint8 *source, 
						uint8 *dirty, 
						int32 rowbytes, 
						size_t size);

void blit_2x_windowed_dirty_mmx(uint8 *src, 
								uint8 *dirty, 
								uint8 *dst, 
								size_t size, 				
								int32 bpp, 
								int32 rowbytes);

void blit_overlay (uint8 *dest, uint8 *source, size_t size, uint32 *y, uint32 *ycbcr);	

};

#endif // _BE_BLITTERS_H_
