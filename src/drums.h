#ifndef II_DRUMS_H
#define II_DRUMS_H

#include "t3f/t3f.h"
#include "t3f/controller.h"
#include "MIDIA5/midia5.h"

#include "key.h"

#define II_DRUMS_CONTROLLER_INPUTS 6

typedef struct
{

	MIDIA5_OUTPUT_HANDLE * midi_out;
	T3F_CONTROLLER * controller;

	II_KEY_NOTE key_note[6];

} II_DRUMS;

II_DRUMS * ii_create_drums(MIDIA5_OUTPUT_HANDLE * hp);
void ii_destroy_drums(II_DRUMS * dp);

void ii_drums_logic(II_DRUMS * dp);

#endif
