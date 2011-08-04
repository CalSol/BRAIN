/*
  HardwareCan.c - Library for interfacing with the MCP2515.
  Created by Ryan Tseng, Oct. 5th 2010.
*/
#include "WProgram.h"
#include "HardwareCan.h"
#include "mcp2515.h"
#include <avr/io.h>
#include <avr/interrupt.h>

CanMessage::CanMessage() {};

CanMessage::CanMessage(boolean valid) {
  if (!valid)
    len = -1;
  else
    CanMessage();
}

CanMessage::CanMessage(int _id, const char * _data, char _len) {
  id = _id;
  for (int i=0; i < _len; i++)
    data[i] = *(_data+i);
  len = _len;
}

HardwareCan::HardwareCan(int CsPin, int IntPin) {
  _CsPin = CsPin;
  _IntPin = IntPin;
  _mcp2515 = Mcp2515(CsPin);
}

// Set the MCP2515 to start listening
void HardwareCan::begin(int Freq, bool do_reset) {
  if (do_reset)
    reset();
  frequency(Freq);
  // Enable interrupt on the int pin when either RX buffer are filled
  _mcp2515.write(CANINTE, 0x03);
  // Start listening in normal mode
  monitor(0);
}

/* Set CAN operating frequency, valid modes are:
125, 250, 500, 1000 */
int HardwareCan::frequency(int hz) {
  _Freq = hz;
  // CNF1, CNF2, CNF3
  char config[3];
  // Obtained from the MCP2515 timing calculator with 20Mhz crystal
  switch (hz) {
    case 10:    config[0] = 0x27;
                config[1] = 0xBF;
                config[2] = 0x07;
                break;
    case 20:    config[0] = 0x13;
                config[1] = 0xBF;
                config[2] = 0x07;
                break;
    case 50:    config[0] = 0x07;
                config[1] = 0xBF;
                config[2] = 0x07;
                break;
    case 125:   config[0] = 0x03;
                config[1] = 0xBA;
                config[2] = 0x07;
                break;
    case 250:   config[0] = 0x01;
                config[1] = 0xBA;
                config[2] = 0x07;
                break;
    case 500:   config[0] = 0x00;
                config[1] = 0xB6;
                config[2] = 0x04;
                break;
    case 1000:  config[0] = 0x00;
                config[1] = 0xA0;
                config[2] = 0x02;
                break;
    default:    return 1;
  }
  _mcp2515.write(CNF1, config[0]);
  _mcp2515.write(CNF2, config[1]);
  _mcp2515.write(CNF3, config[2]);
  return 0;
}

/* Returns:
  0 if both channels are unavailable
  1 if channel 1 is available
  2 if channel 2 is available
  3 if both channels are available
*/
int HardwareCan::available() {
  // status() returns 0b1000000, 0b01000000, or 0b11000000
  // depending on the status of either of the channel
  return (_mcp2515.rxStatus() >> 6) & 0x03;
}

// Returns 1 if there is a pending interrupt, 0 otherwise
boolean HardwareCan::interrupted() {
  return !digitalRead(_IntPin);
}

/* Sends can message. 0 on success, 1 on error */
int HardwareCan::send(CanMessage msg) {
  PCICR &=~ 0x02;  // Disable PC1 Interrupt
  _mcp2515.send(msg.len, msg.id, msg.data);
  PCICR |= 0x02;   // Re-enable PC1 interrupt
  return 0;
}

/* Receives can message from channel. 0 on success, error otherwise */
int HardwareCan::recv(int channel, CanMessage &msg) {
  // Invalid channel
  if (channel != 1 && channel != 2 && channel != 3)
    return 1;
  // Note if channel == 3 (most likely the return value of available())
  // Then we read channel 1
  if (channel == 3)
    channel = 1;
  // Note: receive() expects channel = 0 or 1 instead of 1 or 2
  msg.len = _mcp2515.receive(channel-1, &msg.id, msg.data);
  return 0;
}

/* Set channel ID filter */
/* Can set:
channel 1 filter 1,2
channel 2 filter 1,2,3,4
*/
int HardwareCan::setFilter(int channel, int filter, int id) {
  // Invalid channel/filter
  if ( (channel == 1 && (filter < 1 || filter > 2)) ||
       (channel == 2 && (filter < 1 || filter > 4)) ||
        channel < 1 || channel > 2)
    return 1;
  char reg_sidh;
  if (channel == 1)
    if (filter == 1)
      reg_sidh = RXF0SIDH;
    else // Filter == 2
      reg_sidh = RXF1SIDH;
  else // channel == 2
    if (filter == 1)
      reg_sidh = RXF2SIDH;
    else if (filter == 2)
      reg_sidh = RXF3SIDH;
    else if (filter == 3)
      reg_sidh = RXF4SIDH;
    else // filter == 4
      reg_sidh = RXF5SIDH;
  const char reg_sidl = reg_sidh + 1;
  // 8 MSB of filter ID, left justified
  _mcp2515.write(reg_sidh, ((id >> 3) & 0xFF));
  // 3 LSB of filter ID
  _mcp2515.write(reg_sidl, ((id << 5) & 0xFF));
  return 0;  // Success
}

// Set acceptance mask for channel
// An acceptance mask of 0x000 will not filter anything
int HardwareCan::setMask(int channel, int id) {
  // Invalid channel
  if (channel < 1 || channel > 2)
    return 1;
  char reg_sidh;
  if (channel == 1)
    reg_sidh = RXM0SIDH;
  else // channel == 2
    reg_sidh = RXM1SIDH;
  const char reg_sidl = reg_sidh + 1;
  _mcp2515.write(reg_sidh, ((id >> 3) & 0xFF));
  _mcp2515.write(reg_sidl, ((id << 5) & 0xFF));
  return 0;
}

// Turn on hardware filtering
void HardwareCan::filterOn() {
  _mcp2515.write(RXB0CTRL, 0x04);
  _mcp2515.write(RXB1CTRL, 0x00);
}

// Turn off hardware filtering, receives all messages
void HardwareCan::filterOff() {
  _mcp2515.write(RXB0CTRL, 0x64);
  _mcp2515.write(RXB1CTRL, 0x60);
}

/* Sends a reset to Mcp2515 */
void HardwareCan::reset() {
  _mcp2515.reset();
  delay(10);
}

/* Turns on and off configuration mode */
void HardwareCan::config(boolean enable) {
  if (enable)
    _mcp2515.write(CANCTRL, 0x80);
  else
    monitor(0);
}

/* Turns on and off silent mode */
void HardwareCan::monitor(boolean silent) {
  if (silent)
    _mcp2515.write(CANCTRL, 0x60);  // Listen only mode
  else // !silent
    _mcp2515.write(CANCTRL, 0x00);  // Normal mode
}

/* Attaches a callback to a packet receive event */
void HardwareCan::attach(void (*func)(CanMessage &msg)) {
  _func = func;
}

/* Detaches the packet receive callback */
void HardwareCan::detach() {
  _func = 0;
}

/* Returns number of RX errors */
unsigned int HardwareCan::rxError() {
  // Read Receieve error count register
  PCICR &=~ 0x02;
  unsigned int result = 0xFF & _mcp2515.read(REC);
  PCICR |= 0x02;   // Re-enable PC1 interrupt
  return result;
}

/* Returns number of TX errors */
unsigned int HardwareCan::txError() {
  // Read Transmit error count register
  PCICR &=~ 0x02;
  unsigned int result = 0xFF & _mcp2515.read(TEC);
  PCICR |= 0x02;   // Re-enable PC1 interrupt
  return result;
}

// Init an instance for the CalSol Brain
HardwareCan Can = HardwareCan(4, 3);

CanMessage _can_buffer[CAN_BUFFER_SIZE];
// Needs to be declaired volatile since it can be changed in an ISR
volatile uint8_t _can_buffer_start = 0;
volatile uint8_t _can_buffer_end = 0;
volatile uint8_t _can_buffer_size = 0;
// Brain specific stuff
// This Interrupt Service Routine triggers whenever any pins on port B changes
ISR(PCINT1_vect) {
  // We only care about pin 3 (PB3), so we only call our handler if pin 3 is low
  if (digitalRead(3) == 0)
    CanReadHandler();
}
/* this has to be called to set up interrupts correctly */
void CanBufferInit() {
  PCMSK1 |= 0x08;  // PC Interrupt #11 (Thats the CAN INT pin) enable
  PCICR |= 0x02; // PC Interrupt 1 enable
  DDRC |= (1<<5);
  CanReadHandler();
}
/* Reads a single CanMessage out of the buffer, returns an invalid CanMessage
    if there are no messages in the buffer.
    An invalid message has its length set to -1 */
CanMessage CanBufferRead() {
  if (_can_buffer_size) {
    const CanMessage result = _can_buffer[_can_buffer_start];
    // Modulus
    _can_buffer_start = (_can_buffer_start == CAN_BUFFER_SIZE-1) ? 0 : _can_buffer_start+1;
    _can_buffer_size--;
    return result;
  } else {
    return CanMessage(0);  // Invalid packet
  }
}
/* Called by Pin change ISR if CANINT has a falling edge.  That means the
    mcp2515 has a message ready to be read */
void CanReadHandler() {
  // While we still have packets
  while (1) {
    int available = Can.available();
    if (!available)
      return;
    if (Can._func) {
      CanMessage packet;
      Can.recv(available, packet);
      Can._func(packet);
      continue;
    }
    if (_can_buffer_size && _can_buffer_start == _can_buffer_end) {
      CanMessage dummy;
      Can.recv(available, dummy);
      continue;
    }
    Can.recv(available, _can_buffer[_can_buffer_end]);
    // Increment
    _can_buffer_end++;
    if (_can_buffer_end == CAN_BUFFER_SIZE) _can_buffer_end = 0;
    _can_buffer_size++;
  }
}
int CanBufferSize() {
  return _can_buffer_size;
}