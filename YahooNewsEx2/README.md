# マジョカアイリス　Yahoo! News 電光表示板

# はじめに
　最近はやりのマジマジョピュアーズ！マジョカアイリスの細長液晶をESP32でコントロールし、Yahoo! Newsを流してみました。


# ESP32との接続
マジョカアイリスの細長LCDは、0.5mm Pitchの22Pinのフラットケーブルケーブルで引き出されていますので、これをESP32するために、aitendoの★0.5/22P★FFCコネクタwith基板 [SMT0.5-22P]等を使って接続します。
https://www.aitendo.com/product/14181

LCDのLEDバックライトは、20mAの定電流電源が必要ですので、PT4103等のLEDドライバが必要となります。推奨はしませんが、5.5～5.8V程度に昇圧した定電圧電源でも使用可能と思います。

    1   - GND   [to GND]
    2   - RST   [to ESP32 - 32]     // ESP32 のGPIO32に対応（以下同様）
    3   - GND   [to GND]
    4   - D0    [to ESP32 - 12]     // PCB上のL10(150Ω)に接続
    5   - D1    [to ESP32 - 13]     // PCB上のL11(150Ω)に接続
    6   - D2    [to ESP32 - 26]     // PCB上のL12(150Ω)に接続
    7   - D3    [to ESP32 - 25]     // PCB上のL13(150Ω)に接続
    8   - D4    [to ESP32 - 17]     // PCB上のL14(150Ω)に接続
    9   - D5    [to ESP32 - 16]     // PCB上のL15(150Ω)に接続
    10  - D6    [to ESP32 - 27]     // PCB上のL16(150Ω)に接続
    11  - D7    [to ESP32 - 14]     // PCB上のL17(150Ω)に接続
    12  - CS2   [to GND]            // LCD Right Side CS
    13  - RD    [to ESP32 - 2]      // PCB上のL5 (150Ω)に接続
    14  - WR    [to ESP32 - 4]      // PCB上のL6(1kΩ)に接続
    15  - CS1   [to ESP32 - 33]     // PCB上のL23(150Ω)に接続
    16  - DC    [to ESP32 - 15]     // PCB上のL24(1kΩ)に接続
    17  - TE    [OPEN]
    18  - VDD   [to 3.3V]           // VDDIOと直結
    19  - VDDIO [to 3.3V]           // データシート上は3.3VがMAX
    20  - LED+  [to 5.5V~]          // 20mAの定電流電源が推奨
    21  - LED-  [to GND]            // 本来はシャント抵抗に接続
    22  - GND   [to GND]

# ソースコード

## Yahoo!News取得
ESP32でのYahoo！Newsの取得は、@minwinminさんの以下の記事を参考にしています。
変更点は、Yahoo!Newsのタイトルだけでなく本文も取得しているところぐらいで、ほぼまるパクリ使用させていただきました。
使用に当たっては、private.hppのWiFiアクセスポイント情報のと、Yahoo! Japan RSSサイトの証明書情報を更新する必要があります。証明書の取得方法も以下の記事のリンクを参考させていただきました。

### ESP32からWiFiClientSecureをつかってHTTPS GETをする方法をちょっと理解した
https://qiita.com/minwinmin/items/55ba92edac3e1ec9f602

## News表示
取得したNewsはString型に格納されています。これをLCDに表示させるのに@LovyanさんのLovyanGFX@0.3.8 を利用させていただいています。
NewsはUTF-8のマルチバイト文字をハンドリングする必要がありますが、LovyanGFXのマルチバイト文字のでコード機能をうまく使うことにより、UTF-8をあまり意識せずに使用することができます。

# 開発環境
リポジトリのソースの開発環境は、VS CODEのPIOを使用しています。Arduino IDEでは、main.cppをmain.inoにリネームすることで、ビルドできると思います。<BR>
Arduino環境では、ツールメニューから以下を設定してください。<BR>
- ボード：ESP32 Dev Module<BR>
- Partition Scheme：No OTA(2MB APP/2MB SPIFFS)：
(スケッチエリアを拡大する必要があります。)
- Arduino環境で動作確認したバージョンは以下になります。
    - Arduino IDE 18.19
    - ESP32 Board Manager by Espressif Systems 1.0.6
    - LovyanGFX 0.3.8
- Platformio環境についてはplatformio.iniを参照してください。

# 来歴

- V0.1  2021/01/11 新規作成
- V0.5  2021/01/13 スクロール右端改善
- V1.0  2021/01/14 40/48dot Font Support
- V1.1  2021/01/29 Refactoring(UTF-8処理見直し)
- V1.1a 2021/03/21 README.md見直し(Arduino注意事項追記)
- V1.2  2021/03/26 テキスト/背景色不正を修正
- V1.3  2024/11/26 文字色変更、表示をループ化
