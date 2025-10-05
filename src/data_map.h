#include <Arduino.h>
#include <ArduinoJson.h>

// Struct to hold individual user info
struct UserInfo {
  String uid;
  String dlsu_id;
  String name;
  String discord_username;
};

// Function to parse the JSON and fill the users array
void jsonToHashmap(String json, UserInfo *&users, int &userCount) {
  Serial.println("JSON length: " + String(json.length()) + " bytes");
  
  // Allocate the memory for parsing the JSON - use DynamicJsonDocument for larger data
  const size_t capacity = json.length() + 1024; // Extra space for parsing overhead
  DynamicJsonDocument doc(capacity);

  // Deserialize the JSON document directly from String (more efficient)
  DeserializationError error = deserializeJson(doc, json);

  // Check if parsing succeeded
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    Serial.println("Error code: " + String(error.code()));
    Serial.println("First 200 characters of JSON:");
    Serial.println(json.substring(0, 200));
    Serial.println("Last 200 characters of JSON:");
    Serial.println(json.substring(json.length() - 200));
    
    userCount = 0;
    return;
  }

  // Get the JSON array from the parsed document
  JsonArray arr = doc.as<JsonArray>();
  userCount = arr.size(); // Get the number of elements in the array
  
  Serial.println("Successfully parsed JSON array with " + String(userCount) + " users");

  // Dynamically allocate the users array based on the size of the JSON array
  users = new UserInfo[userCount];

  // Parse the JSON array and populate the users array
  for (size_t i = 0; i < userCount; i++) {
    users[i].uid = arr[i]["uid"].as<String>();
    users[i].dlsu_id = arr[i]["dlsu_id"].as<String>();
    users[i].name = arr[i]["name"].as<String>();
    users[i].discord_username = arr[i]["discord_username"].as<String>();
  }
}