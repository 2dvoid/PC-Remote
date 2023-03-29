#include <IRremote.hpp>

// Define IR receiver pin and instance of IRrecv library
const int IR_RECEIVE_PIN = 7;
//IRrecv irReceiver(IR_RECEIVE_PIN);

// Define serial communication baud rate
const int SERIAL_BAUD_RATE = 9600;

// Define mapping of IR codes to keyboard keypresses using a lookup table
struct IRCodeMap {
  uint32_t irCode;
  uint8_t keyCode;
};

const IRCodeMap IR_CODE_MAP[] = {
  // Add IR code and it's corresponding Key code in here. Structure:  {IR_CODE, KEY_CODE} .
  // You won't have to change anything in the code.

  // Chinese Remote
  //{0x1020, 0x80},  // {IR_CODE_VOLUME_UP, KEY_VOLUME_UP}
  //{0x1820, 0x80},  // {IR_CODE_VOLUME_UP, KEY_VOLUME_UP}
  //{0x1021, 0x81},  // {IR_CODE_VOLUME_DOWN, KEY_VOLUME_DOWN}
  //{0x1821, 0x81},  // {IR_CODE_VOLUME_DOWN, KEY_VOLUME_DOWN}
  //{0x100D, 0x7F},  // {IR_CODE_MUTE, KEY_MUTE}
  //{0x180D, 0x7F},  // {IR_CODE_MUTE, KEY_MUTE}
  //{0x1026, 0x50},  // {IR_CODE_SLEEP, KEY_LEFT_ARROW}
  //{0x1826, 0x50},  // {IR_CODE_SLEEP, KEY_LEFT_ARROW}
  //{0x103B, 0x4f},  // {IR_CODE_MENU, KEY_RIGHT_ARROW}
  //{0x183B, 0x4f},  // {IR_CODE_MENU, KEY_RIGHT_ARROW}
  //{0x1025, 0x2c},  // {IR_CODE_OK, KEY_SPACE}
  //{0x1825, 0x2c},  // {IR_CODE_OK, KEY_SPACE}
  //{0x1037, 0x19},  // {IR_CODE_CBC, KEY_V}
  //{0x1837, 0x19}   // {IR_CODE_CBC, KEY_V}

  // Sony Remote
  {0x92, 0x80},  // {IR_CODE_VOLUME_PLUS, KEY_VOLUME_UP}
  {0x93, 0x81},  // {IR_CODE_VOLUME_MINUS, KEY_VOLUME_DOWN}
  {0x94, 0x7F},  // {IR_CODE_MUTE, KEY_MUTE}
  {0xF4, 0x52},  // {IR_CODE_UP, KEY_UP_ARROW}
  {0xF5, 0x51},  // {IR_CODE_DOWN, KEY_DOWN_ARROW}  
  {0xB4, 0x50},  // {IR_CODE_LEFT, KEY_LEFT_ARROW}
  {0xB3, 0x4f},  // {IR_CODE_RIGHT, KEY_RIGHT_ARROW}
  {0xE5, 0x28},  // {IR_CODE_OK, KEY_ENTER}
  {0x4BA3, 0x2a},  // {IR_CODE_RETURN, KEY_BACKSPACE}
  {0x4BA8, 0x19}, // {IR_CODE_SUBTITLE, KEY_V}
  {0x4B99, 0xe8},  // {IR_CODE_PAUSE, KEY_PLAYPAUSE}
  {0x4B9A, 0xe8},  // {IR_CODE_PLAY, KEY_PLAYPAUSE}
  {0x4B98, 0x78}  // {IR_CODE_STOP, KEY_STOP}
};

// Debounce delay to fix repeatition of same ir codes
// Change the debounceDelay variable time (in ms) as needed
const int debounceDelay = 170;

const int IR_CODE_MAP_SIZE = sizeof(IR_CODE_MAP) / sizeof(IRCodeMap);

// Define keyboard report buffer
uint8_t keyboardReport[8] = { 0 };

void setup() {
  // Initialize serial communication
  Serial.begin(SERIAL_BAUD_RATE);
  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK); // Start the receiver
}

void loop() {
  // Check if there is an IR signal available
  if (IrReceiver.decode()) {
    // Check the received IR code and map it to a keyboard keypress using the lookup table
    uint32_t irCode = IrReceiver.decodedIRData.decodedRawData;
    for (int i = 0; i < IR_CODE_MAP_SIZE; i++) {
      if (irCode == IR_CODE_MAP[i].irCode) {
        sendKeypress(IR_CODE_MAP[i].keyCode);
        break;
      }
    }

    // Reset the IR receiver for the next signal
    IrReceiver.resume();
  }
}

void sendKeypress(uint8_t keyCode) {
  // Prepare the keyboard report buffer with the desired keypress
  keyboardReport[0] = 0;
  keyboardReport[2] = keyCode;

  // Send the keypress over serial communication
  Serial.write(keyboardReport, 8);

  // Release the keypress
  keyboardReport[2] = 0;
  Serial.write(keyboardReport, 8);

  // Debounce delay to fix repeatition of same ir codes
  delay (debounceDelay);
}

