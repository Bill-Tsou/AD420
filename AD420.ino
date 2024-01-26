/*  Functions related to AD420: controll the analog current signal flowing through the pin Iout
    Pin configurations related to Arduino:
      Pin 8 use a button to raise up an interrupt event to stop current task
      Pin 9 is connected to CLEAR of AD420 in order to reset the chip
      Pin 6 is connected to RANGE_SELECT_1 of AD420 and Pin 7 is connected to RANGE_SELECT_2
        in order the select the output current range (Iout) or switch the output to voltage (Vout)
    Author: Bill Tsou, feel free to use and share the program
*/

#include "AD420.h"

#define UART_TERMINATOR '\r'

uint8_t UART_Processing(char *returnString)
{
  char c;
  byte bufferLength = 0;
  do
  {
    c = Serial.read();
    if(c > 0)
    { // the character read from serial buffer is available
      returnString[bufferLength++] = c;
      if(bufferLength >= 255)
        return ERROR_CODE;
    }
  }while(c != UART_TERMINATOR);
  returnString[bufferLength - 1] = '\0';
}

AD420 ad420;  // use the default PIN configurations

void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(115200);
  Serial.println("The demonstration programme for AD420, Date: 2020/1/31, Author: Bill Tsou");
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial.available())
  {
    char uartBuffer[256];
    if(UART_Processing(uartBuffer) == ERROR_CODE)
      Serial.println("The string length is too long!");
    else
    { // processing with retrieving messages
      /*Serial.print("Received message length: ");
      Serial.println(strlen(uartBuffer));
      for(int i = 0; i < strlen(uartBuffer); i++)
      {
        Serial.print((int)uartBuffer[i]);
        Serial.print(" ");
      }
      Serial.println();*/

      if(strcmp(uartBuffer, "*IDN?") == 0)
        Serial.println("AD420 16-bit Voltage/Current Signal Generator, to get the instructions type *HELP");
      else if(strcmp(uartBuffer, "*STEP UP") == 0)
      {
        Serial.println("The output value is stepping up...");
        for(uint32_t i = 0; (i < 65535) && ad420.GetAD420Status(); i++)
        {
          ad420.SetOutputValue(i);
          delayMicroseconds(10);
          if(i % 1000 == 0)
            Serial.println(i);
        }
        Serial.println("DONE!");
      }
      else if(strcmp(uartBuffer, "*STEP DOWN") == 0)
      {
        Serial.println("The output value is stepping down...");
        for(int32_t i = 65535; (i >= 0) && ad420.GetAD420Status(); i--)
        {
          ad420.SetOutputValue(i);
          delayMicroseconds(10);
          if(i % 1000 == 0)
            Serial.println(i);
        }
        Serial.println("DONE!");
      }
      else if(strcmp(uartBuffer, "*OUTPUT MODE") == 0)
      {
        Serial.println("");
        Serial.println("Select the output mode on the list:");
        Serial.println("  1. Current output from 4 to 20 mA");
        Serial.println("  2. Current output from 0 to 20 mA");
        Serial.println("  3. Current output from 0 to 24 mA");
        Serial.println("  4. Voltage output from 0 to 5 V");
        Serial.println();
        Serial.print("Enter the selection number: ");
        if(UART_Processing(uartBuffer) != ERROR_CODE)
        {
          byte userOption = (byte)atoi(uartBuffer);
          if(ad420.SetOutputMode(userOption) != ERROR_CODE)
          {
            Serial.print(userOption);
            Serial.println(". The new output mode has been set");
          }
          else
            Serial.println("Invalid output mode option!");
        }
        else
          Serial.println("Be aware of your input length");
      }
      else if(strcmp(uartBuffer, "*RST") == 0)
        ad420.ResetAD420();
      else if(strcmp(uartBuffer, "*BLINKING") == 0)
      {
        Serial.print("Enter the flashing period (unit in seconds, and can be floating number): ");
        if(UART_Processing(uartBuffer) != ERROR_CODE)
        {
          float tempNumber = atof(uartBuffer);
          if(tempNumber != 0.0f)
          {
            Serial.print(tempNumber, 3);
            Serial.println(" second(s)");
            Serial.print("Blinking starts, press the button to terminate the action...");
            if(ad420.TriangularOutput(tempNumber) == ERROR_CODE)
              Serial.println();
            else
              Serial.println("Task TERMINATED");
          }
          else
            Serial.println("Invalid period value!");
        }
        else
          Serial.println("Be aware of your input lenght");
      }
      else if(strcmp(uartBuffer, "*HELP") == 0)
      {
        Serial.println();
        Serial.println("The instructions available in the programme:");
        Serial.println("  *IDN? - The identification of the programme");
        Serial.println("  *RST - Reset AD420");
        Serial.println("  *STEP DOWN - Programme decreases the output value gradually");
        Serial.println("  *STEP UP - Programme increases the output value gradually");
        Serial.println("  *OUTPUT MODE - Choose the output mode of AD420 from 4 options");
        Serial.println("  *BLINKING - Specify the period of flashing cycle and the");
        Serial.println("              output value varies according to the rate");
        Serial.println("  [other number format] - set the output value directly");
        Serial.println();
        Serial.println("           -- Programme Designer: Bill Tsou --");
      }
      else
      {
        ad420.SetOutputValue((uint16_t)atoi(uartBuffer));
        Serial.print("Output value has been set to: ");
        Serial.print((uint16_t)atoi(uartBuffer));
        Serial.print(", and the real output value is: ");
        Serial.print(ad420.GetRealOutputValue(), 6);
        Serial.println(ad420.GetOutputUnit());
      }
    }
  }

  // check for fault detect pin
  if(ad420.GetAD420Status() == LOW)
  {
    Serial.println("Error occurred! Please check your current loop or chip supply voltage!");
    // block the programme if the chip is currently abnormal
    while(ad420.GetAD420Status() == LOW);
    Serial.println("Error discriminated!");
  }
}
