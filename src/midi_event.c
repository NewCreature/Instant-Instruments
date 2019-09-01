#include "MIDIA5/midia5.h"
#include "rtk/midi.h"

#include "midi.h"
#include "midi_event.h"

II_MIDI_EVENT_BATCH * ii_create_midi_event_batch(MIDIA5_OUTPUT_HANDLE * hp, int max)
{
	II_MIDI_EVENT_BATCH * bp;

	bp = malloc(sizeof(II_MIDI_EVENT_BATCH));
	if(!bp)
	{
		goto fail;
	}
	memset(bp, 0, sizeof(II_MIDI_EVENT_BATCH));
	bp->midi_out = hp;
	bp->event = malloc(sizeof(II_MIDI_EVENT) * max);
	if(!bp->event)
	{
		goto fail;
	}
	memset(bp->event, 0, sizeof(II_MIDI_EVENT) * max);
	bp->event_count = max;
	return bp;

	fail:
	{
		ii_destroy_midi_event_batch(bp);
	}
	return NULL;
}

void ii_destroy_midi_event_batch(II_MIDI_EVENT_BATCH * bp)
{
	if(bp)
	{
		if(bp->event)
		{
			free(bp->event);
		}
		free(bp);
	}
}

void ii_add_midi_event(II_MIDI_EVENT_BATCH * bp, int type, int channel, int d1, int d2, int delay)
{
	bp->event[bp->current_event].type = type;
	bp->event[bp->current_event].channel = channel;
	bp->event[bp->current_event].data[0] = d1;
	bp->event[bp->current_event].data[1] = d2;
	bp->event[bp->current_event].delay = delay;
	bp->current_event++;
	if(bp->current_event >= bp->event_count)
	{
		bp->current_event = 0;
	}
}

static void trigger_event(MIDIA5_OUTPUT_HANDLE * hp, II_MIDI_EVENT * ep)
{
	switch(ep->type)
	{
		case RTK_MIDI_EVENT_TYPE_NOTE_ON:
		{
			ii_send_note_on(hp, ep->channel, ep->data[0], ep->data[1]);
			break;
		}
		case RTK_MIDI_EVENT_TYPE_NOTE_OFF:
		{
			ii_send_note_off(hp, ep->channel, ep->data[0], ep->data[1]);
			break;
		}
		case RTK_MIDI_EVENT_TYPE_PROGRAM_CHANGE:
		{
			ii_send_program_change(hp, ep->channel, ep->data[0]);
			break;
		}
	}
}

void ii_process_midi_event_batch(II_MIDI_EVENT_BATCH * bp)
{
	int i;

	for(i = 0; i < bp->event_count; i++)
	{
		if(bp->event[i].delay > 0)
		{
			bp->event[i].delay--;
			if(bp->event[i].delay <= 0)
			{
				trigger_event(bp->midi_out, &bp->event[i]);
			}
		}
	}
}
