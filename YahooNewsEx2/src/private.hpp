// ###########################################################
//
//      private.hpp : YahooNews取得用WiFi Setting & 証明書
//
//        V1.0  2021/01/30 YahooNews電光掲示板からファイル分離
//
//      (1) ssid, およびpasswordは自分のWiFi環境のものを設定する
//      (2) Yahoo! Japan RSSニュース、ルート証明書は、以下のURL
//          の記事を参考に設定する。
//
//   https://www.mgo-tec.com/blog-entry-arduino-esp32-ssl-stable-root-ca.htm
//
// ###########################################################
//=======================================================================
const char* ssid      = "xxxxxxxx";          //自分のルーターのSSID
const char* password  = "xxxxxxxx";      //自分のルーターのパスワード
//=======================================================================
//=======================================================================
//  Yahoo! Japan RSSニュース、ルート証明書 
//      chrome等で取得しフォーマットを下記の様に変換
//=======================================================================
const char* yahoo_root_ca= \
  "-----BEGIN CERTIFICATE-----\n" \
  "-----END CERTIFICATE-----\n";