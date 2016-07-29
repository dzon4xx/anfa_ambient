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

	 public:

	 Channel(uint8_t channel_num);

	 IO_pin *pin;

	 uint8_t get_num();

	 private:
	 uint8_t num;
 };



#endif /* CHANNEL_H_ */