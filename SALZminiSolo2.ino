// SALZminiSolo2 bbd
// for Atom Lite

#include "M5Atom.h"

//デモモード(ON:1, OFF:0)
//#define DEMO_MODE 1
#define DEMO_MODE 0

#define P(col, row)   (((row) * 5) + (col))
#define RGB(r, g, b)  (((g) << 16) + ((r) << 8) + (b))

#define SECONDS(s)    ((s) * 1000)
#define MINUTES(m)    SECONDS(m * 60)

const int WAIT_TIME = (DEMO_MODE) ? SECONDS(10) : MINUTES(20);
const int PUMP_TIME = (DEMO_MODE) ? SECONDS(10) : MINUTES(2);

#define INPUT_PIN 32
#define PUMP_PIN 26

static int pValue = 0;

#define NCOLORS 5
static int colorRs[NCOLORS] = { 0x60, 0x50, 0x30, 0x00, 0x00 };
static int colorGs[NCOLORS] = { 0x00, 0x10, 0x30, 0x50, 0x00 };
static int colorBs[NCOLORS] = { 0x00, 0x00, 0x00, 0x10, 0x60 };
static int delays[NCOLORS] = { 2, 4, 8, 16, 32 };


void
DispLevel(int lv) {
  pValue = lv;
}

int
WlRate(int wl)
{
  //校正済の水分％を返す。
  //センサーを水に浸けた状態：1400
  //センサーを外に出した状態：2100
  const int wetWl = 1400;
  const int dryWl = 2100;
  int ret = (int)((1.0 - ((float)(wl - wetWl) / (float)(dryWl - wetWl))) * 100.0);

  return (ret < 0) ? 0 : ((ret > 99) ? 99 : ret);
}


int
GetWl(void)
{
  float val = 0.0;
  pinMode(INPUT_PIN, INPUT);
  for (int i = 0; i < 10; i++) {
    float v = analogRead(INPUT_PIN);
    //Serial.printf("ML(%d):%8.3f\r\n", i, v);
    val += v;
    delay(10);
  }
  return (int)(val / 10.0);
}

void
PrintData(int wl) {
  static int c = 0;

  c++;
  Serial.printf("SendData():\n");
  Serial.printf("1:waterLevel:%d\n", wl);
  Serial.printf("2:c:%d\n", c);
}

int
GetDispLevel(void)
{
  int ret;
  ret = pValue / 10;
  ret = (ret > 4) ? 4 : ret;
  return ret;
}

//---------------------------------------

void task1(void * pvParameters)
{
  int a = 0;
  int rPrev = 0;
  int gPrev = 0;
  int bPrev = 0;
  while (1) {
    int lv = GetDispLevel();
    a = (a < 360) ? ++a : 0;
    float v = (sin(PI * (float)a / 180.0) + 1.0) / 2.0;
    int r = (float)colorRs[lv] * v;
    int g = (float)colorGs[lv] * v;
    int b = (float)colorBs[lv] * v;

    if (r != rPrev || g != gPrev || b != bPrev) {
      M5.dis.drawpix(0, RGB(r, g, b));
    }
    delay(delays[lv]);

    rPrev = r;
    gPrev = g;
    bPrev = b;
  }
}

void setup()
{
  Serial.begin(115200);
  M5.begin(true, false, true);

  pinMode(INPUT_PIN, INPUT);
  pinMode(PUMP_PIN, OUTPUT);

  M5.dis.drawpix(0, RGB(0x00, 0x00, 0x00));
  
  // Task 1
  xTaskCreatePinnedToCore(
                  task1,     /* Function to implement the task */
                  "task1",   /* Name of the task */
                  4096,      /* Stack size in words */
                  NULL,      /* Task input parameter */
                  1,         /* Priority of the task */
                  NULL,      /* Task handle. */
                  0);        /* Core where the task should run */
}

void loop()
{
  int wl = WlRate(GetWl());
  DispLevel(wl);
  PrintData(wl);
  Serial.print("wl:");
  Serial.println(wl);

  if (wl < 30) {
    digitalWrite(PUMP_PIN, true);
    Serial.println("PUMP ON");
    delay(PUMP_TIME);
    digitalWrite(PUMP_PIN, false);
    Serial.println("PUMP OFF");
  }
  else
    delay(WAIT_TIME);
}
