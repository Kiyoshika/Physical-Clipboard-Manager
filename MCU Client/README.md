# MCU Client
The microcontroller acts as a client that sends commands to a "server" to interact with the clipboard (via WinAPI).

# Which MCU(s) are supported by this code
This code was written for the STM32F411xx MCU. You may need to tweak the code (specifically the addresses in `addresses.c`) to match your needs.

Consult your data sheet for the memory layouts for flash memory.

# Building
This code was written and built using STM32CubeIDE. If you wish to develop outside of this, you can use STM32CubeMX to change the config to generate a Makefile instead of an STM32CubeIDE project.

# Pinout
There are four (4) copy buttons and four (4) paste buttons. ALL of these must be wired properly or MCU will send garbage signals to the server.

Each button is configured as INPUT and using pull down resistor to force default signal to 0, screenshot will be attached here once I finish assembling the board.

Below is a mapping of the pins:

```text
COPY BUTTONS
==============
COPY1 - PA1
COPY2 - PA3
COPY3 - PA5
COPY4 - PA7

PASTE BUTTONS
==============
PASTE1 - PB6
PASTE2 - PB4
PASTE3 - PA15
PASTE4 - PA10
```

# General Config
By default I'm allowing a maximum of 255 characters per clipboard buffer. This was a somewhat arbitrary choice but can be modified to your needs, although I have not defined a global like I should have and would require a bit of manual work to reset to a new constant.

# Flash Memory Info
Four addresses were chosen from sector 7 of flash memory to store contents from the clipboard (see `addresses.c`).

There is a `METADATA` constant and a `BEGIN` constant for each address. The metadata holds a `uint32_t` representing the length of the buffer; this is so we can know how long the buffer is when we need to fetch the content.

The metadata is *always* four bytes (0x04) ahead of the beginning address. From the beginning address to the next 252 bytes is free memory we can write to for that particular clipboard buffer.

Note that to write to flash memory (properly), we have to clear the entire sector (in this case, sector 7). To preserve the other clipboards, each time we want to clear the flash memory, we store all the current buffers (except the one we're about to write to) in stack allocated character arrays, clear the memory, and rewrite the buffers and metadata back in their same positions. This is probaby not terribly efficient but there isn't really an alternative (I've tried writing to flash without clearing but it was behaving strange). See `flash_memory.c` for details on the implementation.

# Server Communication
Communication is done via Serial communication (see `USB_DEVICE/App/usbd_cdc_if.c` for some of the implementation details)

For copying data, we send a simple "COPY1", "COPY2", etc. command to the server. The server will take this and fetch the clipboard content, then prepend our original command (e.g. COPY1) with the actual content to store back to the MCU. The MCU will then take this data, and depending on which button was pressed (COPY1, COPY2, ...) it will store the buffer into the appropriate memory address (but first it must clear flash memory to do so, see above section for details).

For pasting data, we fetch the buffer from the appropriate address depending on which button was pressed and prepend a "PASTE" text (so the server can decipher this is a PASTE action instead of COPY). The server will then take the buffer, replace the current clipboard content, and physically activate the Ctrl+V keys to actually paste the content wherever the cursor is currently active.

# Clearing Flash Memory Manually
As of now, there is no button in place to clear the flash memory (if one so desires). If you wish to manually dump/clear the flash memory, you must use a programmer (personally I use STM32CubeProgrammer).