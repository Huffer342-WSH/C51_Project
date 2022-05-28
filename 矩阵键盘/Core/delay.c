#include "delay.h"
#include "intrins.h"
void Delayms(unsigned int t) //@11.0592MHz
{
    unsigned char i, j;
    while (t--)
    {
        _nop_();
        _nop_();
        i = 11;
        j = 187;
        do
        {
            while (--j)
                ;
        } while (--i);
    }
}

#if 1
void Delayus(unsigned int t) //@11.0592MHz
{
    while (t--)
        _nop_();
}
#endif

void Delay50us() //@11.0592MHz
{
    unsigned char i;

    _nop_();
    i = 135;
    while (--i)
        ;
}