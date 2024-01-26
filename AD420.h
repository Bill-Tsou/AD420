#ifndef AD420_H
#define AD420_H

#include <SPI.h>

#define ERROR_CODE  (unsigned short)(-1)
#define SUCCESS_CODE 1

enum OutputMode {
  OUTPUT_MODE_CURRENT_4_20 = 1,
  OUTPUT_MODE_CURRENT_0_20,
  OUTPUT_MODE_CURRENT_0_24,
  OUTPUT_MODE_VOLTAGE_0_5,
  MAX_OUTPUT_MODE_OPTIONS
};

class AD420
{
  public:
    inline AD420() { InitializeAD420(2, 6, 7, 8, 9); }
    inline AD420(byte param1, byte param2, byte param3, byte param4, byte param5)
      { InitializeAD420(param1, param2, param3, param4, param5); }

    void SetOutputValue(uint16_t);
    unsigned short SetOutputMode(byte);
    byte GetOutputMode();

    void ResetAD420();
    unsigned short TriangularOutput(float);

    bool GetAD420Status();
    char* GetOutputUnit();
    double GetRealOutputValue();
  
  private:
    void InitializeAD420(byte, byte, byte, byte, byte);

    // variables for the PIN configurations
    byte PIN_FAULT_DETECTION;     // connect to the first interrupt pin of UNO (only for 2 and 3)
    byte PIN_RANGE_SELECT1;
    byte PIN_RANGE_SELECT2;
    byte PIN_TERMINATION_BUTTON;  // active high
    byte PIN_CLEAR;

    byte AD420Mode;
    byte MaxOutputValue;
    byte MinOutputValue;
    uint16_t OutputValue;
};

#endif // AD420_H
