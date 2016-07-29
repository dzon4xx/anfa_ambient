/* 
* Ds_line_hardware.cpp
*
* Created: 2016-03-04 14:13:10
* Author: jan.pleszynski
*/

#include "Ds_line_low_level.h"
#include <util/delay.h>

// Model IDs
#define DS18S20MODEL 0x10  // also DS1820
#define DS18B20MODEL 0x28
#define DS1822MODEL  0x22
#define DS1825MODEL  0x3B

// OneWire commands
#define STARTCONVO      0x44  // Tells device to take a temperature reading and put it on the scratchpad
#define COPYSCRATCH     0x48  // Copy EEPROM
#define READSCRATCH     0xBE  // Read EEPROM
#define WRITESCRATCH    0x4E  // Write to EEPROM
#define RECALLSCRATCH   0xB8  // Reload from last known
#define READPOWERSUPPLY 0xB4  // Determine if device needs parasite power
#define ALARMSEARCH     0xEC  // Query bus for devices with an alarm condition

// Scratchpad locations
#define TEMP_LSB        0
#define TEMP_MSB        1
#define HIGH_ALARM_TEMP 2
// 3 bajt scratchpadu to id dsa, ktore pokrywa sie z jego pozycja w pamieci eeprom.
#define DS_ID			3
#define LOW_ALARM_TEMP  3
#define CONFIGURATION   4
#define INTERNAL_uint8_t   5
#define COUNT_REMAIN    6
#define COUNT_PER_C     7
#define SCRATCHPAD_CRC  8
#define SCRATCHPAD_NUM_OF_uint8_tS 9

// Device resolution
#define TEMP_9_BIT  0x1F //  9 bit
#define TEMP_10_BIT 0x3F // 10 bit
#define TEMP_11_BIT 0x5F // 11 bit
#define TEMP_12_BIT 0x7F // 12 bit

Ds_line_low_level::Ds_line_low_level(IO_pin *pin): OneWire(pin)
{} 

void Ds_line_low_level::request()
{
	this->reset();
	this->skip();
	this->write(STARTCONVO, is_parasite);
}

bool Ds_line_low_level::valid_address(const uint8_t* device_address)
{
	return (this->crc8(device_address, 7) == device_address[7]);
}

bool Ds_line_low_level::get_address(uint8_t* device_address, uint8_t index)
{
	uint8_t depth = 0;
	this->reset_search();
	while (depth <= index && this->search(device_address))
	{
		if (depth == index && this->valid_address(device_address)) return true;
		depth++;
	}
	return false;
}

bool Ds_line_low_level::read_power_supply(const uint8_t* device_address)
{
	bool ret = false;
	this->reset();
	this->select(device_address);
	this->write(READPOWERSUPPLY);
	if (this->read_bit() == 0) ret = true;
	this->reset();
	return ret;
}

void Ds_line_low_level::set_ds_hardware_id (uint8_t* device_address, uint8_t id)
{
	ScratchPad scratchPad;
	scratchPad[DS_ID] = id;
	write_scratch_pad(device_address, scratchPad);
}

uint8_t Ds_line_low_level::get_ds_hardware_id(uint8_t* device_address)
{
	ScratchPad scratchPad;
	read_scratchpad(device_address, scratchPad);
	return scratchPad[DS_ID];
}

bool Ds_line_low_level::is_ds_connected(const uint8_t* device_address)
{
	ScratchPad scratchPad;
	read_scratchpad(device_address, scratchPad);
	return (this->crc8(scratchPad, 8) == scratchPad[SCRATCHPAD_CRC]);
}

bool Ds_line_low_level::set_one_resolution(const uint8_t* device_address, uint8_t newResolution)
{
	ScratchPad scratchPad;
	this->read_scratchpad(device_address, scratchPad);
	if (is_ds_connected(device_address))
	{
		// DS1820 and DS18S20 have no resolution configuration register
		if (device_address[0] != DS18S20MODEL)
		{
			switch (newResolution)
			{
				case 12:
				scratchPad[CONFIGURATION] = TEMP_12_BIT;
				break;
				case 11:
				scratchPad[CONFIGURATION] = TEMP_11_BIT;
				break;
				case 10:
				scratchPad[CONFIGURATION] = TEMP_10_BIT;
				break;
				case 9:
				default:
				scratchPad[CONFIGURATION] = TEMP_9_BIT;
				break;
			}
			write_scratch_pad(device_address, scratchPad);
		}
		return true;  // new value set
	}
	return false;
}

int16_t Ds_line_low_level::get_temp(const uint8_t* device_address)
{
	ScratchPad scratch_pad;
	read_scratchpad(device_address, scratch_pad);
	return calculateTemperature(device_address, scratch_pad);
}

int16_t Ds_line_low_level::calculateTemperature(const uint8_t* device_address, uint8_t* scratchPad)
{
	int temp = (((int16_t) scratchPad[TEMP_MSB]) << 11) |
	(((int16_t) scratchPad[TEMP_LSB]) << 3);
	return  temp;
}

void Ds_line_low_level::write_scratch_pad(const uint8_t* device_address, const uint8_t* scratchPad)
{
	this->reset();
	this->select(device_address);
	this->write(WRITESCRATCH);
	this->write(scratchPad[HIGH_ALARM_TEMP]); // high alarm temp
	this->write(scratchPad[LOW_ALARM_TEMP]); // low alarm temp
	// DS1820 and DS18S20 have no configuration register
	if (device_address[0] != DS18S20MODEL) this->write(scratchPad[CONFIGURATION]); // configuration
	this->reset();
	this->select(device_address); //<--this line was missing
	// save the newly written values to eeprom
	this->write(COPYSCRATCH, is_parasite);
	_delay_ms(20);  // <--- added 20ms delay to allow 10ms long EEPROM write operation (as specified by datasheet)
	if (is_parasite) _delay_ms(10); // 10ms delay
	this->reset();
}

void Ds_line_low_level::read_scratchpad(const uint8_t* device_address, uint8_t* scratchPad)
{
	// send the command

	this->reset();
	this->select(device_address);
	this->write(READSCRATCH);
	for(int i=0; i<SCRATCHPAD_NUM_OF_uint8_tS; i++)
	{
		scratchPad[i] = OneWire::read();
	}
	this->reset();
}

