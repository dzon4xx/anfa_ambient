/* 
* ds18b20_sensor.h
*
* Created: 2016-03-04 13:28:42
* Author: jan.pleszynski
*/
#ifndef __DS18B20_SENSOR_H__
#define __DS18B20_SENSOR_H__

#include "../Sensor.h"

#define DS_SIZE_ADDRESS 8


class ds_sensor : public Sensor
{
	//friend class Ds_line;
	public:
	ds_sensor( uint8_t line_num, uint8_t mem_id, uint8_t *address);
	
	bool is_val_correct();

	int16_t convert_value(int16_t val);

	public:
	uint8_t line_num;
	// Id dallasa w pamieci. Jest ono takze wpisywane do rejestru dallasa. Wykorzystywane do zwracania temperatury na zewnatrz
	uint8_t mem_id;
	//Adress dsa. Uzywany do pobierania jego temperatury
	uint8_t address[DS_SIZE_ADDRESS];
};

#endif /* DS18B20_H_ */