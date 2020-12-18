# sah

Sah is a Simple Aur Helper, unproblematic.

The purpose of sah is to be an aur helper that neither introduces unsafe flags by strictly sticking to the AUR, nor try to be holier than thou by not installing packages with "unsafe" PKGBUILDs deemed safe by makepkg.

## Features
It is meant to have feature parity with most other AUR helpers, even in terms of quality of life and convenience. Currently implemented are:
- Downloading PKGBUILDs from the AUR
- Installation of packages
- Removing make dependencies after compilation
- Searching for packages in the AUR
- Upgrading outdated packages
- Optionally coloured output(partial)

### Nice to haves
- Pacman-like interface
- Very few dependencies, some of which you are likely to already have
- When installing sah defers to makepkg itself, and lets it do what it does best

## Installation

### Dependencies

Sah needs libcurl, yajl, and libgit2 to compile and run. These are easily installed by running

```sh
pacman -S libcurl yajl libgit2
```

as root.

### Install

Simply clone this repository and run

```sh
make install
```

with root privileges.

## Thank you

Without [package-query](https://github.com/archlinuxfr/package-query/) this project would not exist. Thank you for the great piece of software.
