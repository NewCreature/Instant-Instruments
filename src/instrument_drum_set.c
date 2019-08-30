#include "t3f/t3f.h"
#include "t3f/controller.h"
#include "MIDIA5/midia5.h"

#include "midi.h"
#include "instrument.h"
#include "instrument_drum_set.h"
#include "controller.h"

static int ii_drums_key[II_DRUM_SET_CONTROLLER_INPUTS] =
{
	ALLEGRO_KEY_DELETE,
	ALLEGRO_KEY_END,
	ALLEGRO_KEY_INSERT,
	ALLEGRO_KEY_HOME,
	ALLEGRO_KEY_PGUP,
	ALLEGRO_KEY_PGDN,
	ALLEGRO_KEY_SPACE
};

static int drum_note[II_DRUM_SET_CONTROLLER_INPUTS] =
{
	35, // bass drum
	38, // snare drum
	42, // close hi-hat
	46, // open hi-hat
	49, // crash
	47  // tom
};

static void ii_kill_drum_note(II_INSTRUMENT * ip, int note_pos)
{
	int i;

	for(i = 0; i < ip->key_note[note_pos].notes; i++)
	{
		ii_send_note_off(ip->midi_out, ip->channel, ip->key_note[note_pos].note[i], 100);
	}
	ip->key_note[note_pos].notes = 0;
}

static void ii_play_drum_note(II_INSTRUMENT * ip, int note_pos)
{
	ii_kill_drum_note(ip, note_pos);
	ii_send_note_on(ip->midi_out, ip->channel, drum_note[note_pos], 100);
	ip->key_note[note_pos].note[0] = drum_note[note_pos];
	ip->key_note[note_pos].notes = 1;
}

T3F_CONTROLLER * ii_create_drum_set_controller(int option)
{
	T3F_CONTROLLER * cp;
	int i;

	cp = t3f_create_controller(II_DRUM_SET_CONTROLLER_INPUTS);
	if(!cp)
	{
		return NULL;
	}
	for(i = 0; i < II_DRUM_SET_CONTROLLER_INPUTS; i++)
	{
		ii_set_controller_key(cp, i, ii_drums_key[i]);
	}
	t3f_clear_controller_state(cp);

	return cp;
}

void ii_drum_set_logic(II_INSTRUMENT * ip)
{
	int i;

	for(i = 0; i < II_DRUM_SET_CONTROLLER_INPUTS; i++)
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
