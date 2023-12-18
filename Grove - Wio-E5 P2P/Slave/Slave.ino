# include <U8x8lib.h>

U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(/*reset=*/U8X8_PIN_NONE);

String receivedData = ""; // Variable to store received data

void setup() {

  Serial.begin(115200);
  Serial1.begin(9600);

  Serial1.write("AT+MODE=TEST"); // Set the Lora Module to TEST Mode
  delay(2000);
  Serial1.write("AT+TEST=RXLRPKT"); //  Set the Lora Module to Receive Packet
  delay(2000);

  //Setup the OLED
  u8x8.begin();
  u8x8.setFlipMode(1);
  u8x8.setFont(u8x8_font_chroma48medium8_r);
}

void loop() {

  // Read data from Serial and send it to Serial1
  while (Serial.available() > 0) {
    char receivedChar = Serial.read();
    Serial1.write(receivedChar);
  }

  // Read data from Serial1 and store it in the variable
  while (Serial1.available() > 0) {
    String receivedChar = Serial1.readString();
    receivedData = receivedChar;
  }

  String extractedValue = extractValueInQuotes(receivedData);// Get the HEX data received from the Lora Module

  String convertedAsciiValue = hexToAscii(extractedValue); // Convert the Hex data to ASCII String

  // Print the converted ASCII value
  if (convertedAsciiValue.length() > 0) {
    
    Serial.println("Converted ASCII value: " + convertedAsciiValue);
    //This is how the converted data looks like
    //       2334.6435.6543.6553.6543

    //Lets get the various sensor data from the converted Ascii data
    //The getvalue function seperates the sensor data using the fullstop(.);
    
    String temperature = getValue(convertedAsciiValue, '.', 0);
    String  humidity = getValue(convertedAsciiValue, '.', 1);
    String CO = getValue(convertedAsciiValue, '.', 2);
    String NO2 = getValue(convertedAsciiValue, '.', 3);
    String Alcohol = getValue(convertedAsciiValue, '.', 3);

    //Convert the temperature and Humidity values back to floating point
    float T = (temperature.toFloat()) / 100;
    float H = (humidity.toFloat()) / 100;

    //Print the values on the Serial Monitor
    Serial.println(T);
    Serial.println(H);
    Serial.println(CO);
    Serial.println(NO2);
    Serial.println(Alcohol);

    //Display the values on the OLED Screen
    u8x8.setCursor(5, 0);
    u8x8.print("Slave");
    u8x8.setCursor(2, 2);
    u8x8.print("Temp:" + String(T));
    u8x8.setCursor(2, 3);
    u8x8.print("Humi:" + String(H));
    u8x8.setCursor(2, 4);
    u8x8.print("CO:" + CO);
    u8x8.setCursor(2, 5);
    u8x8.print("NO2:" + NO2);
    u8x8.setCursor(2, 6);
    u8x8.print("Alcohol:" + Alcohol);
  }

  // Clear receivedData for the next iteration
  receivedData = "";
 
}

//This function extracts the Hex Data in the Lora Packet Received
//Received data: +TEST: LEN:21, RSSI:-56, SNR:12 +TEST: RX "333137302E393530302E3531322E3436302E373834"
String extractValueInQuotes(const String &input) {
  int startPos = input.indexOf('"');
  int endPos = input.indexOf('"', startPos + 1);

  if (startPos != -1 && endPos != -1) {
    return input.substring(startPos + 1, endPos);
  } else {
    return ""; // Return an empty string if the format is not as expected
  }
}

//This function converts the Hex Data to ASCII Strings
String hexToAscii(const String &hexString) {
  String asciiString = "";

  // Iterate over pairs of characters in the hex string
  for (int i = 0; i < hexString.length(); i += 2) {
    // Extract a pair of characters
    String hexPair = hexString.substring(i, i + 2);

    // Convert the hex pair to an integer
    int intValue = strtol(hexPair.c_str(), NULL, 16);

    // Convert the integer to an ASCII character and append to the result
    asciiString += char(intValue);
  }

  return asciiString;
}

//This function seperates data in a string using a seperator character
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
