@echo off
tasm.exe -85 -b dos_rk.asm SDOS.BIN dos_rk.lst
rem copy hd.bin /b + SDOS.BIN /b SDOS.ORI /b
rem copy RKO_hd.bin /b + SDOS.BIN /b SDOS.RKO /b
