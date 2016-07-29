/*
 * CPPFile1.cpp
 *
 * Created: 2015-11-12 00:13:59
 *  Author: jan.pleszynski
 */ 
#include "ds_line.h"


// Error Codes
#define DS_DISCONNECTED_RAW -7040
#define DS_RESET_VAL 85
#define	DS_LINE_SHORTCUT_VAL 0
#define DS_DEFAULT_ID 0

//DS18B20
#define DS_RESOULTION 11

Ds_line::Ds_line(Channel *channel, Memory *mem) : Ds_line_low_level(channel->pin), Line(channel, mem)
{
	//TODO: Przepinanie czujnikow podczas pracy urzadzenia
	this->begin();
	//this->reset_dss_hardware_id();
	this->num_of_dss_in_mem = mem->get_num_of_all_ds();
	// Jesli w pamieci nie ma zadnych DSow w pamieci to wczytaj DSy z linii do pamieci
	if(this->is_connected)
	{
		this->compare_line_to_mem();
	}
}

void Ds_line::begin(void)
{
	ds_address device_address;
	this->reset_search();
	this->num_of_dss_on_line = 0; // Reset the number of devices when we enumerate wire devices

	while (this->search(device_address))
	{
		if (valid_address(device_address))
		{
			if (!is_parasite && read_power_supply(device_address))
			{
				is_parasite = true;
			}
			this->num_of_dss_on_line++;
		}
	}

	if(this->num_of_dss_on_line)
	{
	this->is_connected = true;
	}

	uint8_t prev_connection_state = mem->get_state(this->channel->get_num());
	this->check_state(prev_connection_state);
	this->mem->set_type(this->channel->get_num(), ONE_WIRE);
	this->set_resolution(DS_RESOULTION);
}

void Ds_line::read()
{
	uint8_t ds_line_short_counter = 0;
	for (uint8_t ds_num=0; ds_num<num_of_ds_objects; ds_num++)
	{
		//TODO: Przetestowac mechanizm zwarcia linii
		ds_sensor *current_sensor = this->dss_objects[ds_num];
		//Jesli czujnik nie jest zgubiony to odczytaj wartosc
		if (this->is_ds_connected(current_sensor->address))
		{
			current_sensor->val = this->get_temp(current_sensor->address);
			current_sensor->correct_measurement_num +=1;
			this->check_line_short(current_sensor, &ds_line_short_counter);
		}
		//Jesli czujnik jest zgubiony to probuj nawiazac komunikacje
		else
		{
			current_sensor->val = DS_DISCONNECTED_RAW;
			this->mem->set_ds_state(current_sensor->mem_id, LOST);
			if (this->is_ds_connected(current_sensor->address))
			{
				this->mem->set_ds_state(current_sensor->mem_id, CONNECTED);
			}
		}
	}
}

void Ds_line::compare_line_to_mem()
{
	// porownuje linie z pamiecia i mowi czy czujnik:
	// 1 Jest na linni i w pamieci -> Tworz obiekt.
	// 2 Nie ma na linni ale jest w pamieci FLAGA ZGUBIONY-> Dzieki fladze obiekt czujnika zamiast odczytywac swoja wartosc probuje odnalezc sie na linii
	// 3 Jest na linii ale nie ma w pamieci FLAGA NOWY -> Tworz nowy obiekt z flaga NOWY i dodaj go na koniec adresow przechowywanych w pamieci.
	
	//Sprawdzenie sytuacji 1 i 2
	//Iteracja po wszystkich komorkach pamieci zaiwrajacych adresy dsow

	// Jesli to nei jest pierwsze wykrycie linii


	//Wczytywanie czujników z pamiêci
	uint8_t ds_object_id = 0;
	for (uint8_t mem_id=0; mem_id<DS_MAX_SENSORS; mem_id++)
	{
		uint8_t mem_address[DS_SIZE_ADDRESS];
		//Jesli adres jest pusty przeszukuj dalej
		if(!this->mem->get_ds_address(mem_id, mem_address))
		{
			continue;
		}

		if (mem->get_ds_line_num(mem_id) == this->channel->get_num())	//Jesli czujnik nale¿y do linii
		{
			this->dss_objects[ds_object_id] = new ds_sensor(this->channel->get_num(), mem_id, mem_address);	//Obiekt ds dodany do tablicy dsow
			ds_object_id++;
			if(is_ds_connected(mem_address))
			{
				this->mem->set_ds_state(mem_id, CONNECTED);
			}
			//Sytuacja 2
			else
			{
				if(mem->get_ds_state(mem_id) != SWITCHED)
				{
					this->mem->set_ds_state(mem_id, LOST);
				}
			}
		}
		else
		{
			//Sprawdz czy czujnik nie zamieniony pomiedzy liniami
			if (is_ds_connected(mem_address))
			{
				this->dss_objects[ds_object_id] = new ds_sensor(this->channel->get_num(), mem_id, mem_address);
				ds_object_id++;
				this->mem->set_ds_state(mem_id, SWITCHED);
				this->mem->set_ds_line_num(mem_id, this->channel->get_num());
			}
		}
	}

	// Stacja obsluguje tylko DS_MAX_SENSORS czujnikow
	if (this->num_of_dss_in_mem == DS_MAX_SENSORS)
	{
		return;
	}

	//Dodawanie nowych czujników
	uint8_t mem_id;
	mem_id = this->num_of_dss_in_mem;
	for (uint8_t line_id=0; line_id<this->num_of_dss_on_line; line_id++)
	{
		uint8_t line_address[DS_SIZE_ADDRESS];
		this->get_address(line_address, line_id);
		if(!this->mem->contains_ds(line_address))
		{
			uint8_t ds_hardware_id = this->get_ds_hardware_id(line_address);
			uint8_t id;
			if (ds_hardware_id == DS_DEFAULT_ID)	//Fabrycznie nowy czujnik. Lub przywrocony do wartosci fabrycznej
			{
				id = mem_id;
				this->set_ds_hardware_id(line_address, mem_id);
			}
			else
			{
				id = ds_hardware_id;
			}
			this->dss_objects[ds_object_id] = new ds_sensor(this->channel->get_num(), mem_id, line_address);	//Obiekt ds dodany do tablicy dsow
			this->mem->set_ds_state(id, NEW);
			this->mem->set_ds_line_num(id, this->channel->get_num());
			this->mem->set_ds_mean_samples(id, MAX_AVERAGE_BUFFER_SIZE);
			this->mem->set_ds_address(id, line_address);
			ds_object_id++;
			mem_id++;
		}
	}
	this->num_of_ds_objects = ds_object_id;
	this->num_of_dss_in_mem = mem_id;
	this->mem->set_num_of_all_ds(this->num_of_dss_in_mem);
}

void Ds_line::check_line_short(ds_sensor *sensor, uint8_t *ds_line_short_counter)
{ 

	if (sensor->val == DS_LINE_SHORTCUT_VAL )
	{
		*ds_line_short_counter+=1;
		if (*ds_line_short_counter == this->num_of_dss_on_line)
		{
			mem->set_state(this->channel->get_num(), SHORT);
			//Linia zwarta
			for (uint8_t ds_num=0; ds_num<this->num_of_ds_objects; ds_num++)
			{
				ds_sensor *current_sensor = this->dss_objects[ds_num];
				current_sensor->val = ERROR_VAL;
				current_sensor->correct_measurement_num -=1;
			}
		}
	}
}

void Ds_line::set_resolution(uint8_t newResolution)
{
	uint8_t bitResolution = newResolution;
	ds_address device_address;
	for (int i=0; i<this->num_of_dss_on_line; i++)
	{
		get_address(device_address, i);
		this->set_one_resolution(device_address, bitResolution);
	}
}

void Ds_line::reset_dss_hardware_id()
{
	for (uint8_t ds_num=0; ds_num<this->num_of_dss_on_line; ds_num++)
	{
		uint8_t line_address[DS_SIZE_ADDRESS];
		get_address(line_address, ds_num);
		set_ds_hardware_id(line_address, DS_DEFAULT_ID);
	}
}