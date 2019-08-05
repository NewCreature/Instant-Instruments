#include "t3f/t3f.h"
#include "t3f/controller.h"

void ii_set_controller_key(T3F_CONTROLLER * cp, int i, int key)
{
	cp->binding[i].type = T3F_CONTROLLER_BINDING_KEY;
	cp->binding[i].button = key;
	cp->binding[i].flags = 0;
}
