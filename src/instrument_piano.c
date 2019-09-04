#include "t3f/t3f.h"
#include "t3f/controller.h"
#include "rtk/midi.h"

#include "midi_event.h"
#include "instrument.h"
#include "instrument_piano.h"

static void ii_kill_piano_note(II_INSTRUMENT * ip, int note_pos)
{
	int i;

	for(i = 0; i < ip->key_note[note_pos].notes; i++)
	{
		ii_add_midi_event(ip->midi_event_batch, RTK_MIDI_EVENT_TYPE_NOTE_OFF, ip->channel, ip->key_note[note_pos].note[i], 100, ip->key_note[note_pos].delay[i]);
	}
	ip->key_note[note_pos].notes = 0;
}

static void ii_play_piano_note(II_INSTRUMENT * ip, int note_pos)
{
	int note = ip->base_note + ip->key_rel_note[note_pos];

	ii_kill_piano_note(ip, note_pos);
	ii_add_midi_event(ip->midi_event_batch, RTK_MIDI_EVENT_TYPE_NOTE_ON, ip->channel, note, 100, 1);
	ip->key_note[note_pos].note[0] = note;
	ip->key_note[note_pos].delay[0] = 1;
	ip->key_note[note_pos].notes = 1;
}

static void ii_play_piano_chord(II_INSTRUMENT * ip, int note_pos)
{
	int note = ip->chord_base_note + ip->key_rel_note[note_pos];

	ii_kill_piano_note(ip, note_pos);
	ii_add_midi_event(ip->midi_event_batch, RTK_MIDI_EVENT_TYPE_NOTE_ON, ip->channel, note, 100, 1);
	ii_add_midi_event(ip->midi_event_batch, RTK_MIDI_EVENT_TYPE_NOTE_ON, ip->channel, note + 4, 100, 1);
	ii_add_midi_event(ip->midi_event_batch, RTK_MIDI_EVENT_TYPE_NOTE_ON, ip->channel, note + 7, 100, 1);
	ip->key_note[note_pos].note[0] = note;
	ip->key_note[note_pos].delay[0] = 1;
	ip->key_note[note_pos].note[1] = note + 4;
	ip->key_note[note_pos].delay[1] = 1;
	ip->key_note[note_pos].note[2] = note + 7;
	ip->key_note[note_pos].delay[2] = 1;
	ip->key_note[note_pos].notes = 3;
}

void ii_piano_logic(II_INSTRUMENT * ip)
{
	int i;

	for(i = 0; i < ip->controller->bindings - 1; i++)
	{
		if(ip->controller->state[i].pressed)
		{
			if(t3f_key[ALLEGRO_KEY_LCTRL])
			{
				ii_play_piano_chord(ip, i);
			}
			else
			{
				ii_play_piano_note(ip, i);
			}
		}
		if(ip->controller->state[i].released)
		{
			ii_kill_piano_note(ip, i);
		}
	}
}
