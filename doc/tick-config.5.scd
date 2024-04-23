TICK-CONFIG(5)

# NAME

tick-config - configuration file format for tick(1)

# DESCRIPTION

The _tick.ini_ file configures the keybinds for the keyboard commands defined in
tick(1), as well as general metronome settings such as the interval and pitch.

Each value is defined in the format of

*name*=*value*

with no spaces around the equal sign.

Comments must start with *#* as the first character of the line.
Inline comments are not supported.

## Configurable values

[- Name
:= Meaning
:- Default value
|- freq>
:- Frequency of the accented tick
:- 587
|- freq.
:- Frequency of the unaccented tick
:- 440
|- interval
:- Interval to increase/decrease BPM by
:- 20
|- up
:- Keybind for *+BPM*
:- k
|- down
:- Keybind for *-BPM*
:- j
|- next
:- Keybind for *Next preset*
:- l
|- prev
:- Keybind for *Prev preset*
:- h
|- toggle_play
:- Keybind for *Toggle play*
:- space
|- save
:- Keybind for *Save*
:- s
|- rename
:- Keybind for *Rename*
:- r
|- create
:- Keybind for *Create*
:- c
|- delete
:- Keybind for *Delete*
:- d
|- open_prompt
:- Keybind for *Open prompt*
:- :
|- quit
:- Keybind for *Quit*
:- q

## Available key names
Any single character may be used as the value for a keybind.
Some special keys are additionally supported and are case-sensitive.
- up
- down
- right
- left
- delete
- backspace
- tab
- enter
- space

# CAVEATS

The program does not check for duplicate keybinds.
It is up to the user to ensure that no key is binded to multiple commands.

# FILES

The tick program reads from *$XDG_CONFIG_HOME/tick/tick.ini* if the
*$XDG_CONFIG_HOME* environment variable is defined and non-empty.
Otherwise, it reads from *$HOME/.config/tick/tick.ini*
if *$HOME* is defined and non-empty.
Otherwise, the default configuration is used.

# SEE ALSO

tick(1)

# AUTHOR

Maintained by Coco Liliace <coco@liliace.dev>.
Up-to-date sources and mailing list can be found at https://sr.ht/~liliace/tick.
