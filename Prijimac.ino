#define IR_PIN_PRIJIMACE 2
#define NEOPIXEL_PIN 3
#define POCET_LED 24

#include <Arduino.h>
#include <IRremote.hpp>
#include <Adafruit_NeoPixel.h>
#include <EEPROM.h>

Adafruit_NeoPixel pasek(POCET_LED, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);  //definice nového LED pásku

//definice pro dekódování ovladačů od různých výrobců
#define DECODE_DENON
#define DECODE_JVC
#define DECODE_KASEIKYO
#define DECODE_PANASONIC
#define DECODE_LG
#define DECODE_NEC
#define DECODE_SAMSUNG
#define DECODE_SONY
#define DECODE_RC5
#define DECODE_RC6

#define DECODE_BOSEWAVE
#define DECODE_LEGO_PF
#define DECODE_MAGIQUEST
#define DECODE_WHYNTER 

boolean zapnuto = false;
byte mod = 0; //0 - R, 1 - G, 2 - B
byte barvy[3] = {255,255,255};

const byte krok = 17; //255/17 = 15

void problikni(){
  byte indikator[3] = {0,0,0};
  indikator[mod] = 255;
  for(byte opakovani = 0; opakovani < 3; opakovani++){
    pasek.setPixelColor(0, indikator[0], indikator[1], indikator[2]);
    pasek.show();
    delay(100);
    pasek.setPixelColor(0, 0, 0, 0);
    pasek.show();
    delay(100);
  }
}

void setup(){
  Serial.begin(115200);
  IrReceiver.begin(IR_PIN_PRIJIMACE);
  pasek.begin();
  //nastavit hodnoty barev na 255 - bílá
  for(byte a = 0; a < 3; a++){
    barvy[a] = 255;
  }
  pasek.clear();
  mod = 0;
  zapnuto = false;
  problikni();
}

void loop(){
  if(IrReceiver.decode()){
    IrReceiver.resume();
    if(IrReceiver.decodedIRData.flags == 0){
      switch(IrReceiver.decodedIRData.command){
        case 22: //reset
          setup();
          break;
        case 19:
          zapnuto = !zapnuto;
          if(!zapnuto){
            pasek.clear();
          }
          else{
            for(byte ledka = 0; ledka < POCET_LED; ledka++){
              pasek.setPixelColor(ledka, barvy[0], barvy[1], barvy[2]);
            }
          }
          pasek.show();
          break;  
        case 27:  //uložení do EEPROM
          for(byte index = 0; index < 3; index++){
            EEPROM.write(index, barvy[index]);
          }
          pasek.clear();
          //efekt
          for(byte ledka = 0; ledka < POCET_LED; ledka++){
            pasek.setPixelColor(ledka, barvy[0], barvy[1], barvy[2]);
            pasek.show();
            delay(30);
          }
          zapnuto = true;
          break;
        case 65:  //načtení z EEPROM
          for(byte index = 0; index < 3; index++){
            barvy[index] = EEPROM.read(index);
          }
          pasek.clear();
          //efekt
          for(int ledka = POCET_LED - 1; ledka >= 0; ledka--){
            pasek.setPixelColor(ledka, barvy[0], barvy[1], barvy[2]);
            pasek.show();
            delay(30);
          }
          zapnuto = true;
          break;  
        case 14: //mode
          switch(mod){
            case 2:
              mod = 0;
              break;
            default:
              mod++;
              break;
          }
          Serial.print("Mod: ");
          Serial.println(mod);
          problikni();
          if(zapnuto){
            pasek.setPixelColor(0, barvy[0], barvy[1], barvy[2]);
            pasek.show();
          }
      }
    }
    switch(IrReceiver.decodedIRData.command){
      case 17:
        if(barvy[mod] <= 255 - krok){
          barvy[mod] += krok;
        }
        for(byte ledka = 0; ledka < POCET_LED; ledka++){
          pasek.setPixelColor(ledka, barvy[0], barvy[1], barvy[2]);
        }
        break;
      case 21:
        if(barvy[mod] >= 0 + krok){
          barvy[mod] -= krok;
        }
        for(byte ledka = 0; ledka < POCET_LED; ledka++){
          pasek.setPixelColor(ledka, barvy[0], barvy[1], barvy[2]);
        }
        break;  
    }
  pasek.show();
  } 
}
