/* automatically generated by gensbox, do not edit! */

#include "des.h"

const uint8_t __code des_sbox[8][64] = {
	{
		0x58, 0x10, 0x04, 0x5c, 0x08, 0x58, 0x40, 0x08, 0x44, 0x0c, 0x5c, 0x14, 0x1c, 0x54, 0x10, 0x40, 
		0x0c, 0x48, 0x18, 0x50, 0x14, 0x44, 0x4c, 0x1c, 0x50, 0x00, 0x00, 0x4c, 0x48, 0x18, 0x54, 0x04, 
		0x54, 0x04, 0x1c, 0x10, 0x40, 0x4c, 0x10, 0x54, 0x18, 0x40, 0x48, 0x0c, 0x4c, 0x08, 0x04, 0x58, 
		0x00, 0x5c, 0x44, 0x48, 0x0c, 0x18, 0x58, 0x00, 0x5c, 0x14, 0x14, 0x50, 0x50, 0x44, 0x08, 0x1c
	},
	{
		0x20, 0x26, 0x0e, 0x00, 0x08, 0x0e, 0x2a, 0x2c, 0x2e, 0x20, 0x00, 0x06, 0x02, 0x04, 0x26, 0x0a, 
		0x0c, 0x2a, 0x22, 0x0c, 0x06, 0x24, 0x2c, 0x22, 0x24, 0x08, 0x0a, 0x2e, 0x28, 0x02, 0x04, 0x28, 
		0x04, 0x28, 0x20, 0x0e, 0x0e, 0x26, 0x26, 0x02, 0x22, 0x04, 0x0c, 0x20, 0x2c, 0x0a, 0x2a, 0x2c, 
		0x0a, 0x06, 0x2e, 0x24, 0x28, 0x00, 0x02, 0x2e, 0x00, 0x2a, 0x24, 0x08, 0x06, 0x0c, 0x08, 0x22
	},
	{
		0x18, 0x28, 0x40, 0x78, 0x28, 0x10, 0x78, 0x20, 0x48, 0x70, 0x20, 0x18, 0x30, 0x48, 0x08, 0x50, 
		0x00, 0x30, 0x58, 0x40, 0x60, 0x58, 0x10, 0x38, 0x38, 0x00, 0x70, 0x68, 0x50, 0x60, 0x68, 0x08, 
		0x48, 0x10, 0x38, 0x60, 0x78, 0x20, 0x50, 0x18, 0x20, 0x48, 0x08, 0x50, 0x18, 0x78, 0x60, 0x28, 
		0x70, 0x68, 0x00, 0x38, 0x10, 0x40, 0x28, 0x70, 0x40, 0x30, 0x58, 0x00, 0x68, 0x08, 0x30, 0x58
	},
	{
		0x01, 0x0b, 0x0a, 0x43, 0x08, 0x01, 0x40, 0x0a, 0x49, 0x08, 0x03, 0x49, 0x43, 0x4a, 0x09, 0x40, 
		0x02, 0x48, 0x48, 0x00, 0x41, 0x4b, 0x4b, 0x03, 0x4a, 0x41, 0x00, 0x42, 0x0b, 0x02, 0x42, 0x09, 
		0x08, 0x43, 0x01, 0x02, 0x40, 0x0a, 0x43, 0x49, 0x03, 0x40, 0x4a, 0x0b, 0x49, 0x01, 0x02, 0x4a, 
		0x4b, 0x09, 0x42, 0x4b, 0x0a, 0x00, 0x48, 0x42, 0x09, 0x03, 0x41, 0x08, 0x00, 0x48, 0x0b, 0x41
	},
	{
		0x61, 0xa1, 0xa1, 0x80, 0xe0, 0xc1, 0x41, 0x21, 0x00, 0x60, 0x60, 0xe1, 0x81, 0x00, 0xc0, 0x41, 
		0x01, 0x20, 0x40, 0x61, 0x80, 0x40, 0x21, 0xa0, 0xc1, 0x01, 0xa0, 0xc0, 0x20, 0xe0, 0xe1, 0x81, 
		0xc0, 0x41, 0x60, 0xe1, 0x81, 0x00, 0x00, 0x60, 0xa0, 0xc0, 0xc1, 0x01, 0x61, 0xa1, 0xa1, 0x80, 
		0xe1, 0x81, 0x01, 0x20, 0x41, 0x21, 0xe0, 0xc1, 0x21, 0xa0, 0x40, 0x61, 0x80, 0x40, 0x20, 0xe0
	},
	{
		0x0a, 0x07, 0x00, 0x0d, 0x06, 0x00, 0x0b, 0x06, 0x09, 0x0c, 0x0c, 0x01, 0x0f, 0x09, 0x05, 0x0a, 
		0x04, 0x08, 0x07, 0x02, 0x03, 0x05, 0x0d, 0x0b, 0x0e, 0x03, 0x01, 0x0e, 0x08, 0x0f, 0x02, 0x04, 
		0x07, 0x04, 0x09, 0x0a, 0x01, 0x07, 0x06, 0x00, 0x02, 0x09, 0x0f, 0x06, 0x0c, 0x02, 0x00, 0x0d, 
		0x0e, 0x01, 0x04, 0x0f, 0x08, 0x0b, 0x03, 0x0c, 0x05, 0x0e, 0x0a, 0x05, 0x0b, 0x08, 0x0d, 0x03
	},
	{
		0xf0, 0xc0, 0x80, 0xb0, 0x10, 0x20, 0x70, 0xe0, 0x60, 0xf0, 0xd0, 0x40, 0xc0, 0x10, 0x20, 0x70, 
		0x90, 0x30, 0xe0, 0x00, 0x40, 0x80, 0xb0, 0x50, 0x30, 0x60, 0x00, 0x90, 0xa0, 0xd0, 0x50, 0xa0, 
		0x00, 0xb0, 0x70, 0x10, 0xe0, 0x50, 0xd0, 0x80, 0x50, 0xc0, 0x20, 0xf0, 0xb0, 0x20, 0x80, 0x40, 
		0xa0, 0xd0, 0x10, 0x60, 0x30, 0xe0, 0x60, 0x30, 0x90, 0x00, 0xc0, 0xa0, 0x40, 0x70, 0xf0, 0x90
	},
	{
		0xc2, 0x00, 0x80, 0xc6, 0xc4, 0x86, 0x04, 0x80, 0x02, 0xc2, 0xc6, 0x02, 0x46, 0xc4, 0x40, 0x04, 
		0x06, 0x42, 0x42, 0x82, 0x82, 0xc0, 0xc0, 0x46, 0x84, 0x44, 0x44, 0x84, 0x00, 0x06, 0x86, 0x40, 
		0x80, 0xc6, 0x04, 0xc0, 0xc2, 0x40, 0x40, 0x02, 0xc4, 0x80, 0x82, 0x44, 0x02, 0x04, 0x46, 0x86, 
		0xc6, 0x84, 0xc0, 0x46, 0x44, 0x06, 0x86, 0xc2, 0x06, 0x42, 0x42, 0x00, 0x84, 0x82, 0x00, 0xc4
	}
};
