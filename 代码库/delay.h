#ifndef __DELAY_H_
#define __DELAY_H_

#define FREQ11
// #define FREQ12
#define delayms Delayms
#define delayus Delayus
#define delay50us Delay50us
void Delayms(unsigned int t); //@11.0592MHz
void Delayus(unsigned int t);
void Delay50us();
#endif