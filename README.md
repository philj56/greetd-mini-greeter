# greetd-mini-greeter

An extremely simple GTK4 greeter for greetd, inspired by
[lightdm-mini-greeter](https://github.com/prikhi/lightdm-mini-greeter).

![Screenshot](screenshot.png)

## Usage

Follow the same steps as for e.g. gtkgreet in the [greetd
wiki](https://man.sr.ht/~kennylevinsen/greetd/). See the man page for sway
config suggestions.

## Install

### Arch

greetd-mini-greeter is available on the [AUR](https://aur.archlinux.org/packages/greetd-mini-greeter-git/):
```sh
paru -S greetd-mini-greeter-git
```

### Source
```sh
meson build
ninja -C build
```
