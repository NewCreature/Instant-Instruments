#ifndef II_DRUMS_H
#define II_DRUMS_H

#include "t3f/t3f.h"
#include "t3f/controller.h"

#include "instrument.h"

#define II_DRUM_SET_CONTROLLER_INPUTS 7

T3F_CONTROLLER * ii_create_drum_set_controller(int option);

void ii_drum_set_logic(II_INSTRUMENT * ip);

#endif
