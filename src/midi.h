#ifndef II_MIDI_H
#define II_MIDI_H

#include "MIDIA5/midia5.h"

extern const char * ii_midi_instrument_name[128];

void ii_send_program_change(MIDIA5_OUTPUT_HANDLE * hp, int channel, int program);
void ii_send_note_on(MIDIA5_OUTPUT_HANDLE * hp, int channel, int note, int velocity);
void ii_send_note_off(MIDIA5_OUTPUT_HANDLE * hp, int channel, int note, int velocity);

#endif
