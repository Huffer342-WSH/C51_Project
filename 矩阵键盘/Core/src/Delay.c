#include "Delay.h"
#include "intrins.h"
void Delayms(unsigned int t)		//@11.0592MHz
{
	unsigned char i, j;
    while(t--){
    	_nop_();
        i = 11;
        j = 185;
        do
        {
            while (--j);
        } while (--i);
    }
}



#if 0
void Delay100us()		//@11.0592MHz
{
	unsigned char i, j;

	i = 2;
	j = 15;
	do
	{
		while (--j);
	} while (--i);
}
#endif