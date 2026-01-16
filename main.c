#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

void ADC_init(void)
{
    ADMUX  = (1 << REFS0);                 // AVcc reference, start on ADC0
    ADCSRA = (1 << ADEN)  |                // Enable ADC
             (1 << ADIE)  |                // Enable ADC interrupt
             (1 << ADPS2) | (1 << ADPS1);  // Prescaler 64 → 125 kHz

    ADCSRA |= (1 << ADSC);                 // Start first conversion
}

void PWM_init(void)
{
    TCCR0A = (1 << COM0A1) | (1 << WGM01) | (1 << WGM00);
    TCCR0B = (1 << CS01);

    TCCR1A = (1 << COM1A1) | (1 << WGM11);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11);

    TCCR2A = (1 << COM2A1) | (1 << WGM21) | (1 << WGM20);
    TCCR2B = (1 << CS22);
}

ISR(ADC_vect)
{
    static uint8_t ch = 0;
    uint16_t v = ADC;

    switch (ch) {
        case 0: OCR0A = v >> 2; break;
        case 1: OCR1A = v;      break;
        case 2: OCR2A = v >> 2; break;
    }

    ch = (ch + 1) % 3;
    ADMUX = (ADMUX & 0xF0) | ch;            // Select next channel
    ADCSRA |= (1 << ADSC);                  // Start next conversion
}

int main(void)
{
    DDRC = 0x00;          // Set PORTC as input (ADC)
    DDRD = 0xFF;          // Set PORTB as output (PWM)

    ADC_init();
    PWM_init();

    set_sleep_mode(SLEEP_MODE_IDLE);
    sei();

    while (1) {
        sleep_cpu();
    }
}
