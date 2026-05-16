#include <Wire.h>
#include <Adafruit_MCP4725.h>

Adafruit_MCP4725 dac;  // DAC object for MCP4725
String receivedData;
String waveType = "SQUARE";
int frequency = 1000;  // Default frequency (1 kHz)

void setup() {
  Wire.begin(8);         // I2C address for the Pico
  Wire.onReceive(receiveEvent);  // Register I2C receive function
  dac.begin(0x60);  // Initialize DAC
}

void loop() {
  // Generate square, sine, or triangle waves based on received data
  if (waveType == "SQUARE") generateSquareWave(frequency);
  else if (waveType == "SINE") generateSineWave(frequency);
  else if (waveType == "TRIANGLE") generateTriangleWave(frequency);
}

void receiveEvent(int bytes) {
  receivedData = "";
  while (Wire.available()) {
    receivedData += (char)Wire.read();  // Read incoming data
  }
  
  // Parse the received data for waveform type and frequency
  int spaceIndex = receivedData.indexOf(' ');
  waveType = receivedData.substring(0, spaceIndex);
  frequency = receivedData.substring(spaceIndex + 1).toInt();
}

void generateSquareWave(int freq) {
  int period = 1000000 / freq;
  for (int i = 0; i < period; i++) {
    digitalWrite(2, HIGH);
    delayMicroseconds(period / 2);
    digitalWrite(2, LOW);
    delayMicroseconds(period / 2);
  }
}

void generateSineWave(int freq) {
  // Generate sine wave using DAC (MCP4725)
  for (int i = 0; i < 360; i++) {
    int value = 2048 + 2047 * sin(i * 3.14159 / 180);  // Sine wave formula
    dac.setVoltage(value, false);  // Set DAC value
    delayMicroseconds(1000000 / freq / 360);  // Delay based on frequency
  }
}

void generateTriangleWave(int freq) {
  // Generate triangle wave using DAC (MCP4725)
  for (int i = 0; i < 360; i++) {
    int value = i < 180 ? 2048 + i * 2047 / 180 : 2048 + (360 - i) * 2047 / 180;
    dac.setVoltage(value, false);
    delayMicroseconds(1000000 / freq / 360);
  }
}