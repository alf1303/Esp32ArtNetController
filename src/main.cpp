#include <Arduino.h>
#include <FastLED.h>
#include <UIPEthernet.h>
#include <artnetESP32/ArtnetESP32.h>
//#define NUM_LEDS 720
#define NUM_LEDS 720
#define UNIVERSE_SIZE 120
#define START_UNIVERSE 0
#define ARTNET_PORT 6454
const uint8_t universesCount = NUM_LEDS/UNIVERSE_SIZE;
ArtnetESP32 artnet;

EthernetUDP udp;
uint8_t mymac[] = { 0x74, 0x69, 0x69, 0x2D, 0x30, 0x31 };
uint8_t uniData[512];
uint8_t headerData[18]; //artnetHeader

CRGB leds[NUM_LEDS];

void displayfunction()
{
  if (artnet.frameslues%100==0)
   Serial.printf("nb frames read: %d  nb of incomplete frames:%d lost:%.2f %%\n",artnet.frameslues,artnet.lostframes,(float)(artnet.lostframes*100)/artnet.frameslues);
   //here the buffer is the led array hence a simple FastLED.show() is enough to display the array
   FastLED.show();
}

void connectWifi() {
    WiFi.mode(WIFI_STA);
    Serial.printf("Connecting ");
    WiFi.begin("udp", "esp18650");
    while (WiFi.status() != WL_CONNECTED) {
      Serial.println(WiFi.status());
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void connectEthernet() {
  Ethernet.begin(mymac, IPAddress(192,168,11,6));
  // udp.begin(ARTNET_PORT);
}

void setup() {
  Serial.begin(115200);
  delay(10);   
  
  connectWifi();
  // connectEthernet();

  FastLED.addLeds<NEOPIXEL, 15>(leds, 0, UNIVERSE_SIZE);
  FastLED.addLeds<NEOPIXEL, 27>(leds, UNIVERSE_SIZE, UNIVERSE_SIZE);
  FastLED.addLeds<NEOPIXEL, 26>(leds, 2*UNIVERSE_SIZE, UNIVERSE_SIZE);
  FastLED.addLeds<NEOPIXEL, 25>(leds, 3*UNIVERSE_SIZE, UNIVERSE_SIZE);
  FastLED.addLeds<NEOPIXEL, 33>(leds, 4*UNIVERSE_SIZE, UNIVERSE_SIZE);
  FastLED.addLeds<NEOPIXEL, 21>(leds, 5*UNIVERSE_SIZE, UNIVERSE_SIZE);
   artnet.setFrameCallback(&displayfunction); //set the function that will be called back a frame has been received
   artnet.setLedsBuffer((uint8_t*)leds); //set the buffer to put the frame once a frame has been received
   artnet.begin(NUM_LEDS,UNIVERSE_SIZE); //configure artnet
}

void readUDP() {
  artnet.readFrame();
}

void loop() {
  readUDP();
}