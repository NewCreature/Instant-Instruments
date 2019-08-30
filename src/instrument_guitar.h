#ifndef II_GUITAR_H
#define II_GUITAR_H

#include "t3f/t3f.h"
#include "t3f/controller.h"

#include "instrument.h"

T3F_CONTROLLER * ii_create_guitar_controller(int option);

void ii_guitar_logic(II_INSTRUMENT * ip);

#endif
