#include <avr/io.h>
#include <avr/interrupt.h>

#define ADCPIN 0 // ADC0

int main(void)
{
    DDRB = 0xFF;
    DDRC = 0x00;
    // DIDR0 = (1 << ADC0D);                               // disable digital buffer
    ADMUX = (1 << REFS0);                               // AVcc reference
    ADCSRA = (1 << ADEN) | (1 << ADIE) | (1 << ADPS2) | (1 << ADPS1); // prescaler 64 = (8MHz / 64 = 125KHz)
    sei();


    while (1)
    {
        ADMUX |= (ADMUX & 0xF0) | (ADCPIN & 0x0F); // select ADC channel and mask the rest ADMUX
        ADCSRA |= (1 << ADSC);         // start conversion
        while (ADCSRA & (1 << ADSC));  // wait
    }
}

ISR(ADC_vect)
{
    PORTB = ~ADCL; // read ADC low byte and output to PORTB
}   
