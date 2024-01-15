#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "SSD1306_Defs.h"
#include "utilities.h"

#define MSG_TEXT "An enormous puppy was looking down at her with large round eyes, and feebly stretching out one paw, trying to touch her. “Poor little thing!” said Alice, in a coaxing tone, and she tried hard to whistle to it; but she was terribly frightened all the time at the thought that it might be hungry, in which case it would be very likely to eat her up in spite of all her coaxing."
#define TEST "This is just a message to test the funtionality"
#define TX 1
#define MODE 2
#define MESSAGE_NUM 150


namespace global {
  unsigned int analogReading;
  Adafruit_SSD1306* oled;
  uint8_t state=0;              //State variable is stored in the global namespace
}

static int error_counter = 0;
static int tx_counter = 0;
static int detected_errors = 0;

static String ms(MSG_TEXT);
//static String ms(TEST);
static bool ACK = false;

void serial_tx(String& msg);
void serial_rx(String& msg);
char computeCRC(String& msg);
String burstGen(String msg, int n);
bool crcCheck(String msg);
void probabilidad();

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(50);

  Wire.begin(SDA, SCL);
  global::oled = new Adafruit_SSD1306(OLED_WIDTH, OLED_HEIGHT);
  if(!global::oled->begin(SSD1306_SWITCHCAPVCC, 60))
    	utilities::blinkBreakpoint(100);
  global::oled->setTextColor(SSD1306_WHITE);

  randomSeed(6);
  ms += computeCRC(ms);
}

void loop() {
    if (tx_counter < MESSAGE_NUM)
    {
      global::oled->clearDisplay();
      if (TX)
      {
        String error;
        if(MODE == 0){
          error = burstGen(ms, 8);
        }
        else if(MODE == 1){
          error = burstGen(ms, 9);
        }
        else if(MODE == 2){
          error = burstGen(ms, 10);
        }
        else{
          error = burstGen(ms, random(0, ms.length()));
        }
        global::oled->setTextSize(1);
        global::oled->println("Mensaje:");
        global::oled->println(error);
        serial_tx(error);
        tx_counter++;
      }
      else{
        serial_rx(ms);
      }
    }
    else
      while(1);
}

void serial_tx(String& msg)
{
  Serial.print(msg);
  Serial.flush();

  while(Serial.available() == 0);
  
  global::oled->setCursor(0, 0);
  global::oled->setTextSize(2);
  global::oled->println("Tx CRC");
  global::oled->setTextSize(1);
  global::oled->println("Num of trans: "+ String(tx_counter));
  global::oled->display();

  while(Serial.available() == 0);
  auto msg_rcv = Serial.readString();

  if(msg_rcv!= "ACK"){
    global::oled->setTextSize(2);
    global::oled->println("Error");
    global::oled->display();
  }
}

void serial_rx(String& msg)
{
  global::oled->setCursor(0, 0);
  global::oled->setTextSize(2);
  global::oled->println("Rx CRC");
  global::oled->setTextSize(1);

  while(Serial.available() == 0){
    global::oled->println("Waiting for a message");
    global::oled->display();
  }
  
  tx_counter++;
  auto msg_rcv = Serial.readString();
  if (msg != msg_rcv)
    error_counter++;

  if(!crcCheck(msg_rcv)){
    detected_errors++;
  }

  global::oled->println("Errores: "+ String(error_counter));
  global::oled->println("Recivido: "+ String(tx_counter));
  global::oled->println("Errores CRC: "+ String(detected_errors));
  global::oled->println("Mensaje:");
  global::oled->println(msg_rcv);

  global::oled->display();

  Serial.print("ACK");
  Serial.flush();

  if(tx_counter==MESSAGE_NUM){
    probabilidad();
  }

  while(Serial.available() == 0);
}

char computeCRC(String& msg){
    char crc = 0;
    for(int i=0; i<msg.length(); i++){      // recorrer cada byte del msg
        crc ^= msg[i];
        for(int j=0; j<8; j++){
            if(crc & 0x80){                 // verificar que se puede hacer la div (hay un 1 en el bit mas sig)
                crc = (crc << 1)^0x1d5;     // aplica xor con el polinomio
            }
            else{
                crc <<= 1;                  // desplaza hasta encontrar uno
            }
        }
    }
    return crc;
}

String burstGen(String msg, int n){
    int ini = rand()% (msg.length()-n);
    msg[ini] ^= 1;
    msg[ini+n-1] ^= 1;
    for(int i=ini+1; i<ini+n-1; i++){
        if(rand()>0.5){
            msg[i] ^= 1;
        }
    }
    return msg;
}

bool crcCheck(String msg){
    char crc = msg[msg.length()-1];
    msg.remove(msg.length()-1);
    char rcv_crc = computeCRC(msg);
    if(crc != rcv_crc){
        return false;
    }
    return true;
}

void probabilidad(){
    global::oled->clearDisplay();
    global::oled->setCursor(0, 0);
    global::oled->setTextSize(2);
    global::oled->println("PROB");
    global::oled->setTextSize(1);
    float porcentaje = (detected_errors*100)/tx_counter;
    if(MODE==0){
      global::oled->println("Con rafaga de 8 bits");
    }
    else if(MODE == 1){
      global::oled->println("Con rafaga de 9 bits");
    }
    else if(MODE == 2){
      global::oled->println("Con rafaga de 10 bits");
    }
    else{
      global::oled->println("Con aleatorios errores");
    }
    global::oled->println("");
    global::oled->println("Errores " + String(tx_counter));
    global::oled->println("Detectados " + String(detected_errors));
    global::oled->println("Porcentaje: %" + String(porcentaje));
    global::oled->display();
    while(1);
}
