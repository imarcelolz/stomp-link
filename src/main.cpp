#include <Arduino.h>
#include <Preferences.h>

enum OperatingMode {
  MODE_REACTOR = 0,
  MODE_MOER = 1
};

OperatingMode currentMode = MODE_REACTOR;
Preferences preferences;

#define MIDI_SERIAL Serial1
#define LED_BRIGHTNESS 10

#define LED_MAIN 5

#define LED_1 9
#define LED_2 8
#define LED_3 7
#define LED_4 6

#define INPUT_1 1
#define INPUT_2 0
#define INPUT_3 3
#define INPUT_4 2

#define COMBO_1 1
#define COMBO_2 2
#define COMBO_3 3

const unsigned long LONG_PRESS_THRESHOLD = 500;
const unsigned long DEBOUNCE_DELAY = 30;

struct Button {
  const char* name;

  int button;
  int led;

  int ccMoer;
  int ccCommand;
  int ccLongPressCommand;

  bool state;
  bool lastState;
  bool longPressHandled;

  unsigned long pressedTime;
  unsigned long lastDebounceTime;
};

Button buttons[] = {
  {"Input 1", INPUT_1, LED_1, 10, 1, 30, false, false, false, 0, 0}, //compressor and tuner
  {"Input 2", INPUT_2, LED_2, 22, 2, 21, false, false, false, 0, 0}, // drive e amp 1
  {"Input 3", INPUT_3, LED_3, 11, 5, 22, false, false, false, 0, 0}, //delay e amp 2
  {"Input 4", INPUT_4, LED_4, 24, 6, 0, false, false, false, 0, 0}, //mod e muda para pedal
};


std::unordered_map<int, Button> buttonMap = {
  {INPUT_1, buttons[0]},
  {INPUT_2, buttons[1]},
  {INPUT_3, buttons[2]},
  {INPUT_4, buttons[3]},
};

int leds[] = {LED_1, LED_2, LED_3, LED_4};

int currentPreset = 0;

void sendMIDICC(uint8_t ccNumber, bool value);
void setLed(int pin);

void setup()
{
  Serial.begin(115200);

  MIDI_SERIAL.begin(31250, SERIAL_8N1, -1, 21);

  pinMode(LED_MAIN, OUTPUT);
  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  pinMode(LED_4, OUTPUT);

  pinMode(INPUT_1, INPUT_PULLUP);
  pinMode(INPUT_2, INPUT_PULLUP);
  pinMode(INPUT_3, INPUT_PULLUP);
  pinMode(INPUT_4, INPUT_PULLUP);

  delay(500);

  // System status indicator
  analogWrite(LED_MAIN, LED_BRIGHTNESS);
}

void doneLoop()
{
  delay(50);
}

void onInput(int buttonPin)
{
  auto input = buttonMap[buttonPin];
  input.state = !input.state;

  Serial.println(input.name);
  // setLed(input.led);
  analogWrite(input.led, input.state ? LED_BRIGHTNESS : 0);
  sendMIDICC(input.ccCommand, input.state);

  doneLoop();
}

void onNewInputLongPress(Button* input)
{
  bool rawReading = (digitalRead(input->button) == LOW);
  unsigned long now = millis();

  if (rawReading && !input->longPressHandled && (now - input->pressedTime >= LONG_PRESS_THRESHOLD)) {
    input->longPressHandled = true;
    sendMIDICC(input->ccLongPressCommand, 0x7F);
    return;
  }

  if (rawReading == input->lastState) {
    return;
  }

  if ((now - input->lastDebounceTime) < DEBOUNCE_DELAY) {
    return;
  }

  input->lastDebounceTime = now;
  input->lastState = rawReading;

  if (rawReading) {
    input->pressedTime = now;
    input->longPressHandled = false;
    return;
  }

  if (input->longPressHandled) {
    return;
  }

  input->state = !input->state;
  analogWrite(input->led, input->state ? LED_BRIGHTNESS : 0);
  sendMIDICC(input->ccCommand, input->state ? 0x7F : 0x00);
}

void onNewInput(Button* input)
{
  bool rawReading = (digitalRead(input->button) == LOW);
  unsigned long now = millis();

  if (rawReading == input->lastState) {
    return;
  }

  if ((now - input->lastDebounceTime) < DEBOUNCE_DELAY) {
    return;
  }

  input->lastDebounceTime = now;
  input->lastState = rawReading;

  if (rawReading) {
    return;
  }

  input->state = !input->state;
  analogWrite(input->led, input->state ? LED_BRIGHTNESS : 0);
  sendMIDICC(input->ccCommand, input->state ? 0x7F : 0x00);
}

void loop()
{
  for(int i = 0; i < 4; i++) {
    auto input = buttons[i];

    onNewInput(&buttons[i]);
  }

  delay(1);
}

void sendMIDICC(uint8_t ccNumber, bool value)
{
  // 0xB0 é o Status Byte para Control Change no Canal 1
  MIDI_SERIAL.write(0xB0);
  MIDI_SERIAL.write(ccNumber);
  MIDI_SERIAL.write(value ? 0x7F : 0x00);

  Serial.printf("CC MIDI Enviado: %d value 127\n", ccNumber);
}

void setLed(int pin)
{
  for (int led : leds)
  {
    analogWrite(led, 0);
  }

  analogWrite(pin, LED_BRIGHTNESS);
}
