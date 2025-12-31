#include <avr/io.h>
#include <avr/interrupt.h>

/* ---------- ADC ---------- */
void ADC_init(void)
{
    // AVcc reference
    ADMUX = (1 << REFS0) | (0 << ADLAR);

    // Enable ADC, prescaler = 64 (8MHz / 64 = 125kHz)
    ADCSRA = (1 << ADEN) |
             (1 << ADPS2) | (1 << ADPS1);
}

uint8_t ADC_read(uint8_t channel)
{
    // Select channel (0–7)
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);

    // Start conversion
    ADCSRA |= (1 << ADSC);

    // Wait for conversion to finish
    while (ADCSRA & (1 << ADSC));

    // Return 8 MSBs
    return ADCL;
}

int main(void)
{
    ADC_init();

    sei();

    while (1)
    {
        // Read potentiometer / analog input
        PORTD = ~ADC_read(0);
    }
}