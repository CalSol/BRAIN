This repository formerly lived at https://code.google.com/p/calsolbrain/. It was
moved to github after news of Google Code's closure. The below information was
last updated ~2012. There is a newer version of the BRAIN (the v3) in the works,
which includes usage of the LPC11c14 and the LPC1549. Currently, the production
quantities have been too low for full verification, but the results are promising.

# Outdated

After a good amount of time making our own circuit boards, the UC Berkeley Solar
Vehicle Team (CalSol) realized that many of the components used on the circuit
boards don't change much (Microcontroller, crystals, powerline capacitors, etc),
and that Arduinos are very easy to use. So we decided to combine these two ideas
together.

The CalSol BRAIN platform/module has the following features:

- 20 Mhz clock
- Fully programmable over USB (once flashed via ICSP Programmer)
- 25 Digital input/output Pins (8 of which can be used as analog input pins)
- 2 independent Serial (UART) channels
- Integrated Controller Area Network (CAN) chip, compatible with all standard 
  CAN buses
- 5V and 3.3V voltage regulator
- SPI Interface
- and more!

Although we are not selling these units, we believe that the designs will help
other solar car teams avoid reinventing the wheel and have to deal with the
numerous headaches we had.

We are releasing the schematics for the Brain, along with support files and
drivers to use in the Arduino environment.

Pinouts (Note: this is *correct* for v2.0):

![Pinouts](https://raw.githubusercontent.com/CalSol/BRAIN/master/Pictures/Brain2Pins.jpg)

Note that digital pin 15 connects to the LED on the BRAIN v1.6
[pinouts](https://github.com/CalSol/BRAIN/blob/master/Pictures/Brain-footprint.gif)

Example usage:

![DMI2](https://raw.githubusercontent.com/CalSol/BRAIN/master/Pictures/DMI2.jpg)
