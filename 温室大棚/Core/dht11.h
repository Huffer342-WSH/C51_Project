#ifndef __DHT11_H_
#define __DHT11_H_

#include "main.h"
/*Òý½Å*/
#define DHT11_DIO DHT11_DIO

uint8 DHT11_Read(uint8 *humidity, uint8 *temperature);
#endif