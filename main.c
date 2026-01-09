#include <avr/io.h>
#include <avr/interrupt.h>

int main(void)
{
    DDRB = 0xFF;

    ADMUX = (1 << REFS0);                  // AVcc ref
    ADCSRA = (1 << ADEN) |                 // ADC enable
             (1 << ADPS2) | (1 << ADPS1);  // prescaler 64

    while (1)
    {
        ADCSRA |= (1 << ADSC);
        while (ADCSRA & (1 << ADSC))
            ;

        PORTB = ~ADCL;
    }
}
