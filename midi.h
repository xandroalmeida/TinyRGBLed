#ifndef MIDI_H_INCLUDED
#define MIDI_H_INCLUDED

void midi_init();


#define C   {0,
#define Cs  {1,
#define D   {2,
#define Ds  {3,
#define E   {4,
#define F   {5,
#define Fs  {6,
#define G   {7,
#define Gs  {8,
#define A   {9,
#define As  {10,
#define B   {11,

#define D1  64}
#define D2  32}
#define D3  16}
#define D4  8}
#define D5  4}
#define D6  2}
#define D7  1}

#define partitura_t(y,x) PROGMEM uint8_t y[(x)][2]

#endif /* MIDI_H_INCLUDED */
