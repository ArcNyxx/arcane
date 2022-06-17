#!/bin/sh
# arcane - qmk keyboard configuration
# Copyright (C) 2022 ArcNyxx
# see LICENCE file for licensing information

# get the qmk firmware required to build arcane from zsa
# install the teensy loader cli for interfacing with the keyboard
git clone --recurse-submodules https://github.com/zsa/qmk_firmware/
pacman -S teensy_loader_cli

# downgrade avr-gcc to 8.3.0 if necessary
[ "$(avr-gcc --version | sed '1q' | cut '-d ' -f3 | cut -d. -f1)" -gt 8 ] &&
	pacman -U https://archive.archlinux.org/packages/a/avr-gcc/\
	avr-gcc-8.3.0-1-x86_64.pkg.tar.xz

# install flashing udev rules
qmk_firmware/util/udev/50-qmk.rules /etc/udev/rules.d/

# clone arcane, move it to the proper location, build, and flash
git clone https://github.com/arcnyxx/arcane/
mv arcane qmk_firmware/keyboards/ergodox_ez/glow/keymaps

cd qmk_firmware
make -j 1 ergodox_ez/glow:arcane:flash
