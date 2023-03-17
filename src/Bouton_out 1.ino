#include <Arduino.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <Bounce2.h>

// build_flags = -D USB_MIDI_AUDIO_SERIAL

// WE WILL attach() THE BUTTON TO THE FOLLOWING PIN IN setup()
/*#define BUTTON1_PIN 32
  #define BUTTON2_PIN 31
  #define BUTTON3_PIN 30
  #define BUTTON4_PIN 29*/

#define LED_PIN 13
int ledState = LOW;

#define MAX9744_I2CADDR 0x4B

// INSTANTIATE A Button OBJECT
/*Button button1 = Button();
  Button button2 = Button();
  Button button3 = Button();
  Button button4 = Button();*/

// GUItool: begin automatically generated code
AudioPlaySdWav           playSdWav1;     //xy=151,241
AudioPlaySdWav           playSdWav4;     //xy=155,345
AudioPlaySdWav           playSdWav2;     //xy=156,275
AudioPlaySdWav           playSdWav3;     //xy=156,308
AudioMixer4              mixer2; //xy=378,351
AudioMixer4              mixer1;         //xy=386,216
AudioEffectFreeverb      freeverb2; //xy=522,288
AudioEffectFreeverb      freeverb1;      //xy=523,254
AudioMixer4              mixer4; //xy=673,346
AudioMixer4              mixer3;         //xy=679,198
AudioOutputI2S           i2s1;           //xy=937,225
AudioConnection          patchCord1(playSdWav1, 0, mixer1, 0);
AudioConnection          patchCord2(playSdWav1, 1, mixer2, 0);
AudioConnection          patchCord3(playSdWav4, 0, mixer1, 3);
AudioConnection          patchCord4(playSdWav4, 1, mixer2, 3);
AudioConnection          patchCord5(playSdWav2, 0, mixer1, 1);
AudioConnection          patchCord6(playSdWav2, 1, mixer2, 1);
AudioConnection          patchCord7(playSdWav3, 0, mixer1, 2);
AudioConnection          patchCord8(playSdWav3, 1, mixer2, 2);
AudioConnection          patchCord9(mixer2, mixer4);
AudioConnection          patchCord10(mixer1, mixer3);
//AudioConnection          patchCord11(freeverb2, 0, mixer4, 3);
//AudioConnection          patchCord12(freeverb1, 0, mixer3, 0);
AudioConnection          patchCord13(mixer4, 0, i2s1, 1);
AudioConnection          patchCord14(mixer3, 0, i2s1, 0);
AudioControlSGTL5000     sgtl5000_1;     //xy=932,414
// GUItool: end automatically generated code

// Use these with the Teensy 3.5 & 3.6 SD card
#define SDCARD_CS_PIN BUILTIN_SDCARD
#define SDCARD_MOSI_PIN 11 // not actually used
#define SDCARD_SCK_PIN 13  // not actually used

// volume level in this variable.
int8_t thevol = 32;

// reverb level in this variable.
float knob_Rev = 0;
float buffer_knob_Rev = 0;

void setup()
{
  Serial.begin(9600);
  Wire.begin();
  Wire1.begin();

  // FONCTION REVERB
  //setreverb();


  /*button1.attach(BUTTON1_PIN, INPUT); // USE EXTERNAL PULL-UP
    button2.attach(BUTTON2_PIN, INPUT); // USE EXTERNAL PULL-UP
    button3.attach(BUTTON3_PIN, INPUT); // USE EXTERNAL PULL-UP
    button4.attach(BUTTON4_PIN, INPUT); // USE EXTERNAL PULL-UP

    // DEBOUNCE INTERVAL IN MILLISECONDS
    button1.interval(5);
    button2.interval(5);
    button3.interval(5);
    button4.interval(5);

    // INDICATE THAT THE LOW STATE CORRESPONDS TO PHYSICALLY PRESSING THE BUTTON
    button1.setPressedState(LOW);
    button2.setPressedState(LOW);
    button3.setPressedState(LOW);
    button4.setPressedState(LOW);*/

  // LED SETUP
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, ledState);

  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(20);

  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN)))
  {
    // stop here, but print a message repetitively
    while (1)
    {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
  Serial.println("Accessed the SD card");

  while (!setvolume(thevol))
  {
    Serial.println("Failed to set volume, MAX9744 not found!");
    delay(500);
  }
}

// Setting the volume is very simple! Just write the 6-bit
// volume to the i2c bus. That's it!
boolean setvolume(int8_t v)
{
  // cant be higher than 63 or lower than 0
  if (v > 63)
    v = 63;
  if (v < 0)
    v = 0;

  Serial.print("Setting volume to ");
  Serial.println(v);
  Wire1.beginTransmission(MAX9744_I2CADDR);
  Wire1.write(v);
  if (Wire1.endTransmission() == 0)
    return true;
  else
    return false;
}

/*void setreverb(void)
  {
  knob_Rev = (float)analogRead(A13) / 1023.0;
  //Serial.print("Pot :");
  //Serial.println(knob_Rev);

  float reverbtime = float(knob_Rev*0.5/1.0);
  //Serial.print("Rev_Time :");
  //Serial.println(reverbtime);

  //freeverb1.reverbTime(reverbtime);
  //freeverb2.reverbTime(reverbtime);
  }*/

void playFile(const char *filename, int chord)
{
  Serial.print("Playing file: ");
  Serial.println(filename);
  Serial.print("Chord: ");
  Serial.println(chord);

  if (chord == 1)
  {
    playSdWav1.play(filename);
    delay(5);
  }

  if (chord == 2)
  {
    playSdWav2.play(filename);
    delay(5);
  }
  if (chord == 3)
  {
    playSdWav3.play(filename);
    delay(5);
  }
  if (chord == 4)
  {
    playSdWav4.play(filename);
    delay(5);
  }
}

void loop()
{

  /*button1.update();
    button2.update();
    button3.update();
    button4.update();*/

  Wire.requestFrom(2, 8);  // request 6 bytes from slave device #8

  while (Wire.available())
  {
    int i = Wire.read(); // receive a byte as character

    if (i == 0x69 || i == 0xE1 || i == 0xE2 || i == 0xE3 || i == 0xE4 || i == 0xE5 || i == 0xE6 || i == 0xEE || i == 0xA1 || i == 0xA2)
    {
      //Serial.print("Rien"); // print the character
    }
    else
    {

      if (i == 0x01)
      {
        playFile("HI-HATS1.wav", 1);     // filenames are always uppercase 8.3 format
        ledState = !ledState;            // SET ledState TO THE OPPOSITE OF ledState
        digitalWrite(LED_PIN, ledState); // WRITE THE NEW ledState
      }

      if (i == 0x02)
      {
        playFile("KICK1.wav", 2);
        ledState = !ledState;            // SET ledState TO THE OPPOSITE OF ledState
        digitalWrite(LED_PIN, ledState); // WRITE THE NEW ledState
      }

      if (i == 0x03)
      {
        playFile("SNARE1.wav", 3);
        ledState = !ledState;            // SET ledState TO THE OPPOSITE OF ledState
        digitalWrite(LED_PIN, ledState); // WRITE THE NEW ledState
      }

      if (i == 0x04)
      {
        playFile("PERC1.wav", 4);
        ledState = !ledState;            // SET ledState TO THE OPPOSITE OF ledState
        digitalWrite(LED_PIN, ledState); // WRITE THE NEW ledState
      }
      //if (i == 0xA1)
      //{
        //Layer Switch
        //digitalWrite(LED_PIN, ledState); // WRITE THE NEW ledState
      //}
      Serial.printf("%x\n\r", i);
    }
    //delay(5);
  }

  if (Serial.available())
  {

    // read a character from serial console
    char c = Serial.read();
    // increase
    if (c == '+')
    {
      thevol++;
    }
    // decrease
    else if (c == '-')
    {
      thevol--;
    }
    // ignore anything else
    else
      return;

    if (thevol > 63)
      thevol = 63;
    if (thevol < 0)
      thevol = 0;
    setvolume(thevol);
  }
  /*if (knob_Rev != buffer_knob_Rev) {
    //setreverb();
    }*/

}

// A known problem occurs on Macintosh computers, where the Mac's driver
// does not seem to be able to adapt and transmit horribly distorted
// audio to Teensy after a matter of minutes.  An imperfect workaround
// can be enabled by editing usb_audio.cpp.  Find and uncomment
// "#define MACOSX_ADAPTIVE_LIMIT".  More detailed info is available here:
// https://forum.pjrc.com/threads/34855-Distorted-audio-when-using-USB-input-on-Teensy-3-1?p=110392&viewfull=1#post110392

// It Works

/*AudioPlaySdWav           playWav1;
  // Use one of these 3 output types: Digital I2S, Digital S/PDIF, or Analog DAC
  AudioOutputI2S           audioOutput;
  //AudioOutputSPDIF       audioOutput;
  //AudioOutputAnalog      audioOutput;
  //On Teensy LC, use this for the Teensy Audio Shield:
  //AudioOutputI2Sslave    audioOutput;

  AudioConnection          patchCord1(playWav1, 0, audioOutput, 0);
  AudioConnection          patchCord2(playWav1, 1, audioOutput, 1);
  //AudioControlSGTL5000     sgtl5000_1;     //xy=239,232

  //AudioConnection          patchCord3(playWav1, 0, i2s2, 0);



  // GUItool: begin automatically generated code
  AudioPlaySdWav           playSdWav1;     //xy=151,241
  AudioPlaySdWav           playSdWav4;     //xy=155,345
  AudioPlaySdWav           playSdWav2;     //xy=156,275
  AudioPlaySdWav           playSdWav3;     //xy=156,308
  AudioMixer4              mixer2; //xy=378,351
  AudioMixer4              mixer1;         //xy=386,216
  AudioEffectReverb        reverb1;        //xy=523,202
  AudioEffectReverb        reverb2; //xy=526,351
  AudioMixer4              mixer4; //xy=673,346
  AudioMixer4              mixer3;         //xy=679,198
  AudioOutputI2S           i2s1;           //xy=937,225
  AudioConnection          patchCord1(playSdWav1, 0, mixer1, 0);
  AudioConnection          patchCord2(playSdWav1, 1, mixer2, 0);
  AudioConnection          patchCord3(playSdWav4, 0, mixer1, 3);
  AudioConnection          patchCord4(playSdWav4, 1, mixer2, 3);
  AudioConnection          patchCord5(playSdWav2, 0, mixer1, 1);
  AudioConnection          patchCord6(playSdWav2, 1, mixer2, 1);
  AudioConnection          patchCord7(playSdWav3, 0, mixer1, 2);
  AudioConnection          patchCord8(playSdWav3, 1, mixer2, 2);
  AudioConnection          patchCord9(mixer2, reverb2);
  AudioConnection          patchCord10(mixer1, reverb1);
  AudioConnection          patchCord11(reverb1, 0, mixer3, 1);
  AudioConnection          patchCord12(reverb2, 0, mixer4, 0);
  AudioConnection          patchCord13(mixer4, 0, i2s1, 1);
  AudioConnection          patchCord14(mixer3, 0, i2s1, 0);
  // GUItool: end automatically generated code
*/
