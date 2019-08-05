#include "MIDIA5/midia5.h"
#include "rtk/midi.h"

void ii_send_program_change(MIDIA5_OUTPUT_HANDLE * hp, int channel, int program)
{
	midia5_send_data(hp, RTK_MIDI_EVENT_TYPE_PROGRAM_CHANGE + channel);
	midia5_send_data(hp, program);
}

void ii_send_note_on(MIDIA5_OUTPUT_HANDLE * hp, int channel, int note, int velocity)
{
	midia5_send_data(hp, RTK_MIDI_EVENT_TYPE_NOTE_ON + channel);
	midia5_send_data(hp, note);
	midia5_send_data(hp, velocity);
}

void ii_send_note_off(MIDIA5_OUTPUT_HANDLE * hp, int channel, int note, int velocity)
{
	midia5_send_data(hp, RTK_MIDI_EVENT_TYPE_NOTE_OFF + channel);
	midia5_send_data(hp, note);
	midia5_send_data(hp, velocity);
}
