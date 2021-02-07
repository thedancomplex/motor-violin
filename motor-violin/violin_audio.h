#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveformModulated wav0;   //xy=457.1000061035156,91.10000610351562
AudioSynthWaveformModulated wav1; //xy=461.1000061035156,142.10000610351562
AudioSynthWaveformModulated wav2; //xy=462.1000061035156,197.10000610351562
AudioSynthWaveformModulated wav3; //xy=465.1000061035156,248.10000610351562
AudioMixer4              mixer1;         //xy=645.1000061035156,152.10000610351562
AudioEffectEnvelope      envelope1;      //xy=785.0999755859375,149.10000610351562
AudioAmplifier           amp1;           //xy=924.0999755859375,149.10000610351562
AudioOutputAnalog        dac1;           //xy=1097.0999755859375,148.10000610351562
AudioConnection          patchCord1(wav0, 0, mixer1, 0);
AudioConnection          patchCord2(wav1, 0, mixer1, 1);
AudioConnection          patchCord3(wav2, 0, mixer1, 2);
AudioConnection          patchCord4(wav3, 0, mixer1, 3);
AudioConnection          patchCord5(mixer1, envelope1);
AudioConnection          patchCord6(envelope1, amp1);
AudioConnection          patchCord7(amp1, dac1);
// GUItool: end automatically generated code
