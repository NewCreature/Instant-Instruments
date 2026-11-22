#include "t3f/t3f.h"
#include <fluidsynth.h>
#include "synthesizer.h"

#define _II_FLUIDSYNTH_BUFFER_SIZE 512

typedef struct
{

    ALLEGRO_THREAD * thread;
    ALLEGRO_AUDIO_STREAM * audio_stream;
	fluid_synth_t * synthesizer;
	fluid_settings_t * synthesizer_settings;

} _II_SYNTHESIZER_DATA;

static void _ii_fluid_synth_destroy(void * data)
{
    _II_SYNTHESIZER_DATA * synthesizer_data = (_II_SYNTHESIZER_DATA *)data;

    if(synthesizer_data)
    {
        if(synthesizer_data->thread)
        {
            al_destroy_thread(synthesizer_data->thread);
        }
        if(synthesizer_data->synthesizer)
        {
            delete_fluid_synth(synthesizer_data->synthesizer);
        }
        if(synthesizer_data->synthesizer_settings)
        {
            delete_fluid_settings(synthesizer_data->synthesizer_settings);
        }
        if(synthesizer_data->audio_stream)
        {
            al_destroy_audio_stream(synthesizer_data->audio_stream);
        }
        free(synthesizer_data);
    }
}

static void _ii_fluid_synth_note_on(void * data, int channel, int note, int velocity)
{
    _II_SYNTHESIZER_DATA * synthesizer_data = (_II_SYNTHESIZER_DATA *)data;

    fluid_synth_noteon(synthesizer_data->synthesizer, channel, note, velocity);
}

static void _ii_fluid_synth_note_off(void * data, int channel, int note)
{
    _II_SYNTHESIZER_DATA * synthesizer_data = (_II_SYNTHESIZER_DATA *)data;

    fluid_synth_noteoff(synthesizer_data->synthesizer, channel, note);
}

static void _ii_fluid_synth_program_change(void * data, int channel, int program)
{
    _II_SYNTHESIZER_DATA * synthesizer_data = (_II_SYNTHESIZER_DATA *)data;

    fluid_synth_program_change(synthesizer_data->synthesizer, channel, program);
}

static void _ii_fluid_synth_reset(void * data)
{
    _II_SYNTHESIZER_DATA * synthesizer_data = (_II_SYNTHESIZER_DATA *)data;

    fluid_synth_system_reset(synthesizer_data->synthesizer);
}

static void * _ii_fluid_synth_thread_proc(ALLEGRO_THREAD * thread, void * data)
{
    _II_SYNTHESIZER_DATA * synthesizer_data = (_II_SYNTHESIZER_DATA *)data;
    ALLEGRO_MIXER * default_mixer = NULL;
    ALLEGRO_EVENT_SOURCE * event_source = NULL;
    ALLEGRO_EVENT_QUEUE * event_queue;
    ALLEGRO_EVENT event;
    int16_t * fragment;

	default_mixer = al_get_default_mixer();
	if(!default_mixer)
	{
        goto fail;
	}
    event_source = al_get_audio_stream_event_source(synthesizer_data->audio_stream);
    if(!event_source)
    {
        goto fail;
    }
    event_queue = al_create_event_queue();
    if(!event_queue)
    {
        goto fail;
    }
    al_register_event_source(event_queue, event_source);
    al_attach_audio_stream_to_mixer(synthesizer_data->audio_stream, default_mixer);
	al_set_audio_stream_playing(synthesizer_data->audio_stream, true);
    while(!al_get_thread_should_stop(thread))
    {
        al_wait_for_event(event_queue, &event);
        if(event.type == ALLEGRO_EVENT_AUDIO_STREAM_FRAGMENT)
        {
            fragment = al_get_audio_stream_fragment(synthesizer_data->audio_stream);
            if(fragment)
            {
                if(fluid_synth_write_s16(synthesizer_data->synthesizer, _II_FLUIDSYNTH_BUFFER_SIZE, fragment, 0, sizeof(uint16_t), fragment, 1, sizeof(int16_t)) == FLUID_OK)
                {
                    if(!al_set_audio_stream_fragment(synthesizer_data->audio_stream, fragment))
                    {
                        break;
                    }
                }
                else
                {
                    break;
                }
            }
        }
    }
    al_destroy_event_queue(event_queue);

    return NULL;

    fail:
    {
        return NULL;
    }
}

II_SYNTHESIZER * ii_create_fluid_synth_synthesizer(void)
{
    II_SYNTHESIZER * synthesizer = NULL;
    _II_SYNTHESIZER_DATA * synthesizer_data = NULL;

    /* backend specific data */
    synthesizer_data = malloc(sizeof(_II_SYNTHESIZER_DATA));
    if(!synthesizer_data)
    {
        goto fail;
    }
    memset(synthesizer_data, 0, sizeof(_II_SYNTHESIZER_DATA));
	synthesizer_data->audio_stream = al_create_audio_stream(2, _II_FLUIDSYNTH_BUFFER_SIZE, 44100, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
	if(!synthesizer_data->audio_stream)
	{
        goto fail;
	}
    synthesizer_data->thread = al_create_thread(_ii_fluid_synth_thread_proc, synthesizer_data);
    if(!synthesizer_data->thread)
    {
        goto fail;
    }

	/* synthesizer settings */
	synthesizer_data->synthesizer_settings = new_fluid_settings();
	if(!synthesizer_data->synthesizer_settings)
	{
        goto fail;
	}
	fluid_settings_setnum(synthesizer_data->synthesizer_settings, "synth.sample-rate", 44100);
	fluid_settings_setint(synthesizer_data->synthesizer_settings, "synth.polyphony", 128);
	
	synthesizer_data->synthesizer = new_fluid_synth(synthesizer_data->synthesizer_settings);
	if(!synthesizer_data->synthesizer)
	{
        goto fail;
	}
	if(fluid_synth_sfload(synthesizer_data->synthesizer, "data/GeneralUser-GS.sf2", 1) == FLUID_FAILED)
	{
        goto fail;
	}

    synthesizer = malloc(sizeof(II_SYNTHESIZER));
    if(!synthesizer)
    {
        goto fail;
    }
    memset(synthesizer, 0, sizeof(II_SYNTHESIZER));
    synthesizer->data = synthesizer_data;
    synthesizer->destroy = _ii_fluid_synth_destroy;
    synthesizer->note_on = _ii_fluid_synth_note_on;
    synthesizer->note_off = _ii_fluid_synth_note_off;
    synthesizer->program_change = _ii_fluid_synth_program_change;
    synthesizer->reset = _ii_fluid_synth_reset;
    al_start_thread(synthesizer_data->thread);

    return synthesizer;

    fail:
    {
        _ii_fluid_synth_destroy(synthesizer_data);
        if(synthesizer)
        {
            free(synthesizer);
        }
        return NULL;
    }
}
