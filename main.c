#include <avr/io.h>
#include <avr/interrupt.h>

void ADC_init(void)
{
    // AVcc reference
    ADMUX = (1 << REFS0);
    ADMUX &= ~(1 << ADLAR);

    // Enable ADC, prescaler = 64 (8MHz / 64 = 125kHz)
    ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1);
    ADCSRA &= ~(1 << ADPS0);
}

uint8_t ADC_read(uint8_t channel)
{
    // Select channel (0–7) by masking top and bottom bits
    // ADMUX bits 3:0 are a channel select and the binary representation of the channel correlates ot DECIMAL values 0-7
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);

    // Start conversion
    ADCSRA |= (1 << ADSC);

    // Wait for conversion to finish
    while (ADCSRA & (1 << ADSC));

    // At 10-bit resolution with 125 kHz ADC clock (with prescaler 128), one conversion takes ~7-8 µs × 13 cycles = ~101-102 µs

    // Only trust the second conversion result
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));

    // Return 8 MSBs
    return ADCL;
}

int main(void)
{
    ADC_init();

    // Set interrupts. Only do this when all the initializations are done
    sei();

    while (1)
    {
        // Read potentiometer / analog input
        PORTD = ~ADC_read(0);
    }
}