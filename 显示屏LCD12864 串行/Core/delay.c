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



#if 1
void Delayus(unsigned int t) //@11.0592MHz—”≥ŸtŒ¢√Î
{
	while (t--)
		_nop_();
}
#endif