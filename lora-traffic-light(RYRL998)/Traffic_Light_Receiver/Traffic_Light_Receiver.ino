#include<SoftwareSerial.h>

// LoRa Module Configurations Reciever Node
String lora_band = "915000000";     // Enter LoRa band as per your country
String lora_networkid = "7";        // Enter Lora Network ID - Must be same for Tx and Rx Nodes
String lora_RX_address = "621";

// Setup the Traffic Light
#define red 33
#define yellow 32
#define green 14

// Software Serial Pins Rx and Tx For Lora Module
const byte rxPin = 25;
const byte txPin = 26;

SoftwareSerial loraReceive (txPin, rxPin); // (RX, TX)

void setup() {
  Serial.begin(115200);
  loraReceive.begin(115200); // Start Serial Communication With Lora Module
  delay(1000);
  
  pinMode(red, OUTPUT);
  pinMode(yellow, OUTPUT);
  pinMode(green, OUTPUT);

  // Configure LoRa module at Startup
  loraReceive.println("AT+BAND=" + lora_band);
  delay(1000);
  loraReceive.println("AT+ADDRESS=" + lora_RX_address);
  delay(1000);
  loraReceive.println("AT+NETWORKID=" + lora_networkid);
  delay(1000);
}


void loop() {
  checkSerial();
}

//====================== Function to Check Incomming Serial Data ====================================//
void checkSerial() {
  while (loraReceive.available() > 0) {
    // get the new byte:
    String inString = loraReceive.readString();
    Serial.print("Received Command : ");
    Serial.println(inString);

    // Data receiving format : +RCV=<Address>,<Length>,<Data>,<RSSI>,<SNR>
    // Example: +RCV=50,5,HELLO,-99,40
    //                | |   |    |   |
    //                0 1   2    3   4

    // So to get command data : String command = getValue(inString, ',', 2);
    String DataIn = getValue(inString, ',', 2);     //--> data

    Serial.println(DataIn);

    String  red_cmd = getValue(DataIn, '%', 0); //--> Get temperature data
    String  yellow_cmd = getValue(DataIn, '%', 1); //--> Get humidity data
    String  green_cmd  = getValue(DataIn, '%', 2); //--> Get moisture data

    if (red_cmd == "1") {
      digitalWrite(red, HIGH);
      digitalWrite(yellow, LOW);
      digitalWrite(green, LOW);
    }

    if (yellow_cmd == "1") {
      digitalWrite(red, LOW);
      digitalWrite(yellow, HIGH);
      digitalWrite(green, LOW);
    }

    if (green_cmd == "1") {
      digitalWrite(red, LOW);
      digitalWrite(yellow, LOW);
      digitalWrite(green, HIGH);
    }
    //    digitalWrite(red, HIGH);
    //    digitalWrite(yellow, HIGH);
    //    digitalWrite(green, HIGH);
    //
    //    delay(2000);
    //
    //    digitalWrite(red, LOW);
    //    digitalWrite(yellow, LOW);
    //    digitalWrite(green, LOW);
    //
    //    delay(2000);
  }
}



//====================== Function for parsing the incomming data String ======================================//
String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length() - 1;

  for (int i = 0; i <= maxIndex && found <= index; i++) {
    if (data.charAt(i) == separator || i == maxIndex) {
      found++;
      strIndex[0] = strIndex[1] + 1;
      strIndex[1] = (i == maxIndex) ? i + 1 : i;
    }
  }
  return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
