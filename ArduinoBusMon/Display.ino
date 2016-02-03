
void drawtext(int x, int y, const char *text, uint16_t color, int size)
{
  tft.setCursor(x, y);
  tft.setTextColor(color);
  tft.setTextSize(size);
  tft.setTextWrap(true);
  tft.print(text);
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


#define SHOW_DURATION 2500

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
        drawtext(0, 70, o["in"], ST7735_BLUE, 7);
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
      drawtext(5, y + 10, o["in"], ST7735_BLUE, 1);
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
