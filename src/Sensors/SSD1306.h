#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <Fonts/Picopixel.h>

#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define charHeight 7
#define charWidth 3

const int SCREEN_WIDTH = 128; // OLED display width, in pixels
const int SCREEN_HEIGHT = 64; // OLED display height, in pixels
const int OLED_RESET = -1; // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
const uint8_t menuCount = 12;
const String catText[menuCount] = {"Faces:", "Bright:", "Accent:", "Mic:", "MicLevel:", "Boop:", "Spectrum:", "FaceSize:", "Color:", "HueF:", "HueB:", "EffectS"};

class SSD1306{
public:
  String lineText[menuCount];
  // Initialize the display
  void Begin(uint8_t maxSettings[]) {
    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
      Serial.print("SSD1306 allocation failed");
      for(;;); // Don't proceed, loop forever
    }
    display.display();
    delay(200);
    display.setTextWrap(false);
    display.setFont(&Picopixel);
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.clearDisplay();
    wasBlinked = false;
  }

  // Clear the display
  void Clear() {
    display.clearDisplay();
  }

  void Display(){
    display.display();
  }

  void MenuLine(uint8_t currentSetting, uint8_t max){
    settingCount = max == 2 ? "Off On" : String(NumberLine(max));
    lineText[currentSetting]=catText[currentSetting]+settingCount;
    PrintLine(currentSetting, catText[currentSetting]+settingCount);
    display.display();
  }

  void SettingValueUnderline(uint8_t currentSetting, uint8_t value, uint8_t color){
    uint8_t charPos = 0;
    value = value > 9 ? 64+value-9 : value+48;
    if ((currentSetting == 3) || (currentSetting == 5) || (currentSetting == 6)){
      charPos = GetCharPos(lineText[currentSetting], "O");
      charPos = value == 48 ? charPos : charPos+10;
      display.drawLine(charPos-1,(currentSetting+1)*charHeight-1,charPos+10-1, (currentSetting+1)*charHeight-1, color);
    }
    else{
      charPos = GetCharPos(lineText[currentSetting], String(char(value)));
      display.drawLine(charPos-1,(currentSetting+1)*charHeight-1,charPos+PicopixelGlyphs[value-32].xAdvance, (currentSetting+1)*charHeight-1, color);
    }
  }

  void SettingUnderline(uint8_t currentSetting, uint8_t color){
    display.drawLine(0,(currentSetting+1)*charHeight-1,GetCharPos(lineText[currentSetting], ":"), (currentSetting+1)*charHeight-1, color);
  }

  uint8_t GetCharPos(String text, String value){
    uint8_t pixelLength = 0;
    uint8_t textLength = text.length();
    for (int i=0;i<textLength;i++){
      if (text[i] == value[0] && (value.length()==1 || text[i+1] == value[1])) break;
      pixelLength += PicopixelGlyphs[int(text[i])-32].xAdvance;
    }
    return pixelLength;
  }

  void SetBlinkTime(uint16_t timeToBlink){
    blinkTime = timeToBlink;
  }

  void BlinkCurrentSetting(uint8_t currentSetting, uint8_t value){
    bool blink = millis() % (blinkTime * 2) > blinkTime;
    if (blink && !wasBlinked){ 
      SettingValueUnderline(currentSetting, value, SSD1306_BLACK);
      display.display();
      wasBlinked = true;
    }
    else if(!blink && wasBlinked){
      SettingValueUnderline(currentSetting, value, SSD1306_WHITE);
      display.display();
      wasBlinked = false;
    }
  }

  // Print text to a specific line on the display
  void PrintLine(int line, String text) {
    display.setCursor(0, line*charHeight+5); // Set the text position
    display.print(text);
  }

private:
  String settingCount;
  uint8_t settingLen;
  uint16_t blinkTime;
  bool wasBlinked;

  String NumberLine(uint8_t count){
    String numbers = "";
    for (int i=0; i<count; i++){
      if (i<10)
        numbers += i;
      else
        numbers += String(char(64+i-9));
    }
    return numbers;
  }
};