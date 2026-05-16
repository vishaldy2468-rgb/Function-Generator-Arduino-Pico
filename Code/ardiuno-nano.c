#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Encoder.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Rotary encoder pins
#define CLK 2
#define DT 3
#define SW 4

Encoder knob(CLK, DT);

long oldPosition = -999;
int frequency = 1000;

String waveTypes[] = {"SQUARE", "SINE", "TRIANGLE"};
int waveIndex = 0;

bool lastButtonState = HIGH;

void setup() {

  Wire.begin();       // Nano as master
  pinMode(SW, INPUT_PULLUP);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();

  updateDisplay();
}

void loop() {

  // Frequency control
  long newPosition = knob.read()/4;

  if(newPosition != oldPosition){

      frequency += (newPosition-oldPosition)*100;

      if(frequency < 100)
          frequency = 100;

      if(frequency > 100000)
          frequency = 100000;

      oldPosition = newPosition;

      sendToPico();
      updateDisplay();
  }

  // Button press changes waveform
  bool buttonState = digitalRead(SW);

  if(buttonState == LOW && lastButtonState == HIGH){

      waveIndex++;

      if(waveIndex > 2)
          waveIndex = 0;

      sendToPico();
      updateDisplay();

      delay(200);
  }

  lastButtonState = buttonState;
}

void sendToPico(){

    String message =
    waveTypes[waveIndex]
    + " "
    + String(frequency);

    Wire.beginTransmission(8);   // Pico address
    Wire.write(message.c_str());

    Wire.endTransmission();
}

void updateDisplay(){

    display.clearDisplay();

    display.setTextSize(1);
    display.setTextColor(WHITE);

    display.setCursor(0,10);
    display.print("Wave:");
    display.println(waveTypes[waveIndex]);

    display.setCursor(0,30);
    display.print("Freq:");
    display.print(frequency);
    display.println(" Hz");

    display.display();
}
