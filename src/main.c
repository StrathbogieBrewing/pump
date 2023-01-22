#include <avr/io.h>
#include <string.h>
#include <util/delay.h>

// #include "trace.h"

/* AVR GPIO helper macros */
#define GPIO_INPUT 0
#define GPIO_OUTPUT 1
#define GPIO_SET_BIT(type, port, bit) (type##port |= (1 << bit))
#define GPIO_CLEAR_BIT(type, port, bit) (type##port &= ~(1 << bit))
#define GPIO_GET_BIT(type, port, bit) ((type##port & (1 << bit)) ? 1 : 0)
#define GPIO_SET(gpio, value) (value ? GPIO_SET_BIT(PORT, gpio) : GPIO_CLEAR_BIT(PORT, gpio))
#define GPIO_GET(gpio) (GPIO_GET_BIT(PIN, gpio))
#define GPIO_MODE(gpio, mode) (mode ? GPIO_SET_BIT(DDR, gpio) : GPIO_CLEAR_BIT(DDR, gpio))

/* AVR adc helper function*/
// set ADC prescaler for 50-200 KHz
#if F_CPU >= 16000000 // 16 MHz / 128 = 125 KHz
#define ADC_ADPS 7
#elif F_CPU >= 8000000 // 8 MHz / 64 = 125 KHz
#define ADC_ADPS 6
#elif F_CPU >= 4000000 // 4 MHz / 32 = 125 KHz
#define ADC_ADPS 5
#elif F_CPU >= 2000000 // 2 MHz / 16 = 125 KHz
#define ADC_ADPS 4
#elif F_CPU >= 1000000 // 1 MHz / 8 = 125 KHz
#define ADC_ADPS 3
#else // 128 kHz / 2 = 64 KHz -> This is the closest you can get, the prescaler is 2
#define ADC_ADPS 1
#endif

uint16_t adc_get(uint8_t channel) {
    ADCSRA = (1 << ADEN) | ADC_ADPS;
    ADMUX = (1 << REFS0) | (channel & 0x07);
    ADCSRA |= (1 << ADSC);
    while (ADCSRA | (1 << ADSC))
        ;
    return ADC;
}

#define PUMP_BORE B, 0
#define VALVE_PATIO B, 1
#define VALVE_HOTHOUSE D, 4
#define VALVE_CAGE D, 5

int main(void) {

    // trace_init();

    GPIO_MODE(PUMP_BORE, GPIO_OUTPUT);
    GPIO_SET(PUMP_BORE, 1);

    uint8_t seconds = 0;

    while (1) {
        // 4 second delay
        for (uint8_t i = 0; i < 200; i++) {
            _delay_ms(20);
        }

        seconds++;
        uint8_t phase = seconds >> 5;
        switch (phase) {
        case 0:
            if (GPIO_GET(VALVE_HOTHOUSE)) {
                GPIO_SET(PUMP_BORE, 1);
            } else {
                GPIO_SET(PUMP_BORE, 0);
            }

            break;
        case 1:
            GPIO_SET(PUMP_BORE, 1);
            break;
        case 2:

            break;
        case 3:

            break;
        default:

            break;
        }

        // trace_write((uint8_t[]){seconds, phase}, 2);
    }
    return 0;
}
