#include "t3f/t3f.h"
#include "t3f/controller.h"
#include "rtk/midi.h"
#include "MIDIA5/midia5.h"

static int ii_note_chart[2][12] =
{
	{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11},
	{0, 2, 4, 5, 7, 9, 10, 12, 14, 15, 17, 19}
};

typedef struct
{

	int note[16];
	int notes;

} II_KEY_NOTE;

/* structure to hold all of our app-specific data */
typedef struct
{

	MIDIA5_OUTPUT_HANDLE * midi_out;
	T3F_CONTROLLER * guitar_controller;

	int guitar_octave;
	int guitar_chord_octave;
	int guitar_note_chart;
	II_KEY_NOTE guitar_key_note[12];
	int key_row_1_octave;
	int key_row_2_octave;
	int state;

} APP_INSTANCE;

static void ii_send_program_change(MIDIA5_OUTPUT_HANDLE * hp, int channel, int program)
{
	midia5_send_data(hp, RTK_MIDI_EVENT_TYPE_PROGRAM_CHANGE + channel);
	midia5_send_data(hp, program);
}

static void ii_send_note_on(MIDIA5_OUTPUT_HANDLE * hp, int channel, int note, int velocity)
{
	midia5_send_data(hp, RTK_MIDI_EVENT_TYPE_NOTE_ON + channel);
	midia5_send_data(hp, note);
	midia5_send_data(hp, velocity);
}

static void ii_send_note_off(MIDIA5_OUTPUT_HANDLE * hp, int channel, int note, int velocity)
{
	midia5_send_data(hp, RTK_MIDI_EVENT_TYPE_NOTE_OFF + channel);
	midia5_send_data(hp, note);
	midia5_send_data(hp, velocity);
}

static void ii_kill_guitar_note(APP_INSTANCE * app, int note_pos)
{
	int i;

	for(i = 0; i < app->guitar_key_note[note_pos].notes; i++)
	{
		ii_send_note_off(app->midi_out, 0, app->guitar_key_note[note_pos].note[i], 100);
	}
	app->guitar_key_note[note_pos].notes = 0;
}

static void ii_play_guitar_note(APP_INSTANCE * app, int note_pos)
{
	int note = app->guitar_octave * 12 + ii_note_chart[app->guitar_note_chart][note_pos];

	ii_kill_guitar_note(app, note_pos);
	ii_send_note_on(app->midi_out, 0, note, 100);
	app->guitar_key_note[note_pos].note[0] = note;
	app->guitar_key_note[note_pos].notes = 1;
}

static void ii_play_guitar_chord(APP_INSTANCE * app, int note_pos)
{
	int note = app->guitar_chord_octave * 12 + ii_note_chart[app->guitar_note_chart][note_pos];

	ii_kill_guitar_note(app, note_pos);
	ii_send_note_on(app->midi_out, 0, note, 100);
	ii_send_note_on(app->midi_out, 0, note + 4, 100);
	ii_send_note_on(app->midi_out, 0, note + 8, 100);
	app->guitar_key_note[note_pos].note[0] = note;
	app->guitar_key_note[note_pos].note[1] = note + 4;
	app->guitar_key_note[note_pos].note[2] = note + 8;
	app->guitar_key_note[note_pos].notes = 3;
}

static void ii_play_guitar_power_chord(APP_INSTANCE * app, int note_pos)
{
	int note = app->guitar_chord_octave * 12 + ii_note_chart[app->guitar_note_chart][note_pos];

	ii_kill_guitar_note(app, note_pos);
	ii_send_note_on(app->midi_out, 0, note, 100);
	ii_send_note_on(app->midi_out, 0, note + 8, 100);
	app->guitar_key_note[note_pos].note[0] = note;
	app->guitar_key_note[note_pos].note[1] = note + 8;
	app->guitar_key_note[note_pos].notes = 2;
}

/* main logic routine */
void app_logic(void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;
	int frets = 0;
	bool power_chord = false;
	int i, c;

	switch(app->state)
	{
		default:
		{
			/* insert logic here, as your project grows you can add more states
			 * to deal with various parts of your app (logo, title screen, in-
			 * game, etc.) */
			if(t3f_key[ALLEGRO_KEY_TAB])
			{
				app->guitar_note_chart = 1 - app->guitar_note_chart;
				t3f_key[ALLEGRO_KEY_TAB] = 0;
			}
			t3f_read_controller(app->guitar_controller);
			t3f_update_controller(app->guitar_controller);

			/* detect chords */
			c = 0;
			for(i = 0; i < 12; i++)
			{
				if(app->guitar_controller->state[i].held)
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

			if(app->guitar_controller->state[12].pressed)
			{
				for(i = 0; i < 12; i++)
				{
					if(app->guitar_controller->state[i].held)
					{
						if(frets > 1)
						{
							if(power_chord)
							{
								ii_play_guitar_power_chord(app, i);
							}
							else
							{
								ii_play_guitar_chord(app, i);
							}
							break;
						}
						else
						{
							ii_play_guitar_note(app, i);
						}
					}
				}
			}
			/* check for hammer-on/pull-off */
			else if(app->guitar_controller->state[12].held)
			{
				for(i = 0; i < 12; i++)
				{
					if(app->guitar_controller->state[i].pressed)
					{
						ii_play_guitar_note(app, i);
					}
					if(app->guitar_controller->state[i].released)
					{
						ii_kill_guitar_note(app, i);
					}
				}
			}
			for(i = 0; i < 12; i++)
			{
				if(app->guitar_controller->state[i].released)
				{
					ii_kill_guitar_note(app, i);
				}
			}
			break;
		}
	}
}

/* main rendering routine */
void app_render(void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	switch(app->state)
	{
		default:
		{
			/* insert rendering code here, see app_logic() for more info */
			break;
		}
	}
}

/* initialize our app, load graphics, etc. */
bool app_initialize(APP_INSTANCE * app, int argc, char * argv[])
{
	int i;

	/* initialize T3F */
	if(!t3f_initialize(T3F_APP_TITLE, 640, 480, 60.0, app_logic, app_render, T3F_DEFAULT, app))
	{
		printf("Error initializing T3F\n");
		return false;
	}
	memset(app, 0, sizeof(APP_INSTANCE));
	app->midi_out = midia5_create_output_handle(0);
	if(!app->midi_out)
	{
		printf("Could not get MIDI output device!\n");
		return false;
	}

	/* set up guitar controller */
	ii_send_program_change(app->midi_out, 0, 30);
	app->guitar_controller = t3f_create_controller(14);
	if(!app->guitar_controller)
	{
		printf("Could not create guitar controller!\n");
		return false;
	}
	for(i = 0; i < 12; i++)
	{
		app->guitar_controller->binding[i].type = T3F_CONTROLLER_BINDING_KEY;
		app->guitar_controller->binding[i].button = ALLEGRO_KEY_F1 + i;
		app->guitar_controller->binding[i].flags = 0;
	}
	app->guitar_controller->binding[12].type = T3F_CONTROLLER_BINDING_KEY;
	#ifdef ALLEGRO_MACOSX
		app->guitar_controller->binding[12].button = ALLEGRO_KEY_LSHIFT;
	#else
		app->guitar_controller->binding[12].button = ALLEGRO_KEY_RSHIFT;
	#endif
	app->guitar_controller->binding[12].flags = 0;
	t3f_clear_controller_state(app->guitar_controller);

	/* set up initial state */
	app->guitar_octave = 6;
	app->guitar_chord_octave = 3;
	app->guitar_note_chart = 0;

	app->state = 0;
	return true;
}

int main(int argc, char * argv[])
{
	APP_INSTANCE app;

	if(app_initialize(&app, argc, argv))
	{
		t3f_run();
	}
	else
	{
		printf("Error: could not initialize T3F!\n");
	}
	if(app.midi_out)
	{
		midia5_destroy_output_handle(app.midi_out);
	}
	if(app.guitar_controller)
	{
		t3f_destroy_controller(app.guitar_controller);
	}
	t3f_finish();
	return 0;
}
