#!/bin/bash

set -x

dd if=External/GALAXY_ROM_v8_6.bin of=gp_rom_A.bin skip=0     bs=1 count=4096
dd if=External/GALAXY_ROM_v8_6.bin of=gp_rom_B.bin skip=4096  bs=1 count=4096
dd if=External/GALAXY_ROM_v8_6.bin of=gp_rom_C.bin skip=8192  bs=1 count=4096
dd if=External/GALAXY_ROM_v8_6.bin of=gp_rom_D.bin skip=12288 bs=1 count=4096

diff -b gp_rom_A.bin External/ROM_A_with_ROM_B_init_ver_29.bin
diff -b gp_rom_B.bin External/ROM_B.bin
diff -b gp_rom_C.bin External/ROM_C.bin

cat gp_rom_A.bin gp_rom_B.bin gp_rom_C.bin ../../SDOS_V8C_GP40/SDOS.bin > gp40_16k.bin
cat gp40_16k.bin gp40_16k.bin gp40_16k.bin gp40_16k.bin > ../GP40_U11_ROM.bin

