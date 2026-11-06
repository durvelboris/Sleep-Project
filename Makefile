.SUFFIXES:

ifeq ($(strip $(DEVKITARM)),)
$(error "DEVKITARM n'est pas defini. Installe devkitPro ou utilise l'image devkitpro/devkitarm.")
endif

include $(DEVKITARM)/3ds_rules

TARGET          := SleepButton
APP_TITLE       := Veille 2DS
APP_DESCRIPTION := Veille logicielle et ecran eteint
APP_AUTHOR      := Boris

CTRULIB ?= $(DEVKITPRO)/libctru
ICON    := $(CTRULIB)/default_icon.png

ARCH    := -march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft
CC      := arm-none-eabi-gcc
CFLAGS  := -g -Wall -O2 -mword-relocations -ffunction-sections $(ARCH) \
           -I$(CTRULIB)/include -D__3DS__
LDFLAGS := -specs=3dsx.specs -g $(ARCH) -Wl,-Map,$(TARGET).map
LIBS    := -L$(CTRULIB)/lib -lctru -lm

OFILES  := $(patsubst %.c,%.o,$(wildcard source/*.c))

.PHONY: all clean

all: $(TARGET).3dsx

$(TARGET).elf: $(OFILES)
	$(CC) $(LDFLAGS) $(OFILES) $(LIBS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).smdh: $(ICON)
	smdhtool --create "$(APP_TITLE)" "$(APP_DESCRIPTION)" "$(APP_AUTHOR)" $(ICON) $@

$(TARGET).3dsx: $(TARGET).elf $(TARGET).smdh
	3dsxtool $< $@ --smdh=$(TARGET).smdh

clean:
	rm -f source/*.o $(TARGET).elf $(TARGET).3dsx $(TARGET).smdh $(TARGET).map
