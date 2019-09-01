#ifndef II_MIDI_EVENT_H
#define II_MIDI_EVENT_H

#include "MIDIA5/midia5.h"

#define II_MIDI_EVENT_MAX_DATA 8

typedef struct
{

	int type;
	int channel;
	int data[II_MIDI_EVENT_MAX_DATA];
	int delay;

} II_MIDI_EVENT;

typedef struct
{

	MIDIA5_OUTPUT_HANDLE * midi_out;
	II_MIDI_EVENT * event;
	int event_count;
	int current_event;

} II_MIDI_EVENT_BATCH;

II_MIDI_EVENT_BATCH * ii_create_midi_event_batch(MIDIA5_OUTPUT_HANDLE * hp, int max);
void ii_destroy_midi_event_batch(II_MIDI_EVENT_BATCH * bp);

void ii_add_midi_event(II_MIDI_EVENT_BATCH * bp, int type, int channel, int d1, int d2, int delay);

void ii_process_midi_event_batch(II_MIDI_EVENT_BATCH * bp);

#endif
