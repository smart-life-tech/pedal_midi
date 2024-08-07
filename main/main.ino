#define DEBOUNCE 1500

struct key
{
    int pin;
    int midiKey;
    int debounce;
    int keySent;
};

struct key keys[] =
    {
        {22, 25, 0, 0}, // Db red
        {24, 26, 0, 0}, // D  red
        {26, 27, 0, 0}, // Eb orange
        {28, 28, 0, 0}, // E  orange
        {30, 29, 0, 0}, // F  yellow
        {32, 30, 0, 0}, // Gb green
        {34, 31, 0, 0}, // G  green
        {36, 32, 0, 0}, // Ab blue
        {38, 33, 0, 0}, // A  blue
        {40, 34, 0, 0}, // Bb violet
        {42, 35, 0, 0}, // B  violet
        {44, 36, 0, 0}, // C  brown
        {48, 24, 0, 0}, // C  brown
        {0, 0, 0, 0}    // end of list marker
};

int keyOffset = 0;
int keyVelocity = 100;

void setup()
{
    // put your setup code here, to run once:
    for (int i = 0; keys[i].pin != 0; ++i)
    {
        pinMode(keys[i].pin, INPUT_PULLUP);
    }
    // start serial with midi baudrate 31250
    Serial.begin(31250);
}

void Midi_Send(byte cmd, byte data1, byte data2)
{
    Serial.write(cmd);
    Serial.write(data1);
    Serial.write(data2);
}

void noteOn(int midiKey)
{
    Midi_Send(0x90, midiKey, keyVelocity);
}

void noteOff(int midiKey)
{
    Midi_Send(0x80, midiKey, keyVelocity);
}

void loop()
{
    // put your main code here, to run repeatedly:
    byte byte1;
    byte byte2;
    byte byte3;
    int value;

    //*************** MIDI THRU ******************//
    if (Serial.available() > 0)
    {
        byte1 = Serial.read();
        byte2 = Serial.read();
        byte3 = Serial.read();

        Midi_Send(byte1, byte2, byte3);
    }

    // Look for bass pedal key events
    for (int i = 0; keys[i].pin != 0; ++i)
    {
        value = digitalRead(keys[i].pin);
        if (keys[i].debounce == 0) // Key has been off
        {
            if (value == LOW) // Key is now on
            {
                noteOn(keys[i].midiKey + keyOffset); // Send the MIDI note on message
                keys[i].keySent = keys[i].midiKey + keyOffset;
                keys[i].debounce = DEBOUNCE; // Set the note off debounce counter
            }
        }
        else // Key has been on
        {
            if (value == HIGH) // Key has gone off
            {
                if (--keys[i].debounce == 0)  // If Key has remained off for DEBOUNCE scans,
                    noteOff(keys[i].keySent); // In case the offset has changed, send MIDI off for the right note
            }
            else                             // Key has not gone off
                keys[i].debounce = DEBOUNCE; // Reset debounce counter in case we got
                                             // a small number of key off scans
        }
    }
}
