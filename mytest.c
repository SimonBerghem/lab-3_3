#include "tinythreads.h"
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#define LCD_POSADDR 0xec
#define ODD_MASK 0x0f
#define EVEN_MASK 0xf0
#define SMALLEST_NIBBLE 0xf


//mutex lock_button = MUTEX_INIT;
//mutex lock_blink = MUTEX_INIT;
int b_cc = 0; // not clock counter (click counter)
bool seg_on = false;
uint16_t time_c = 2;


void inter_init(void);
//void writeChar(char ch, int pos); // from lab 1

//bool is_prime(long i); // from lab 1

extern inline bool is_prime(long i){
	// try divifing with all the numbers
	for (int n = 2; n < i; n++){
		if (i % n == 0){
			return false;
		}
	}
	return true;
}

extern inline void writeChar(char ch, int pos){

	//SCC table with num 0-9
	uint16_t scc_table[] = {0x1551, 0x0110, 0x1e11, 0x1b11, 0xb50, 0x1b41, 0x1f41,0x0111, 0x1f51, 0x0b51};

	//set set addr to first pos
	uint8_t *lcdaddr = (uint8_t *) LCD_POSADDR;

	//mask to get nibble
	uint8_t nibble_mask;

	//nibble to send to lcd
	uint8_t lcd_nibble = 0x0;

	// check if pos is unvalied
	if (pos < 0 || pos > 5){
		return;
	}

	uint16_t number = 0x0;
	// chec if char is valid
	if (ch >= '0' || ch <= '9'){
		//get number on table
		number = scc_table[ch - '0'];
	}


	// point to right pos
	lcdaddr += pos >> 1;


	// check if od or even
	if (pos % 2 == 0){
		nibble_mask = EVEN_MASK;
	} else {
		nibble_mask = ODD_MASK;
	}

	// place nibbel on right lcd adr
	for (int i = 0; i < 4; i++){
		lcd_nibble = number & SMALLEST_NIBBLE;
		number = number >> 4;

		//check pos even/odd
		if (pos % 2 != 0){
			lcd_nibble = lcd_nibble << 4;
		}

		*lcdaddr = (*lcdaddr & nibble_mask) | lcd_nibble;
	 

		lcdaddr += 5;
	}
}

void printAt(long num, int pos) {
	int pp;
    pp = pos;
    writeChar( (num % 100) / 10 + '0', pp);
	//for (int k = 0; k < 10000; k++);
    pp++;
    writeChar( num % 10 + '0', pp);
}

void computePrimes(int pos) {
    long n;

    for(n = 1; ; n++) {
        if (is_prime(n)) {
            printAt(n, pos);
			//yield();
        }
    }
}

/*
 * D0003E_lab1
 *
 * By: Erik Serrander, Simon Malmström Berghem
 */


extern inline void lcd_init(void){
	// set lowpower waveform, no frame interupts, no blinking, LCD enable
	LCDCRA = (1 << LCDAB) | (1 << LCDEN);

	// set drivetime 300 micro s, contrast control v 3.35
	LCDCCR = (1 << LCDCC0) | (1 << LCDCC1) | (1 << LCDCC2) | (1 << LCDCC3);

	//ext async clk source, 1:3 bias, 1:4 duty cycle, 25 segmts
	LCDCRB = (1 << LCDCS) | (1 << LCDMUX0) | (1 << LCDMUX1) | (1 << LCDPM0) | (1 << LCDPM1) | (1 << LCDPM2);

	// prescaler N=16, clk div D = 8
	LCDFRR = (1 << LCDCD0) | (1 << LCDCD1) | (1 << LCDCD2);
}



 
// make segment blink with 1HZ
void blink(int k){

	//shadow lcd on
	
	//blink
	// put on if time is right and seg is on
	if (tiny_cc >= time_c){
		tiny_cc = 0;

			// check if seg is on or of and flip it
		if (seg_on){
			LCDDR8 = 0x00; //turn off
			seg_on = false;
		} else {
			LCDDR8 = 0x01; //turn on
			seg_on = true;
		}
	}
}


void button(int p){

	//waiting...
	printAt(b_cc, p);

}

int main() {
	CLKPR = 0x80;
	CLKPR = 0x00;

	lcd_init();

	//for (int i = 0; i < 100; i++){
	//	printAt(i,4);
	//	for (volatile int n = 0; n < 30000; n++);
	//}


	spawn(blink, 0);
	spawn(button, 4);
//    computePrimes(0);
	while(1);

}

// joystick down interrupt
ISR(PCINT1_vect){
	if ((PINB >> 7) == 0){
		spawn(button, 4);
		b_cc++;
		yield();
	}
}

ISR(TIMER1_COMPA_vect){
	spawn(blink, 0);
	tiny_cc++;
	yield();
}
