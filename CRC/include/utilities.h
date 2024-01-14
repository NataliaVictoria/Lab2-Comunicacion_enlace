#include <Arduino.h>

namespace utilities{
/**
 * @brief This function stops the microcontroller when is invoked. It could be
 * used to set a breakpoint in your code.
 * @param ms Blink time
 */
void blinkBreakpoint(unsigned int ms) {
    static bool ledStatus = false;
    while(true){
        digitalWrite(LED_BUILTIN, ledStatus);
        ledStatus = !ledStatus;
        delay(ms);
    }
}
/**
 * @brief Utilizes an array on digital outputs to display the value of an
 * unsigned value
 * 
 * @param value The integer to emit
 * @param pinArray The array holding the digital outputs
 * @param arraySize The number digital outputs
 */

void writeInt(unsigned value, unsigned pinArray[], size_t arraySize) {
  for (size_t i = 0; i < arraySize; i++) {
    bool bit = (value & (1 << i)) >> i;
    digitalWrite(pinArray[i], bit);
  }
}

} //namespace utilities