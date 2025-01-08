# tick
A preset-oriented metronome TUI.

## Installation
To build from source, tick requires
the [`alsa-lib`](https://www.alsa-project.org/wiki/Download) library
and [`scdoc`](https://sr.ht/~sircmpwn/scdoc/) for generating man pages.
```
make
doas make PREFIX=/usr install
```
To uninstall
```
doas make PREFIX=/usr uninstall
```

## Usage
See tick(1)
