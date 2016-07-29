/*
 * Sensor.h
 *
 * Created: 2015-11-29 17:54:02
 *  Author: jan.pleszynski
 */ 


#ifndef SENSOR_H_
#define SENSOR_H_
#include "../settings.h"
#include "../utilities/memory/memory.h"
#include "../utilities/io/channel/channel.h"

#define ERROR_VAL 9999

class Sensor
{
	public:
	Sensor();
		
	//Zwraca usredniona wartosc zmierzona przez czujnik. Liczba usrednien okreslona jest przez mean_samples_num
	int16_t get_val();

	void set_mean_sample(uint8_t mean_sample);
		

	//Generyczna zmienna dla mierzonej wartosci
	int16_t val = 0;

	//Ilosc poprawnie zmierzonych wartosci.
	long correct_measurement_num = 0;

	private:
	// Zwraca wartosc odczytana z czujnika. Obslugiwana przez kazda dziedziczaca klase oddzielnie
	virtual int16_t read_value(){return this->val;};
		
	//Sprawdza czy dana wartosc zostala zmierzona poprawnie. Specyficzna dla kazdego czujnika
	virtual bool is_val_correct();
	
	//Konwertuje wartosc z kazdego czujnika do wartosci zrozumialej dla czlowieka
	virtual int16_t convert_value(int16_t val){return this->val;};

	//Usrednia zmierzone wartosci. Liczba usrednien okreslona jest przez mean_samples_num. Jesli zmierzona wartosc jest nie prawidlowa nie jest brana pod uwage
	int16_t average_value(int16_t actual_value);
	
	protected:

	//Liczba próbek do uœrednianai
	uint8_t mean_samples_num = MAX_AVERAGE_BUFFER_SIZE;
	
	uint8_t avarage_counter = 0;
	
	//Bufor do usredniania pomiarow
	int16_t buffer[MAX_AVERAGE_BUFFER_SIZE];
};

class Line
{
	//TODO: ZROBIC LICZNIK KANALOW STATYCZNA ZMIENNA
	public:

	Line(Channel *channel, Memory *mem);
	
	bool is_connected = false;	

	protected:	//Dzieci linii beda zanly te atrybuty

	void check_state(uint8_t prev_connection_state);

	//Kanal na ktorym dziala linia one wire. Kana³ jest indeksem linni one wire w pamieci
	Channel *channel;
		
	//Wskaznik do pamieci
	Memory* mem;

	// Stan linii
	uint8_t state = NOT_CONNECTED;
};
#endif /* SENSOR_H_ */