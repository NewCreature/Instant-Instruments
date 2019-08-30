#include "t3f/t3f.h"
#include "t3f/controller.h"
#include "MIDIA5/midia5.h"

#include "midi.h"
#include "instrument.h"
#include "controller.h"

static int ii_note_chart[2][12] =
{
	{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11},
	{0, 2, 4, 5, 7, 9, 10, 12, 14, 15, 17, 19}
};

static void ii_kill_guitar_note(II_INSTRUMENT * ip, int note_pos)
{
	int i;

	for(i = 0; i < ip->key_note[note_pos].notes; i++)
	{
		ii_send_note_off(ip->midi_out, ip->channel, ip->key_note[note_pos].note[i], 100);
	}
	ip->key_note[note_pos].notes = 0;
}

static void ii_play_guitar_note(II_INSTRUMENT * ip, int note_pos)
{
	int note = ip->base_note + ii_note_chart[ip->mode][ip->key_rel_note[note_pos]];

	ii_kill_guitar_note(ip, note_pos);
	ii_send_note_on(ip->midi_out, ip->channel, note, 100);
	ip->key_note[note_pos].note[0] = note;
	ip->key_note[note_pos].notes = 1;
}

static void ii_play_guitar_chord(II_INSTRUMENT * ip, int note_pos)
{
	int note = ip->chord_base_note + ii_note_chart[ip->mode][ip->key_rel_note[note_pos]];

	ii_kill_guitar_note(ip, note_pos);
	ii_send_note_on(ip->midi_out, ip->channel, note, 100);
	ii_send_note_on(ip->midi_out, ip->channel, note + 4, 100);
	ii_send_note_on(ip->midi_out, ip->channel, note + 7, 100);
	ip->key_note[note_pos].note[0] = note;
	ip->key_note[note_pos].note[1] = note + 4;
	ip->key_note[note_pos].note[2] = note + 7;
	ip->key_note[note_pos].notes = 3;
}

static void ii_play_guitar_power_chord(II_INSTRUMENT * ip, int note_pos)
{
	int note = ip->chord_base_note + ii_note_chart[ip->mode][ip->key_rel_note[note_pos]];

	ii_kill_guitar_note(ip, note_pos);
	ii_send_note_on(ip->midi_out, ip->channel, note, 100);
	ii_send_note_on(ip->midi_out, ip->channel, note + 7, 100);
	ip->key_note[note_pos].note[0] = note;
	ip->key_note[note_pos].note[1] = note + 7;
	ip->key_note[note_pos].notes = 2;
}

void ii_guitar_logic(II_INSTRUMENT * ip)
{
	int frets = 0;
	bool power_chord = false;
	int i, c;

	/* detect chords */
	c = 0;
	for(i = 0; i < ip->controller->bindings - 2; i++)
	{
		if(ip->controller->state[i].held)
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

	if(ip->controller->state[ip->controller->bindings - 2].pressed)
	{
		for(i = 0; i < ip->controller->bindings - 2; i++)
		{
			if(ip->controller->state[i].held)
			{
				if(frets > 1)
				{
					if(power_chord)
					{
						ii_play_guitar_power_chord(ip, i);
					}
					else
					{
						ii_play_guitar_chord(ip, i);
					}
					break;
				}
				else
				{
					ii_play_guitar_note(ip, i);
				}
			}
		}
	}
	/* check for hammer-on/pull-off */
	else if(ip->controller->state[ip->controller->bindings - 2].held)
	{
		for(i = 0; i < ip->controller->bindings - 2; i++)
		{
			if(ip->controller->state[i].pressed)
			{
				ii_play_guitar_note(ip, i);
			}
			if(ip->controller->state[i].released)
			{
				ii_kill_guitar_note(ip, i);
			}
		}
	}
	for(i = 0; i < ip->controller->bindings - 2; i++)
	{
		if(ip->controller->state[i].released)
		{
			ii_kill_guitar_note(ip, i);
		}
	}
}
