#ifndef II_INSTRUMENT_H
#define II_INSTRUMENT_H

#include "t3f/t3f.h"
#include "t3f/controller.h"
#include "MIDIA5/midia5.h"

#include "key.h"

#define II_INSTRUMENT_MAX_KEY_NOTES 48

#define II_INSTRUMENT_TYPE_DRUM_SET  0
#define II_INSTRUMENT_TYPE_PIANO     1
#define II_INSTRUMENT_TYPE_GUITAR    2

typedef struct
{

	MIDIA5_OUTPUT_HANDLE * midi_out;
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

II_INSTRUMENT * ii_load_instrument(const char * fn, MIDIA5_OUTPUT_HANDLE * hp);
void ii_destroy_instrument(II_INSTRUMENT * ip);

void ii_instrument_logic(II_INSTRUMENT * ip);

#endif
