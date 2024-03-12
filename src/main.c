// ux2d1minicast1
// tcs 2023 08 09

#include <axm0f2.h>

//  __disable_irq();
//  __enable_irq();

#define PINMODE_OFF 		0x0 // OFF high Z analog
#define PINMODE_INPUT 		0x1 // IN  high Z digital input
#define PINMODE_PULL_HIGH 	0x2 // 0 P drive low, pull high
#define PINMODE_PULL_LOW 	0x3 // P 1 drive high, pull low
#define PINMODE_OPEN_DRAIN 	0x4 // 0 Z drive low, open (open drain)
#define PINMODE_OPEN_SOURCE 0x5 // Z 1 drive high, open
#define PINMODE_PUSH_PULL 	0x6 // 1 0 push pull
#define PINMODE_PULL_BOTH 	0x7 // P P pull high and low

void gpio_mode_set(char port, char pin, char mode) {
	switch (port) {
		case 0:
			GPIO_PRT0->PC = mode << (pin * 3);
		break;
		case 1:
			GPIO_PRT1->PC = mode << (pin * 3);
		break;
		case 2:
			GPIO_PRT2->PC = mode << (pin * 3);
		break;
		case 3:
			GPIO_PRT3->PC = mode << (pin * 3);
		break;
		case 4:
			GPIO_PRT4->PC = mode << (pin * 3);
		break;
	}
}

void gpio_set(char port, char pin, char data) {
	if (data) {
		data = 1;
	} else {
		data = 0;
	}
	switch (port) {
		case 0:
			GPIO_PRT0->DR = data << pin;
		break;
		case 1:
			GPIO_PRT1->DR = data << pin;
		break;
		case 2:
			GPIO_PRT2->DR = data << pin;
		break;
		case 3:
			GPIO_PRT3->DR = data << pin;
		break;
		case 4:
			GPIO_PRT4->DR = data << pin;
		break;
	}
}

void nothing(int duration) {
	for (int i = 0; i < duration; i++) {
		__asm volatile("nop");
	}
}

void main(void) {
	HSIOM->PORT_SEL0 = 0;
	gpio_mode_set(0, 5, PINMODE_PUSH_PULL);
	gpio_mode_set(1, 2, PINMODE_PUSH_PULL);
	gpio_mode_set(1, 7, PINMODE_PUSH_PULL);

    while (1) {
		gpio_set(1, 2, 0);
		gpio_set(0, 5, 1);
		gpio_set(1, 7, 1);
		nothing(1000000);
		gpio_set(1, 2, 1);
		gpio_set(0, 5, 1);
		gpio_set(1, 7, 0);
		nothing(1000000);
    }
}








