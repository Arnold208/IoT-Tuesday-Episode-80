#include <SoftwareSerial.h>

// LoRa Module Configurations Transmitter Node
String lora_band = "915000000";     // Enter LoRa band as per your country
String lora_networkid = "7";        // Enter Lora Network ID - Must be same for Tx and Rx Nodes
String lora_TX_address = "126";     // Enter Lora RX address

#define red_button 32
#define yellow_button 33
#define green_button 35
 
// Software Serial Pins Rx and Tx For Lora Module
const byte rxPin = 26;
const byte txPin = 25;

// Set up a new SoftwareSerial object
SoftwareSerial sendLoRa (rxPin, txPin);   // Software Serial Pins Rx=D7 and Tx=D8

int data_length;
String red_light, yellow_light, green_light;

void setup() {

  Serial.begin(115200);
  sendLoRa.begin(115200);
  delay(1000);

  pinMode(red_button, INPUT);
  pinMode(yellow_button, INPUT);
  pinMode(green_button, INPUT);
 
  // Configure LoRa module at Startup
  sendLoRa.println("AT+BAND=" + lora_band);
  delay(1000);
  sendLoRa.println("AT+ADDRESS=" + lora_TX_address);
  delay(1000);
  sendLoRa.println("AT+NETWORKID=" + lora_networkid);
  delay(1000);
}

void loop() {

  int but1 = digitalRead(red_button);
  int but2 = digitalRead(yellow_button);
  int but3 = digitalRead(green_button);
 
  if (but1 == 1) {
    Serial.println("Red Button");
//                         RED YELLOW GREEN
//                           |   |    |
    red_light = "1%0%0";  // ON%OFF%OFF
    data_length = red_light.length(); // Get the length of data to be transmitted
    send_data(red_light, data_length);
  }

  if (but2 == 1) {
    Serial.println("Yellow Button");

    yellow_light = "0%1%0";
    data_length = yellow_light.length();
    send_data(yellow_light, data_length);
  }

  if (but3 == 1) {
    Serial.println("Green Button");

    green_light = "0%0%1";
    data_length = green_light.length();
    send_data(green_light, data_length);
  }
  
}

void send_data(String sensorvalue, int valuelength)
{
  String mymessage;
  mymessage = mymessage + "AT+SEND=621" + "," + valuelength + "," + sensorvalue + "\r\n";
  sendLoRa.print(mymessage);
  Serial.print("Data Sent : ");
  Serial.println(mymessage);
  delay(250);
}
