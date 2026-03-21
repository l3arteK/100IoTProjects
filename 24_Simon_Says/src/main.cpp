#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 64
#define INPUT_PIN     15
#define LED_PIN       5
#define PATTERN_SIZE  10
#define LED_ON_MS     200
#define COUNTDOWN_SEC 3

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

enum class GameState : uint8_t { SHOW_PATTERN, COUNTDOWN, USER_INPUT, RESULT };

struct GameData {
  uint16_t pattern[PATTERN_SIZE];
  uint16_t userPattern[PATTERN_SIZE];
  uint8_t  count      = 0;
  uint8_t  step       = 0;
  float    accuracy   = 0.f;
  bool     calculated = false;
};

static GameState     state = GameState::SHOW_PATTERN;
static portMUX_TYPE  synch = portMUX_INITIALIZER_UNLOCKED;
static GameData      game;
static volatile bool clicked      = false;
static unsigned long stateTimer   = 0;
static unsigned long ledTimer     = 0;
static unsigned long clickTimer   = 0;
static bool          ledState     = false;
static int8_t        countdownVal = COUNTDOWN_SEC;

void IRAM_ATTR isr() {
  unsigned long        now           = millis();
  static unsigned long lastInterrupt = 0;

  if (now - lastInterrupt > 200) {
    portENTER_CRITICAL(&synch);
    clicked = true;
    portEXIT_CRITICAL(&synch);
    lastInterrupt = now;
  }
}

static void setLed(bool on) {
  ledState = on;
  digitalWrite(LED_PIN, on ? HIGH : LOW);
  if (on) ledTimer = millis();
}

static bool consumeClick() {
  bool val;
  portENTER_CRITICAL(&synch);
  val     = clicked;
  clicked = false;
  portEXIT_CRITICAL(&synch);
  return val;
}

static void enterState(GameState next) {
  state      = next;
  stateTimer = millis();

  if (next == GameState::COUNTDOWN) {
    countdownVal = COUNTDOWN_SEC;
    display.clearDisplay();
    display.display();
  }
  if (next == GameState::USER_INPUT) {
    game.step  = 0;
    clickTimer = millis();
    consumeClick();
  }
  if (next == GameState::RESULT) {
    game.calculated = false;
    game.accuracy   = 0.f;
  }
}

static void startGame() {
  game.count      = 3;
  game.calculated = false;
  game.accuracy   = 0.f;
  game.step       = 0;
  setLed(false);
  for (uint8_t i = 0; i < game.count; i++) {
    game.pattern[i] = (uint16_t)random(1000, 5000);
  }

  enterState(GameState::SHOW_PATTERN);
}

static void handleShowPattern() {
  unsigned long now = millis();
  if (!ledState) {
    if (now - stateTimer >= game.pattern[game.step]) {
      setLed(true);
    }
  } else {
    if (now - ledTimer >= LED_ON_MS) {
      setLed(false);
      stateTimer = now;
      game.step++;

      if (game.step == game.count) {
        enterState(GameState::COUNTDOWN);
      }
    }
  }
}

static void handleCountdown() {
  unsigned long now = millis();
  if (now - stateTimer < 1000) return;
  stateTimer = now;

  display.clearDisplay();
  display.setCursor(20, 30);
  display.setTextSize(2);

  if (countdownVal > 0) {
    display.print(countdownVal);
    display.display();
    countdownVal--;
  } else {
    display.print(F("START"));
    display.display();
    delay(600);
    display.clearDisplay();
    display.display();
    enterState(GameState::USER_INPUT);
    return;
  }
}

static void handleUserInput() {
  unsigned long now = millis();
  if (consumeClick() && game.step < game.count) {
    uint16_t elapsed            = (uint16_t)(now - clickTimer);
    game.userPattern[game.step] = elapsed;
    clickTimer                  = now;
    setLed(true);
    game.step++;

    if (game.step == game.count) {
      enterState(GameState::RESULT);
      return;
    }
  }

  if (ledState && now - ledTimer >= LED_ON_MS) {
    setLed(false);
  }
}

static void handleResult() {
  if (!game.calculated) {
    float total = 0.f;
    for (uint8_t i = 0; i < game.count; i++) {
      int32_t delta = abs((int32_t)game.pattern[i] - (int32_t)game.userPattern[i]);
      float   error = (float)delta / (float)game.pattern[i];
      total += (1.f - min(error, 1.f)) * 100.f;
    }
    game.accuracy   = total / game.count;
    game.calculated = true;

    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print(F("Accuracy: "));
    display.print(game.accuracy, 1);
    display.println(F(" %"));

    for (uint8_t i = 0; i < game.count; i++) {
      display.print(game.pattern[i]);
      display.print(F(" vs "));
      display.println(game.userPattern[i]);
    }

    display.display();
  } else if (consumeClick()) {
    display.clearDisplay();
    display.display();
    startGame();
  }
}

void setup() {
  Serial.begin(115200);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for (;;)
      ;
  }
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.clearDisplay();
  display.display();

  pinMode(INPUT_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(INPUT_PIN), isr, RISING);
  pinMode(LED_PIN, OUTPUT);

  randomSeed(esp_random());
  startGame();
}

void loop() {
  switch (state) {
  case GameState::SHOW_PATTERN:
    handleShowPattern();
    break;
  case GameState::COUNTDOWN:
    handleCountdown();
    break;
  case GameState::USER_INPUT:
    handleUserInput();
    break;
  case GameState::RESULT:
    handleResult();
    break;
  }
}
