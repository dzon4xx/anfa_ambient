/*
 * analog.h
 *
 * Created: 2015-11-12 00:18:09
 *  Author: jan.pleszynski
 */ 


#ifndef DS18B20_H_
#define DS18B20_H_

#include "ds_line_low_level.h"

#include "../../settings.h"
//#include "../Sensor.h"
#include "../../Utilities/memory/memory.h"
#include "ds_sensor.h"


typedef uint8_t ds_address[8];

class Ds_line : public Ds_line_low_level, public Line
{
	//methods 
	public:	
	Ds_line(Channel *channel, Memory *mem);
	
	//Adresy obiektow czujnikow ds18b20
	ds_sensor *dss_objects[DS_MAX_SENSORS];
		
	//Ustawia temperatury czujnikom
	void read() ;
	
	// Przywraca adres DSa do defaultowego
	void reset_dss_hardware_id();

	private:

	// initialise bus
	void begin(void);
			
	//Porownuje dallasy na linii z dallasami w pamieci
	void compare_line_to_mem();

	//Sprawdza czy linia 1w jest zwarta. Jesli jest zwarta wszystkie czujniki zwracaja 0. Wtedy ustawia kazdemu z nich wartosc bledu.
	void check_line_short(ds_sensor *sensor, uint8_t *ds_line_short_counter);

	// returns the number of devices found on the bus
	uint8_t get_device_count(void);

	void set_resolution(uint8_t newResolution);

	public:	//variables

	//Licznik stworzonych obiektow ds dla linii. Sa to wszystkie czujniki jakie kiedykolwiek byly na linii.
	uint8_t num_of_ds_objects = 0;

	private: //variables

	//Liczba dsow znajdujacych sie aktualnie na linii
	uint8_t num_of_dss_on_line = 0;
		
	//Liczba dsow zapisanych w pamieci
	uint8_t num_of_dss_in_mem = 0;
		
	uint8_t line_num;
		
};

#endif