#include <Arduino.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <ctime>

// Wifi credentials

const char* ssid = "***REMOVED***";
const char* password = "***REMOVED***";


// function declaration
String formatTimeStamp(long timestamp);


void setup() {
  Serial.begin(115200);
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
        countryCode = locationDoc["country_code"].as<String>();
      }
      else {
        Serial.println("Error on HTTP request for country code");
      }

      // Print the ISS location and timestamp to the Serial Monitor
      Serial.print("Latitude: ");
      Serial.print(latitude, 6);
      Serial.print(" | Longitude: ");
      Serial.print(longitude, 6);
      Serial.print(" | Timestamp: ");
      Serial.print(formattedTime);
      Serial.print(" | Country Code: ");
      Serial.println((countryCode == "??") ? "N/A" : countryCode);

    }
    // if the request failed, print the error code
    else {
      Serial.println("Error on HTTP request");
    }
  }
  else {
    Serial.println("Connection lost");
  }
  delay(10000); // Update every 10 seconds
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
