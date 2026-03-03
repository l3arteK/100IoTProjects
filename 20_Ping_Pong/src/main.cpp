
#include <Arduino.h>
#include <U8g2lib.h>

constexpr uint8_t PLAYER1_UP = 14;
constexpr uint8_t PLAYER1_DOWN = 12;
constexpr uint8_t PLAYER2_UP = 25;
constexpr uint8_t PLAYER2_DOWN = 26;
constexpr int SCREEN_W = 128;
constexpr int SCREEN_H = 64;
constexpr int BALL_START_X = 60;
constexpr int BALL_START_Y = 30;
constexpr int BALL_SPEED = 3;
constexpr int BALL_SIZE = 2;
constexpr int PADDLE_LENGTH = 18;
constexpr int PADDLE_WIDTH = 5;

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0);

class Paddle
{
public:
  int x;
  int y;
  int length = PADDLE_LENGTH;
  int width = PADDLE_WIDTH;

  Paddle(int x, int y)
  {
    this->x = x;
    this->y = y;
  }
  Paddle() {}

  void up()
  {
    if (y > 0)
      y -= 1;
  }

  void down()
  {
    if (y < SCREEN_H - length)
      y += 1;
  }
};

struct Ball
{
  int x;
  int y;
  int dx;
  int dy;
};

Ball ball;
Paddle player_1;
Paddle player_2;

void startGame()
{
  ball = {BALL_START_X, BALL_START_Y, BALL_SPEED, 0};
  player_1 = Paddle(5, 10);
  player_2 = Paddle(117, 5);
}

void checkCollisionLeft(Paddle &paddle)
{
  if (ball.x >= paddle.x + paddle.width && ball.x <= paddle.x + paddle.width - ball.dx &&
      ball.y >= paddle.y &&
      ball.y <= paddle.y + paddle.length)
  {
    ball.dx = -ball.dx;
    ball.dy = ((ball.y - paddle.y) / paddle.length) * 2.0f - 1.0f;
  }
}

void checkCollisionRight(Paddle &paddle)
{
  if (ball.x >= paddle.x - 2 && ball.x - paddle.width <= paddle.x &&
      ball.y >= paddle.y &&
      ball.y <= paddle.y + paddle.length)
  {
    ball.dx = -ball.dx;
    ball.dy = ((ball.y - paddle.y) / paddle.length) * 2.0f - 1.0f;
  }
}

void input_task(void *pvParameters)
{

  while (true)
  {
    if (digitalRead(PLAYER1_DOWN))
    {
      player_1.down();
    }
    if (digitalRead(PLAYER1_UP))
    {
      player_1.up();
    }
    if (digitalRead(PLAYER2_DOWN))
    {
      player_2.down();
    }
    if (digitalRead(PLAYER2_UP))
    {
      player_2.up();
    }

    vTaskDelay(pdMS_TO_TICKS(20));
  }
}

void physics_task(void *pvParameters)
{
  while (true)
  {
    ball.x += ball.dx;
    ball.y += ball.dy;
    if (ball.dx > 0)
    {
      checkCollisionRight(player_2);
    }
    else
    {
      checkCollisionLeft(player_1);
    }
    if (ball.y < 0 || ball.y > SCREEN_H)
    {
      ball.dy = -ball.dy;
    }
    else if (ball.x < 0 || ball.x > SCREEN_W)
    {

      startGame();
    }

    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void render_task(void *pvParameters)
{

  while (true)
  {
    u8g2.clearBuffer();
    u8g2.drawBox(player_1.x, player_1.y, player_1.width, player_1.length);
    u8g2.drawBox(player_2.x, player_2.y, player_2.width, player_2.length);
    u8g2.drawBox(ball.x, ball.y, BALL_SIZE, BALL_SIZE);
    u8g2.sendBuffer();
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(PLAYER1_DOWN, INPUT);
  pinMode(PLAYER1_UP, INPUT);
  pinMode(PLAYER2_DOWN, INPUT);
  pinMode(PLAYER2_UP, INPUT);
  u8g2.begin();
  startGame();
  xTaskCreate(input_task, "Input", 2048, NULL, 1, NULL);
  xTaskCreate(render_task, "Render", 2048, NULL, 1, NULL);
  xTaskCreate(physics_task, "Physic", 2048, NULL, 1, NULL);
}

void loop()
{
}