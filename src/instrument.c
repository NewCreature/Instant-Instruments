#include "t3f/t3f.h"
#include "t3f/controller.h"
#include "MIDIA5/midia5.h"

#include "instrument.h"
#include "instrument_drum_set.h"
#include "instrument_piano.h"
#include "instrument_guitar.h"
#include "midi.h"
#include "key.h"

II_INSTRUMENT * ii_create_instrument(MIDIA5_OUTPUT_HANDLE * hp, int channel, int type, int option)
{
	II_INSTRUMENT * ip = NULL;

	ip = malloc(sizeof(II_INSTRUMENT));
	if(!ip)
	{
		goto fail;
	}
	memset(ip, 0, sizeof(II_INSTRUMENT));
	ip->midi_out = hp;
	ip->channel = channel;
	ip->type = type;

	/* create controller */
	switch(ip->type)
	{
		case II_INSTRUMENT_TYPE_DRUM_SET:
		{
			ip->controller = ii_create_drum_set_controller(option);
			break;
		}
		case II_INSTRUMENT_TYPE_PIANO:
		{
			ip->controller = ii_create_piano_controller(option);
			ip->base_note = 3 * 12 + option * 3 * 12;
			ip->chord_base_note = 3 * 12 + option * 3 * 12;
			break;
		}
		case II_INSTRUMENT_TYPE_GUITAR:
		{
			ip->controller = ii_create_guitar_controller(option);
			ip->base_note = 6 * 12;
			ip->chord_base_note = 3 * 12;
			break;
		}
	}
	if(!ip->controller)
	{
		goto fail;
	}

	/* set current MIDI program */
	switch(ip->type)
	{
		case II_INSTRUMENT_TYPE_DRUM_SET:
		{
			ip->program = 0;
			ii_send_program_change(ip->midi_out, ip->channel, ip->program);
			break;
		}
		case II_INSTRUMENT_TYPE_PIANO:
		{
			ip->program = 0;
			ii_send_program_change(ip->midi_out, ip->channel, ip->program);
			break;
		}
		case II_INSTRUMENT_TYPE_GUITAR:
		{
			ip->program = 30;
			ii_send_program_change(ip->midi_out, ip->channel, ip->program);
			break;
		}
	}

	return ip;

	fail:
	{
		ii_destroy_instrument(ip);
	}
	return NULL;
}

void ii_destroy_instrument(II_INSTRUMENT * ip)
{
	if(ip)
	{
		if(ip->controller)
		{
			t3f_destroy_controller(ip->controller);
		}
		free(ip);
	}
}

void ii_instrument_logic(II_INSTRUMENT * ip)
{

	if(t3f_key[ALLEGRO_KEY_TAB])
	{
		ip->mode = 1 - ip->mode;
		t3f_key[ALLEGRO_KEY_TAB] = 0;
	}

	t3f_read_controller(ip->controller);
	t3f_update_controller(ip->controller);

	if(ip->controller->state[ip->controller->bindings - 1].held)
	{
		if(t3f_key[ALLEGRO_KEY_DOWN])
		{
			ip->program--;
			if(ip->program < 0)
			{
				ip->program = 127;
			}
			ii_send_program_change(ip->midi_out, ip->channel, ip->program);
			t3f_key[ALLEGRO_KEY_DOWN] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_UP])
		{
			ip->program++;
			if(ip->program > 127)
			{
				ip->program = 0;
			}
			ii_send_program_change(ip->midi_out, ip->channel, ip->program);
			t3f_key[ALLEGRO_KEY_UP] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_LEFT])
		{
			ip->base_note -= 12;
			if(ip->base_note < 0)
			{
				ip->base_note = 108;
			}
			t3f_key[ALLEGRO_KEY_LEFT] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_RIGHT])
		{
			ip->base_note += 12;
			if(ip->base_note > 108)
			{
				ip->base_note = 0;
			}
			t3f_key[ALLEGRO_KEY_RIGHT] = 0;
		}
	}
	else
	{
		switch(ip->type)
		{
			case II_INSTRUMENT_TYPE_DRUM_SET:
			{
				ii_drum_set_logic(ip);
				break;
			}
			case II_INSTRUMENT_TYPE_PIANO:
			{
				ii_piano_logic(ip);
				break;
			}
			case II_INSTRUMENT_TYPE_GUITAR:
			{
				ii_guitar_logic(ip);
				break;
			}
		}
	}
}
