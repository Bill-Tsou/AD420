#include "AD420.h"

void AD420::InitializeAD420(byte faultDetection, byte rangeSelect1, byte rangeSelect2, byte terminateButton, byte clear)
{
  PIN_FAULT_DETECTION = faultDetection;
  PIN_RANGE_SELECT1 = rangeSelect1;
  PIN_RANGE_SELECT2 = rangeSelect2;
  PIN_TERMINATION_BUTTON = terminateButton;
  PIN_CLEAR = clear;
  
  pinMode(SS, OUTPUT);
  pinMode(PIN_CLEAR, OUTPUT);
  pinMode(PIN_RANGE_SELECT1, OUTPUT);
  pinMode(PIN_RANGE_SELECT2, OUTPUT);

  pinMode(PIN_FAULT_DETECTION, INPUT_PULLUP);
  pinMode(PIN_TERMINATION_BUTTON, INPUT);
  
  digitalWrite(SS, HIGH);
  digitalWrite(PIN_CLEAR, LOW);

  // default range using 4-20mA current output
  SetOutputMode(OUTPUT_MODE_CURRENT_4_20);

  SPI.begin();
  SPI.beginTransaction(SPISettings(30000, MSBFIRST, SPI_MODE0));
}

void AD420::SetOutputValue(uint16_t data)
{
  OutputValue = data;
  digitalWrite(SS, LOW);
    SPI.transfer16(OutputValue);
  digitalWrite(SS, HIGH);
}

unsigned short AD420::SetOutputMode(byte outputMode)
{
  if((outputMode == 0) || (outputMode >= MAX_OUTPUT_MODE_OPTIONS))
    return ERROR_CODE;
  else
    AD420Mode = outputMode;

  switch(AD420Mode)
  {
  case OUTPUT_MODE_CURRENT_4_20:
      digitalWrite(PIN_RANGE_SELECT1, HIGH);
      digitalWrite(PIN_RANGE_SELECT2, LOW);
      MaxOutputValue = 20;
      MinOutputValue = 4;
      break;

  case OUTPUT_MODE_CURRENT_0_20:
      digitalWrite(PIN_RANGE_SELECT1, LOW);
      digitalWrite(PIN_RANGE_SELECT2, HIGH);
      MaxOutputValue = 20;
      MinOutputValue = 0;
      break;

  case OUTPUT_MODE_CURRENT_0_24:
      digitalWrite(PIN_RANGE_SELECT1, HIGH);
      digitalWrite(PIN_RANGE_SELECT2, HIGH);
      MaxOutputValue = 24;
      MinOutputValue = 0;
      break;

  case OUTPUT_MODE_VOLTAGE_0_5:
      digitalWrite(PIN_RANGE_SELECT1, LOW);
      digitalWrite(PIN_RANGE_SELECT2, LOW);
      MaxOutputValue = 5;
      MinOutputValue = 0;
      break;
  }
  return SUCCESS_CODE;
}

void AD420::ResetAD420()
{
  digitalWrite(PIN_CLEAR, HIGH);
  delay(10);
  digitalWrite(PIN_CLEAR, LOW);
}

unsigned short AD420::TriangularOutput(float period)
{ // keep on tracking that the button is at low (unpressed) status
  double i = 0;
  double steppingValue = 65535.0f / (period / 2.0f * 1000.0f);
  do
  {
    for(; ((int32_t)i <= 65535) && (digitalRead(PIN_TERMINATION_BUTTON) == LOW) && GetAD420Status(); i += steppingValue)
    {
      SetOutputValue((uint16_t)i);
      delay(1);
    }
    if((int32_t)i > 65535)
      i = 65535.0f;
    for(; ((int32_t)i >= 0) && (digitalRead(PIN_TERMINATION_BUTTON) == LOW) && GetAD420Status(); i -= steppingValue)
    {
      SetOutputValue((uint16_t)i);
      delay(1);
    }
    if((int32_t)i < 0)
      i = 0.0f;

    // check whether any error occurs
    if(GetAD420Status() == LOW)
      return ERROR_CODE;
  }while(digitalRead(PIN_TERMINATION_BUTTON) == LOW);
  return SUCCESS_CODE;
}

byte AD420::GetOutputMode()
{  return AD420Mode;  }

bool AD420::GetAD420Status()
{  return digitalRead(PIN_FAULT_DETECTION);  }

char* AD420::GetOutputUnit()
{  return ((GetOutputMode() == OUTPUT_MODE_VOLTAGE_0_5) ? "V" : "mA");  }

double AD420::GetRealOutputValue()
{  return ((double)OutputValue / 65535.0f * (double)(MaxOutputValue - MinOutputValue) + MinOutputValue);  }
