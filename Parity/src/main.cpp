#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "SSD1306_Defs.h"
#include "utilities.h"

#define MSG_TEXT "An enormous puppy was looking down at her with large round eyes, and feebly stretching out one paw, trying to touch her. “Poor little thing!” said Alice, in a coaxing tone, and she tried hard to whistle to it; but she was terribly frightened all the time at the thought that it might be hungry, in which case it would be very likely to eat her up in spite of all her coaxing."
#define TEST "This is just a message to test the funtionality"
#define TX 1
#define EVEN 1

namespace global {
  unsigned int analogReading;
  Adafruit_SSD1306* oled;
  uint8_t state=0;              //State variable is stored in the global namespace
}

static int error_counter = 0;
static int tx_counter = 0;
static int detected_errors = 0;

static String msg(MSG_TEXT);
//static String msg(TEST);
static bool ACK = false;

void serial_tx(String& msg);
void serial_rx(String& msg);
void addParity(String& msg);
bool getParity(char character);
String BitParity(String msg, int mode);
bool ParityCheck(String& msg);
String clearParity(String msg);

void setup() {
  Serial.begin(9600);
  Serial.setTimeout(50);

  Wire.begin(SDA, SCL);
  global::oled = new Adafruit_SSD1306(OLED_WIDTH, OLED_HEIGHT);
  if(!global::oled->begin(SSD1306_SWITCHCAPVCC, 60))
    	utilities::blinkBreakpoint(100);
  global::oled->setTextColor(SSD1306_WHITE);

  randomSeed(6);
  addParity(msg);
}

void loop() {
    if (tx_counter < 1000)
    {
      global::oled->clearDisplay();
      if (TX)
      {
        global::oled->setTextSize(1);
        global::oled->println("Mensaje:");
        String error = BitParity(msg, 1);
        global::oled->println(clearParity(error));
        serial_tx(error);
        tx_counter++;
      }
      else{
        serial_rx(msg);
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
  global::oled->println("Tx Parity");
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
  global::oled->println("Rx Parity");
  global::oled->setTextSize(1);

  while(Serial.available() == 0){
    global::oled->println("Waiting for a message");
    global::oled->display();
  }

  tx_counter++; 
  auto msg_rcv = Serial.readString();
  if (msg != msg_rcv)
    error_counter++;

  if(ParityCheck(msg_rcv)){
    detected_errors++;
  }

  global::oled->println("Errores: "+ String(error_counter));
  global::oled->println("Recivido: "+ String(tx_counter));
  global::oled->println("Errores paridad: "+ String(detected_errors));
  global::oled->println("Mensaje:");
  global::oled->println(clearParity(msg_rcv));

  global::oled->display();

  Serial.print("ACK");
  Serial.flush();

  while(Serial.available() == 0);
}

/* Funtion that adds the even parity to every character of a string */
void addParity(String& msg){
  for(int i=0; i<msg.length(); i++){
      if(getParity(msg[i])){ // add a 1
        msg[i]^=128;
      }
  }
}

bool getParity(char character){
  int ones_count = 0;

  for(int i=0; i<7; i++){
    if((character & (1<<i)) !=0 )
      ones_count ++;
  }
  if(EVEN){
    return ones_count%2;
  }
  return !(ones_count%2);
}

String BitParity(String msg, int mode){
  if(mode == 0){
    return msg;
  }
  else if(mode == 1){
    int bit = random(0, msg.length()*8);
    int error = bit/8;
    msg[error] ^= (1<<(bit%8));
  }
  else{
    int error_count = random(0, msg.length()*8);
    int mem[msg.length()*8];
    bool repeated= false;
    for(int i=0; i<error_count; i++){
      int bit = random(0, msg.length()*8);
      for(int j=0; j<error_count; j++){
        if(bit == mem[j])
          repeated = true;
      }
      if(!repeated){
        mem[i]=bit;
        int error =bit/8;
        msg[error] ^= (1<<(bit%8));
      }
      repeated = false;
    }
  }
  return msg;
}

bool ParityCheck(String& msg){
  bool error = false;
  int bitError = 0;
  for(int i=0; i<msg.length(); i++){
    if(getParity(msg[i]) && (msg[i]&128)!=128){
      bitError++;
      error = true;
    }
    else if(!getParity(msg[i])&&(msg[i]&128)!=0){
      bitError++;
      error = true;
    }
  }
  return error;
}

String clearParity(String msg) {
    for(int i=0; i<msg.length(); i++){
        msg[i] &= 0x7F;
    }
    return msg;
}

