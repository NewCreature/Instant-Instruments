#include "t3f/t3f.h"
#include "t3f/controller.h"
#include "MIDIA5/midia5.h"

#include "midi.h"
#include "guitar.h"
#include "controller.h"

static int ii_note_chart[2][12] =
{
	{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11},
	{0, 2, 4, 5, 7, 9, 10, 12, 14, 15, 17, 19}
};

static void ii_kill_guitar_note(II_GUITAR * gp, int note_pos)
{
	int i;

	for(i = 0; i < gp->key_note[note_pos].notes; i++)
	{
		ii_send_note_off(gp->midi_out, 0, gp->key_note[note_pos].note[i], 100);
	}
	gp->key_note[note_pos].notes = 0;
}

static void ii_play_guitar_note(II_GUITAR * gp, int note_pos)
{
	int note = gp->octave * 12 + ii_note_chart[gp->note_chart][note_pos];

	ii_kill_guitar_note(gp, note_pos);
	ii_send_note_on(gp->midi_out, 0, note, 100);
	gp->key_note[note_pos].note[0] = note;
	gp->key_note[note_pos].notes = 1;
}

static void ii_play_guitar_chord(II_GUITAR * gp, int note_pos)
{
	int note = gp->chord_octave * 12 + ii_note_chart[gp->note_chart][note_pos];

	ii_kill_guitar_note(gp, note_pos);
	ii_send_note_on(gp->midi_out, 0, note, 100);
	ii_send_note_on(gp->midi_out, 0, note + 4, 100);
	ii_send_note_on(gp->midi_out, 0, note + 8, 100);
	gp->key_note[note_pos].note[0] = note;
	gp->key_note[note_pos].note[1] = note + 4;
	gp->key_note[note_pos].note[2] = note + 8;
	gp->key_note[note_pos].notes = 3;
}

static void ii_play_guitar_power_chord(II_GUITAR * gp, int note_pos)
{
	int note = gp->chord_octave * 12 + ii_note_chart[gp->note_chart][note_pos];

	ii_kill_guitar_note(gp, note_pos);
	ii_send_note_on(gp->midi_out, 0, note, 100);
	ii_send_note_on(gp->midi_out, 0, note + 8, 100);
	gp->key_note[note_pos].note[0] = note;
	gp->key_note[note_pos].note[1] = note + 8;
	gp->key_note[note_pos].notes = 2;
}

II_GUITAR * ii_create_guitar(MIDIA5_OUTPUT_HANDLE * hp)
{
	II_GUITAR * gp = NULL;
	int i;

	gp = malloc(sizeof(II_GUITAR));
	if(!gp)
	{
		goto fail;
	}
	memset(gp, 0, sizeof(II_GUITAR));
	gp->midi_out = hp;

	/* create controller */
	gp->controller = t3f_create_controller(14);
	if(!gp->controller)
	{
		goto fail;
	}
	for(i = 0; i < 12; i++)
	{
		ii_set_controller_key(gp->controller, i, ALLEGRO_KEY_F1 + i);
	}
	#ifdef ALLEGRO_MACOSX
		ii_set_controller_key(gp->controller, 12, ALLEGRO_KEY_LSHIFT);
	#else
		ii_set_controller_key(gp->controller, 12, ALLEGRO_KEY_RSHIFT);
	#endif
	t3f_clear_controller_state(gp->controller);

	/* set current MIDI program for guitar channel */
	ii_send_program_change(gp->midi_out, 0, 30);

	/* default settings */
	gp->octave = 6;
	gp->chord_octave = 3;
	gp->note_chart = 0;

	return gp;

	fail:
	{
		ii_destroy_guitar(gp);
	}
	return NULL;
}

void ii_destroy_guitar(II_GUITAR * gp)
{
	if(gp)
	{
		if(gp->controller)
		{
			t3f_destroy_controller(gp->controller);
		}
		free(gp);
	}
}

void ii_guitar_logic(II_GUITAR * gp)
{
	int frets = 0;
	bool power_chord = false;
	int i, c;

	if(t3f_key[ALLEGRO_KEY_TAB])
	{
		gp->note_chart = 1 - gp->note_chart;
		t3f_key[ALLEGRO_KEY_TAB] = 0;
	}
	t3f_read_controller(gp->controller);
	t3f_update_controller(gp->controller);

	/* detect chords */
	c = 0;
	for(i = 0; i < 12; i++)
	{
		if(gp->controller->state[i].held)
		{
			if(frets)
			{
				if(c > 1)
				{
					power_chord = true;
				}
			}
			frets++;
			c = 0;
		}
		c++;
	}

	if(gp->controller->state[12].pressed)
	{
		for(i = 0; i < 12; i++)
		{
			if(gp->controller->state[i].held)
			{
				if(frets > 1)
				{
					if(power_chord)
					{
						ii_play_guitar_power_chord(gp, i);
					}
					else
					{
						ii_play_guitar_chord(gp, i);
					}
					break;
				}
				else
				{
					ii_play_guitar_note(gp, i);
				}
			}
		}
	}
	/* check for hammer-on/pull-off */
	else if(gp->controller->state[12].held)
	{
		for(i = 0; i < 12; i++)
		{
			if(gp->controller->state[i].pressed)
			{
				ii_play_guitar_note(gp, i);
			}
			if(gp->controller->state[i].released)
			{
				ii_kill_guitar_note(gp, i);
			}
		}
	}
	for(i = 0; i < 12; i++)
	{
		if(gp->controller->state[i].released)
		{
			ii_kill_guitar_note(gp, i);
		}
	}
}
