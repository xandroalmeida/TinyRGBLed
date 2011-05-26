#ifndef MIDI_H_INCLUDED
#define MIDI_H_INCLUDED

#include <avr/pgmspace.h>

void midi_init();


#define C(t)   {0+((t)*12),
#define Cs(t)  {1+((t)*12),
#define D(t)   {2+((t)*12),
#define Ds(t)  {3+((t)*12),
#define E(t)   {4+((t)*12),
#define F(t)   {5+((t)*12),
#define Fs(t)  {6+((t)*12),
#define G(t)   {7+((t)*12),
#define Gs(t)  {8+((t)*12),
#define A(t)   {9+((t)*12),
#define As(t)  {10+((t)*12),
#define B(t)   {11+((t)*12),
#define P()    {255,
#define PAUSE   255


#define D1   16}
#define D1p  16+8}
#define D2   8}
#define D2p  8+4}
#define D3   4}
#define D3p  4+2}
#define D4   2}
#define D4p  2+1}
#define D5   1}
#define D5p  1+0}

#define music_t(y,x) PROGMEM uint8_t y[(x)][2]

#define music_play(x,y) _music_play((uint16_t)(x), (y))

void _music_play(uint16_t musicTab, uint8_t len);
void music_pause();
void music_stop();

#endif /* MIDI_H_INCLUDED */
