// Libraries for display
#include <U8g2lib.h>
#include <Wire.h>

// LUT for 4/8 coding used in Railcom
#include "48coding.h"

// Sprites for animation
#include "sprites.h"

// PCB wiring: CUTOUT is connected to Pro Micro D10.
#define CUTOUT_PIN  10
#define CUTOUT_ACTIVE_LEVEL HIGH

// Lost ID timeout
#define LOST_ID_TIME  1000

// Delay between animation steps
#define ANIMATION_DELAY 600

// Global variables
uint8_t longAddrMSB = 0;
uint8_t longAddrLSB = 0;
uint8_t lastDecoded = 0;
uint16_t lastAddress = 0;
unsigned long lastAddressSeen = 0;
bool haveFirst = false;
bool haveMSB = false;
bool haveLSB = false;
bool validAddress = false;
bool addressLostMessagePending = false;
bool displayAddressPending = false;
bool lastCutoutActive = false;
int animationStep = 0;
int animationDirection = 1;
unsigned long lastAnimationStep = 0;
int xPos = 0;

// Display
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

void setup() {

  // CUTOUT pin, input
  pinMode(CUTOUT_PIN, INPUT);
  lastCutoutActive = (digitalRead(CUTOUT_PIN) == CUTOUT_ACTIVE_LEVEL);
  
  Serial.begin(115200);
  Serial1.begin(250000); 
  u8g2.begin();

  Serial.println("--- MONITOR RAILCOM + CUTOUT D10 ---");
}


void loop() {

  // Realign RailCom symbol pairs at the start of each CUTOUT window. Keep
  // reading Serial1 continuously because bytes can arrive at a window edge
  // and may already be buffered when this pin is polled.
  bool cutoutActive = (digitalRead(CUTOUT_PIN) == CUTOUT_ACTIVE_LEVEL);
  if (cutoutActive && !lastCutoutActive) {
    haveFirst = false;
  }
  lastCutoutActive = cutoutActive;

  // check the time since last address seen
  if(validAddress && (millis() - lastAddressSeen > LOST_ID_TIME)) {

    validAddress = false;
    addressLostMessagePending = true;

    // Start the animation from its initial position.
    animationStep = 0;
    animationDirection = 1;
    haveFirst = false;
    haveMSB = false;
    haveLSB = false;
  }

  // Read the UART independently of CUTOUT_PIN. The UART can deliver symbols
  // at the edge of the cutout, and polling the cutout level can miss them.
  // Non-RailCom bytes are rejected by the 4/8 decoder below.
  while (Serial1.available()) {
      uint8_t raw = Serial1.read();

      // Use the LUT to decode the byte.
      uint8_t decoded = railcom_decode[raw];

      // Valid 4/8 symbols decode to six-bit values. Marker, reserved, and
      // invalid values break a possible two-symbol datagram.
      if (decoded > 0x3F) {
        haveFirst = false;
        continue;
      }

      // RailCom CH1 has two symbols per datagram.
      if (!haveFirst) {
        lastDecoded = decoded;
        haveFirst = true;
        continue;
      }

      // Second symbol, so the datagram can be decoded.
      uint8_t high = lastDecoded;
      uint8_t low  = decoded;
      haveFirst = false;

      // CH1 datagram is 12 bits.
      uint16_t datagram = ((uint16_t)high << 6) | low;
      //Serial.println(datagram, HEX);

      // Command occupies the first four bits; payload occupies the next eight.
      uint8_t command = (datagram >> 8) & 0x0F;
      uint8_t payload = datagram & 0xFF;

      // Instruction type MOB.
      switch (command) {

        // adr_high (only the first 6 bits).
        case 0x1: {
          longAddrMSB = payload & 0x3F;
          haveMSB = true;
          break;
        }

        // adr_low.
        case 0x2: {
          longAddrLSB = payload;
          haveLSB = true;
          break;
        }
      }

      // Reconstruct the address once both components have arrived.
      if (haveMSB && haveLSB) {
        uint16_t longAddress = ((uint16_t)longAddrMSB << 8) | longAddrLSB;
        bool addressWasValid = validAddress;
        validAddress = true;
        if (!addressWasValid || longAddress != lastAddress) {
          lastAddress = longAddress;
          displayAddressPending = true;
        }

        // Update last-seen time and clear the components for the next address.
        lastAddressSeen = millis();
        haveMSB = false;
        haveLSB = false;
      }
  }

  // Keep I2C display transfers outside the cutout so they cannot block UART
  // reception while RailCom symbols are arriving.
  if (!cutoutActive) {
    if (addressLostMessagePending) {
      Serial.println("Address lost");
      addressLostMessagePending = false;
    }
    if (displayAddressPending) {
      Serial.print("Loco ID: ");
      Serial.println(lastAddress);
      displayLocoID();
      displayAddressPending = false;
    } else if (!validAddress && (millis() - lastAnimationStep > ANIMATION_DELAY)) {
      displayAnimation();
      lastAnimationStep = millis();
    }
  }
}

void displayAnimation() {

  u8g2.clearBuffer();

  xPos = animationStep * 20;
  if(animationDirection == 1) u8g2.drawXBMP(xPos, 0, loco_width, loco_height, loco_bits);
  else u8g2.drawXBMP(xPos, 0, loco_width, loco_height, locorev_bits);

  u8g2.sendBuffer();

  animationStep += animationDirection;
  if(animationStep == 4) {
    animationDirection = -1;
    animationStep = 3;
  }
  else if(animationStep == -1) {
    animationDirection = 1;
    animationStep = 0;
  }  
}

void displayLocoID() {

  u8g2.clearBuffer();

  u8g2.setFont(u8g2_font_logisoso32_tn); 
  char buffer[6];
  snprintf(buffer, sizeof(buffer), "%04u", (unsigned int)lastAddress);
  int width = u8g2.getStrWidth(buffer);
  int x_pos = (128 - width) / 2;
  int y_pos = 50;

  u8g2.drawStr(x_pos, y_pos, buffer);
  u8g2.sendBuffer();
}
