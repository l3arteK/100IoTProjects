#include <Arduino.h>

#define NUM_BUTTONS      4
#define BUTTON_1_PIN     5
#define BUTTON_2_PIN     18
#define BUTTON_3_PIN     19
#define BUTTON_4_PIN     21
#define LED_1_PIN        13
#define LED_2_PIN        12
#define LED_3_PIN        14
#define LED_4_PIN        27
#define BUZZER_PIN       26
#define MAX_PATTERN_SIZE 20

static constexpr uint8_t NO_LED = UINT8_MAX;

enum class GameState : uint8_t { SHOW_PATTERN, USER_INPUT, RESULT };
enum class PatternStep : uint8_t { LED_ON, LED_OFF };
struct Button {
  const uint8_t pin;
  volatile bool clicked;
  unsigned long lastInterrupt;
  portMUX_TYPE  mux;
};

struct GameData {
  uint8_t pattern[MAX_PATTERN_SIZE];
  uint8_t level;
  uint8_t step;
  uint8_t patternLength;
  bool    win;
  uint8_t activeLedPin;
};

static Button buttons[NUM_BUTTONS] = {{BUTTON_1_PIN, false, 0, portMUX_INITIALIZER_UNLOCKED},
                                      {BUTTON_2_PIN, false, 0, portMUX_INITIALIZER_UNLOCKED},
                                      {BUTTON_3_PIN, false, 0, portMUX_INITIALIZER_UNLOCKED},
                                      {BUTTON_4_PIN, false, 0, portMUX_INITIALIZER_UNLOCKED}};
static constexpr uint8_t diodes[NUM_BUTTONS] = {LED_1_PIN, LED_2_PIN, LED_3_PIN, LED_4_PIN};
static GameState         state               = GameState::SHOW_PATTERN;
static GameData          game;

void IRAM_ATTR isr(void *arg) {
  Button       *btn = (Button *)arg;
  unsigned long now = millis();

  if (now - btn->lastInterrupt > 200) {
    portENTER_CRITICAL_ISR(&btn->mux);
    btn->clicked = true;
    portEXIT_CRITICAL_ISR(&btn->mux);
    btn->lastInterrupt = now;
  }
}

void beep(int freq, int duration) {
  ledcWriteTone(0, freq);
  delay(duration);
  ledcWriteTone(0, 0);
}

void playError() {
  beep(800, 200);
  beep(400, 300);
}

void startGame(int level) {
  game.level         = level;
  game.step          = 0;
  game.win           = true;
  game.patternLength = game.level * 2;
  for (int i = 0; i < game.patternLength; i++) {
    game.pattern[i] = (uint8_t)random(0, 4);
  }

  for (const auto &diode : diodes) {
    digitalWrite(diode, false);
  }
  delay(200);
  for (const auto &diode : diodes) {
    digitalWrite(diode, true);
  }
  for (const auto &diode : diodes) {
    delay(500);
    digitalWrite(diode, false);
  }
  delay(1000);
  state = GameState::SHOW_PATTERN;
}

void handleShowPattern() {
  static uint8_t     patternIndex = 0;
  static PatternStep step         = PatternStep::LED_ON;
  static uint32_t    lastTime     = 0;

  const uint32_t onDuration  = 200;
  const uint32_t offDuration = max(200, 1200 - game.level * 100);

  if (step == PatternStep::LED_ON && millis() - lastTime >= onDuration) {
    digitalWrite(diodes[game.pattern[patternIndex]], true);
    step     = PatternStep::LED_OFF;
    lastTime = millis();
  } else if (step == PatternStep::LED_OFF && millis() - lastTime >= offDuration) {
    digitalWrite(diodes[game.pattern[patternIndex]], false);
    patternIndex++;
    step     = PatternStep::LED_ON;
    lastTime = millis();
  }
  if (patternIndex >= game.level * 2) {
    patternIndex = 0;
    state        = GameState::USER_INPUT;
  }
}

static bool consumeClick(Button &btn) {
  bool val;
  portENTER_CRITICAL(&btn.mux);
  val         = btn.clicked;
  btn.clicked = false;
  portEXIT_CRITICAL(&btn.mux);
  return val;
}

void handleUserInput() {
  if (game.win && game.step != game.patternLength) {
    for (int i = 0; i < NUM_BUTTONS; i++) {
      if (consumeClick(buttons[i])) {
        if (game.pattern[game.step] == i) {
          if (game.step > 0) {
            digitalWrite(diodes[game.pattern[game.step - 1]], false);
          }
          digitalWrite(diodes[i], true);
          game.step++;
        } else {
          game.win = false;
        }
      }
    }
  } else {
    state = GameState::RESULT;
  }
}

void handleResult() {
  if (game.win) {
    startGame(game.level + 1);
  } else {
    playError();
    startGame(1);
  }
}

void setup() {
  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(buttons[i].pin, INPUT);
    attachInterruptArg(digitalPinToInterrupt(buttons[i].pin), isr, &buttons[i], RISING);
    pinMode(diodes[i], OUTPUT);
  }

  ledcSetup(0, 1000, 8);
  ledcAttachPin(BUZZER_PIN, 0);
  randomSeed(esp_random());
  startGame(1);
}

void loop() {
  switch (state) {
  case GameState::SHOW_PATTERN:
    handleShowPattern();
    break;
  case GameState::USER_INPUT:
    handleUserInput();
    break;
  case GameState::RESULT:
    handleResult();
    break;
  }
}
