#include "t3f/t3f.h"
#include "t3f/controller.h"
#include "MIDIA5/midia5.h"

#include "midi.h"
#include "piano.h"
#include "controller.h"

static int ii_piano_key[2][II_PIANO_CONTROLLER_INPUTS] =
{
	{
		ALLEGRO_KEY_Z, ALLEGRO_KEY_S, ALLEGRO_KEY_X, ALLEGRO_KEY_D,
		ALLEGRO_KEY_C, ALLEGRO_KEY_V, ALLEGRO_KEY_G, ALLEGRO_KEY_B,
		ALLEGRO_KEY_H, ALLEGRO_KEY_N, ALLEGRO_KEY_J, ALLEGRO_KEY_M,
		ALLEGRO_KEY_COMMA, ALLEGRO_KEY_L, ALLEGRO_KEY_FULLSTOP, ALLEGRO_KEY_SEMICOLON, ALLEGRO_KEY_SLASH
	},
	{
		ALLEGRO_KEY_Q, ALLEGRO_KEY_2, ALLEGRO_KEY_W, ALLEGRO_KEY_3,
		ALLEGRO_KEY_E, ALLEGRO_KEY_R, ALLEGRO_KEY_5, ALLEGRO_KEY_T,
		ALLEGRO_KEY_6, ALLEGRO_KEY_Y, ALLEGRO_KEY_7, ALLEGRO_KEY_U,
		ALLEGRO_KEY_I, ALLEGRO_KEY_9, ALLEGRO_KEY_O, ALLEGRO_KEY_0,
		ALLEGRO_KEY_P
	}
};

static void ii_kill_piano_note(II_PIANO * pp, int note_pos)
{
	int i;

	for(i = 0; i < pp->key_note[note_pos].notes; i++)
	{
		ii_send_note_off(pp->midi_out, 1, pp->key_note[note_pos].note[i], 100);
	}
	pp->key_note[note_pos].notes = 0;
}

static void ii_play_piano_note(II_PIANO * pp, int note_pos)
{
	int note = pp->octave * 12 + note_pos;

	ii_kill_piano_note(pp, note_pos);
	ii_send_note_on(pp->midi_out, 1, note, 100);
	pp->key_note[note_pos].note[0] = note;
	pp->key_note[note_pos].notes = 1;
}

static void ii_play_piano_chord(II_PIANO * pp, int note_pos)
{
	int note = pp->octave * 12 + note_pos;

	ii_kill_piano_note(pp, note_pos);
	ii_send_note_on(pp->midi_out, 1, note - 4, 100);
	ii_send_note_on(pp->midi_out, 1, note, 100);
	ii_send_note_on(pp->midi_out, 1, note + 4, 100);
	pp->key_note[note_pos].note[0] = note - 4;
	pp->key_note[note_pos].note[1] = note;
	pp->key_note[note_pos].note[2] = note + 4;
	pp->key_note[note_pos].notes = 3;
}


II_PIANO * ii_create_piano(MIDIA5_OUTPUT_HANDLE * hp, int row, int function_key)
{
	II_PIANO * pp = NULL;
	int i;

	pp = malloc(sizeof(II_PIANO));
	if(!pp)
	{
		goto fail;
	}
	memset(pp, 0, sizeof(II_PIANO));
	pp->midi_out = hp;

	/* create controller */
	pp->controller = t3f_create_controller(II_PIANO_CONTROLLER_INPUTS);
	if(!pp->controller)
	{
		goto fail;
	}
	for(i = 0; i < II_PIANO_CONTROLLER_INPUTS; i++)
	{
		ii_set_controller_key(pp->controller, i, ii_piano_key[row][i]);
	}
	t3f_clear_controller_state(pp->controller);
	pp->function_key = function_key;

	/* set current MIDI program for piano channel */
	pp->program = 0;
	ii_send_program_change(pp->midi_out, 1, pp->program);

	/* default settings */
	pp->octave = 3 + row * 3;

	return pp;

	fail:
	{
		ii_destroy_piano(pp);
	}
	return NULL;
}

void ii_destroy_piano(II_PIANO * pp)
{
	if(pp)
	{
		if(pp->controller)
		{
			t3f_destroy_controller(pp->controller);
		}
		free(pp);
	}
}

void ii_piano_logic(II_PIANO * pp)
{
	int i;

	t3f_read_controller(pp->controller);
	t3f_update_controller(pp->controller);

	if(t3f_key[pp->function_key])
	{
		if(t3f_key[ALLEGRO_KEY_DOWN])
		{
			pp->program--;
			if(pp->program < 0)
			{
				pp->program = 127;
			}
			ii_send_program_change(pp->midi_out, 1, pp->program);
			t3f_key[ALLEGRO_KEY_DOWN] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_UP])
		{
			pp->program++;
			if(pp->program > 127)
			{
				pp->program = 0;
			}
			ii_send_program_change(pp->midi_out, 1, pp->program);
			t3f_key[ALLEGRO_KEY_UP] = 0;
		}
	}
	for(i = 0; i < II_PIANO_CONTROLLER_INPUTS; i++)
	{
		if(pp->controller->state[i].pressed)
		{
			if(t3f_key[ALLEGRO_KEY_LCTRL])
			{
				ii_play_piano_chord(pp, i);
			}
			else
			{
				ii_play_piano_note(pp, i);
			}
		}
		if(pp->controller->state[i].released)
		{
			ii_kill_piano_note(pp, i);
		}
	}
}
