/* 
* Ds_line_hardware.h
*
* Created: 2016-03-04 14:13:10
* Author: jan.pleszynski
*/


#ifndef __DS_LINE_HARDWARE_H__
#define __DS_LINE_HARDWARE_H__

#include "OneWire.h"
#include "../../utilities/io/pin/pin.h"
#include <stdint.h>

class Ds_line_low_level	: public OneWire
{
// functions
public:
	Ds_line_low_level(IO_pin *pin);
	
	//Prosba o rozpoczecie konwersji
	void request();

protected:

	// returns true if address is valid
	bool valid_address(const uint8_t*);

	// finds an address at a given index on the bus
	bool get_address(uint8_t*, uint8_t);

	// attempt to determine if the device at the given address is connected to the bus
	bool is_ds_connected(const uint8_t*);

	// read device's scratchpad
	void read_scratchpad(const uint8_t*, uint8_t*);

	// write device's scratchpad
	void write_scratch_pad(const uint8_t*, const uint8_t*);

	// read device's power requirements
	bool read_power_supply(const uint8_t*);

	// get global resolution
	uint8_t get_resolution();

	// set global resolution to 9, 10, 11, or 12 bits
	void set_resolution(uint8_t);

	// returns the device resolution: 9, 10, 11, or 12 bits
	uint8_t get_resolution(const uint8_t*);

	// set resolution of a device to 9, 10, 11, or 12 bits
	bool set_one_resolution(const uint8_t*, uint8_t);

	// sends command for one device to perform a temperature conversion by address
	bool requestTemperaturesByAddress(const uint8_t*);

	// sends command for one device to perform a temperature conversion by index
	bool requestTemperaturesByIndex(uint8_t);

	// reads scratchpad and returns the raw temperature
	int16_t calculateTemperature(const uint8_t*, uint8_t*);

	// returns temperature raw value (12 bit integer of 1/16 degrees C)
	int16_t get_temp(const uint8_t*);

	//Sets id in dallas memory
	void set_ds_hardware_id (uint8_t* deviceAddress, uint8_t id );
	
	//Gets id from dallas memory
	uint8_t get_ds_hardware_id(uint8_t* deviceAddress);	
private:

// variables
public:
protected:
	typedef uint8_t ScratchPad[9];

	// parasite power on or off
	bool is_parasite;
private:



}; //Ds_line_hardware

#endif //__DS_LINE_HARDWARE_H__
