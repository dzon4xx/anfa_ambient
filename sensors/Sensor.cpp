/*
 * Sensor.cpp
 *
 * Created: 2015-11-29 17:53:46
 *  Author: jan.pleszynski
 */ 
#include "Sensor.h"

bool Sensor::is_val_correct()
{
	if (this->val == ERROR_VAL)
	{
		return false;
	}
	else
	{
		return true;
	}
}

int16_t Sensor::average_value(int16_t actual_value)
{
	int32_t sum_value = 0;
	this->buffer[avarage_counter] = actual_value;
	avarage_counter==(this->mean_samples_num-1) ? avarage_counter = 0 : avarage_counter++;
	
	// Usredniaj bufor
	if(this->correct_measurement_num>=this->mean_samples_num)
	{
		for (uint8_t i=0; i<this->mean_samples_num; i++)
		{
			sum_value += buffer[i];
		}
		double averaged_value = (double)sum_value/(double)this->mean_samples_num;
		return 	(int16_t)averaged_value;
	}
	// Obsluga pierwszych prawid³owych pomiarow zeby program nie wyrzucal zanizonej wartosci
	else if(this->correct_measurement_num<this->mean_samples_num)
	{
		for (uint8_t i=0; i<this->correct_measurement_num; i++)
		{
			sum_value += this->buffer[i];
		}
	}
	double averaged_value = (double)sum_value/(double)correct_measurement_num;
	return (int16_t)averaged_value;
}

 Sensor::Sensor()
{
}

int16_t Sensor::get_val()
{
	int16_t avaraged_val = this->average_value(this->val);
	int16_t converted_val = this->convert_value(avaraged_val);
	return converted_val;
}

void Sensor::set_mean_sample(uint8_t mean_sample)
{
	this->mean_samples_num = mean_sample;
}

 Line::Line(Channel *channel, Memory *mem) : channel(channel), mem(mem) 
{}

void Line::check_state(uint8_t prev_connection_state)
{
	if (this->is_connected)
	{
		this->state	= CONNECTED;
		if(prev_connection_state == NEW || prev_connection_state == LOST)
		{	
			mem->set_state(this->channel->get_num(), CONNECTED);
		}
		else if (prev_connection_state == NOT_CONNECTED || prev_connection_state == EMPTY_CELL)
		{
			this->state = NEW;
			mem->set_state(this->channel->get_num(), NEW);
		}
	}

	else
	{
		this->state = LOST;
		if (prev_connection_state == CONNECTED || prev_connection_state == NEW)
		{
			mem->set_state(this->channel->get_num(), LOST);
		}
		else if (prev_connection_state == NOT_CONNECTED || prev_connection_state == EMPTY_CELL)
		{
			this->state = NOT_CONNECTED;
			mem->set_state(this->channel->get_num(), NOT_CONNECTED);
		}
	}
}
