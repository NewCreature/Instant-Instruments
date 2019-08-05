#ifndef II_GUITAR_H
#define II_GUITAR_H

#include "t3f/t3f.h"
#include "t3f/controller.h"
#include "MIDIA5/midia5.h"

#include "key.h"

typedef struct
{

	MIDIA5_OUTPUT_HANDLE * midi_out;
	T3F_CONTROLLER * controller;

	int octave;
	int chord_octave;
	int note_chart;
	II_KEY_NOTE key_note[12];

} II_GUITAR;

II_GUITAR * ii_create_guitar(MIDIA5_OUTPUT_HANDLE * hp);
void ii_destroy_guitar(II_GUITAR * gp);

void ii_guitar_logic(II_GUITAR * gp);

#endif
