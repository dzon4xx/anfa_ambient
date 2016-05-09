/*
 * eeprom.cpp
 *
 * Created: 2015-11-14 15:38:01
 *  Author: jan.pleszynski
 */ 
#include "memory.h"

#include <avr/eeprom.h>

#include "../../settings.h"


///////////////////STATES/////////////////////////

//////////////////EEPROM/////////////////////////////
#define MODULE_NOT_PROGRAMMED 255
//////////////////EEPROM/////////////////////////////

#define SIZE_MEMORY 1024
#define SIZE_LONG	4
#define SIZE_INT	2
#define SIZE_uint8_t	1
// ILOSC PAMIECI POTRZEBNA DO ZAKODOWANIA POSZCZEGOLNYCH USTAWIEN
#define SIZE_MODULE_NAME 1
#define SIZE_MODULE_PRODUCTION_TIME 4
#define SIZE_MODBUS_ADDRESS 1
#define SIZE_BAUDE_RATE 4
#define SIZE_DOUBLE_PRECISSION 1
#define SIZE_NUM_OF_DSS 1
#define SIZE_SETTINGS SIZE_MODULE_NAME+SIZE_MODULE_PRODUCTION_TIME+SIZE_MODBUS_ADDRESS+SIZE_BAUDE_RATE+SIZE_DOUBLE_PRECISSION+SIZE_NUM_OF_DSS

// ILOSC PAMIECI DO ZAKODOWANIA USTAWIEN CZUJNIKOW
#define SIZE_TYPE 1				// Typ podlaczonego czujnik (ANALOG, DHT, IMPULSE, PIR, REED_SWITCH, ONE WIRE)
#define SIZE_STATE 1			// Stan podlaczonego czujnika (EXISTS, NEW, LOST)
#define SIZE_MEAN_SAMPLES 1		// Ilosc probek do usredniania. Maksymalna wartosc to: MAX_AVERAGE_BUFFER_SIZE
#define SIZE_NAME		1//(int)(SIZE_MEMORY-SIZE_SETTINGS-(NUM_OF_CHANNELS*(SIZE_TYPE+SIZE_STATE+SIZE_MEAN_SAMPLES))-(DS_MAX_SENSORS*(SIZE_STATE+SIZE_MEAN_SAMPLES+DS_SIZE_ADDRESS)))/(DS_MAX_SENSORS+NUM_OF_CHANNELS)	// Nazwa czujnika. Jej wielkosc wynika z pozostalej wolnej pamieci

#define SIZE_SENSOR		(SIZE_TYPE+SIZE_STATE+SIZE_MEAN_SAMPLES+SIZE_NAME)
#define SIZE_DS_LINE_NUM 1
#define SIZE_DS			(DS_SIZE_ADDRESS+SIZE_DS_LINE_NUM+SIZE_STATE+SIZE_MEAN_SAMPLES+SIZE_NAME)

//POZYCJA USTAWIEN W PAMIECI
#define POS_MODULE_NAME				0
#define POS_MODULE_PRODUCTION_TIME	SIZE_MODULE_NAME
#define POS_MODBUS_ADDRESS			POS_MODULE_PRODUCTION_TIME+SIZE_MODULE_PRODUCTION_TIME
#define POS_BAUDE_RATE				POS_MODBUS_ADDRESS+SIZE_MODBUS_ADDRESS
#define POS_DOUBLE_PRECISSION		POS_BAUDE_RATE+SIZE_BAUDE_RATE
#define POS_NUM_OF_DSS				POS_DOUBLE_PRECISSION+SIZE_DOUBLE_PRECISSION
//POZYCJA DANYCH CZUJNIKOW W PAMIECI

// Zwraca pozycje danych czujnika
unsigned int inline pos_sensor(uint8_t sens_num);

// Zwraca pozycje typu czujnika
unsigned int inline pos_sensor_type(uint8_t sens_num);

// Zwraca pozycje stanu czujnika
unsigned int inline pos_sensor_state(uint8_t sens_num);

// Zwraca pozycje ilosci probek usredniania
unsigned int inline pos_sensor_mean_samples(uint8_t sens_num);

// Zwraca pozycje ilosci dsow na danej linii
unsigned int pos_1w_num_of_dss(uint8_t sens_num);

// Zwraca pozycje precyzji wystawiania pomiarow
//unsigned int inline pos_sensor_precision(uint8_t sens_num);

// Zwraca pozycje nazwy czujnika
unsigned int inline pos_sensor_name(uint8_t sens_num);

#define END_OF_SENSORS pos_sensor_name(NUM_OF_CHANNELS-1)+SIZE_NAME-1
#define START_OF_1W END_OF_SENSORS

#define POS_1W		END_OF_SENSORS + 1
unsigned int inline pos_ds_state(uint8_t ds_num);
unsigned int inline pos_ds_line_num(uint8_t ds_num);
unsigned int inline pos_ds_mean_samples(uint8_t ds_num);
unsigned int inline pos_ds_name(uint8_t ds_num);
unsigned int inline pos_ds_address(uint8_t ds_num);

//////////////////PUBLIC FUNCTIONS//////////////////////
Memory::Memory()
{
	//TODO: Obsluga nowej stacji. Ewentualnie jest to czêœc do zerowania jej
	//TODO: PRzywracanie pamieci do wartosci poczatkowej powoduje restartowanie kodu w kolko
	//this->restore_memory_to_default();
}

void Memory::restore_memory_to_default()
{
	this->set_double_precission(true);
	this->set_num_of_all_ds(0);	//Domyslnie wartosc ilosci wykrytych DSow to 0	
	//this->set_modbus_address(DEFAULT_MODBUS_ADDRESS)
	for (uint8_t i=0; i<NUM_OF_CHANNELS; i++)
	{
		this->set_state(i, NOT_CONNECTED);
		this->set_type(i, 0);
		this->set_mean_samples(i, MAX_AVERAGE_BUFFER_SIZE);
		this->clear_name(i);
	}
	uint8_t default_ds_address[] = {0, 0, 0, 0, 0, 0, 0, 0};
	for (uint8_t i=0; i<DS_MAX_SENSORS; i++)
	{
		this->set_ds_state(i, NOT_CONNECTED);
		this->set_ds_line_num(i, EMPTY_CELL);
		this->set_ds_mean_samples(i, MAX_AVERAGE_BUFFER_SIZE);
		this->set_ds_address(i, default_ds_address);
		this->clear_ds_name(i);
	}
}

void Memory::set_module_name(char module_name)
{
	if(	this->read(POS_MODULE_NAME) == 255)
	{
	update(POS_MODULE_NAME, module_name);
	}
}

void Memory::set_production_time(unsigned long production_time)
{
	bool is_prod_t_set = true;
	//Sprawdz czy czas produkcji ustawiony
	uint8_t not_set_counter=0;
	for (uint8_t i=0; i<SIZE_LONG; i++)
	{
		
		//Jesli pole na ktorym zapisywany jest czas produkcji wynosi 255
		if(this->read(POS_MODULE_PRODUCTION_TIME+i)==255)
		{
			not_set_counter++;
			//Jesli wszystkie 4 pola rowne 255 to znaczy ze czas produkcji nie ustawiony
			if (not_set_counter==SIZE_LONG)
			{
				is_prod_t_set = false;
			}
		}
	}
	//jesli czas produkcji nie ustawiony to ustaw.
	if(!is_prod_t_set)
	{
		uint8_t production_time_array[SIZE_LONG];
		long_to_uint8_t_array(production_time, production_time_array);
		for (uint8_t i=0; i<SIZE_LONG; i++)
		{
			update(POS_MODULE_PRODUCTION_TIME+i, production_time_array[i]);
		}
	}
}

void Memory::set_modbus_address(uint8_t modbus_address)
{
	update(POS_MODBUS_ADDRESS, modbus_address);
}

void Memory::set_baud_rate(unsigned long baude_rate)
{
	uint8_t baude_rate_array[SIZE_LONG];
	uint8_t_array_to_long(baude_rate_array);
	for (uint8_t i=0; i<SIZE_LONG; i++)
	{
		this->update(POS_BAUDE_RATE+i, baude_rate_array[i]);
	}
}

void Memory::set_double_precission(bool is_double_precission)
{
	this->update(POS_DOUBLE_PRECISSION, (uint8_t)is_double_precission);
}

void Memory::set_num_of_all_ds(uint8_t num_of_ds)
{
	this->update(POS_NUM_OF_DSS, num_of_ds);

}

void Memory::set_type(uint8_t channel, uint8_t type)
{
	this->update(this->get_position(channel, pos_sensor_type), type);
}

void Memory::set_state(uint8_t channel, uint8_t state)
{
	this->update(this->get_position(channel, pos_sensor_state), state);
}

void Memory::set_ds_state(uint8_t ds_num, uint8_t state)
{
	this->update(pos_ds_state(ds_num), state);
}

void Memory::set_ds_line_num(uint8_t ds_num, uint8_t line_num)
{
	this->update(pos_ds_line_num(ds_num), line_num);
}

void Memory::set_mean_samples(uint8_t channel, uint8_t mean_samples)
{
	this->update(this->get_position(channel, pos_sensor_mean_samples), mean_samples);
}

void Memory::set_ds_mean_samples(uint8_t ds_num, uint8_t mean_samples)
{
	this->update(pos_ds_mean_samples(ds_num), mean_samples);
}

void Memory::set_name(uint8_t channel, char name[])
{
	unsigned int name_start = this->get_position(channel, pos_sensor_name);
	this->set_name(name_start, name);
}

void Memory::set_ds_name(uint8_t ds_num, char name[])
{
	unsigned int name_start = pos_ds_name(ds_num);
	this->set_name(name_start, name);
}

void Memory::set_ds_address(uint8_t ds_num, uint8_t address[])
{
	unsigned int address_start = pos_ds_address(ds_num);
	for (uint8_t i=0; i<DS_SIZE_ADDRESS; i++)
	{
		update(address_start+i, address[i]);
	}
}

char Memory::get_module_name()
{
	return this->read(POS_MODULE_NAME);
}

long Memory::get_production_time()
{
	uint8_t production_time_array[SIZE_LONG];
	for (uint8_t i=0; i<SIZE_LONG; i++)
	{
		production_time_array[i] = this->read(POS_MODULE_PRODUCTION_TIME+i);
	}
	return uint8_t_array_to_long(production_time_array);
}

long Memory::get_baud_rate()
{
	uint8_t baud_rate[SIZE_LONG];
	for (uint8_t i=0; i<SIZE_LONG; i++)
	{
		baud_rate[i] = this->read(POS_BAUDE_RATE+i);
	}
	return uint8_t_array_to_long(baud_rate);
}

bool Memory::get_double_precission()
{
	return this->read(POS_DOUBLE_PRECISSION);
}

uint8_t Memory::get_modbus_address()
{
	return this->read(POS_MODBUS_ADDRESS);
}

uint8_t Memory::get_num_of_all_ds()
{
	return this->read(POS_NUM_OF_DSS);
}

uint8_t Memory::get_type(uint8_t channel)
{
	return this->read(this->get_position(channel, pos_sensor_type));
}

uint8_t Memory::get_state(uint8_t channel)
{
	return this->read(this->get_position(channel, pos_sensor_state));
}

uint8_t Memory::get_ds_state(uint8_t ds_num)
{
	return this->read(pos_ds_state(ds_num));
}

uint8_t Memory::get_ds_line_num(uint8_t ds_num)
{
	return this->read(pos_ds_line_num(ds_num));
}

uint8_t Memory::get_mean_samples(uint8_t channel)
{
	return this->read(this->get_position(channel, pos_sensor_mean_samples));
}

uint8_t Memory::get_ds_mean_samples(uint8_t ds_num)
{
	return this->read(pos_ds_mean_samples(ds_num));
}

uint8_t Memory::get_num_of_ds(uint8_t channel)
{
	return  this->read(this->get_position(channel, pos_sensor_mean_samples));
}

void Memory::get_name(uint8_t channel, char name[])
{

}

void Memory::get_ds_name(uint8_t ds_num, char name[])
{

}

bool Memory::get_ds_address(uint8_t ds_num, uint8_t address[])
{
	unsigned int address_start = pos_ds_address(ds_num);
	if (this->read(address_start) == NOT_CONNECTED)
	{
		return false;
	}
		
	for (uint8_t i=0; i<DS_SIZE_ADDRESS; i++)
	{
		address[i] = this->read(address_start+i);
	}
	return true;
}

void Memory::clear_name(uint8_t channel)
{
	for (uint8_t i=0; i<SIZE_NAME; i++)
	{
		this->update(pos_sensor_name(channel), EMPTY_CELL);
	}
}

void Memory::clear_ds_name(uint8_t ds_num)
{
	for (uint8_t i=0; i<SIZE_NAME; i++)
	{
		this->update(pos_ds_name(ds_num), EMPTY_CELL);
	}
}

bool Memory::contains_ds(uint8_t address[])
{
	// Sprawdz wszystkie czujniki znajdujace sie w pamieci
	for (uint8_t ds_num=0; ds_num<DS_MAX_SENSORS; ds_num++)
	{
		uint8_t compatible_uint8_ts_num = 0;
		unsigned int pos_ds_adress = pos_ds_address(ds_num);
		for(uint8_t address_uint8_t_num = 0; address_uint8_t_num< DS_SIZE_ADDRESS; address_uint8_t_num++)
		
		if(address[address_uint8_t_num] == this->read(pos_ds_adress+address_uint8_t_num))
		{
			compatible_uint8_ts_num++;
			if (compatible_uint8_ts_num==DS_SIZE_ADDRESS)
			{
				// Odnaleziono dsa w pamieci
				return true;
			}
		}
	}
	// Nie odnaleziono dsa w pamieci
	return false;
}

bool Memory::is_empty()
{
	if (this->read(POS_MODULE_NAME)==MODULE_NOT_PROGRAMMED)
	{
		return true;
	}
	return false;
}

#ifdef DEBUG
#include "avr/pgmspace.h"

void Memory::print_eeprom()
{
	//Serial1.println(F("////SETTINGS////"));
	//Serial1.print("pos: "); Serial1.print(POS_MODULE_NAME); Serial1.print("\t"); Serial1.print(F("Module name:\t")); Serial1.print(this->get_module_name()); Serial1.println();
//
	//Serial1.print("pos: "); Serial1.print(POS_MODULE_PRODUCTION_TIME); Serial1.print("\t"); Serial1.print(F("Production_time:\t")); Serial1.print(this->get_production_time()); Serial1.println();
//
	//Serial1.print("pos: "); Serial1.print(POS_MODBUS_ADDRESS); Serial1.print("\t"); Serial1.print(F("Modbus address:\t")); Serial1.print(this->get_modbus_address()); Serial1.println();
//
	//Serial1.print("pos: "); Serial1.print(POS_BAUDE_RATE); Serial1.print("\t"); Serial1.print(F("Baud_rate:\t")); Serial1.print(this->get_baud_rate()); Serial1.println();
//
	//Serial1.print("pos: "); Serial1.print(POS_NUM_OF_DSS); Serial1.print("\t"); Serial1.print(F("Num_of_ds:\t")); Serial1.print(this->get_num_of_ds()); Serial1.println();
	//Serial1.println(F("////SETTINGS////"));
//
	//Serial1.print("pos: "); Serial1.print(POS_MODULE_NAME); Serial1.print("\t"); Serial1.println(F("////CHANNEL 1////"));
	//Serial1.print(F("Type\t")); Serial1.print(get_type(CHANNEL_1)); Serial1.println();
	//Serial1.print(F("State\t")); Serial1.print(get_state(CHANNEL_1)); Serial1.println();
	//Serial1.print(F("Mean_samples\t")); Serial1.print(get_mean_samples(CHANNEL_1)); Serial1.println();
	//Serial1.print(F("Precision\t")); Serial1.print(get_precision(CHANNEL_1)); Serial1.println();
}
#endif
////////////////////////////////////////////////////////////

void Memory::write(unsigned int addr, uint8_t val)
{
	eeprom_write_byte((unsigned char *) addr, val);
}

uint8_t Memory::read(unsigned int addr)
{
	return eeprom_read_byte((unsigned char *) addr);
}

////////////////PRIVATE FUNCTIONS///////////////////////////
void Memory::write_name(unsigned int position, char name[])
{
	uint8_t name_pointer = 0;
	while(name[name_pointer]!='\0')
	{
		update(position, name[name_pointer]);
		name_pointer++;
		if(name_pointer==SIZE_NAME){break;} // Osiagnieta maksymalna dlugosc napisu
	}
}

void Memory::read_name(unsigned int position, char name[])
{

}

unsigned int Memory::get_position(uint8_t channel, unsigned int (*pos_fun)(uint8_t))
{			 
		return (*pos_fun)(channel);
}

void Memory::update(unsigned int pos, uint8_t data)
{
	uint8_t old_data = this->read(pos);
	if (old_data!=data)
	{
		this->write(pos, data);
	}
}

void Memory::int_to_uint8_t_array(int num, uint8_t* array)
{
	array[0] = num >> 8;
	array[1] = num;
}

void Memory::long_to_uint8_t_array(long num, uint8_t* array)
{
	
	array[0] = num >> 24;
	array[1] = num >> 16;
	array[2] = num >> 8;
	array[3] = num;
}

unsigned int Memory::uint8_t_array_to_int(uint8_t* array)
{
	return  ((unsigned int)array[0] << 8) +
	((unsigned int)array[1]);
}

unsigned long Memory::uint8_t_array_to_long(uint8_t* array)
{
	return  (((unsigned long)array[0]) << 24) +
	(((unsigned long)array[1]) << 16) +
	(((unsigned long)array[2]) << 8) +
	((unsigned long)array[3]);
}

unsigned int pos_sensor(uint8_t sens_num)
{
 return SIZE_SETTINGS+(sens_num)*SIZE_SENSOR;
}

unsigned int pos_sensor_state(uint8_t sens_num)
{
	return pos_sensor(sens_num);
}

unsigned int pos_sensor_type(uint8_t sens_num)
{
	return pos_sensor_state(sens_num)+SIZE_STATE;
}

unsigned int pos_sensor_mean_samples(uint8_t sens_num)
{
	return pos_sensor_type(sens_num)+SIZE_TYPE;
}

unsigned int pos_1w_num_of_dss(uint8_t sens_num)
{
	return pos_sensor_type(sens_num)+SIZE_TYPE;
}

unsigned int pos_sensor_name(uint8_t sens_num)
{
	return pos_sensor_mean_samples(sens_num)+SIZE_MEAN_SAMPLES;
}


unsigned int pos_ds_address(uint8_t ds_num)
{
	return POS_1W+(ds_num)*SIZE_DS;
}

unsigned int pos_ds_line_num(uint8_t ds_num)
{
	return pos_ds_address(ds_num) + DS_SIZE_ADDRESS;
}

unsigned int pos_ds_state(uint8_t ds_num)
{
	return pos_ds_line_num(ds_num) + SIZE_DS_LINE_NUM;
}

unsigned int pos_ds_mean_samples(uint8_t ds_num)
{
	return pos_ds_state(ds_num) + SIZE_STATE;
}

unsigned int pos_ds_name(uint8_t ds_num)
{
	return pos_ds_mean_samples(ds_num) + SIZE_MEAN_SAMPLES;
}


