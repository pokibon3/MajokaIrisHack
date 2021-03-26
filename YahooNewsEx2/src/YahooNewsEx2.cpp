// ###########################################################
//
//      マジョカアイリス　Yahoo! News 電光表示板
//
//        V0.1  2021/01/11 by Kimio Ohe
//        V0.5  2021/01/13 スクロール右端改善
//        V1.0  2021/01/14 40dot Font Support
//        V1.1  2021/01/29 Refactoring
//        V1.2  2021/03/26 テキスト色不正を修正
// ###########################################################
//
//      マジョカアイリス 640*48px 細長LCD 動作テストコード
//
//      Hideto Kikuchi / PJ (@pcjpnet) - http://pc-jp.net/
//
// ###########################################################
//
//
//      Example Code for ESP32 Dev Module
//      Require LovyanGFX 0.3.8 Library
//
//
// ####################  LCD Pin Assignments  ####################
//  
//  0.5mm Pitch - 22Pin
//  
//  1   - GND   [to GND]
//  2   - RST   [to ESP32 - 32]     // ESP32 のGPIO32に対応（以下同様）
//  3   - GND   [to GND]
//  4   - D0    [to ESP32 - 12]     // PCB上のL10(150Ω)に接続
//  5   - D1    [to ESP32 - 13]     // PCB上のL11(150Ω)に接続
//  6   - D2    [to ESP32 - 26]     // PCB上のL12(150Ω)に接続
//  7   - D3    [to ESP32 - 25]     // PCB上のL13(150Ω)に接続
//  8   - D4    [to ESP32 - 17]     // PCB上のL14(150Ω)に接続
//  9   - D5    [to ESP32 - 16]     // PCB上のL15(150Ω)に接続
//  10  - D6    [to ESP32 - 27]     // PCB上のL16(150Ω)に接続
//  11  - D7    [to ESP32 - 14]     // PCB上のL17(150Ω)に接続
//  12  - CS2   [to GND]            // LCD Right Side CS
//  13  - RD    [to ESP32 - 2]      // PCB上のL5 (150Ω)に接続
//  14  - WR    [to ESP32 - 4]      // PCB上のL6(1kΩ)に接続
//  15  - CS1   [to ESP32 - 33]     // PCB上のL23(150Ω)に接続
//  16  - DC    [to ESP32 - 15]     // PCB上のL24(1kΩ)に接続
//  17  - TE    [OPEN]
//  18  - VDD   [to 3.3V]           // VDDIOと直結
//  19  - VDDIO [to 3.3V]           // データシート上は3.3VがMAX
//  20  - LED+  [to 5.5V~]          // 20mAの定電流電源が推奨
//  21  - LED-  [to GND]            // 本来はシャント抵抗に接続
//  22  - GND   [to GND]
//
#include <Arduino.h>
#include <string.h>
#include "YahooNewsLib.hpp"

#define LCD_FREQ      15000000
#define FONT          &fonts::lgfxJapanMincho_40
#define FONT_SIZE     48                // 40 or 48

uint32_t WebGet_LastTime = 0;
#include <LovyanGFX.hpp>

//=======================================================================
//  LGFX COnfig
//=======================================================================
struct LGFX_Config {
  static constexpr int gpio_wr  =  4; //to LCD WR(14)
  static constexpr int gpio_rd  =  2; //to LCD RD(13)
  static constexpr int gpio_rs  = 15; //to LCD DC(16)
  static constexpr int gpio_d0  = 12; //to LCD D0(4)
  static constexpr int gpio_d1  = 13; //to LCD D1(5)
  static constexpr int gpio_d2  = 26; //to LCD D2(6)
  static constexpr int gpio_d3  = 25; //to LCD D3(7)
  static constexpr int gpio_d4  = 17; //to LCD D4(8)
  static constexpr int gpio_d5  = 16; //to LCD D5(9)
  static constexpr int gpio_d6  = 27; //to LCD D6(10)
  static constexpr int gpio_d7  = 14; //to LCD D7(11)
};
static lgfx::LGFX_PARALLEL<LGFX_Config> lcd;
static lgfx::Panel_ILI9342 panel;
static lgfx::LGFX_Sprite buf;       // [640 * 48] Buffer
static lgfx::LGFX_Sprite cBuf;      // ex [40 * 40] Buffer

inline void lcd_buffer_write();

//=======================================================================
//  setup()
//=======================================================================
void arduino_setup() {
  Serial.begin(115200); 
  WebGet_LastTime = 200000; //起動時に記事をGETするために、多めの数値で初期化しておく  
  //=====================================================================
  //  Setup LCD
  //=====================================================================
  panel.freq_write = LCD_FREQ;            // WiFiと同居時は8MHzに設定
  panel.freq_fill  = 27000000;
  panel.freq_read  = 16000000;
  panel.len_dummy_read_pixel = 8;

  panel.spi_cs = 33;  //to LCD CS(15)
  panel.spi_dc = -1;
  panel.gpio_rst = 32;  //to LCD RST(2)

  panel.gpio_bl  = -1;
  panel.pwm_ch_bl = -1;
  panel.backlight_level = true;

  panel.memory_width  = 320;
  panel.memory_height = 240;

  panel.panel_width  = 320;
  panel.panel_height = 96;

  panel.offset_x = 0;
  panel.offset_y = 144;

  panel.rotation = 0;

  panel.offset_rotation = 0;

  lcd.setPanel(&panel);

  lcd.init();
  lcd.setColorDepth(16);

  lcd.setRotation(0); // 0 or 2

  buf.setColorDepth(16);
  buf.createSprite(640, 48);
  cBuf.createSprite(FONT_SIZE, FONT_SIZE);
//  buf.setFont(&fonts::lgfxJapanGothic_40);        // 40ドットフォント設定
  cBuf.setFont(FONT);       // 40ドットフォント設定
  if (FONT_SIZE == 48) {
    cBuf.setTextSize(1.2, 1.2);
  }
  cBuf.setTextColor(TFT_WHITE, TFT_BLACK);
  cBuf.setTextWrap(false);
  buf.setScrollRect(0, (48 - FONT_SIZE) / 2, 640, FONT_SIZE);               // スクロール幅設定
}

//=======================================================================
//  main loop()
//=======================================================================
void arduino_loop() {
  String str;

  //=====================================================================
  // 180秒(3分)毎にYahoo Newsを取得
  //=====================================================================
  if((millis() - WebGet_LastTime) > 180000){        
    wifiConnect();
    str = https_Web_Get("news.yahoo.co.jp", "/rss/topics/top-picks.xml", '\n', "</rss>", "<title>", "</title>", "<description>", "</description>", "◇");
//    str = "　" + str + "　　　　　　　　　　　　　　　　　　　";
    str = str + "　　　　　　　　　　　　　　　　　　　";
    WebGet_LastTime = millis();
    wifiDisconnect();
  }

  //=====================================================================
  // Newsをスクロール表示
  //=====================================================================
  int32_t pStr = 0;
  int32_t cursor_x = 0;

  cBuf.setCursor(cursor_x, 0);                      // カーソル位置を初期化
  while (str[pStr] != '\0') {   
    // マルチバイト文字1文字を取り出し1文字バッファに表示
    cBuf.print(str[pStr]);                          // 1バイトずつ出力 (マルチバイト文字でもこの処理で動作します)
    cursor_x = cBuf.getCursorX();                   // 出力後のカーソル位置を取得
    pStr++;
    // 細長LCDの右端に1文字分表示
    if (cursor_x != 0) {                            // 表示する文字があるか？
      for (int i = 0; i < cursor_x; i++) {          // 文字幅分スクロール表示
        buf.scroll( -1, 0);                         // 1ドットスクロール
        cBuf.pushSprite(&buf, 639 - i, (48 - FONT_SIZE) / 2); // 1文字バッファをbuf右端に転送
        lcd_buffer_write();                          // LCDへデータ転送
      }
    }
    // 次の文字表示の準備
    cursor_x = 0;
    cBuf.setCursor(cursor_x, 0);                    // カーソル位置を初期化
  }
}

//=======================================================================
//  640*48px Buffer to 320*96px LCD
//=======================================================================
inline void lcd_buffer_write() {
  lcd.startWrite();
  lcd.setAddrWindow(0, 0, 320, 96);
  lcd.writePixels((lgfx::swap565_t*)buf.getBuffer(), 640 * 48);
  lcd.endWrite();
}