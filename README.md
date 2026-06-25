# tick
A preset-oriented metronome TUI.

> [!NOTE]
> This is a fork of  https://git.sr.ht/~liliace/tick .
> The only functional change made (so far) has been
> to move the `preset.ini` file from `XDG_DATA_HOME` to
> `XDG_CONFIG_HOME`.

## Installation
Build time dependencies:
- [alsa-lib](https://www.alsa-project.org/wiki/Download)
- [scdoc](https://sr.ht/~sircmpwn/scdoc/)
- [GNU Make](https://www.gnu.org/software/make/#download)
- A C compiler

```
make
doas make install
```

To uninstall
```
doas make uninstall
```

## Usage
See [tick(1)](doc/tick.1.scd), [tick-config(5)](doc/tick-config.5.scd), [tick-presets(5)](doc/tick-presets.5.scd)

## Contributing
For the upstream, send discussions and patches to the public inbox at
https://lists.sr.ht/~liliace/public-inbox .

For this fork, use standard github procedures.
