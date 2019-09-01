#include "t3f/t3f.h"
#include "t3f/controller.h"
#include "rtk/midi.h"

#include "midi_event.h"
#include "instrument.h"
#include "instrument_drum_set.h"

static void ii_kill_drum_note(II_INSTRUMENT * ip, int note_pos)
{
	int i;

	for(i = 0; i < ip->key_note[note_pos].notes; i++)
	{
		ii_add_midi_event(ip->midi_event_batch, RTK_MIDI_EVENT_TYPE_NOTE_OFF, ip->channel, ip->key_note[note_pos].note[i], 100, 1);
	}
	ip->key_note[note_pos].notes = 0;
}

static void ii_play_drum_note(II_INSTRUMENT * ip, int note_pos)
{
	ii_kill_drum_note(ip, note_pos);
	ii_add_midi_event(ip->midi_event_batch, RTK_MIDI_EVENT_TYPE_NOTE_ON, ip->channel, ip->key_rel_note[note_pos], 100, 1);
	ip->key_note[note_pos].note[0] = ip->key_rel_note[note_pos];
	ip->key_note[note_pos].notes = 1;
}

void ii_drum_set_logic(II_INSTRUMENT * ip)
{
	int i;

	for(i = 0; i < ip->controller->bindings - 1; i++)
	{
		if(ip->controller->state[i].pressed)
		{
			ii_play_drum_note(ip, i);
		}
		if(ip->controller->state[i].released)
		{
			ii_kill_drum_note(ip, i);
		}
	}
}
