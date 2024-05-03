TICK(1)

# NAME

tick - a preset-oriented metronome TUI

# SYNOPSIS

*tick* [*-v*|*--version*] [*config*|*presets*]

# DESCRIPTION

When called with the *config* or *presets* argument,
the tick program opens the config or presets file for editing,
using the command defined in the $EDITOR environment variable.
See tick-config(5) and tick-presets(5) for details of the files.

When called without arguments,
the tick program plays metronome at the specified BPM and rhythm.

The BPM and rhythm can be adjusted manually or loaded from a _preset_.
A _preset_ is an object consisting of a name, a BPM, and a rhythm.
See tick-presets(5) for defining presets.

The currently playing preset is displayed in the format of

(*PRESET NAME*) *RHYTHM* @ *BPM*

prepended by an asterisk (\*) if the BPM or rhythm have been modified but not
yet saved:

\*(*PRESET NAME*) *RHYTHM* @ *BPM*

The metronome can be controlled by a set of keyboard commands.
See tick-config(5) for configuring the keybinds.

## KEYBOARD COMMANDS

*+BPM*++
*-BPM*
	Increase/decrease the BPM by _interval_.
	The changes will not be saved to the preset until the *Save* command is run.

	See tick-config(5) for configuring the value of _interval_.

*Create*
	Create a new preset with the current rhythm and BPM.

	This opens a prompt for the new preset name, which cannot be longer than 61
	characters.

*Delete*
	Delete the preset.

*Next preset*++
*Prev preset*
	Cycle through the presets.

*Quit*
	Exit the program.

*Rename*
	Rename the preset.

	This opens a prompt for the new preset name, which cannot be longer than 61
	characters.

*Save*
	Save the rhythm and BPM of the preset.

*Show prompt*
	Bring up the prompt to enter a prompt command.
	See the *PROMPT COMMANDS* section below.

*Toggle play*
	Toggle between playing and paused.

## PROMPT COMMANDS

*:bpm* <bpm>
	Set the BPM.

*:preset* <name>
	Apply the preset. Case-sensitive.

*:rhythm* <rhythm>
	Set the rhythm.

	A rhythm is represented by a sequence of greater than symbols (*>*),
	periods (*.*), or other characters.
	*>* represets an accented tick while *.* represets a unaccented tick.
	Any other character represents a rest.
	See tick-config(5) for configuring the pitch of the accented and unaccented
	ticks.

	The rhythm cannot be longer than 31 characters.

*:set* <rhythm>*@*<bpm>
	Set the BPM and rhythm.
	Note the lack of space between rhythm and BPM,
	differing from the display format.

The prompt, opened by *Show prompt* or any other keyboard command that prompts
for further input (eg. *Rename* prompting for new preset name),
may be cancelled with an empty input.

# CAVEATS

The program does not check for duplicate preset names.
It is up to the user to ensure that the preset name is not already in use
when creating or renaming a preset.

# SEE ALSO

tick-config(5), tick-presets(5)

# AUTHORS

Maintained by Coco Liliace <coco@liliace.dev>.
Up-to-date sources and mailing list can be found at https://sr.ht/~liliace/tick.
