
#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <util/delay.h>

#define ADCPIN 0 // ADC0

void ADC_init(void)
{
    ADMUX  = (1 << REFS0);                 // AVcc reference, start on ADC0
    ADCSRA = (1 << ADEN)  |                // Enable ADC
             (1 << ADIE)  |                // Enable ADC interrupt
             (1 << ADPS2) | (1 << ADPS1);  // Prescaler 64 → 125 kHz

    ADCSRA |= (1 << ADSC);                 // Start first conversion
}

void UART_init(void)
{
        /* UART init */    
    UCSR0A = 0;                             // Reset to default
    UCSR0B = (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // Master SPI
    UBRR0L = 51;                            // 9600 baud at 8MHz
    UBRR0H = 0;                             // Write high byte to 0
}
/* So called callback function for stdout - replaces stdout and streams characters to /dev/ttyUSBn via UART transmit register */
static int log_putchar(char c, FILE *stream)
{
    // wait for empty transmit buffer and then send data
    while (!(UCSR0A & (1 << UDRE0)))
        ;
    UDR0 = c;
    return 0;
}

int main(void)
{
    /* Hijacks stdout to UART */
    static FILE log_stdout = FDEV_SETUP_STREAM(log_putchar, NULL, _FDEV_SETUP_WRITE);
    stdout = &log_stdout; // replaces stdout with log_stdout address - "reroutes" all printf calls to log_putchar function

    printf("ADC Test Program\r\n");

    DDRB = 0xFF;
    DDRC = 0x00;
    // DIDR0 = (1 << ADC0D);                            // disable digital buffer

    ADC_init();
    UART_init();

    while (1)
    {
        ADMUX |= (ADMUX & 0xF0) | (ADCPIN & 0x0F); // select ADC channel and mask the rest ADMUX
        ADCSRA |= (1 << ADSC);                     // start conversion
        while (ADCSRA & (1 << ADSC));              // wait for conversion to complete
        // PORTB = ~ADCL;                             // output result to PORTB
        printf("ADC%d: %d\r\n", ADCPIN, ADC);
        _delay_ms(333);
    }
}