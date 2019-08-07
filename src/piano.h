#ifndef II_PIANO_H
#define II_PIANO_H

#include "t3f/t3f.h"
#include "t3f/controller.h"
#include "MIDIA5/midia5.h"

#include "key.h"

#define II_PIANO_CONTROLLER_INPUTS 29

typedef struct
{

	MIDIA5_OUTPUT_HANDLE * midi_out;
	T3F_CONTROLLER * controller;

	int octave;
	II_KEY_NOTE key_note[24];

} II_PIANO;

II_PIANO * ii_create_piano(MIDIA5_OUTPUT_HANDLE * hp, int row);
void ii_destroy_piano(II_PIANO * pp);

void ii_piano_logic(II_PIANO * pp);

#endif
