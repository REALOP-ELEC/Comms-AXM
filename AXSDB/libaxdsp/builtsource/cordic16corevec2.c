#include "libaxdsp.h"

#define cordic16_core_vec cordic16_core_vec2

#if defined(SDCC)

// dec2hex(round(arg(1+i*2.^-(0:15))/2/pi*2^16))
// 0x2000, 0x12E4, 0x09FB, 0x0511, 0x028B, 0x0146, 0x00A3, 0x0051
// 0x0029, 0x0014, 0x000A, 0x0005, 0x0003, 0x0001, 0x0001, 0x0000

__reentrantb uint16_t cordic16_core_vec(uint16_t phase) __reentrant
{
	// special calling conventions!
	// R3:R2: I
	// R5:R4: Q
	// DPTR: phase
	__asm
	;; unrolled
	;; pre stage
	mov	a,r3
	jnb	acc.7,00000$
	clr	c
	clr	a
	subb	a,r2
	mov	r2,a
	clr	a
	subb	a,r3
	mov	r3,a
	clr	c
	clr	a
	subb	a,r4
	mov	r4,a
	clr	a
	subb	a,r5
	mov	r5,a
	mov	a,dph
	xrl	a,#0x80
	mov	dph,a
00000$:
	__endasm;
	__asm
	;; stage 0
	mov	a,r5
	jnb	acc.7,00004$
	clr	c
	mov	a,r2
	mov	r0,a
	subb	a,r4
	mov	r2,a
	mov	a,r3
	mov	r1,a
	subb	a,r5
	mov	r3,a
	mov	a,r4
	add	a,r0
	mov	r4,a
	mov	a,r5
	addc	a,r1
	mov	r5,a
	mov	a,dph
	add	a,#(-0x2000)>>8
	mov	dph,a
	sjmp	00005$
00004$:	mov	a,r2
	mov	r0,a
	add	a,r4
	mov	r2,a
	mov	a,r3
	mov	r1,a
	addc	a,r5
	mov	r3,a
	clr	c
	mov	a,r4
	subb	a,r0
	mov	r4,a
	mov	a,r5
	subb	a,r1
	mov	r5,a
	mov	a,dph
	add	a,#0x2000>>8
	mov	dph,a
00005$:
	__endasm;
	__asm
	;; stage 1
	mov	a,r3
	mov	c,acc.7
	rrc	a
	mov	r1,a
	mov	a,r2
	rrc	a
	mov	r0,a
	mov	a,r5
	mov	c,acc.7
	rrc	a
	mov	r7,a
	mov	a,r4
	rrc	a
	mov	r6,a
	mov	a,r5
	jnb	acc.7,00008$
	clr	c
	mov	a,r2
	subb	a,r6
	mov	r2,a
	mov	a,r3
	subb	a,r7
	mov	r3,a
	mov	a,r4
	add	a,r0
	mov	r4,a
	mov	a,r5
	addc	a,r1
	mov	r5,a
	mov	a,dpl
	add	a,#-0x12E4
	mov	dpl,a
	mov	a,dph
	addc	a,#(-0x12E4)>>8
	mov	dph,a
	sjmp	00009$
00008$:	mov	a,r2
	add	a,r6
	mov	r2,a
	mov	a,r3
	addc	a,r7
	mov	r3,a
	clr	c
	mov	a,r4
	subb	a,r0
	mov	r4,a
	mov	a,r5
	subb	a,r1
	mov	r5,a
	mov	a,dpl
	add	a,#0x12E4
	mov	dpl,a
	mov	a,dph
	addc	a,#(0x12E4>>8)
	mov	dph,a
00009$:
	__endasm;
	__asm
	;; stage 2
	mov	a,r2
	rr	a
	rr	a
	anl	a,#0x3f
	mov	r0,a
	mov	a,r3
	rr	a
	rr	a
	mov	r1,a
	anl	a,#0xc0
	orl	a,r0
	mov	r0,a
	mov	a,r1
	anl	a,#0x3f
	mov	r1,a
	mov	a,r3
	rlc	a
	subb	a,acc
	anl	a,#0xc0
	orl	a,r1
	mov	r1,a
	mov	a,r4
	rr	a
	rr	a
	anl	a,#0x3f
	mov	r6,a
	mov	a,r5
	rr	a
	rr	a
	mov	r7,a
	anl	a,#0xc0
	orl	a,r6
	mov	r6,a
	mov	a,r7
	anl	a,#0x3f
	mov	r7,a
	mov	a,r5
	rlc	a
	subb	a,acc
	anl	a,#0xc0
	orl	a,r7
	mov	r7,a
	mov	a,r5
	jnb	acc.7,00012$
	clr	c
	mov	a,r2
	subb	a,r6
	mov	r2,a
	mov	a,r3
	subb	a,r7
	mov	r3,a
	mov	a,r4
	add	a,r0
	mov	r4,a
	mov	a,r5
	addc	a,r1
	mov	r5,a
	mov	a,dpl
	add	a,#-0x09FB
	mov	dpl,a
	mov	a,dph
	addc	a,#(-0x09FB)>>8
	mov	dph,a
	sjmp	00013$
00012$:	mov	a,r2
	add	a,r6
	mov	r2,a
	mov	a,r3
	addc	a,r7
	mov	r3,a
	clr	c
	mov	a,r4
	subb	a,r0
	mov	r4,a
	mov	a,r5
	subb	a,r1
	mov	r5,a
	mov	a,dpl
	add	a,#0x09FB
	mov	dpl,a
	mov	a,dph
	addc	a,#(0x09FB>>8)
	mov	dph,a
00013$:
	__endasm;
}

#endif