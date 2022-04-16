# Clipboard Manager Server
The server application to listen to inputs from the microcontroller.

When a "copy" button is pressed, the MCU will send a message to copy the clipboard contents and write it into FLASH memory.

When a "paste" button is presesd, the MCU will read the appropriate contents from FLASH memory and send a message to server to replace the current clipboard content.

# Usage
```text
cbm [PORT] # PORT = port the device is connected to. On Windows, it will be something like COM3

# example
cbm COM3 # connect to COM3 port to listen to requests from MCU
```