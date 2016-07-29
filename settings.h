

#ifndef SETTINGS_H_
#define SETTINGS_H_

#define ATMEGA328

///////////////TYPY OBSLUGIWANYCH CZUJNIKOW///////////////
#define ONE_WIRE 1
///////////////////STATES/////////////////////////

#define	NOT_CONNECTED	0
#define	CONNECTED		1
#define	LOST			2
#define	NEW				3
#define	SHORT			4
#define	SWITCHED		5


#define EMPTY_CELL 0xff


#define		NUM_OF_CHANNELS 5
////////////////////PINS///////////////////////////

///////////////////COMMUNICATION///////////////////
#define MEASURE_PERIOD 700				//  Okres pomiaru stacji 

///////////////////COMMUNICATION///////////////////

//BASE SENSOR
#define MAX_AVERAGE_BUFFER_SIZE 10


/////////////////////////////////////////////////////
#define DS_MAX_SENSORS 32
#define DS_SIZE_ADDRESS 8		//Adres czujnika ds18b20
#define DS_MAX_NUM_OF_LINES 2
//////////////////ERRORS/////////////////////////////
 

#endif /* SETTINGS_H_ */
