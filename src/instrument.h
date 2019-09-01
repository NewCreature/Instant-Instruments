#ifndef II_INSTRUMENT_H
#define II_INSTRUMENT_H

#include "t3f/t3f.h"
#include "t3f/controller.h"

#include "midi_event.h"
#include "key.h"

#define II_INSTRUMENT_MAX_KEY_NOTES 48

#define II_INSTRUMENT_TYPE_DRUM_SET  0
#define II_INSTRUMENT_TYPE_PIANO     1
#define II_INSTRUMENT_TYPE_GUITAR    2

typedef struct
{

	II_MIDI_EVENT_BATCH * midi_event_batch;
	T3F_CONTROLLER * controller;
	int type;

	int channel;
	int program;
	int mode;
	int base_note;
	int chord_base_note;
	II_KEY_NOTE key_note[II_INSTRUMENT_MAX_KEY_NOTES];
	int key_rel_note[II_INSTRUMENT_MAX_KEY_NOTES];

} II_INSTRUMENT;

II_INSTRUMENT * ii_load_instrument(const char * fn, II_MIDI_EVENT_BATCH * bp);
void ii_destroy_instrument(II_INSTRUMENT * ip);

void ii_instrument_logic(II_INSTRUMENT * ip);

#endif
