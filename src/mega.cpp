#if defined(AVR)
#include <Arduino.h>

void setup()
{
    Serial.begin(115200);
    Serial3.begin(115200);
}

void loop()
{
    if (Serial3.available())
    {
        Serial.println(Serial3.readStringUntil('\n'));
    }
}
#endif