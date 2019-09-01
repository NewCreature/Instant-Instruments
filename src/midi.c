#include "MIDIA5/midia5.h"
#include "rtk/midi.h"

const char * ii_midi_instrument_name[128] =
{
	/* piano */
	"Acoustic Grand Piano",
	"Bright Acoustic Piano",
	"Electric Grand Piano",
	"Honky-tonk Piano",
	"Electric Piano 1",
	"Electric Piano 2",
	"Harpsichord",
	"Clavinet",

	/* chromatic percussion */
	"Celesta",
	"Glockenspiel",
	"Music Box",
	"Vibraphone",
	"Marimba",
	"Xylophone",
	"Tubular Bells",
	"Dulcimer",

	/* organ */
	"Drawbar Organ",
	"Percussive Organ",
	"Rock Organ",
	"Church Organ",
	"Reed Organ",
	"Accordion",
	"Harmonica",
	"Tango Accordion",

	/* guitar */
    "Acoustic Guitar (nylon)",
	"Acoustic Guitar (steel)",
	"Electric Guitar (jazz)",
	"Electric Guitar (clean)",
	"Electric Guitar (muted)",
	"Overdriven Guitar",
	"Distortion Guitar",
	"Guitar Harmonics",

	/* bass */
	"Acoustic Bass",
	"Electric Bass (finger)",
	"Electric Bass (pick)",
	"Fretless Bass",
	"Slap Bass 1",
	"Slap Bass 2",
	"Synth Bass 1",
	"Synth Bass 2",

	/* strings */
    "Violin",
	"Viola",
	"Cello",
	"Contrabass",
	"Tremolo Strings",
	"Pizzicato Strings",
	"Orchestral Harp",
	"Timpani",

	/* ensemble */
	"String Ensemble 1",
	"String Ensemble 2",
	"Synth Strings 1",
	"Synth Strings 2",
	"Choir Aahs",
	"Voice Oohs",
	"Synth Choir",
	"Orchestra Hit",

	/* brass */
	"Trumpet",
	"Trombone",
	"Tuba",
	"Muted Trumpet",
	"French Horn",
	"Brass Section",
	"Synth Brass 1",
	"Synth Brass 2",

	/* reed */
	"Soprano Sax",
	"Alto Sax",
	"Tenor Sax",
	"Baritone Sax",
	"Oboe",
	"English Horn",
	"Bassoon",
	"Clarinet",

	/* pipe */
	"Piccolo",
	"Flute",
	"Recorder",
	"Pan Flute",
	"Blown bottle",
	"Shakuhachi",
	"Whistle",
	"Ocarina",

	/* synth lead */
	"Lead 1 (square)",
	"Lead 2 (sawtooth)",
	"Lead 3 (calliope)",
	"Lead 4 (chiff)",
	"Lead 5 (charang)",
	"Lead 6 (voice)",
	"Lead 7 (fifths)",
	"Lead 8 (bass + lead)",

	/* synth pad */
	"Pad 1 (new age)",
	"Pad 2 (warm)",
	"Pad 3 (polysynth)",
	"Pad 4 (choir)",
	"Pad 5 (bowed)",
	"Pad 6 (metallic)",
	"Pad 7 (halo)",
	"Pad 8 (sweep)",

	/* synth effects */
	"FX 1 (rain)",
	"FX 2 (soundtrack)",
	"FX 3 (crystal)",
	"FX 4 (atmosphere)",
	"FX 5 (brightness)",
	"FX 6 (goblins)",
	"FX 7 (echoes)",
	"FX 8 (sci-fi)",

	/* ethnic */
	"Sitar",
	"Banjo",
	"Shamisen",
	"Koto",
	"Kalimba",
	"Bagpipe",
	"Fiddle",
	"Shanai",

	/* percussive */
	"Tinkle Bell",
	"Agogo",
	"Steel Drums",
	"Woodblock",
	"Taiko Drum",
	"Melodic Tom",
	"Synth Drum",
	"Reverse Cymbal",

	/* sound effects */
	"Guitar Fret Noise",
	"Breath Noise",
	"Seashore",
	"Bird Tweet",
	"Telephone Ring",
	"Helicopter",
	"Applause",
	"Gunshot"
};

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
