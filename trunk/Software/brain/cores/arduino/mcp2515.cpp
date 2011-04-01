#include "mcp2515.h"
#include "SPI.h"

/* Implicitly required emtpy constructor */
Mcp2515::Mcp2515() {};

/* Initalizes CS pin and SPI */
Mcp2515::Mcp2515(int CsPin) {
  _CsPin = CsPin;
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  pinMode(_CsPin, OUTPUT);
  digitalWrite(_CsPin, HIGH);
}

/* Shorthand for asserting Chip Select */
void Mcp2515::SpiStart() {
  digitalWrite(_CsPin, HIGH);
  digitalWrite(_CsPin, LOW);
}

/* Shorthand for deserting Chip Select */
void Mcp2515::SpiEnd() {
  digitalWrite(_CsPin, HIGH);
}

/* Sends a reset command */
char Mcp2515::reset()
{
  SpiStart();
  const char response = SPI.transfer(RESET);
  SpiEnd();
  return response;
}

/* Reads a value from a register in the MCP2515 */
char Mcp2515::read(const char addr)
{
  SpiStart();
  SPI.transfer(READ);  // READ
  SPI.transfer(addr);  // Address to read from
  const char response = SPI.transfer(0xFF);
  SpiEnd();
  return response;
}

/* Writes a value into a register in the MCP2515 */
void Mcp2515::write(const char addr, const char value)
{
  SpiStart();
  SPI.transfer(WRITE); //WRITE
  SPI.transfer(addr);
  SPI.transfer(value);
  SpiEnd();
}

/* Modify a byte in a register */
void Mcp2515::modify(char addr, char mask, char byte) {
  SpiStart();
  SPI.transfer(MODIFY);
  SPI.transfer(addr);
  SPI.transfer(mask);
  SPI.transfer(byte);
  SpiEnd();
}

/* Transmit CAN data. Returns 0 on okay, non-zero on error*/
int Mcp2515::send(int length, int can_id, char * data)
{
  // Find an empty send buffer
  unsigned char status = readStatus();
  // Choose the correct buffer
  char command;
  char rts;
  if (!(status & 0x04)) {  // Buffer 0 is not pending transfer
    command = LOAD_TX_BUFFER0;
    rts = RTS_BASE | 0x01;
  }
  else if (!(status & 0x08)) { // Buffer 1 is not pending transfer
    command = LOAD_TX_BUFFER1;
    rts = RTS_BASE | 0x02;
  }
  else if (!(status & 0x10)) { // Buffer 2 is not pending transfer
    command = LOAD_TX_BUFFER2;
    rts = RTS_BASE | 0x04;
  }
  else  // All buffers full, return error
    return 1;
  length = constrain(length, 0, 8);
  const char SIDH = can_id >> 3;   // CAN ID High bits
  const char SIDL = (can_id & 0x07) << 5;  // CAN ID Low bits
  const char EID8 = 0x00;          // Extended bits not used
  const char EID0 = 0x00;          // Extended bits not used
  const char DLC  = 0x0F & length; // Data Frame & set length
  int i;
  // Start transfer, control bytes then data bytes
  SpiStart();
  SPI.transfer(command);
  SPI.transfer(SIDH);
  SPI.transfer(SIDL);
  SPI.transfer(EID8);
  SPI.transfer(EID0);
  SPI.transfer(DLC);   
  for(i = 0; i < length; i++)
    SPI.transfer(*(data+i));
  SpiEnd();
  
  // Initialize transmission
  SpiStart();
  SPI.transfer(rts);
  SpiEnd();
  
  return 0;
}

/* Receives CAN data from a channel, reads anywyas even if there are no messages ready */
/* Note: expects channel = {0, 1} */
int Mcp2515::receive(int channel, int * id, char * msg)
{
  *id = 0;
  // Choose correct channel to read from
  char cmd = (channel) ? READ_RX_BUFFER1 : READ_RX_BUFFER0;
  char sid_h, sid_l;  // Can ID, high and low
  char dlc;           // Data length code
  SpiStart();
  SPI.transfer(cmd);  // Send command to read the entire RX buffer
  // Highs and lows of Standard ID
  sid_h = SPI.transfer(0xAA);
  sid_l = SPI.transfer(0xAA);
  // We don't care about extended ID
  SPI.transfer(0xAA);
  SPI.transfer(0xAA);
  // Data length code, note dlc[3:0] represent message length
  dlc = SPI.transfer(0xAA);
  for (int i=0; i < (dlc & 0x0F); i++)
    *(msg+i) = SPI.transfer(0xAA);
  SpiEnd();
  unsigned int temp_id = ((sid_h << 3) & 0x7F8) | ((sid_l >> 5) & 0x07);
  *id = temp_id;
  //Clear buffer
  modify(CANINTF, (channel) ? 0x02 : 0x01, 0x00);
  return (int) dlc & 0x0F;
}

char Mcp2515::readStatus() {
  SpiStart();
  SPI.transfer(READ_STATUS);
  const char r = SPI.transfer(0xFF);
  SpiEnd();
  return r;
}

char Mcp2515::rxStatus() {
  SpiStart();
  SPI.transfer(RX_STATUS);
  const char r = SPI.transfer(0xFF);
  SpiEnd();
  return r;
}