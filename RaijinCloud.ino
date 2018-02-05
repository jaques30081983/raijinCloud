/*
* ESP8266_NeoPixel.ino - Simple sketch to listen for E1.31 data on an ESP8266
*                        and drive WS2811 LEDs using the NeoPixel Library
*
* == Requires Adafruit_NeoPixel - http://github.com/adafruit/Adafruit_NeoPixel                        
*
* Project: E131 - E.131 (sACN) library for Arduino
* Copyright (c) 2015 Shelby Merrick
* http://www.forkineye.com
*
*  This program is provided free for you to use in any way that you wish,
*  subject to the laws and regulations where you are using it.  Due diligence
*  is strongly suggested before using this code.  Please give credit where due.
*
*  The Author makes no warranty of any kind, express or implied, with regard
*  to this program or the documentation contained in this document.  The
*  Author shall not be liable in any event for incidental or consequential
*  damages in connection with, or arising out of, the furnishing, performance
*  or use of these programs.
*
*/

#include <ESP8266WiFi.h>
#include <E131.h>
#define FASTLED_ESP8266_D1_PIN_ORDER
#include "FastLED.h"

#define NUM_LEDS 120 /* Number of pixels */
#define UNIVERSE 1      /* Universe to listen for */
#define CHANNEL_START 1 /* Channel to start listening at */
#define DATA_PIN 5      /* Pixel output - GPIO0 */

const char ssid[] = "";         /* Replace with your SSID */
const char passphrase[] = "";   /* Replace with your WPA2 passphrase */

E131 e131;

CRGB leds[NUM_LEDS];



int i_int = 0; //Intensity over all

int i_red_sky = 0; //Sky background color red
int i_green_sky = 0; //Sky background color green
int i_blue_sky = 0; //Sky background color blue
int i_max_int_sky = 25; //Sky background color blue


int i_red_flash = 0; //Flash color red
int i_green_flash = 0; //Flash color green
int i_blue_flash = 0; //Flash color blue

int i_kind_flash = 0; //Nature of flash
int i_energy_flash = 0; //Energy of flash

int i_pos_flash = 0; //Position of flash (random)
int i_rate_flash = 0; //Rate of flash (random)
int i_duration_flash = 0; //Duration of flash (random)


bool seq_run_1 = false;  
bool seq_run_2 = false; 
bool seq_run_3 = false; 
bool seq_run_4 = false; 

int previousMillisSeqOne = 0;

int seq_count_1 = 0;
                
int on_off_1 = 1;

void setup() {
    Serial.begin(115200);
    delay(10);

    /* Choose one to begin listening for E1.31 data */
    //e131.begin(ssid, passphrase);                       /* via Unicast on the default port */
    e131.beginMulticast(ssid, passphrase, UNIVERSE);  /* via Multicast for Universe 1 */


   /*Fastled init*/
   FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
}

void loop() {
  // Milliseconds since arduino is running  
  unsigned long currentMillis = millis();
  
    /* Parse a packet and update pixels */
    if(e131.parsePacket()) {
        if (e131.universe == UNIVERSE) {
            i_int = e131.data[CHANNEL_START - 1];
            
            i_red_sky = e131.data[CHANNEL_START];
            i_green_sky = e131.data[CHANNEL_START + 1];
            i_blue_sky = e131.data[CHANNEL_START + 2];
            
            i_red_flash = e131.data[CHANNEL_START + 3];
            i_green_flash = e131.data[CHANNEL_START + 4];
            i_blue_flash = e131.data[CHANNEL_START + 5];
            
            i_kind_flash = e131.data[CHANNEL_START + 6];
            i_energy_flash = e131.data[CHANNEL_START + 7];


            
            //Intensity for all Leds
            FastLED.setBrightness(i_int);
            
            //Sky background color 
                i_red_sky = map(i_red_sky, 1, 255, 0, i_max_int_sky); 
                i_green_sky = map(i_green_sky, 1, 255, 0, i_max_int_sky);
                i_blue_sky = map(i_blue_sky, 1, 255, 0, i_max_int_sky);

            for (int i = 0; i < NUM_LEDS; i++) {
                //int j = i * 3 + (CHANNEL_START - 1);  
                leds[i].setRGB( i_red_sky, i_green_sky, i_blue_sky);   
            }

            //Serial.printf("[Int-event] event: %d\n", i_int);
            //Serial.printf("[Kind] event: %d\n",i_kind_flash);
            //Serial.printf("[Kind:Energy] %d: %d\n",i_kind_flash, i_energy_flash);
            }
            }
           // Serial.printf("[Kind:Energy] %d: %d\n",i_kind_flash, i_energy_flash);

            //Lightning
            if (i_kind_flash >= 15 && i_kind_flash <= 75){
              //singel cell (low)
              if(seq_run_1 == false){
                seq_run_1 = true; //seq running

                i_duration_flash = random(20,120);
                //i_rate_flash = random(2,30);
                
                i_pos_flash = random(NUM_LEDS); //start of range
                //if((i_pos_flash +  i_energy_flash) >= NUM_LEDS){ 
                  //i_pos_flash = NUM_LEDS - i_energy_flash;//prevent overflow
                //}


                for (int i = i_pos_flash; i < i_pos_flash + i_energy_flash; i++) {
                leds[i].setRGB( i_red_flash, i_green_flash, i_blue_flash);   //first lightning
                
                }
                
              }else{
                //Seq running
                if(currentMillis - previousMillisSeqOne > i_duration_flash) {
                  // save the last time lightning flashed
                  previousMillisSeqOne = currentMillis;  

                  if(seq_count_1 <= 10){
                  if(on_off_1 == 1){
                    for (int i = i_pos_flash; i < i_pos_flash + i_energy_flash; i++) {
                      leds[i].setRGB( 0, 0, 0);   
                    }
                    on_off_1 = 0;
                  }else{
                    for (int i = i_pos_flash; i < i_pos_flash + i_energy_flash; i++) {
                      leds[i].setRGB( i_red_flash, i_green_flash, i_blue_flash);   
                    }
                    on_off_1 = 1;
                  }
                  Serial.printf("[Pos] %d: %d: %d\n",i_pos_flash, i_energy_flash, on_off_1);
                  }else if(seq_count_1 >= 11 && seq_count_1 <= 15){
                    //After lightning let it sparkle

                      for (int i = i_pos_flash; i < i_pos_flash + i_energy_flash; i++) {
                        leds[i].setRGB( 0, 0, 0); 
                        leds[random(i_pos_flash, i_pos_flash+i_energy_flash)].setRGB( i_red_flash, i_green_flash, i_blue_flash);   
                         
                      }
                      
                  }else if(seq_count_1 >= 16 && seq_count_1 <= 21){
                    for (int i = i_pos_flash; i < i_pos_flash + i_energy_flash; i++) {
                      leds[i].fadeToBlackBy( 64);    
                    }
                  }else if(seq_count_1 >= 22){
                    //reset / off
                    seq_run_1 = false;
                    seq_count_1 = 0;
                    on_off_1 = 1;

                   
                    for (int i = 0; i < NUM_LEDS; i++) {
                      //int j = i * 3 + (CHANNEL_START - 1);  
                      leds[i].setRGB( i_red_sky, i_green_sky, i_blue_sky);   
                    }

                  }
                  
                  seq_count_1 = seq_count_1 + 1;           
                }
              }
            
            }else if (i_kind_flash >= 76 && i_kind_flash <= 135){
              //multicell cluster (low-medium)
              
            }else if (i_kind_flash >= 136 && i_kind_flash <= 195){
              //multicell line (medium)
            
            }else if (i_kind_flash >= 196 && i_kind_flash <= 255){
              //supercell (heavy)
            }
            
            FastLED.show();


}
