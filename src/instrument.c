#include "t3f/t3f.h"
#include "t3f/controller.h"
#include "rtk/midi.h"

#include "instrument.h"
#include "instrument_drum_set.h"
#include "instrument_piano.h"
#include "instrument_guitar.h"
#include "midi_event.h"
#include "key.h"

static char command_buffer[256] = {0};
static int command_buffer_pos = 0;

static void ii_set_controller_key(T3F_CONTROLLER * cp, int i, int key)
{
	cp->binding[i].type = T3F_CONTROLLER_BINDING_KEY;
	cp->binding[i].button = key;
	cp->binding[i].flags = 0;
}

II_INSTRUMENT * ii_load_instrument(const char * fn, II_MIDI_EVENT_BATCH * bp)
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
		ip->midi_event_batch = bp;

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
		ii_add_midi_event(ip->midi_event_batch, RTK_MIDI_EVENT_TYPE_PROGRAM_CHANGE, ip->channel, ip->program, 0, 1);
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

static void clear_command_buffer(void)
{
	strcpy(command_buffer, "");
	command_buffer_pos = 0;
}

static void append_command_buffer(int key)
{
	if(command_buffer_pos < 255)
	{
		command_buffer[command_buffer_pos] = key;
		command_buffer_pos++;
		command_buffer[command_buffer_pos] = 0;
	}
}

void ii_instrument_logic(II_INSTRUMENT * ip)
{
	int program;
	int shift_amount;

	if(t3f_key[ALLEGRO_KEY_TAB])
	{
		ip->mode = 1 - ip->mode;
		t3f_key[ALLEGRO_KEY_TAB] = 0;
	}

	t3f_read_controller(ip->controller);
	t3f_update_controller(ip->controller);

	if(ip->controller->state[ip->controller->bindings - 1].pressed)
	{
		clear_command_buffer();
	}
	if(ip->controller->state[ip->controller->bindings - 1].released)
	{
		program = atoi(command_buffer);
		if(program > 0 && program <= 128)
		{
			ip->program = program - 1;
			ii_add_midi_event(ip->midi_event_batch, RTK_MIDI_EVENT_TYPE_PROGRAM_CHANGE, ip->channel, ip->program, 0, 1);
		}
		clear_command_buffer();
	}
	if(ip->controller->state[ip->controller->bindings - 1].held)
	{
		if(t3f_key[ALLEGRO_KEY_PAD_0])
		{
			append_command_buffer('0');
			t3f_key[ALLEGRO_KEY_PAD_0] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_PAD_1])
		{
			append_command_buffer('1');
			t3f_key[ALLEGRO_KEY_PAD_1] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_PAD_2])
		{
			append_command_buffer('2');
			t3f_key[ALLEGRO_KEY_PAD_2] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_PAD_3])
		{
			append_command_buffer('3');
			t3f_key[ALLEGRO_KEY_PAD_3] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_PAD_4])
		{
			append_command_buffer('4');
			t3f_key[ALLEGRO_KEY_PAD_4] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_PAD_5])
		{
			append_command_buffer('5');
			t3f_key[ALLEGRO_KEY_PAD_5] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_PAD_6])
		{
			append_command_buffer('6');
			t3f_key[ALLEGRO_KEY_PAD_6] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_PAD_7])
		{
			append_command_buffer('7');
			t3f_key[ALLEGRO_KEY_PAD_7] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_PAD_8])
		{
			append_command_buffer('8');
			t3f_key[ALLEGRO_KEY_PAD_8] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_PAD_9])
		{
			append_command_buffer('9');
			t3f_key[ALLEGRO_KEY_PAD_9] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_DOWN])
		{
			ip->program--;
			if(ip->program < 0)
			{
				ip->program = 127;
			}
			ii_add_midi_event(ip->midi_event_batch, RTK_MIDI_EVENT_TYPE_PROGRAM_CHANGE, ip->channel, ip->program, 0, 1);
			t3f_key[ALLEGRO_KEY_DOWN] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_UP])
		{
			ip->program++;
			if(ip->program > 127)
			{
				ip->program = 0;
			}
			ii_add_midi_event(ip->midi_event_batch, RTK_MIDI_EVENT_TYPE_PROGRAM_CHANGE, ip->channel, ip->program, 0, 1);
			t3f_key[ALLEGRO_KEY_UP] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_LSHIFT])
		{
			shift_amount = 1;
		}
		else
		{
			shift_amount = 12;
		}
		if(t3f_key[ALLEGRO_KEY_LEFT])
		{
			ip->base_note -= shift_amount;
			if(ip->base_note < 0)
			{
				ip->base_note = 108;
			}
			t3f_key[ALLEGRO_KEY_LEFT] = 0;
		}
		if(t3f_key[ALLEGRO_KEY_RIGHT])
		{
			ip->base_note += shift_amount;
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
