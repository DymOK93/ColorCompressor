# ColorCompressor
* Works in conjunction with [PhotoViewer](https://github.com/DymOK93/PhotoViewer): acts as a slave on a software-supported parallel port and a master on a hardware serial port (USART).
* Depending on the header, the data packets received through the parallel port are interpreted as commands to switch the state of the on-board LEDs or pixel colors in the BGR888 format for a second device. The color is compressed in RGB666 and sent to the second device for rendering on the display. 
* Pressing the button initiates sending a command to switch the picture to the next one from the SD card.

## Target
STM32F051R8-Discovery board
