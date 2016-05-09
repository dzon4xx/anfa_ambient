/*
 * channel.h
 *
 * Created: 2016-03-03 16:56:10
 *  Author: jan.pleszynski
 */ 


#ifndef CHANNEL_H_
#define CHANNEL_H_

#include "../pin/pin.h"
#include "../../../settings.h"

 class Channel
 {
	static uint8_t port_letters[NUM_OF_CHANNELS];
	static uint8_t pin_numbers[NUM_OF_CHANNELS];

	 public:

	 Channel(uint8_t channel_num);

	 Pin *pin;

	 uint8_t get_num();

	 private:
	 uint8_t num;
 };



#endif /* CHANNEL_H_ */