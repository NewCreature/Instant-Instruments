#include "t3f/t3f.h"
#include "synthesizer.h"

void ii_destroy_synthesizer(II_SYNTHESIZER * synthesizer)
{
    if(synthesizer)
    {
        if(synthesizer->destroy)
        {
            synthesizer->destroy(synthesizer->data);
        }
        free(synthesizer);
    }
}
