#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>

/* ADC and PWM initialization */
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
    TCCR0A = (1 << COM0A1) | (1 << WGM01) | (1 << WGM00); // Fast PWM
    TCCR0B = (1 << CS01); // Prescaler 8

    TCCR1A = (1 << COM1A1) | (1 << WGM12) | (1 << WGM10); // Fast PWM 8-bit
    TCCR1B = (1 << CS11); // Prescaler 8

    TCCR2A = (1 << COM2A1) | (1 << WGM21) | (1 << WGM20); // Fast PWM
    TCCR2B = (1 << CS21); // Prescaler 8
}

/* ADC interrupt service routine - fires when ADC conversion is complete */
ISR(ADC_vect)
{
    static uint8_t ch = 0;
    uint16_t v = ADC;

    switch (ch) {
        case 0: OCR0A = v >> 2; break;
        case 1: OCR1A = v;      break;
        case 2: OCR2A = v >> 2; break;
    }

    // if is more faster for AVR to do increment and compare than modulo
    ch++;
    if (ch >= 3)
    {
        ch = 0;
    }

    ADMUX = (ADMUX & 0xF0) | ch;            // Select next channel
    ADCSRA |= (1 << ADSC);                  // Start next conversion
}

int main(void)
{
    DDRC = 0x00;          // Set PORTC as input (ADC)
    DDRD = 0xFF;          // Set PORTB as output (PWM)

    ADC_init();
    PWM_init();

    // Enable sleep mode - SLEEP_MODE_IDLE means CPU is stopped, peripherals work
    set_sleep_mode(SLEEP_MODE_IDLE);
    // After every initialization enable global interrupts
    sei();

    // Main loop. CPU sleeps, ADC and PWM work in background
    while (1) {
        sleep_cpu();
    }
}
