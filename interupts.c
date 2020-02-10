// interuppts
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdbool.h>
#include <stdint.h>

#include "tinythreads.h"

void inter_init(void){
	PORTB = 0x80; // set joystic down to read
	EIMSK = (1 << PCIE1);
	PCMSK1 = (1 << PCINT15);

	TCCR1A = (1 << COM1A1) | (1 << COM1A0);
	TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);

	OCR1A = 3906; // 500 ms
	TCNT1 = 0x0;

	TIMSK1 = (1 << OCIE1A);
}

