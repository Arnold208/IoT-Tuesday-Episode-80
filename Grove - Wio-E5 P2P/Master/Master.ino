#include "DHT.h"
#include <Multichannel_Gas_GMXXX.h>
#include <Wire.h>
# include <U8x8lib.h>

#define DHTTYPE DHT11
#define DHTPIN D0

DHT dht(DHTPIN, DHTTYPE);
GAS_GMXXX<TwoWire> gas;
U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/*reset=*/U8X8_PIN_NONE);


static uint8_t recv_cmd[8] = {};

String data, n02, alcohol, co;

int temp, humi;

void setup() {
  Serial.begin(115200);
  Serial1.begin(9600);

  gas.begin(Wire, 0x08);
  dht.begin();
  u8x8.begin();
  u8x8.setFlipMode(1);
  u8x8.setFont(u8x8_font_chroma48medium8_r);

  delay(4000);
  Serial1.write("AT+MODE=TEST");
  delay(2000);

}

void loop() {

  uint8_t len = 0;
  uint8_t addr = 0;
  uint8_t i;
  uint32_t val = 0;

  val = gas.measure_NO2(); Serial.print("n02: "); Serial.print(val); Serial.print("  =  ");
  n02 = String(val);

  val = gas.measure_C2H5OH(); Serial.print("C2H5OH: "); Serial.print(val); Serial.print("  =  ");
  alcohol = String(val);

  val = gas.measure_CO(); Serial.print("co: "); Serial.print(val); Serial.print("  =  ");
  co = String(val);


  //Read Temperature and Humidity Data

  float temp_hum_val[2] = {0};

  if (!dht.readTempAndHumidity(temp_hum_val)) {

    //Converting the floating value to decimal
    //Eg. 23.25 * 100 ==> 2325

    humi = (temp_hum_val[0] * 100);

    temp = (temp_hum_val[1] * 100);

  } else {
    Serial.println("Failed to get temprature and humidity value.");
  }


  // Add the Temperature, Humidity, and other sensor data
  // seperated by a fullstop(.)
  //Eg. data = "2421.2451.4541.6574.9877";

  data = String(temp) + "." + String(humi) + "." + co + "." + n02 + "." + alcohol;

  //Print the data on the OLED Screen
  u8x8.setCursor(5, 0);
  u8x8.print("Master");
  u8x8.setCursor(2, 2);
  u8x8.print("Hemp:" + String(temp_hum_val[1]));
  u8x8.setCursor(2, 3);
  u8x8.print("Humi:" + String(temp_hum_val[0]));
  u8x8.setCursor(2, 4);
  u8x8.print("Co:" + co);
  u8x8.setCursor(2, 5);
  u8x8.print("N02:" + n02);
  u8x8.setCursor(2, 6);
  u8x8.print("Alcohol:" + alcohol);

  //After collecting and preparing the data send
  //it through AT command over the Lora module
  Serial1.write(("AT+TEST=TXLRSTR," + data).c_str());

  delay(5000); //Wait for 5 seconds after sending the data


}
