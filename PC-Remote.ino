#include <IRremote.hpp>

// Define IR receiver pin on Arduino 
const int IR_RECEIVE_PIN = 7;

// Define mapping of IR codes to keyboard keypresses using a lookup table
struct IRCodeMap {
    uint32_t irCode;
    uint8_t keyCode;
};

const IRCodeMap IR_CODE_MAP[] = {
    // Sony Remote
    {0x92, 0x80},    // {VOLUME_PLUS, KEY_VOLUME_UP}
    {0x93, 0x81},    // {VOLUME_MINUS, KEY_VOLUME_DOWN}
    {0x94, 0x7F},    // {MUTE, KEY_MUTE}
    {0xF4, 0x52},    // {UP, KEY_UP_ARROW}
    {0xF5, 0x51},    // {DOWN, KEY_DOWN_ARROW}  
    {0xB4, 0x50},    // {LEFT, KEY_LEFT_ARROW}
    {0xB3, 0x4f},    // {RIGHT, KEY_RIGHT_ARROW}
    {0x4B9B, 0x2f},  // {BACKWARD, KEY_LEFT_SQUARE_BRACE}
    {0x4B9C, 0x30},  // {FORWARD, KEY_RIGHT_SQUARE_BRACE}
    {0xE5, 0x2c},    // {OK, KEY_SPACE}
    {0x4BA3, 0x2a},  // {RETURN, KEY_BACKSPACE}
    {0x4BA8, 0x06},  // {SUBTITLE, KEY_c}
    {0xBF, 0x0d},    // {CHANGE_SUBTITLE, KEY_j}
    {0x4B99, 0xe8},  // {PAUSE, KEY_MEDIA_PLAYPAUSE}
    {0x4B9A, 0xe8},  // {PLAY, KEY_MEDIA_PLAYPAUSE}
    {0x4B98, 0xf3},  // {STOP, KEY_MEDIA_STOP}
    {0x4BBC, 0xea},  // {PREVIOUS, KEY_MEDIA_PREVIOUS}
    {0x4BBD, 0xeb},  // {NEXT, KEY_MEDIA_NEXT}
    {0x523D, 0x09},  // {FULLSCREEN, KEY_f}
    {0x4BA5, 0x14},  // {RED, KEY_q}
    {0x4BB6, 0x28}   // {OPTIONS, KEY_ENTER}
};

// Debouncing and repeat prevention settings
const unsigned long DEBOUNCE_DELAY = 200;  // Increased delay to prevent repeats
const unsigned long REPEAT_DELAY = 500;    // Minimum time between different key presses

// Last IR code and the time it was received
uint32_t lastIrCode = 0;
unsigned long lastIrTime = 0;
bool keyPressed = false;

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
        
        // Ignore if the receiver is still processing
        if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_WAS_OVERFLOW) {
            IrReceiver.resume();
            return;
        }
        
        uint32_t irCode = IrReceiver.decodedIRData.decodedRawData;
        unsigned long currentTime = millis();
        
        // Check if this is a repeat signal (many IR protocols send these)
        if (IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT) {
            IrReceiver.resume();
            return; // Ignore repeat signals entirely
        }
        
        // Debounce: ignore the same code received within DEBOUNCE_DELAY time
        if (irCode == lastIrCode && (currentTime - lastIrTime) < DEBOUNCE_DELAY) {
            IrReceiver.resume();
            return;
        }
        
        // Prevent rapid key presses even for different keys
        if ((currentTime - lastIrTime) < 100) { // Very short delay between any keys
            IrReceiver.resume();
            return;
        }
        
        // Check the received IR code and map it to a keyboard keypress
        for (int i = 0; i < IR_CODE_MAP_SIZE; i++) {
            if (irCode == IR_CODE_MAP[i].irCode) {
                sendKeypress(IR_CODE_MAP[i].keyCode);
                lastIrCode = irCode;
                lastIrTime = currentTime;
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
    
    // Small delay to ensure the key press is registered
    delay(10);
    
    // Release the keypress
    keyboardReport[2] = 0;
    Serial.write(keyboardReport, 8);
    
    // Additional small delay to ensure key release is processed
    delay(10);
}
