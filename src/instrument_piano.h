#ifndef II_PIANO_H
#define II_PIANO_H

#include "t3f/t3f.h"
#include "t3f/controller.h"

#include "instrument.h"

#define II_PIANO_CONTROLLER_INPUTS 18

T3F_CONTROLLER * ii_create_piano_controller(int option);

void ii_piano_logic(II_INSTRUMENT * ip);

#endif
