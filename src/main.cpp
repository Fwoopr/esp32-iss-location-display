#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <ctime>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <SPI.h>

// Wifi credentials

const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// defining the pins
#define LED_PIN 23
#define SCL 22
#define SDA 21

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);



// Variable to store the country code of the ESP32's location
String esp32CountryCode = "N/A";

// function declaration
String formatTimeStamp(long timestamp);
String getEsp32CountryCode();
void disp(String lat, String lon, String time, String country);


void setup() {
  Wire.begin(SDA, SCL); // Initialize I2C communication
  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS); // Initialize the OLED display
  display.clearDisplay();
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT); // Initialize the LED pin as an output
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  // Set the current country code based on the location of the microcontroller
  esp32CountryCode = getEsp32CountryCode();
  Serial.print("ESP32 Country Code: ");
  Serial.println(esp32CountryCode);

}

void loop() {
  if (WiFi.status() == WL_CONNECTED) // Check WiFi connection status
  {
    // Create an HTTP client and make a GET request to the ISS location API
    HTTPClient client;

    client.begin("https://api.wheretheiss.at/v1/satellites/25544"); // Specify the URL
    int httpCode = client.GET(); // Make the request

    // Check the returning code
    if (httpCode > 0) {
      // Parse the JSON response
      JsonDocument doc;
      deserializeJson(doc, client.getStream());

      // The API returns an object with latitude and longitude as floats and timestamp as a long integer
      double latitude = doc["latitude"];
      double longitude = doc["longitude"];
      long timestamp = doc["timestamp"];
      // Format the timestamp into a human-readable string
      String formattedTime = formatTimeStamp(timestamp);

      client.end(); // Close the connection

      String countryCode = "";
      client.begin("https://api.wheretheiss.at/v1/coordinates/" + String(latitude, 6) + "," + String(longitude, 6)); // Make another request to get the location name
      httpCode = client.GET(); // Make the request

      if (httpCode > 0) {
        JsonDocument locationDoc;
        deserializeJson(locationDoc, client.getStream());
        // The API returns an object with a "country_code" field that contains the country code or "??" if it's over the ocean
        countryCode = locationDoc["country_code"].as<String>();
        client.end(); // Close the connection
      }
      else {
        Serial.println("Error on HTTP request for country code");
      }

      // Print the ISS location and timestamp to the Display
      disp(String(latitude, 6), String(longitude, 6), formattedTime, countryCode);
      

      if (countryCode != "??" && countryCode == esp32CountryCode) {
        Serial.println("The ISS is currently over the same country as the ESP32!");
        digitalWrite(LED_PIN, HIGH); // Turn on the LED
      }
      else {
        digitalWrite(LED_PIN, LOW); // Turn off the LED
      }

    }
    // if the request failed, print the error code
    else {
      Serial.println("Error on HTTP request");
    }
  }
  else {
    Serial.println("Connection lost");
  }
  delay(3000); // Update every 3 seconds (plus the delays in the disp function, so the total update time will be around 10 seconds)
}

// Function to format the timestamp into a human-readable string
String formatTimeStamp(long timestamp) {
  time_t rawTime = timestamp;
  struct tm * timeInfo;
  char buffer[20];
  timeInfo = localtime(&rawTime);
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeInfo);
  return String(buffer);

}

// Function to get the current country code based on the location of the microcontroller
String getEsp32CountryCode() {
  HTTPClient client;
  client.begin("http://ip-api.com/json/?fields=countryCode"); // Make a request to an IP geolocation API
  int httpCode = client.GET(); // Make the request

  if (httpCode > 0) {
    JsonDocument doc;
    deserializeJson(doc, client.getStream());
    String countryCode = doc["countryCode"].as<String>();
    client.end(); // Close the connection
    return countryCode;
  }
  else {
    Serial.println("Error on HTTP request for country code");
    return "N/A";
  }

}

void disp(String lat, String lon, String time, String country) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.print("Lat:");
  display.setCursor(0, 16);
  display.print(lat);
  display.setCursor(0, 32);
  display.print("Lon: ");
  display.setCursor(0, 48);
  display.println(lon);
  display.display();
  delay(4000); // Display the coordinates for 4 seconds
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Time: ");
  display.setCursor(0, 32);
  display.print(time);
  display.display();
  delay(3000); // Display the time for 2.5 seconds
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("Country: ");
  display.setCursor(0, 36);
  display.print(country=="??" ? "N/A" : country );
  display.display();

}