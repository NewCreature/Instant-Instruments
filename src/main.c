#include "t3f/t3f.h"
#include "t3f/controller.h"
#include "rtk/midi.h"
#include "MIDIA5/midia5.h"

#include "midi_event.h"
#include "instrument.h"

/* structure to hold all of our app-specific data */
typedef struct
{

	MIDIA5_OUTPUT_HANDLE * midi_out;
	II_MIDI_EVENT_BATCH * midi_event_batch;
	II_INSTRUMENT * guitar;
	II_INSTRUMENT * piano[2];
	II_INSTRUMENT * drums;
	bool kill_fluidsynth;

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
			ii_instrument_logic(app->guitar);
			ii_instrument_logic(app->piano[0]);
			ii_instrument_logic(app->piano[1]);
			ii_instrument_logic(app->drums);
			ii_process_midi_event_batch(app->midi_event_batch);
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

static int get_fluidsynth_device(void)
{
	int device_count;
	int i;

	device_count = midia5_get_output_device_count();
	for(i = 0; i < device_count; i++)
	{
		if(!memcmp(midia5_get_output_device_name(i), "FLUID", 5))
		{
			return i;
		}
	}
	return -1;
}

static int get_midi_device(void)
{
	const char * val;
	int val_i;
	int device_count;

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
		if(get_fluidsynth_device() >= 0)
		{
			return get_fluidsynth_device();
		}
	}
	if(device_count < 0)
	{
		return -1;
	}
	return 0;
}

#ifdef ALLEGRO_UNIX
	#ifndef ALLEGRO_MACOSX
		static char * get_helper_script_path(char * buf, int buf_size)
		{
			ALLEGRO_PATH * path;

			path = al_get_standard_path(ALLEGRO_EXENAME_PATH);
			if(path)
			{
				al_set_path_filename(path, "instant-instruments-helper.sh");
				strcpy(buf, al_path_cstr(path, '/'));
				al_destroy_path(path);
				return buf;
			}
			return NULL;
		}

		static int start_fluidsynth(void)
		{
			char command[1024];

			if(get_helper_script_path(command, 1024))
			{
				strcat(command, " start");
				return system(command);
			}
			return 0;
		}

		static int stop_fluidsynth(void)
		{
			char command[1024];

			if(get_helper_script_path(command, 1024))
			{
				strcat(command, " stop");
				return system(command);
			}
			return 0;
		}

		static void enable_fluidsynth(APP_INSTANCE * app)
		{
			int try = 0;
			int i;

			if(get_fluidsynth_device() < 0)
			{
				for(i = 0; i < 5; i++)
				{
					start_fluidsynth();
					al_rest(0.5);
					while(get_fluidsynth_device() < 0 && try < 5)
					{
						printf("FluidSynth device not found. Retrying...\n");
						al_rest(0.5);
						try++;
					}
					if(try < 5)
					{
						app->kill_fluidsynth = true;
					}
					else
					{
						stop_fluidsynth();
						try = 0;
					}
				}
			}
			else
			{
				app->kill_fluidsynth = false;
			}
		}
	#endif
#endif

/* initialize our app, load graphics, etc. */
bool app_initialize(APP_INSTANCE * app, int argc, char * argv[])
{
	int midi_device;

	/* initialize T3F */
	if(!t3f_initialize(T3F_APP_TITLE, 960, 540, 60.0, app_logic, app_render, T3F_DEFAULT | T3F_RESIZABLE, app))
	{
		printf("Error initializing T3F\n");
		return false;
	}
	memset(app, 0, sizeof(APP_INSTANCE));

	#ifdef ALLEGRO_UNIX
		#ifndef ALLEGRO_MACOSX
			enable_fluidsynth(app);
		#endif
	#endif
	midi_device = get_midi_device();
	if(midi_device < 0)
	{
		printf("Could not detect MIDI device!\n");
		return false;
	}
	app->midi_out = midia5_create_output_handle(midi_device);
	if(!app->midi_out)
	{
		printf("Could not get MIDI output device!\n");
		return false;
	}
	app->midi_event_batch = ii_create_midi_event_batch(app->midi_out, 256);
	if(!app->midi_event_batch)
	{
		printf("Could not create MIDI event batch!\n");
		return false;
	}

	/* set up guitar controller */
	app->guitar = ii_load_instrument("data/instrument_guitar.ini", app->midi_event_batch);
	if(!app->guitar)
	{
		printf("Failed to create guitar!\n");
		return false;
	}

	/* set up piano controllers */
	app->piano[0] = ii_load_instrument("data/instrument_piano_0.ini", app->midi_event_batch);
	if(!app->piano[0])
	{
		printf("Failed to create piano!\n");
		return false;
	}
	app->piano[1] = ii_load_instrument("data/instrument_piano_1.ini", app->midi_event_batch);
	if(!app->piano[1])
	{
		printf("Failed to create piano!\n");
		return false;
	}

	/* set up drums controllers */
	app->drums = ii_load_instrument("data/instrument_drum_set.ini", app->midi_event_batch);
	if(!app->drums)
	{
		printf("Failed to create drums!\n");
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
	#ifdef ALLEGRO_UNIX
		#ifndef ALLEGRO_MACOSX
			if(app.kill_fluidsynth)
			{
				stop_fluidsynth();
			}
		#endif
	#endif
	if(app.guitar)
	{
		ii_destroy_instrument(app.guitar);
	}
	if(app.piano[0])
	{
		ii_destroy_instrument(app.piano[0]);
	}
	if(app.piano[1])
	{
		ii_destroy_instrument(app.piano[1]);
	}
	if(app.drums)
	{
		ii_destroy_instrument(app.drums);
	}
	t3f_finish();
	return 0;
}
