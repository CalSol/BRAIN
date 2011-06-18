/*
  Can.h - Library for interfacing with the MCP2515.
  Created by Ryan Tseng, Oct. 5th 2010.
*/
#ifndef Can_h
#define Can_h

#include "WProgram.h"
#include "mcp2515.h"

#define CAN_BUFFER_SIZE 30

class CanMessage {
  public:
    CanMessage();
    CanMessage(boolean valid);
    CanMessage(int _id, const char * _data, char _len = 8);
    int id;
    char data[8];
    char len;
};

class HardwareCan
{
  public:
    HardwareCan(int CsPin, int IntPin);
    void begin(int hz, bool do_reset = true);
    int frequency(int khz);
    int available();
    boolean interrupted();
    int send(CanMessage msg);
    int recv(int channel, CanMessage &msg);
    int setFilter(int channel, int filter, int id);
    int setMask(int channel, int mask);
    void filterOn();
    void filterOff();
    void reset();
    void config(boolean enable);
    void monitor(boolean silent);
    void attach(void (*func)(CanMessage &msg));
    void detach();
    unsigned int rxError();
    unsigned int txError();
    void (*_func)(CanMessage &msg);
  private:
    int _CsPin;
    int _IntPin;
    int _Freq;
    Mcp2515 _mcp2515;
};


void CanReadHandler();
extern void CanBufferInit();
extern CanMessage CanBufferRead();
extern int CanBufferSize();
extern HardwareCan Can;

#endif