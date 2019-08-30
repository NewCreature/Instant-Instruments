#include "t3f/t3f.h"
#include "t3f/controller.h"
#include "MIDIA5/midia5.h"

#include "midi.h"
#include "instrument.h"
#include "instrument_piano.h"
#include "controller.h"

static int ii_piano_key[2][II_PIANO_CONTROLLER_INPUTS] =
{
	{
		ALLEGRO_KEY_Z, ALLEGRO_KEY_S, ALLEGRO_KEY_X, ALLEGRO_KEY_D,
		ALLEGRO_KEY_C, ALLEGRO_KEY_V, ALLEGRO_KEY_G, ALLEGRO_KEY_B,
		ALLEGRO_KEY_H, ALLEGRO_KEY_N, ALLEGRO_KEY_J, ALLEGRO_KEY_M,
		ALLEGRO_KEY_COMMA, ALLEGRO_KEY_L, ALLEGRO_KEY_FULLSTOP,
		ALLEGRO_KEY_SEMICOLON, ALLEGRO_KEY_SLASH,

		/* function key */
		ALLEGRO_KEY_A
	},
	{
		ALLEGRO_KEY_Q, ALLEGRO_KEY_2, ALLEGRO_KEY_W, ALLEGRO_KEY_3,
		ALLEGRO_KEY_E, ALLEGRO_KEY_R, ALLEGRO_KEY_5, ALLEGRO_KEY_T,
		ALLEGRO_KEY_6, ALLEGRO_KEY_Y, ALLEGRO_KEY_7, ALLEGRO_KEY_U,
		ALLEGRO_KEY_I, ALLEGRO_KEY_9, ALLEGRO_KEY_O, ALLEGRO_KEY_0,
		ALLEGRO_KEY_P,

		/* function key */
		ALLEGRO_KEY_1
	}
};

static void ii_kill_piano_note(II_INSTRUMENT * ip, int note_pos)
{
	int i;

	for(i = 0; i < ip->key_note[note_pos].notes; i++)
	{
		ii_send_note_off(ip->midi_out, ip->channel, ip->key_note[note_pos].note[i], 100);
	}
	ip->key_note[note_pos].notes = 0;
}

static void ii_play_piano_note(II_INSTRUMENT * ip, int note_pos)
{
	int note = ip->base_note + note_pos;

	ii_kill_piano_note(ip, note_pos);
	ii_send_note_on(ip->midi_out, ip->channel, note, 100);
	ip->key_note[note_pos].note[0] = note;
	ip->key_note[note_pos].notes = 1;
}

static void ii_play_piano_chord(II_INSTRUMENT * ip, int note_pos)
{
	int note = ip->chord_base_note + note_pos;

	ii_kill_piano_note(ip, note_pos);
	ii_send_note_on(ip->midi_out, ip->channel, note, 100);
	ii_send_note_on(ip->midi_out, ip->channel, note + 4, 100);
	ii_send_note_on(ip->midi_out, ip->channel, note + 7, 100);
	ip->key_note[note_pos].note[0] = note;
	ip->key_note[note_pos].note[1] = note + 4;
	ip->key_note[note_pos].note[2] = note + 7;
	ip->key_note[note_pos].notes = 3;
}


T3F_CONTROLLER * ii_create_piano_controller(int option)
{
	T3F_CONTROLLER * cp;
	int i;

	cp = t3f_create_controller(II_PIANO_CONTROLLER_INPUTS);
	if(!cp)
	{
		return NULL;
	}
	for(i = 0; i < II_PIANO_CONTROLLER_INPUTS; i++)
	{
		ii_set_controller_key(cp, i, ii_piano_key[option][i]);
	}
	t3f_clear_controller_state(cp);

	return cp;
}

void ii_piano_logic(II_INSTRUMENT * ip)
{
	int i;

	for(i = 0; i < II_PIANO_CONTROLLER_INPUTS; i++)
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
