/**
******************************************************************************
* @file
* @brief
* @internal
* @author Thomas Sailer
* $Rev: $
* $Date: $
******************************************************************************
* Copyright 2016 Semiconductor Components Industries LLC (d/b/a �ON Semiconductor�).
* All rights reserved.  This software and/or documentation is licensed by ON Semiconductor
* under limited terms and conditions.  The terms and conditions pertaining to the software
* and/or documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
* (�ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software�) and
* if applicable the software license agreement.  Do not use this software and/or
* documentation unless you have carefully read and you agree to the limited terms and
* conditions.  By using this software and/or documentation, you agree to the limited
* terms and conditions.
*
* THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
* OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
* ON SEMICONDUCTOR SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL,
* INCIDENTAL, OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
* @endinternal
*
* @ingroup
*
* @details
*/

#include "libmftypes.h"

#if defined(SDCC)

__reentrantb uint8_t hweight32(uint32_t x) __reentrant __naked
{
	x;
	__asm;
	push	acc
	lcall	_hweight16
	mov	dph,a
	pop	dpl
	lcall	_hweight8
	mov	a,dph
	add	a,dpl
	mov	dph,a
	mov	dpl,b
	lcall	_hweight8
	mov	a,dph
	add	a,dpl
	mov	dpl,a
	ret
	__endasm;
}

#else

__reentrantb uint8_t hweight32(uint32_t x) __reentrant
{
	return hweight8(x) + hweight8(x >> 8) + hweight8(x >> 16) + hweight8(x >> 24);
}

#endif
