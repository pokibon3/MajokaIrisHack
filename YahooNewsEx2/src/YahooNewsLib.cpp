// ###########################################################
//
//      Yahoo! News取得ライブラリ
//
//        V1.0  2021/01/30 YahooNews電光掲示板からファイル分離
//
// ###########################################################
#include <Arduino.h>
#include <string.h>
#include <WiFiClientSecure.h>
#include "private.hpp"
#include "YahooNewsLib.hpp"

//=======================================================================
//  https_Web_Get() : news取得
//=======================================================================
String https_Web_Get(const char* host1, String target_page, char char_tag, String Final_tag, String Begin_tag, String End_tag, String Begin_tag2, String End_tag2, String Paragraph){
 
  String ret_str;
   
  WiFiClientSecure https_client;
  https_client.setCACert(yahoo_root_ca); //Yahooサイトのルート証明書をセットする
   
  if (https_client.connect(host1, 443)){
    Serial.print(host1); Serial.print(F("-------------"));
    Serial.println(F("connected"));
    Serial.println(F("-------WEB HTTPS GET Request Send"));
 
    String str1 = String("GET https://") + String( host1 ) + target_page + " HTTP/1.1\r\n";
           str1 += "Host: " + String( host1 ) + "\r\n";
           str1 += "User-Agent: BuildFailureDetectorESP32\r\n";
           str1 += "Connection: close\r\n\r\n"; //closeを使うと、サーバーの応答後に切断される。最後に空行必要
           str1 += "\0";
 
    https_client.print(str1); //client.println にしないこと。最後に改行コードをプラスして送ってしまう為
    https_client.flush(); //client出力が終わるまで待つ
    Serial.print(str1);
    Serial.flush(); //シリアル出力が終わるまで待つ
     
  }else{
    Serial.println(F("------connection failed"));
  }
 
  if(https_client){
    String dummy_str;
    uint16_t from, to;
    Serial.println(F("-------WEB HTTPS Response Receive"));
 
    while(https_client.connected()){
      while(https_client.available()) {
        if(dummy_str.indexOf(Final_tag) == -1){          
          dummy_str = https_client.readStringUntil(char_tag);
 
          if(dummy_str.indexOf(Begin_tag) >= 0){
            from = dummy_str.indexOf(Begin_tag) + Begin_tag.length();
            to = dummy_str.indexOf(End_tag);
            ret_str += Paragraph;
            ret_str += dummy_str.substring(from,to);
//            ret_str += "  ";
          }
          if(dummy_str.indexOf(Begin_tag2) >= 0){
            from = dummy_str.indexOf(Begin_tag2) + Begin_tag2.length();
            to = dummy_str.indexOf(End_tag2);
//            ret_str += "：　";
            ret_str += "：";
            ret_str += dummy_str.substring(from,to);
            ret_str += "  ";
          }
        }else{
          while(https_client.available()){
            https_client.read(); //サーバーから送られてきた文字を１文字も余さず受信し切ることが大事
            //delay(1);
          }
          delay(10);
          https_client.stop(); //特に重要。コネクションが終わったら必ず stop() しておかないとヒープメモリを食い尽くしてしまう。
          delay(10);
          Serial.println(F("-------Client Stop"));
 
          break;
        }
        //delay(1);
      }
      //delay(1);
    }
  }
   
  ret_str += "\0";
  ret_str.replace("&amp;","&"); //XMLソースの場合、半角&が正しく表示されないので、全角に置き換える
  ret_str.replace("&#039;","\'"); //XMLソースの場合、半角アポストロフィーが正しく表示されないので置き換える
  ret_str.replace("&#39;","\'"); //XMLソースの場合、半角アポストロフィーが正しく表示されないので置き換える
  ret_str.replace("&apos;","\'"); //XMLソースの場合、半角アポストロフィーが正しく表示されないので置き換える
  ret_str.replace("&quot;","\""); //XMLソースの場合、ダブルクォーテーションが正しく表示されないので置き換える
   
  if(ret_str.length() < 20) ret_str = "※ニュース記事を取得できませんでした";
   
  if(https_client){
    delay(10);
    https_client.stop(); //特に重要。コネクションが終わったら必ず stop() しておかないとヒープメモリを食い尽くしてしまう。
    delay(10);
    Serial.println(F("-------Client Stop"));
  }
  Serial.flush(); //シリアル出力が終わるまで待つ
 
  return ret_str;
}

//=======================================================================
//   WiFi control
//=======================================================================
void wifiConnect(void) {
  delay(100);                   // wait for Wifi module

  Serial.println();
  Serial.print("Attempting to connect to SSID: ");
  Serial.println(ssid);
  WiFi.disconnect(true, true);  // おまじない
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.println(WiFi.localIP());
  delay(100);  
}

void wifiDisconnect(void) {

  delay(100);                   // wait for Wifi module
  WiFi.disconnect(true, true);  // おまじない
  delay(100);  
}
 