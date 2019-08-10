#include "t3f/t3f.h"
#include "t3f/controller.h"
#include "rtk/midi.h"
#include "MIDIA5/midia5.h"

#include "guitar.h"
#include "piano.h"

/* structure to hold all of our app-specific data */
typedef struct
{

	MIDIA5_OUTPUT_HANDLE * midi_out;
	II_GUITAR * guitar;
	II_PIANO * piano[2];

	int key_row_1_octave;
	int key_row_2_octave;
	int state;

} APP_INSTANCE;

/* main logic routine */
void app_logic(void * data)
{
	APP_INSTANCE * app = (APP_INSTANCE *)data;

	switch(app->state)
	{
		default:
		{
			/* insert logic here, as your project grows you can add more states
			 * to deal with various parts of your app (logo, title screen, in-
			 * game, etc.) */
			ii_guitar_logic(app->guitar);
			ii_piano_logic(app->piano[0]);
			ii_piano_logic(app->piano[1]);
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

static int get_midi_device(void)
{
	const char * val;
	int val_i;
	int device_count;
	int i;

	device_count = midia5_get_output_device_count();
	val = al_get_config_value(t3f_config, "Settings", "midi_device");
	if(val)
	{
		val_i = atoi(val);
		if(val_i < device_count)
		{
			return val_i;
		}
	}
	else
	{
		/* look for FLUID Synth */
		for(i = 0; i < device_count; i++)
		{
			if(!memcmp(midia5_get_output_device_name(i), "FLUID", 5))
			{
				return i;
			}
		}
	}
	if(device_count < 0)
	{
		return -1;
	}
	return 0;
}

/* initialize our app, load graphics, etc. */
bool app_initialize(APP_INSTANCE * app, int argc, char * argv[])
{
	int midi_device;

	/* initialize T3F */
	if(!t3f_initialize(T3F_APP_TITLE, 640, 480, 60.0, app_logic, app_render, T3F_DEFAULT, app))
	{
		printf("Error initializing T3F\n");
		return false;
	}
	memset(app, 0, sizeof(APP_INSTANCE));
	midi_device = get_midi_device();
	if(midi_device < 0)
	{
		printf("Could not detect MIDI device!\n");
		return false;
	}
	app->midi_out = midia5_create_output_handle(0);
	if(!app->midi_out)
	{
		printf("Could not get MIDI output device!\n");
		return false;
	}

	/* set up guitar controller */
	app->guitar = ii_create_guitar(app->midi_out);
	if(!app->guitar)
	{
		printf("Failed to create guitar!\n");
		return false;
	}

	/* set up piano controllers */
	app->piano[0] = ii_create_piano(app->midi_out, 0);
	if(!app->piano[0])
	{
		printf("Failed to create piano!\n");
		return false;
	}
	app->piano[1] = ii_create_piano(app->midi_out, 1);
	if(!app->piano[1])
	{
		printf("Failed to create piano!\n");
		return false;
	}

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
	if(app.guitar)
	{
		ii_destroy_guitar(app.guitar);
	}
	if(app.piano[0])
	{
		ii_destroy_piano(app.piano[0]);
	}
	if(app.piano[1])
	{
		ii_destroy_piano(app.piano[1]);
	}
	t3f_finish();
	return 0;
}
