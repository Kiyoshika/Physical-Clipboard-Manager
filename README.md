# Physical Clipboard Manager
A simple embedded project to hold four (4) in-memory clipboards on a microcontroller so I can copy and paste multiple things.

# MCU Client
This contains the code for the microcontroller which acts as a "client" USB device that sends commands to the server application to make a request to copy/paste with the clipboard.

# Server
A simple "server" application that listens to commands from the MCU and interacts with the Windows API to manipulate the clipboard.