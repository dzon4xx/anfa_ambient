/*
 * ambient.cpp
 *
 * Created: 2016-03-03 10:51:56
 * Author : jan.pleszynski
 */ 

 //TODO: Ogarnac poprawne wyswietlanie komunikatow o zmianie dsa z linii na linie. Pole SWITCHED

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "sensors/ds18b20/ds_sensor.h"
#include "sensors/ds18b20/ds_line.h"
#include "communication/modbus_slave.h"
#include "utilities/memory/memory.h"
#include "utilities/misc/misc.h"
#include "utilities/clock/clock.h"

#include "sensors/ds18b20/ds_line_low_level.h"
#include "utilities/io/pin/pin.h"


Memory mem;
Channel ch4(4);
Channel ch5(5);
Ds_line ds_line_1(&ch4, &mem);
Ds_line ds_line_2(&ch5, &mem);

void populate_regs();
int main(void)
{
	cli();
	frame		= new Frame;
	read_reg	= new Modbus_read_reg;
	clock		= new Clock;
	clock->start();
	sei();
    while (1) 
    {
		ds_line_1.request();
		ds_line_2.request();
		_delay_ms(MEASURE_PERIOD);
		ds_line_1.read();
		ds_line_2.read();
		populate_regs();
    }
}


void populate_regs()
{
	
	for (uint8_t ds_num = 0; ds_num<ds_line_1.num_of_ds_objects; ds_num++)
	{
		ds_sensor *ds = ds_line_1.dss_objects[ds_num];
		if (ds->is_val_correct())
		{
			read_reg->regs[ds->mem_id] = ds->get_val();
		}
	}

	for (uint8_t ds_num = 0; ds_num<ds_line_2.num_of_ds_objects; ds_num++)
	{
		ds_sensor *ds = ds_line_2.dss_objects[ds_num];
		if (ds->is_val_correct())
		{
			read_reg->regs[ds->mem_id] = ds->get_val();
		}
	}


	//read_reg->respond_flag = false;
}
