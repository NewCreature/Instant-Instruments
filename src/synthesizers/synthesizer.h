#ifndef _II_SYNTHESIZER_H
#define _II_SYNTHESIZER_H

#include "t3f/t3f.h"

typedef struct
{
    char id[128];

	/* init/exit routines */
	void (*destroy)(void * data);
    void * data;

    void (*note_on)(void * data, int channel, int note, int velocity);
    void (*note_off)(void * data, int channel, int note);
    void (*program_change)(void * data, int channel, int program);
    void (*reset)(void * data);

} II_SYNTHESIZER;

void ii_destroy_synthesizer(II_SYNTHESIZER * synthesizer);

#endif
