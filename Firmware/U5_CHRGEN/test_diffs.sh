#!/bin/bash

set -x

# CHARGEN 8k+ ROM
# CHARGEN 64k is just 8 x 8k repeated

dd if=External/GALAXY_CHRGENP.bin of=chrgenp_0.bin skip=0    bs=1 count=2048
dd if=External/GALAXY_CHRGENP.bin of=chrgenp_1.bin skip=2048 bs=1 count=2048
dd if=External/GALAXY_CHRGENP.bin of=chrgenp_2.bin skip=4096 bs=1 count=2048
dd if=External/GALAXY_CHRGENP.bin of=chrgenp_3.bin skip=6144 bs=1 count=2048

# lower 4k = 2 copies of 2k original CHARGEN from Galaksija
#            in other words, chrgenp_0.bin == chrgenp_1.bin
# upper 4k = the graphics map

# CHRGEN_MIPRO.bin is identical to PVVs chrgenp_0.bin and chrgenp_1.bin

g++ -o rom_chrgen src/rom_chrgen.cpp

./rom_chrgen External/GALAXY_CHRGENP.bin > chrgenp.txt

./rom_chrgen External/CHRGEN_MIPRO.bin > chrgen_mipro.txt
./rom_chrgen External/CHRGEN_ELEKTRONIKA_INZENJERING.bin > chrgen_elinz.txt
./rom_chrgen chrgenp_0.bin > chrgenp_0.txt
./rom_chrgen chrgenp_1.bin > chrgenp_1.txt

diff -a chrgen_mipro.txt chrgen_elinz.txt
diff -a chrgen_mipro.txt chrgenp_0.txt
diff -a chrgen_mipro.txt chrgenp_1.txt
