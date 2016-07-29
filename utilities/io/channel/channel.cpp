/*
 * channel.cpp
 *
 * Created: 2016-03-03 17:27:16
 *  Author: jan.pleszynski
 */
 #include <stdint.h>
 #include "channel.h"

 uint8_t channels_ports_letters[]	=	{'d', 'c', 'c', 'd', 'd'};

 uint8_t channels_pins_numbers[]	=	{7, 0, 1, 2, 3};

Channel::Channel(uint8_t channel_num)
{
	this->num = channel_num-1;
	this->pin = new IO_pin(channels_ports_letters[this->num], channels_pins_numbers[this->num]);
}
  
uint8_t Channel::get_num()
{
	return this->num;
}


