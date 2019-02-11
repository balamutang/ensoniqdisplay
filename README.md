# ensoniqdisplay
ensoniq asr10/eps16 display hardware sniffer with display emulator

ensoniq display hardware sniffer based on arduino uno/nano Atmega328 16mhz/5v and typical 240x64 lcd module

General idea is listen serial data (7n1  62500 bit/s) between ensoniq mainboard and keyboard plate (on pin 2 or 3? cant remember, update this later)

Ensoniq send big data blocks (more than 64bytes) so arduino need patch to enlarge receive buffer (in C:\Arduino\hardware\arduino\avr\cores\arduino\HardwareSerial.h add first definition #define SERIAL_RX_BUFFER_SIZE 128 ), without this display data will be broken

all other things about connection display to arduino you can found at many internet guides
