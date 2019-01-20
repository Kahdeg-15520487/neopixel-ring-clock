#include <RTClib.h>
#include <RealTimeClockDS1307.h>
const int DS1307 = 0x68; // Address of DS1307 see data sheets

#include <WiFi.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// Replace with your network credentials
const char* ssid     = "Vinhtat";
const char* password = "731997vinh";

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

#include <Adafruit_NeoPixel.h>

#define PIN    23    // Digital IO pin connected to the NeoPixels.

#define NUM_LEDS 12

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_RGB + NEO_KHZ800);

byte second = 0;
byte minute = 0;
byte hour = 0;
int day = 0;
byte weekday = 0;
int month = 0;
int year = 0;

int pixelHour = 0;
int pixelMinute = 0;
int pixelSecond = 0;

void wipePixel();
void setPixelHour(int h);
void setPixelMinute(int m);
void setPixelSecond(int s);

void Get_time();
byte bcdToDec(byte val);

void setup() {
  strip.begin();
  Serial.begin(115200);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

// Initialize a NTPClient to get time
  timeClient.begin();
  timeClient.setTimeOffset(25200);
  

  if(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  
  day = timeClient.getDate();
  year = timeClient.getYear();
  month = timeClient.getMonth();

  hour = timeClient.getHours();
  minute = timeClient.getMinutes();
  second = timeClient.getSeconds();

  RTC.switchTo24h();
  RTC.setHours(hour);
  RTC.setMinutes(minute);
  RTC.setSeconds(second);
  RTC.setYear(year);
  RTC.setMonth(month);
  RTC.setDay(day);
}

void loop() {

  Get_time();

  Serial.print(hour);
  Serial.write(':');
  Serial.print(minute);
  Serial.write(':');
  Serial.print(second);
  Serial.println();

  setPixelHour(hour);
  setPixelMinute(minute);
  setPixelSecond(second);
  wipePixel();
  strip.show();
  Serial.print(pixelHour);
  Serial.print('-');
  Serial.print(pixelMinute);
  Serial.print('-');
  Serial.print(pixelSecond);
  Serial.println();
  delay(500);
}

void wipePixel(){
  for(int i=0;i<NUM_LEDS;i++){
    if(i != pixelHour
    && i != pixelMinute
    && i != pixelSecond){ 
      strip.setPixelColor(i,strip.Color(0,0,0));
    }
  }
}

void setPixelHour(int h){
  pixelHour = (h+3) % 12;
  strip.setPixelColor(pixelHour, strip.Color(150,0,0));
}

void setPixelMinute(int m){
  pixelMinute = ((m/5)+3) % 12;
  uint8_t r = 0;
  if(pixelMinute == pixelHour){
    r = (strip.getPixelColor(pixelMinute) & 0xff0000) >> 16;
    //Serial.println(strip.getPixelColor(pixelMinute) & 0xff0000);
  }  
  strip.setPixelColor(pixelMinute,strip.Color(r,150,0));
}

void setPixelSecond(int s){
  pixelSecond = ((s/5)+3) % 12;
  uint8_t r = 0;
  uint8_t g = 0;
  uint32_t color = 0;
  if (pixelSecond == pixelHour){
    color = strip.getPixelColor(pixelSecond);
    //Serial.println(color & 0xff0000);
    r = (color & 0xff0000) >> 16;
  }  
  if (pixelSecond == pixelMinute){
    color = strip.getPixelColor(pixelSecond);
    //Serial.println(color & 0x00ff00);
    g = (color & 0x00ff00) >> 8;
  }
  //int lower = pow(5.0f,floor(logg(5,s)));
  //Serial.println(s-lower);
  //strip.setPixelColor(pixelSecond,strip.Color(r,g,map(s-lower,0,5,0,255)));
  strip.setPixelColor(pixelSecond,strip.Color(r,g,150));
}

float logg(float base, float n){
  return log(n) / log(base);
}

void Get_time(){
  Wire.beginTransmission(DS1307);
  Wire.write(byte(0));
  Wire.endTransmission();
  Wire.requestFrom(DS1307, 7);
  second = bcdToDec(Wire.read());
  minute = bcdToDec(Wire.read());
  hour = bcdToDec(Wire.read());
  weekday = bcdToDec(Wire.read());
  day = bcdToDec(Wire.read());
  month = bcdToDec(Wire.read());
  year = bcdToDec(Wire.read());
}

byte bcdToDec(byte val) {
  int msbdec=val/16*10; //gets decimel msb of BCD value(4 bits)
  int lsbdec = val%16;//gets decimel lsb of BCD value(4 bits)
  int total=msbdec+lsbdec;
  return (total);
    //return ((val/16*10) + (val%16));
}
