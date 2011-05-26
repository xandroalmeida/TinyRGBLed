#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include "midi.h"

#define midi_prescaler_value(x) pgm_read_byte_near(&_midiT0Tab1MHZ[(x)][0])
#define midi_div_value(x)       pgm_read_byte_near(&_midiT0Tab1MHZ[(x)][1])

#define set_prescaler(x)        TCCR0B &= 0xf8;TCCR0B |= ((x)&0x7);GTCCR |= 1;
#define play(x)                 set_prescaler(midi_prescaler_value((x)));TCNT0=0;OCR0A=midi_div_value((x));
#define play_pause()            TCCR0B &= 0xf8;GTCCR |= 1;TCNT0=0;

#define wdt_set(value)   \
    __asm__ __volatile__ (  \
        "in __tmp_reg__,__SREG__" "\n\t"    \
        "cli" "\n\t"    \
        "wdr" "\n\t"    \
        "sts %0,%1" "\n\t"  \
        "out __SREG__,__tmp_reg__" "\n\t"   \
        "sts %0,%2" \
        : /* no outputs */  \
        : "M" (_SFR_MEM_ADDR(_WD_CONTROL_REG)), \
        "r" (_BV(_WD_CHANGE_BIT) | _BV(WDE)), \
            "r" ((uint8_t) ((value & 0x08 ? _WD_PS3_MASK : 0x00) | \
                _BV(WDIE) | (value & 0x07)) ) \
        : "r0"  \
    )

#define DIV256  4
#define DIV64   3
#define DIV8    2
#define DIV1    1

/* For more details about how this table was calculed, see the excel in this project
and this link about notes
    http://en.wikipedia.org/wiki/Note_(music)
    http://pt.wikipedia.org/wiki/Dura%C3%A7%C3%A3o_(m%C3%BAsica)
*/

PROGMEM static uint8_t _midiT0Tab1MHZ[128][2] = {
{DIV256,238},{DIV256,224},{DIV256,212},{DIV256,200},{DIV256,189},{DIV256,178},{DIV256,168},
{DIV256,158},{DIV256,149},{DIV256,141},{DIV256,133},{DIV256,126},{DIV256,118},{DIV256,112},
{DIV256,105},{DIV256,99},{DIV256,94},{DIV256,88},{DIV256,83},{DIV256,79},{DIV256,74},{DIV256,70},
{DIV256,66},{DIV64,252},{DIV64,238},{DIV64,224},{DIV64,212},{DIV64,200},{DIV64,189},{DIV64,178},
{DIV64,168},{DIV64,158},{DIV64,149},{DIV64,141},{DIV64,133},{DIV64,126},{DIV64,118},{DIV64,112},
{DIV64,105},{DIV64,99},{DIV64,94},{DIV64,88},{DIV64,83},{DIV64,79},{DIV64,74},{DIV64,70},
{DIV64,66},{DIV64,62},{DIV64,59},{DIV64,55},{DIV64,52},{DIV64,49},{DIV64,46},{DIV64,44},{DIV64,41},
{DIV64,39},{DIV64,37},{DIV64,35},{DIV64,33},{DIV64,31},{DIV64,29},{DIV8,224},{DIV8,212},
{DIV8,200},{DIV8,189},{DIV8,178},{DIV8,168},{DIV8,158},{DIV8,149},{DIV8,141},{DIV8,133},
{DIV8,126},{DIV8,118},{DIV8,112},{DIV8,105},{DIV8,99},{DIV8,94},{DIV8,88},{DIV8,83},{DIV8,79},
{DIV8,74},{DIV8,70},{DIV8,66},{DIV8,62},{DIV8,59},{DIV8,55},{DIV8,52},{DIV8,49},{DIV8,46},{DIV8,44},
{DIV8,41},{DIV8,39},{DIV8,37},{DIV8,35},{DIV8,33},{DIV1,252},{DIV1,238},{DIV1,224},{DIV1,212},
{DIV1,200},{DIV1,189},{DIV1,178},{DIV1,168},{DIV1,158},{DIV1,149},{DIV1,141},{DIV1,133},
{DIV1,126},{DIV1,118},{DIV1,112},{DIV1,105},{DIV1,99},{DIV1,94},{DIV1,88},{DIV1,83},{DIV1,79},
{DIV1,74},{DIV1,70},{DIV1,66},{DIV1,62},{DIV1,59},{DIV1,55},{DIV1,52},{DIV1,49},{DIV1,46},{DIV1,44},
{DIV1,41},{DIV1,39}
};

void midi_init(){
    wdt_set(WDTO_30MS);
}

typedef enum
{
    Stoped,
    Paused,
    Playing

} play_status_t;

static volatile play_status_t    music_status = Paused;
static volatile uint8_t          music_pass = 0;
static volatile uint8_t          music_len = 0;
static uint16_t                  music_playing;

void _music_play(uint16_t musicTab, uint8_t len)
{
    music_pass = 0;
    music_len = len;
    music_playing = musicTab;
    music_status = Playing;
}

void music_pause()
{
    music_status = Paused;
}

void music_stop()
{
    music_pass = 0;
    music_status = Stoped;
}

ISR(WDT_vect)
{
    static uint8_t duration = 0;
    uint8_t note = 0;

    switch(music_status) {
        case Stoped:
        break;
        case Paused:
        break;
        case Playing:
            if (duration-- == 0) {
                note = pgm_read_byte_near(music_playing + (music_pass * 2));
                if (note !=PAUSE) {
                    play(pgm_read_byte_near(music_playing + (music_pass * 2)));
                } else {
                    play_pause();
                }
                duration = pgm_read_byte_near(music_playing + (music_pass * 2) + 1);
                if (++music_pass == music_len) {
                    music_pass = 0;
                }
            }
        break;
    }
}
