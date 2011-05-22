/*
avrdude -c usbtiny -patn85 -U flash:w:main.hex
 */

#include "version.h"
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>  /* for sei() */
#include <avr/pgmspace.h>
#include "midi.h"


#define R_BIT            1
#define G_BIT            3
#define B_BIT            4
#define BZ_BIT           0

static volatile uint8_t r = 0;
static volatile uint8_t g = 0;
static volatile uint8_t b = 0;
/*

                 +-------------------+
                 |1 RST         VCC 8|
          --<R|--|2 PB3    PB2/INT0 7|
          --<G|--|3 PB4         PB1 6|--|B>--
                 |4 GND         PB0 5|--|-|--
                 +-------------------+
                       ATTiny85
*/

PROGMEM uint8_t _pwmTab[10] = {0, 3, 20, 30, 40, 50, 60, 70, 100, 150};
#define pwmTab(x) pgm_read_byte_near(_pwmTab+(x))

const uint8_t ledprog[10][3] =
{
    {0,9,5},
    {3,0,0},
    {3,0,1},
    {0,3,0},
    {5,9,0},
    {0,0,3},
    {5,9,0},
    {2,0,1},
    {9,9,0},
    {0,0,5}
};

/*

static double midi3Freq(uint8_t nota)
{
    static double passo = 1.059463094359300;
    return 8.1758 + (nota * passo);
}
*/

void update_leds()
{
    static uint8_t count = 0;
    static uint8_t rb = 0;
    static uint8_t gb = 0;
    static uint8_t bb = 0;

    if (++count == 0)
    {
        rb = r;
        gb = g;
        bb = b;
        /* check if switch on r, g and b */
        if (rb > 0)
        {
            PORTB |= (1 << R_BIT);
        }
        if (gb > 0)
        {
            PORTB |= (1 << G_BIT);
        }
        if (bb > 0)
        {
            PORTB |= (1 << B_BIT);
        }
    }

    /* check if switch off r, g and b */
    if (count == rb)
    {
        PORTB &= ~(1 << R_BIT);
    }
    if (count == gb)
    {
        PORTB &= ~(1 << G_BIT);
    }
    if (count == bb)
    {
        PORTB &= ~(1 << B_BIT);
    }
}

int main(void)
{
    uint8_t i;
    uint8_t nota = 0;
    uint16_t t;

    wdt_enable(WDTO_1S);
    DDRB |= _BV(R_BIT);   /* make the LED bit an output */
    DDRB |= _BV(G_BIT);   /* make the LED bit an output */
    DDRB |= _BV(B_BIT);   /* make the LED bit an output */
    DDRB |= _BV(BZ_BIT);   /* make the Buzzer bit an output */

    TCCR0A = 0x42;
    TCCR0B = 0x00;

    while(1)
    {
        play(nota);

        if (++nota==128) {
            nota = 0;
        }

        for (i = 0; i < 10; i++)
        {
            r = pwmTab(ledprog[i][0]);
            g = pwmTab(ledprog[i][1]);
            b = pwmTab(ledprog[i][2]);

            for (t = 0; t < 500; t++)
            {
                update_leds();
            }
            wdt_reset();
        }
    }
    /* _delay_ms(1); */

    return 0;
}
