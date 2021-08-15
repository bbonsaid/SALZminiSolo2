// SALZminiSolo2 bbd
// for Atom Lite

#include "M5Atom.h"

#define RGB(r, g, b)  (((g) << 16) + ((r) << 8) + (b))

static int pValue = 0;

#define NCOLORS 5
static int colorRs[NCOLORS] = { 0x60, 0x50, 0x30, 0x00, 0x00 };
static int colorGs[NCOLORS] = { 0x00, 0x10, 0x30, 0x50, 0x00 };
static int colorBs[NCOLORS] = { 0x00, 0x00, 0x00, 0x10, 0x60 };
static int delays[NCOLORS] = { 2, 4, 8, 16, 32 };

void task1(void * pvParameters)
{
  int a = 0;
  int rPrev = 0;
  int gPrev = 0;
  int bPrev = 0;
  while (1) {
    int lv = pValue / 20;
    a = (a < 360) ? ++a : 0;
    float v = (sin(PI * (float)a / 180.0) + 1.0) / 2.0;
    int r = (float)colorRs[lv] * v;
    int g = (float)colorGs[lv] * v;
    int b = (float)colorBs[lv] * v;

    char buf[256];
    sprintf(buf, "a:%3d lv:%d RGB(%02X, %02X, %02X)", a, lv, r, g, b);
    Serial.println(buf);

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
  M5.begin(true, false, true);
  delay(10);
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
  if (M5.Btn.wasPressed()) {
    pValue = ((pValue += 10) < 100) ? pValue : 0;
  }

  delay(10);
  M5.update();
}
