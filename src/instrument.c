#include "t3f/t3f.h"
#include "t3f/controller.h"
#include "MIDIA5/midia5.h"

#include "instrument.h"
#include "instrument_drum_set.h"
#include "instrument_piano.h"
#include "instrument_guitar.h"
#include "midi.h"
#include "key.h"

static void ii_set_controller_key(T3F_CONTROLLER * cp, int i, int key)
{
	cp->binding[i].type = T3F_CONTROLLER_BINDING_KEY;
	cp->binding[i].button = key;
	cp->binding[i].flags = 0;
}

II_INSTRUMENT * ii_load_instrument(const char * fn, MIDIA5_OUTPUT_HANDLE * hp)
{
	ALLEGRO_CONFIG * cp;
	II_INSTRUMENT * ip = NULL;
	const char * val;
	char buf[256];
	int inputs = 0;
	int i;

	cp = al_load_config_file(fn);
	if(cp)
	{
		ip = malloc(sizeof(II_INSTRUMENT));
		if(!ip)
		{
			goto fail;
		}
		memset(ip, 0, sizeof(II_INSTRUMENT));
		ip->midi_out = hp;

		val = al_get_config_value(cp, "Settings", "type");
		if(val)
		{
			ip->type = atoi(val);
		}
		val = al_get_config_value(cp, "Settings", "channel");
		if(val)
		{
			ip->channel = atoi(val);
		}
		val = al_get_config_value(cp, "Settings", "program");
		if(val)
		{
			ip->program = atoi(val);
		}
		val = al_get_config_value(cp, "Settings", "base_note");
		if(val)
		{
			ip->base_note = atoi(val);
		}
		val = al_get_config_value(cp, "Settings", "chord_base_note");
		if(val)
		{
			ip->chord_base_note = atoi(val);
		}
		val = al_get_config_value(cp, "Settings", "inputs");
		if(val)
		{
			inputs = atoi(val);
		}
		if(inputs > 0)
		{
			ip->controller = t3f_create_controller(inputs);
			if(!ip->controller)
			{
				goto fail;
			}
			for(i = 0; i < inputs; i++)
			{
				sprintf(buf, "input_%d_key", i);
				val = al_get_config_value(cp, "Settings", buf);
				if(val)
				{
					ii_set_controller_key(ip->controller, i, atoi(val));
				}
				ip->key_rel_note[i] = i;
				sprintf(buf, "input_%d_rel_note", i);
				val = al_get_config_value(cp, "Settings", buf);
				if(val)
				{
					ip->key_rel_note[i] = atoi(val);
				}
			}
			t3f_clear_controller_state(ip->controller);
		}
		else
		{
			goto fail;
		}
		ii_send_program_change(ip->midi_out, ip->channel, ip->program);
		return ip;
	}

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
