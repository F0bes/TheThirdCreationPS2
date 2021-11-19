#----------------------------------------------------------------------------
# File:		Makefile
# Author:	Tony Saveski, t_saveski@yahoo.com
#----------------------------------------------------------------------------
CC=/usr/local/ps2dev/ee/bin/mips64r5900-sf-elf-gcc
AS=/usr/local/ps2dev/ee/bin/mips64r5900-sf-elf-as
LD=/usr/local/ps2dev/ee/bin/mips64r5900-sf-elf-ld
BMP2C=/home/gdelaros/ps2tut_02c/bmp2c/bmp2c

DEMO = ps2flight

OBJ_DIR = obj
BMP_DIR = resources
PS2DEV		= /usr/local/ps2dev
GCC_DIR		= $(PS2DEV)
STDC_DIR	= $(PS2DEV)/gustavo

CFLAGS = -Wall -W -EL -G0 -O0 -mips3 -nostdlib -DPS2_EE -fshort-double\
	-I$(GCC_DIR)/ee/lib/gcc-lib/mips64r5900-sce-elf/2.9-ee-991111b/include \
	-I$(STDC_DIR)/common/include \
	-I$(STDC_DIR)/ee/include \
	-L$(STDC_DIR)/ee/lib 


BMP_SRC = stone.c stone1.c stone2.c hrntpnl.c clouds.c
C_SRC = gs.c g2.c math.c matrix.c clip.c loadmodule.c pad.c pukko.c  $(DEMO).c $(BMP_SRC)
S_SRC = crt0.s ps2_asm.s dma_asm.s gs_asm.s fptoint.s hw.s

C_OBJ = $(addprefix $(OBJ_DIR)/, $(C_SRC:.c=.o))
S_OBJ = $(addprefix $(OBJ_DIR)/, $(S_SRC:.s=.o))

$(DEMO).elf: $(C_OBJ) $(S_OBJ)
	@echo "-------------------------------------------------"
	$(CC) $(CFLAGS) -Tlinkfile -o $(DEMO).elf $(C_OBJ) $(S_OBJ)  -lm -lkernel -lc -lgcc

$(OBJ_DIR)/%.o: %.c
	@echo "-------------------------------------------------"
	$(CC) -c $(CFLAGS) $< -o $@

$(OBJ_DIR)/%.o: %.s
	@echo "-------------------------------------------------"
	$(CC) -xassembler-with-cpp -c $(CFLAGS) $< -o $@

#--%.c: $(BMP_DIR)/%.bmp
#--	@echo "-------------------------------------------------"
#--	$(BMP2C) $< $(*F) > $@

clean:
	rm -f $(C_OBJ) $(S_OBJ) *.elf $(BMP_SRC)

#hover.c: $(BMP_DIR)/hover.bmp
#message.c: $(BMP_DIR)/message.bmp
