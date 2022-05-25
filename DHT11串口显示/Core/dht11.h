#ifndef __DHT11_H_
#define __DHT11_H_

#include "main.h"

sbit DHT11_DIO = P1 ^ 0; // dht11data端接单片机的P1^0口//

uint8 DHT11_Read(uint8 *humidity, uint8 *temperature);
#endif