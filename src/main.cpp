/*****************************************************************************
 * IR Analyzer version 1.0                                      (2025-06-10) *
 * ------------------------------------------------------------------------- *
 * Copyright (c) Michal A. Valasek - Altairis, 2025                          *
 *               Licensed under terms of the MIT License                     *
 * ------------------------------------------------------------------------- *
 * www.rider.cz | www.altair.blog | github.com/ridercz/IRAnalyzer            *
 *****************************************************************************/

#include <Arduino.h>
#include <IRremote.hpp>

#define IR_RECEIVE_PIN 2        // Pin for IR receiver
#define NAME_PLACEHOLDER "NAME" // Placeholder for the function name, will be manually replaced with actual function names
#define PRINT_SEND_CODE         // Comment to not print sample code for sending the IR signal

uint8_t lastDevice = 0;
uint8_t lastSubdevice = 0;
uint8_t lastFunction = 0;
String lastProtocol = "";

void setup()
{
  // Initialize serial communication
  Serial.begin(115200);

#ifdef LED_BUILTIN
  // Initialize the IR receiver with LED feedback
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK, LED_BUILTIN);
  pinMode(LED_BUILTIN, OUTPUT);
#else
  // Initialize the IR receiver without LED feedback
  IrReceiver.begin(IR_RECEIVE_PIN);
#endif

  // Print a CSV header for the output
  Serial.println();
  Serial.println();
  Serial.println();
  Serial.print("functionname,protocol,device,subdevice,function");
#ifdef PRINT_SEND_CODE
  Serial.print(",usage");
#endif
  Serial.println();
}

void loop()
{
  // Check if IR data is available
  if (!IrReceiver.decode())
    return;

  // Check for various non-standard conditions
  if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_WAS_OVERFLOW)
  {
    Serial.println("# Overflow, try to increase the RAW_BUFFER_LENGTH value.");
  }
  else if (IrReceiver.decodedIRData.protocol == UNKNOWN)
  {
    Serial.println("# Unknown protocol, check the IR signal.");
  }
  else if (IrReceiver.decodedIRData.protocol == PULSE_WIDTH)
  {
    Serial.println("# Supposedly pulse width protocol, check the IR signal.");
  }
  else if (IrReceiver.decodedIRData.protocol == PULSE_DISTANCE)
  {
    Serial.println("# Supposedly pulse distance protocol, check the IR signal.");
  }
  else
  {
    // Parse the IR data
    String protocolStr = String(IrReceiver.getProtocolString());
    uint8_t device = IrReceiver.decodedIRData.address & 0xFF;
    uint8_t subdevice = (IrReceiver.decodedIRData.address >> 8) & 0xFF;
    uint8_t function = IrReceiver.decodedIRData.command;

    // Print the IR data in a CSV format, only if changed
    if (protocolStr != lastProtocol || device != lastDevice || subdevice != lastSubdevice || function != lastFunction)
    {
      Serial.print(NAME_PLACEHOLDER ",");
      Serial.print(protocolStr);
      Serial.print(",");
      Serial.print(device);
      Serial.print(",");
      Serial.print(subdevice);
      Serial.print(",");
      Serial.print(function);
#ifdef PRINT_SEND_CODE
      Serial.print(",");
      IrReceiver.printIRSendUsage(&Serial);
#else
      Serial.println();
#endif

      // Store the current values for comparison in the next loop
      lastProtocol = protocolStr;
      lastDevice = device;
      lastSubdevice = subdevice;
      lastFunction = function;
    }
  }

  // Prepare for the next IR frame
  IrReceiver.resume();
}
