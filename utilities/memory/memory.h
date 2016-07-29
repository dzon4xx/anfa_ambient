#ifndef EEPROM_H_
#define EEPROM_H_
/*
 * eeprom.h
 *
 * Created: 2015-11-14 19:19:23
 *  Author: jan.pleszynski
 */ 

 //dlugosc nazwy 1. eeprom 92 columns 12

/*SETTINGS
1					4					3				1						1
MODULE_NAME	MODULE_PRODUCTION_TIME	BAUDE_RATE	TRANSSMISION_MODE	 	DOUBLE_PRECISSION

SENSORS DATA
			  1		  1			1			n
SENSOR1		STATE	TYPE	MEAN_SAMPLES	NAME
SENSOR2		STATE	TYPE	MEAN_SAMPLES	NAME
SENSOR3		STATE	TYPE	MEAN_SAMPLES	NAME
SENSOR4		STATE	TYPE	MEAN_SAMPLES	NAME

ONEWIRE LINES
ONE_WIRE_0  STATE	TYPE	DS_NUM			NAME
ONE_WIRE_1	STATE	TYPE	DS_NUM			NAME
									n
DSS_0		ADDDRESS	1W_NUM	STATE		MEAN_SAMPLES	NAME	
DSS_1		ADDDRESS	1W_NUM	STATE		MEAN_SAMPLES	NAME	
DSS_2		ADDDRESS	1W_NUM	STATE		MEAN_SAMPLES	NAME	

*/
#include <stdint.h>


class Memory
{
	public:
	
	Memory();
	//Setters
	
	void restore_memory_to_default();

	//Ustawia nazwe modulu. Nazwa to jedna litera odpowiadajaca danemu modulowi.
	void set_module_name(char module_name);
	
	//Ustawia czas produkcji w sekudnach. Jest to liczba 4 bajtowa. Czas produkcji jednoznacznie okresla dane urzadzenie.
	void set_production_time(unsigned long production_time);

	//Ustawia adres modbus urzadzenia
	void set_modbus_address(uint8_t modbus_address);

	//Ustawia predkosc transmisji modbus.
	void set_baud_rate(unsigned long baude_rate);

	//Ustawia kod trybu pracy komunikacji modbus.
	void set_transmission_mode(uint8_t transmission_mode);
		
	//Ustawia czy podwojna precyzja zwrotu danych jest aktywna
	void set_double_precission(bool is_double_precission);

	void set_num_of_all_ds(uint8_t num_of_ds);

	//Ustawia typ czujnika podpiety pod dany kanal.
	void set_type(uint8_t channel, uint8_t who);
	
	//Ustawia stan czujnika podpietego do stacji. Dostepne stany to EXISTING NEW LOST.
	void set_state(uint8_t channel,  uint8_t state);
	
	void set_ds_state(uint8_t ds_num,  uint8_t state);

	//Ustawia linie 1W do ktorej nalezy czujnik
	void set_ds_line_num(uint8_t ds_num,  uint8_t line_num);

	//Ustawia ilosc probek do usredniania
	void set_mean_samples(uint8_t channel, uint8_t mean_samples);
	
	void set_ds_mean_samples(uint8_t ds_num, uint8_t mean_samples);



	//Ustawia precyzje zwracanego wyniku
	//void set_precision(uint8_t channel, uint8_t precision, uint8_t ds_num=0);
	
	//Ustawia nazwe czujnika.
	void set_name(uint8_t channel, char name[]);
	
	void set_ds_name(uint8_t ds_num, char name[]);

	//Ustawia ilosc czujnikow ds znajdujacych sie w pamieci. Nie trzeba wyliczac ich z ilosci zapisanych adresow.
	void set_num_of_ds(uint8_t num_od_ds);
		
	//Ustawia adres czujnika ds18b20.
	void set_ds_address(uint8_t ds_num, uint8_t address[]);
		
	//Getters
	char get_module_name();

	long get_production_time();

	uint8_t get_modbus_address();

	long get_baud_rate();

	uint8_t get_transmission_mode();

	bool get_double_precission();
	
	uint8_t get_num_of_all_ds();

	uint8_t get_num_of_ds(uint8_t channel);

	uint8_t get_type(uint8_t channel);

	uint8_t get_state(uint8_t channel);

	uint8_t get_ds_state(uint8_t ds_num);

	uint8_t get_ds_line_num(uint8_t ds_num);

	uint8_t get_mean_samples(uint8_t channel);

	uint8_t get_ds_mean_samples(uint8_t ds_num);

	void get_name(uint8_t channel, char name[]);

	void get_ds_name(uint8_t ds_num, char name[]);

	bool get_ds_address(uint8_t ds_num, uint8_t address[]);
	
	//Przywraca nazwe czujnika do ustawien fabrycznych
	void clear_name(uint8_t channel);

	//Przywraca nazwe czujnika do ustawien fabrycznych
	void clear_ds_name(uint8_t ds_num);

	//Sprawdza czy czujnik ds18b20 o podanym adresie znajduje sie w pamieci
	bool contains_ds(uint8_t address[]);
	
	//Sprawdza czy pamiec jest pusta. Pamiec kazdego nowo zaprogramowanego urzadzenia jest pusta.
	bool is_empty();
		
	#ifdef DEBUG

	void print_eeprom();

#endif
	
	private:
	
	void write(unsigned int addr, uint8_t val);

	uint8_t read(unsigned int addr);

	//Ustawia tablice z nazwa czujnika na wskazana pozycje	
	void write_name(unsigned int position, char name[]);
	
	//Odczytuje nazwê czujnika ze wskazanej pozycji
	void read_name(unsigned int position, char name[]);


	//Zwraca pozycje atrybutu w pamiêci dla czujnika zadeklarowanego na danym kanale
	unsigned int get_position(uint8_t channel, unsigned int (*pos_fun)(uint8_t));
		
	//Uaktualnia wartosc w pamieci. Jesli wartosc nie ulegla zmianie nie przeprowadza zapisu
	void update(unsigned int pos, uint8_t data);
	
	// Konwertuje zmienna typu int do 2 elementowej tablicy typu uint8_t
	void int_to_uint8_t_array(int num, uint8_t* array);
	
	// Konwertuje zmienna typu long do 4 elementowej tablicy typu uint8_t
	void long_to_uint8_t_array(long num, uint8_t* array);
	
	// Konwertuje 2 elementowa tablice typu uint8_t do zmiennej typu int 
	unsigned int uint8_t_array_to_int(uint8_t* array);
	
	// Konwertuje 4 elementowa tablice typu uint8_t do zmiennej typu long
	unsigned long uint8_t_array_to_long(uint8_t* array);
};



#endif /* EEPROM_H_ */