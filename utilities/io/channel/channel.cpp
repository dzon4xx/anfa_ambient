/*
 * channel.cpp
 *
 * Created: 2016-03-03 17:27:16
 *  Author: jan.pleszynski
 */
 #include <stdint.h>
 #include "channel.h"
#ifdef ATMEGA32U4
 uint8_t Channel::port_letters[]	=	{'d', 'd', 'd', 'b', 'b'};

 uint8_t Channel::pin_numbers[]	=	{1, 0, 4, 2, 3};
#endif

#ifdef ATMEGA328 
 uint8_t Channel::port_letters[]	=	{'d', 'c', 'c', 'd', 'd'};

 uint8_t Channel::pin_numbers[]	=	{7, 0, 1, 2, 3};
#endif

Channel::Channel(uint8_t channel_num)
{
	this->num = channel_num-1;
	this->pin = new Pin(Channel::port_letters[this->num], Channel::pin_numbers[this->num ]);
}
  
uint8_t Channel::get_num()
{
	return this->num;
}


