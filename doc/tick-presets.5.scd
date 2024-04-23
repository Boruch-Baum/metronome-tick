TICK-PRESESTS(5)

# NAME

tick-presets - presets file format for tick(1)

# DESCRIPTION

The _presets.ini_ file defines the _presets_ to load to the tick(1) program.

A _preset_ is an object consisting of a name, a BPM, and a rhythm.
Each preset is defined in the following format in the file:

\[*Preset name*]++
bpm=*bpm*++
rhythm=*rhythm*

with no spaces around the equal signs.

A rhythm is represented by a sequence of greater than symbols (*>*),
periods (*.*), or other characters.
*>* represets an accented tick while *.* represets a unaccented tick.
Any other character represents a rest.
See tick-config(5) for configuring the pitch of the accented and unaccented
ticks.

The preset name may not be longer than 61 characters,
and the rhythm may not be longer than 31 characters.

Comments must start with *#* as the first character of the line.
Inline comments are not supported.

# CAVEATS

The program does not check for duplicate preset names.
It is up to the user to ensure that each preset name in the file is unique.

# FILES

The tick program reads from *$XDG_DATA_HOME/tick/presets.ini* if the
*$XDG_DATA_HOME* environment variable is defined and non-empty.
Otherwise, it reads from *$HOME/.local/share/tick/presets.ini*
if *$HOME* is defined and non-empty.

# SEE ALSO

tick(1), tick-config(5)

# AUTHORS

Maintained by Coco Liliace <coco@liliace.dev>.
Up-to-date sources and mailing list can be found at https://sr.ht/~liliace/tick.
