#include "t3f/t3f.h"
#include "t3f/controller.h"
#include "MIDIA5/midia5.h"

#include "midi.h"
#include "drums.h"
#include "controller.h"

static int ii_drums_key[II_DRUMS_CONTROLLER_INPUTS] =
{
	ALLEGRO_KEY_DELETE,
	ALLEGRO_KEY_END,
	ALLEGRO_KEY_INSERT,
	ALLEGRO_KEY_HOME,
	ALLEGRO_KEY_PGUP,
	ALLEGRO_KEY_PGDN
};

static int drum_note[II_DRUMS_CONTROLLER_INPUTS] =
{
	35, // bass drum
	38, // snare drum
	42, // close hi-hat
	46, // open hi-hat
	49, // crash
	47  // tom
};

static void ii_kill_drum_note(II_DRUMS * dp, int note_pos)
{
	int i;

	for(i = 0; i < dp->key_note[note_pos].notes; i++)
	{
		ii_send_note_off(dp->midi_out, 9, dp->key_note[note_pos].note[i], 100);
	}
	dp->key_note[note_pos].notes = 0;
}

static void ii_play_drum_note(II_DRUMS * dp, int note_pos)
{
	ii_kill_drum_note(dp, note_pos);
	ii_send_note_on(dp->midi_out, 9, drum_note[note_pos], 100);
	dp->key_note[note_pos].note[0] = drum_note[note_pos];
	dp->key_note[note_pos].notes = 1;
}

II_DRUMS * ii_create_drums(MIDIA5_OUTPUT_HANDLE * hp)
{
	II_DRUMS * dp = NULL;
	int i;

	dp = malloc(sizeof(II_DRUMS));
	if(!dp)
	{
		goto fail;
	}
	memset(dp, 0, sizeof(II_DRUMS));
	dp->midi_out = hp;

	/* create controller */
	dp->controller = t3f_create_controller(II_DRUMS_CONTROLLER_INPUTS);
	if(!dp->controller)
	{
		goto fail;
	}
	for(i = 0; i < II_DRUMS_CONTROLLER_INPUTS; i++)
	{
		ii_set_controller_key(dp->controller, i, ii_drums_key[i]);
	}
	t3f_clear_controller_state(dp->controller);

	/* set current MIDI program for guitar channel */
	ii_send_program_change(dp->midi_out, 9, 0);

	return dp;

	fail:
	{
		ii_destroy_drums(dp);
	}
	return NULL;
}

void ii_destroy_drums(II_DRUMS * dp)
{
	if(dp)
	{
		if(dp->controller)
		{
			t3f_destroy_controller(dp->controller);
		}
		free(dp);
	}
}

void ii_drums_logic(II_DRUMS * dp)
{
	int i;

	t3f_read_controller(dp->controller);
	t3f_update_controller(dp->controller);

	for(i = 0; i < II_DRUMS_CONTROLLER_INPUTS; i++)
	{
		if(dp->controller->state[i].pressed)
		{
			ii_play_drum_note(dp, i);
		}
		if(dp->controller->state[i].released)
		{
			ii_kill_drum_note(dp, i);
		}
	}
}
