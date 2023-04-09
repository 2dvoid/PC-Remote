#include <IRremote.hpp>


// Define IR receiver pin on Arduino 
const int IR_RECEIVE_PIN = 7;


// Define mapping of IR codes to keyboard keypresses using a lookup table
struct IRCodeMap {
	uint32_t irCode;
	uint8_t keyCode;
};

const IRCodeMap IR_CODE_MAP[] = {

	// Add IR codes and their corresponding Keyboard codes (HID) in here. Structure:  {IR_CODE, KEY_CODE}
	// Do not add comma after the last entry

	// Sony Remote
	{0x92, 0x80},  // {VOLUME_PLUS, KEY_VOLUME_UP}
	{0x93, 0x81},  // {VOLUME_MINUS, KEY_VOLUME_DOWN}
	{0x94, 0x7F},  // {MUTE, KEY_MUTE}
	{0xF4, 0x52},  // {UP, KEY_UP_ARROW}
	{0xF5, 0x51},  // {DOWN, KEY_DOWN_ARROW}  
	{0xB4, 0x50},  // {LEFT, KEY_LEFT_ARROW}
	{0xB3, 0x4f},  // {RIGHT, KEY_RIGHT_ARROW}
	{0xE5, 0x2c},  // {OK, KEY_SPACE}
	{0x4BA3, 0x2a},  // {RETURN, KEY_BACKSPACE}
	{0x4BA8, 0x19},  // {SUBTITLE, KEY_V}
	{0x4B99, 0xe8},  // {PAUSE, KEY_MEDIA_PLAYPAUSE}
	{0x4B9A, 0xe8},  // {PLAY, KEY_MEDIA_PLAYPAUSE}
	{0x4B98, 0xf3},  // {STOP, KEY_MEDIA_STOP}
	{0x4B9B, 0xf1},  // {BACKWARD, KEY_MEDIA_BACKWARD}
	{0x4B9C, 0xf2},  // {FORWARD, KEY_MEDIA_FORWARD}
	{0x4BBC, 0xea},  // {PREVIOUS, KEY_MEDIA_PREVIOUS}
	{0x4BBD, 0xeb}   // {NEXT, KEY_MEDIA_NEXT}

};


// Increase the delay if IR codes get registered multiple times on a single press
const int IR_CODE_DELAY = 170;


// Last ir code and the time it was recieved. Used for checking repeatition
uint32_t lastIrCode = 0;
unsigned long lastIrTime = 0;


const int IR_CODE_MAP_SIZE = sizeof(IR_CODE_MAP) / sizeof(IRCodeMap);


// Define keyboard report buffer
uint8_t keyboardReport[8] = { 0 };


void setup() {

	Serial.begin(9600);

	// Start the receiver
	IrReceiver.begin(IR_RECEIVE_PIN);
}


void loop() {

	// Check if there is an IR signal available
	if (IrReceiver.decode()) {

		uint32_t irCode = IrReceiver.decodedIRData.decodedRawData;


		// Ignore the same code received within IR_CODE_DELAY time
		if (irCode == lastIrCode && (millis() - lastIrTime) < IR_CODE_DELAY) {
			IrReceiver.resume();
			return;
		}    


		// Check the received IR code and map it to a keyboard keypress using the lookup table
		for (int i = 0; i < IR_CODE_MAP_SIZE; i++) {

			if (irCode == IR_CODE_MAP[i].irCode) {
				sendKeypress(IR_CODE_MAP[i].keyCode);
				lastIrCode = irCode;
				lastIrTime = millis();
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

}

