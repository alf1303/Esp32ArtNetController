#include <Arduino.h>
#include <FastLED.h>
#include <UIPEthernet.h>

#define NUM_LEDS 240
#define UNIVERSE_SIZE 120
#define START_UNIVERSE 0
#define ARTNET_PORT 6454
const uint8_t universesCount = NUM_LEDS/UNIVERSE_SIZE;

EthernetUDP udp;
uint8_t mymac[] = { 0x74, 0x69, 0x69, 0x2D, 0x30, 0x31 };
uint8_t uniData[512];
uint8_t headerData[18]; //artnetHeader
bool universesReceived[universesCount]; //mark received universe
bool send; //flag, allowing to show when all universes received
uint32_t syncmax,sync;

CRGB leds[NUM_LEDS];

void setup() {
  Serial.begin(115200);
  delay(10);
  FastLED.addLeds<NEOPIXEL, 21>(leds, 0, UNIVERSE_SIZE);
  FastLED.addLeds<NEOPIXEL, 15>(leds, UNIVERSE_SIZE, UNIVERSE_SIZE);
  // FastLED.addLeds<NEOPIXEL, 25>(leds, 2*UNIVERSE_SIZE, UNIVERSE_SIZE);
  // FastLED.addLeds<NEOPIXEL, 25>(leds, 3*UNIVERSE_SIZE, UNIVERSE_SIZE);
  // FastLED.addLeds<NEOPIXEL, 33>(leds, 4*UNIVERSE_SIZE, UNIVERSE_SIZE);
  // FastLED.addLeds<NEOPIXEL, 21>(leds, 5*UNIVERSE_SIZE, UNIVERSE_SIZE);
  Ethernet.begin(mymac, IPAddress(192,168,11,6));
  udp.begin(ARTNET_PORT);
  syncmax=(1<<universesCount)-1;
  printf("setup syncmax: %d, univerCount: %d\n", syncmax, universesCount);
  sync=0;
}

void readUDP() {
 // bool send = 1;
  if(udp.parsePacket()) {
    udp.read(headerData, 18);
         //if ( headerData[0] == 'A' && headerData[4] == 'N') {
         int uniSize = (headerData[16] << 8) + (headerData[17]);
          uint8_t universe = headerData[14];
          int offset = (universe - START_UNIVERSE)*UNIVERSE_SIZE*3;
          //if the colors are not correct change I can make a quick change
         udp.read((uint8_t *)(leds+offset), UNIVERSE_SIZE*3);
        
         printf("uni: %d, size: %d", universe, uniSize);
         /*
           for(int i = 0; i < UNIVERSE_SIZE; i++) {
             leds[offset + i].red = uniData[i*3];
             leds[offset + i].green = uniData[i*3 + 1];
             leds[offset + i].blue = uniData[i*3 + 2];
           }*/
         /*  universesReceived[universe-START_UNIVERSE] = 1;
         for(int j = 0; j < universesCount; j++) {
           if(universesReceived[j] == 0) {
             send = 0;
             break;
           }
         }*/
         
         sync=sync | (1<<(universe - START_UNIVERSE));
         printf(", sync: %d, syncMax: %d\n", sync, syncmax);
        /* the code below is to avoid frame mix */
        /* comment the code below if it's not too important for you*/
         if(universe==START_UNIVERSE)
         {
          sync=1;
         }
          /* end of code **/
         if(sync==syncmax) {
           FastLED.show();
           //memset(universesReceived, 0, universesCount);
           sync=0;
         }
       // }    
  }
}

void loop() {
  readUDP();
}