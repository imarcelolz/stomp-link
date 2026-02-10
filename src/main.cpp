#include <Arduino.h>
#include <unordered_map>

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

struct Button {
  const char* name;

  int button;
  int led;
  int ccCommand;
};

struct Combo {
  const char* name;

  int button1;
  int button2;
  int led1;
  int led2;

  int ccCommand;
};

Button buttons[] = {
  {"Input 1", INPUT_1, LED_1, 10},
  {"Input 2", INPUT_2, LED_2, 22},
  {"Input 3", INPUT_3, LED_3, 11},
  {"Input 4", INPUT_4, LED_4, 24},
};

std::unordered_map<int, Button> buttonMap = {
  {INPUT_1, buttons[0]},
  {INPUT_2, buttons[1]},
  {INPUT_3, buttons[2]},
  {INPUT_4, buttons[3]},
};

Combo combos[] = {
  {"Combo 1", INPUT_1, INPUT_2, LED_1, LED_2, 20},
  {"Combo 2", INPUT_2, INPUT_3, LED_2, LED_3, 21},
  {"Combo 3", INPUT_3, INPUT_4, LED_3, LED_4, 22},
};

std::unordered_map<int, Combo> comboMap = {
  {COMBO_1, combos[0]},
  {COMBO_2, combos[1]},
  {COMBO_3, combos[2]},
};

int leds[] = {LED_1, LED_2, LED_3, LED_4};

int currentPreset = 0;
int currentCombo = 0;

void sendMIDICC(uint8_t ccNumber);
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

void onCombo(int comboId)
{
  if(currentCombo == comboId) {
    return;
  }

  auto combo = comboMap[comboId];

  Serial.println(combo.name);
  currentCombo = comboId;

  Serial.printf("Combo activated: %s\n", combo.name);

  setLed(combo.led1);
  analogWrite(combo.led2, LED_BRIGHTNESS);
  sendMIDICC(combo.ccCommand);

  doneLoop();
}

void onInput(int buttonPin)
{
  if (currentPreset == buttonPin) {
    return;
  }

  auto input = buttonMap[buttonPin];
  Serial.println(input.name);
  currentCombo = 0;
  currentPreset = input.button;

  setLed(input.led);
  sendMIDICC(input.ccCommand);

  doneLoop();
}

void loop()
{
  bool input1 = digitalRead(INPUT_1) == HIGH;
  bool input2 = digitalRead(INPUT_2) == HIGH;
  bool input3 = digitalRead(INPUT_3) == HIGH;
  bool input4 = digitalRead(INPUT_4) == HIGH;

  if (!input1 && !input2 && !input3 && !input4)
  {
    return doneLoop();
  }

  delay(50);

  input1 = digitalRead(INPUT_1) == HIGH;
  input2 = digitalRead(INPUT_2) == HIGH;
  input3 = digitalRead(INPUT_3) == HIGH;
  input4 = digitalRead(INPUT_4) == HIGH;

  Serial.printf("inputs-2: %d %d %d %d\n", input1, input2, input3, input4);

  if (input1 && input2)
  {
    return onCombo(COMBO_1);
  }

  if (input2 && input3)
  {
    return onCombo(COMBO_2);
  }

  if (input3 && input4)
  {
    return onCombo(COMBO_3);
  }

  if (input1)
  {
    return onInput(INPUT_1);
  }

  if (input2)
  {
    return onInput(INPUT_2);
  }

  if (input3)
  {
    return onInput(INPUT_3);
  }

  if (input4)
  {
    return onInput(INPUT_4);
  }
}

void sendMIDICC(uint8_t ccNumber)
{
  // 0xB0 é o Status Byte para Control Change no Canal 1
  MIDI_SERIAL.write(0xB0);
  // O número do CC (10, 22, 11 ou 24)
  MIDI_SERIAL.write(ccNumber);
  // Valor 127 (0x7F) para simular o "pressionar" do botão
  MIDI_SERIAL.write(0x7F);

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
