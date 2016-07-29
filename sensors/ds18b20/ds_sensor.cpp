/* 
* ds18b20_sensor.cpp
*
* Created: 2016-03-04 13:28:42
* Author: jan.pleszynski
*/
#include "ds_sensor.h"
#include <string.h>

#define DS_DISCONNECTED_RAW -7040
#define DS_RESET_VAL 10880
ds_sensor::ds_sensor(uint8_t line_num, uint8_t mem_id, uint8_t *address) : Sensor()
{
	this->line_num = line_num;
	this->mem_id = mem_id;
	memcpy(this->address, address, DS_SIZE_ADDRESS);
}

bool ds_sensor::is_val_correct()
{
	if (this->val == DS_RESET_VAL)
	{
		//Jesli na linii pojawi sie wartos 85 to nie jest ona brana pod uwage. Jest to wartosc RESET
		return false;
	}

	else if (this->val == ERROR_VAL)
	{
		return false;
	}

	else if (this->val == DS_DISCONNECTED_RAW)
	{
		return false;
	}
	else
	{
		return true;
	}
}

int16_t ds_sensor::convert_value(int16_t val)
{
	int16_t ret_val = (int16_t) (val * 0.78125);
	return ret_val;
}