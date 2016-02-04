#define ST7735_BLACK   0x0000
#define ST7735_GRAY    0x8410
#define ST7735_WHITE   0xFFFF
#define ST7735_RED     0xF800
#define ST7735_ORANGE  0xFA60
#define ST7735_YELLOW  0xFFE0  
#define ST7735_LIME    0x07FF
#define ST7735_GREEN   0x07E0
#define ST7735_CYAN    0x07FF
#define ST7735_AQUA    0x04FF
#define ST7735_BLUE    0x001F
#define ST7735_MAGENTA 0xF81F
#define ST7735_PINK    0xF8FF

void drawtext(int x, int y, const char *text, uint16_t color, int size)
{
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setTextSize(size);
  tft.setTextWrap(true);
  tft.print(text);
}

void displayOn(bool on)
{
  analogWrite(TFT_BACKLIGHT_PIN, on ? 255 : 0);
}

void clearScreen()
{
  tft.fillScreen(ST7735_BLACK);
}

void trace(const char* message)
{
  //tft.fillScreen(ST7735_BLACK);
  //drawtext(0, 0, message, ST7735_WHITE, 1);
#ifdef USE_SOFT_SERIAL
  softSerial.println(message);
#endif
}


#define SHOW_DURATION 2000

void displayTimes(JsonArray& array)
{
  if (array.success() && array.size() > 0)
  {
    for (int j = 0; j < 2; j++)
    {
      for (int i = 0; i < array.size(); i++)
      {
        const JsonObject& o = array[i];
        clearScreen();
        drawtext(110, 10, o["l"], ST7735_GREEN, 4);
        drawtext(5, 35, o["to"], ST7735_GREEN, 1);
        int minutes = o["in"];
        drawtext(0, 70, String(minutes).c_str(), minutes <= 3 ? ST7735_ORANGE : ST7735_BLUE, 7);
        drawtext(75, 105, "Minuten", ST7735_GREEN, 2);
        
        delay(SHOW_DURATION);
      }
    }
    //
    // overview
    //
    clearScreen();
    int y = 5;
    for (int i = 0; i < array.size(); i++)
    {
      const JsonObject& o = array[i];
      drawtext(5, y, o["l"], ST7735_GREEN, 1);
      drawtext(25, y, o["to"], ST7735_GREEN, 1);
      int minutes = o["in"];
      drawtext(5, y + 10, String(minutes).c_str(), minutes <= 3 ? ST7735_ORANGE : ST7735_BLUE, 1);
      drawtext(25, y + 10, "Minuten", ST7735_GREEN, 1);
      y += 30;
    }    
  }
  else
  {
    clearScreen();      
    drawtext(random(80), random(50), ":-(", ST7735_GREEN, 2);
  }
}
