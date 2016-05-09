#include "Clock.h"
#include "../../communication/modbus_slave.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "../../settings.h"

#define DEFAULT_RESPOND_RATE 1000	// Co jaki czas stacja uzyskuje
#define  OVF_VAL 249	//wartosc rejestru TCNT0 aby timer przepelnial sie co 1ms

//Zmienna globalna - szybszy dostep
volatile unsigned long miliseconds_passed = 0;

 Clock::Clock()
{
	cli();
	OCR0A = OVF_VAL;
	TIMSK0 |= (1<<OCIE0A);
	TCCR0A |= (1<WGM01);			
	sei();
}

void Clock::start()
{
	cli();
	TCCR0B |= (1<<CS00)|(1<<CS01);
	sei();
}

void Clock::stop()
{
	TCCR0B &= ~(1<<CS00)|(1<<CS01);
}

void Clock::restart()
{
	miliseconds_passed = 0;
	this->stop();
	OCR0A = OVF_VAL;
	this->start();
}

unsigned long Clock::get_ms()
{
	return miliseconds_passed;
}


Clock *clock;

ISR (TIMER0_COMPA_vect) 
{
	miliseconds_passed++;
	unsigned long current_t = clock->get_ms();
	if (current_t - read_reg->last_repond_t >= DEFAULT_RESPOND_RATE)
	{
		read_reg->last_repond_t = current_t;
		read_reg->respond_flag = true;
	}
}