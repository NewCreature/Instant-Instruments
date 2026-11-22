#ifndef II_MIDI_H
#define II_MIDI_H

#include "synthesizers/synthesizer.h"

extern const char * ii_midi_instrument_name[128];

void ii_send_program_change(II_SYNTHESIZER * synthesizer, int channel, int program);
void ii_send_note_on(II_SYNTHESIZER * synthesizer, int channel, int note, int velocity);
void ii_send_note_off(II_SYNTHESIZER * synthesizer, int channel, int note, int velocity);

#endif
