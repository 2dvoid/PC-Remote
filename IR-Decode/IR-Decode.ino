#include <IRremote.hpp>


// Define IR receiver pin on Arduino 
int IR_RECEIVE_PIN = 7; 


void setup() {

	Serial.begin(9600);
	// Start the receiver
	IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK); 

}


void loop() {

	if (IrReceiver.decode()) { 

		// Print the HEX code
		Serial.println (IrReceiver.decodedIRData.decodedRawData, HEX); 
		// Enable receiving of the next value
		IrReceiver.resume();
       
	}

}
