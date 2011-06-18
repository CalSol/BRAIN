/*
  Mcp2515.h - Low level library for interfacing with the mcp2515
  Created by Ryan Tseng, Oct. 5th 2010.
*/

#ifndef Mcp2515_h
#define Mcp2515_h

// Control Register Definitions
#define BFPCTRL 0x0C    // RXnBF pin control and status (Not used)
#define TXRTSCTRL 0x0D  // 
#define CANSTAT 0x0E    // CAN status register
#define CANCTRL 0x0F
#define TEC 0x1C        // Transmit error count
#define REC 0x1D        // Receieve error count
#define CNF3 0x28       // Configuration (for bit timing)
#define CNF2 0x29       // Configuration (for bit timing)
#define CNF1 0x2A       // Configuration (for bit timing)
#define CANINTE 0x2B    // Interrupt enable
#define CANINTF 0x2C    // Interrupt flags
#define EFLG 0x2D       // Error flags
#define TXB0CTRL 0x30   // Transmit buffer 0 control
#define TXB1CTRL 0x40   // Transmit buffer 1 control
#define TXB2CTRL 0x50   // Transmit buffer 2 control
#define RXB0CTRL 0x60   // Receive buffer 0 control
#define RXB1CTRL 0x70   // Receive buffer 1 control

// Receieve buffer
#define RXB0SIDH 0x61   // Receive buffer 0 standard ID high
#define RXB0SIDL 0x62   // Receive buffer 0 standard ID low
#define RXB0EIDH 0x63   // Receive buffer 0 extended ID high
#define RXB0EIDL 0x64   // Receive buffer 0 extended ID low
#define RXB0DLC  0x65   // Receive buffer 0 data length code
#define RXB0D0   0x66   // Receive buffer 0 data byte 0
#define RXB0D1   0x67   // Receive buffer 0 data byte 1
#define RXB0D2   0x68   // Receive buffer 0 data byte 2
#define RXB0D3   0x69   // Receive buffer 0 data byte 3
#define RXB0D4   0x6A   // Receive buffer 0 data byte 4
#define RXB0D5   0x6B   // Receive buffer 0 data byte 5
#define RXB0D6   0x6C   // Receive buffer 0 data byte 6
#define RXB0D7   0x6D   // Receive buffer 0 data byte 7
#define RXB1SIDH 0x71   // Receive buffer 1 stardard ID high
#define RXB1SIDL 0x72   // Receive buffer 1 stardard ID low
#define RXB1EIDH 0x73   // Receive buffer 1 extended ID high
#define RXB1EIDL 0x75   // Receive buffer 1 extended ID low
#define RXB1DLC  0x75   // Receive buffer 1 data length code
#define RXB1D0   0x76   // Receive buffer 1 data byte 0
#define RXB1D1   0x77   // Receive buffer 1 data byte 1
#define RXB1D2   0x78   // Receive buffer 1 data byte 2
#define RXB1D3   0x79   // Receive buffer 1 data byte 3
#define RXB1D4   0x7A   // Receive buffer 1 data byte 4
#define RXB1D5   0x7B   // Receive buffer 1 data byte 5
#define RXB1D6   0x7C   // Receive buffer 1 data byte 6
#define RXB1D7   0x7D   // Receive buffer 1 data byte 7

// Filters
// Note: Filters 0 and 1 are associated with buffer 0
// Filters 2,3,4,5 are associated with buffer 1
#define RXF0SIDH 0x00   // Filter 0 standard ID high
#define RXF0SIDL 0x01   // Filter 0 standard ID low
#define RXF1SIDH 0x04   // Filter 1 standard ID high
#define RXF1SIDL 0x05   // Filter 1 standard ID low
#define RXF2SIDH 0x08   // Filter 2 standard ID high
#define RXF2SIDL 0x09   // Filter 2 standard ID low
#define RXF3SIDH 0x10   // Filter 3 standard ID high
#define RXF3SIDL 0x11   // Filter 3 standard ID low
#define RXF4SIDH 0x14   // Filter 4 standard ID high
#define RXF4SIDL 0x15   // Filter 4 standard ID low
#define RXF5SIDH 0x18   // Filter 5 standard ID high
#define RXF5SIDL 0x19   // Filter 5 standard ID low

#define RXF0EIDH 0x02   // Filter 0 extended ID high
#define RXF0EIDL 0x03   // Filter 0 extended ID low
#define RXF1EIDH 0x06   // Filter 1 extended ID high
#define RXF1EIDL 0x07   // Filter 1 extended ID low
#define RXF2EIDH 0x0A   // Filter 2 extended ID high
#define RXF2EIDL 0x0B   // Filter 2 extended ID low
#define RXF3EIDH 0x12   // Filter 3 extended ID high
#define RXF3EIDL 0x13   // Filter 3 extended ID low
#define RXF4EIDH 0x16   // Filter 4 extended ID high
#define RXF4EIDL 0x17   // Filter 4 extended ID low
#define RXF5EIDH 0x1A   // Filter 5 extended ID high
#define RXF5EIDL 0x1B   // Filter 5 extended ID low

// Filters - Masks
#define RXM0SIDH 0x20   // Mask 0 standard ID high
#define RXM0SIDL 0x21   // Mask 0 standard ID low
#define RXM1SIDH 0x24   // Mask 1 standard ID high
#define RXM1SIDL 0x25   // Mask 1 standard ID low

#define RXM0EIDH 0x22   // Mask 0 extended ID high
#define RXM0EIDL 0x23   // Mask 0 extended ID low
#define RXM1EIDH 0x26   // Mask 1 extended ID high
#define RXM1EIDL 0x27   // Mask 1 extended ID low

// Commands
#define RESET     0xC0            // Sends a reset command
#define READ      0x03            // Reads data from mcp2515 register
#define READ_RX_BUFFER0 0x90      // Reads RX buffer 0, starting from ID
#define READ_RX_BUFFER0_MSG 0x92  // Reads RX buffer 0, starting from msg
#define READ_RX_BUFFER1 0x94      // Reads RX buffer 1, starting from ID
#define READ_RX_BUFFER1_MSG 0x96  // Reads RX buffer 1, starting from msg
#define WRITE 0x02                // Write data to mcp2515 register
#define MODIFY 0x05               // Modifies a single register byte
#define RTS_BASE 0x80             // Base address of RTS commands
#define RTS_TX_BUFFER0  0x81      // Request to send data in TX buffer 0
#define RTS_TX_BUFFER1  0x82      // Request to send data in TX buffer 1
#define RTS_TX_BUFFER2  0x84      // Request to send data in TX buffer 2
#define LOAD_TX_BUFFER0 0x40      // Write to TX buffer 0, starting from ID
#define LOAD_TX_BUFFER0_MSG 0x41  // Write to TX buffer 0, starting from msg
#define LOAD_TX_BUFFER1 0x42      // Write to TX buffer 1, starting from ID
#define LOAD_TX_BUFFER1_MSG 0x43  // Write to TX buffer 1, starting from msg
#define LOAD_TX_BUFFER2 0x44      // Write to TX buffer 2, starting from ID
#define LOAD_TX_BUFFER3_MSG 0x45  // Write to TX buffer 3, starting from msg
#define READ_STATUS   0xA0        // See read status comment
#define RX_STATUS     0xB0        // See RX status comment

/* Read status:
  Bit 0: RX buffer 0 full (interrupt flag)
  Bit 1: RX buffer 1 full (interrupt flag)
  Bit 2: TX buffer 0 pending transmission
  Bit 3: TX buffer 0 empty (interrupt flag)
  Bit 4: TX buffer 1 pending transmission
  Bit 5: TX buffer 1 empty (interrupt flag)
  Bit 6: TX buffer 2 pending transmission
  Bit 7: TX buffer 2 empty (interrupt flag)
*/
/* RX status:
  Bit [2:0] Filter match (0 through 5)
  Bit [4:3] Message type
              0: Standard data frame
              1: Standard remote frame
              2: Extended data frame
              3: Extended remote frame
  Bit 6: Message in RX buffer 0
  Bit 7: Message in RX buffer 1
*/  

class Mcp2515
{
  public:
    Mcp2515();
    Mcp2515(int CsPin);
    char reset();
    char read(char addr);
    void write(char addr, char value);
    void modify(char addr, char mask, char data);
    int send(int length, int can_id, char * data);
    int receive(int channel, int * id, char * msg);
    char readStatus();
    char rxStatus();
  private:
    void SpiStart();
    void SpiEnd();
    int _CsPin;
};

#endif