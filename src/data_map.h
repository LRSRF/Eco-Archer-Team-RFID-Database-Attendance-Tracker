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
  // Convert the String to a char array for ArduinoJson parsing
  const size_t capacity = 1024;
  char jsonChar[capacity];
  json.toCharArray(jsonChar, capacity);

  // Allocate the memory for parsing the JSON
  JsonDocument doc;

  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, jsonChar);

  // Check if parsing succeeded
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return;
  }

  // Get the JSON array from the parsed document
  JsonArray arr = doc.as<JsonArray>();
  userCount = arr.size(); // Get the number of elements in the array

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