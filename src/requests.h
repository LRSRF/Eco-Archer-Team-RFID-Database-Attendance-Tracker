/**
 * HTTP Request Functions
 * Handles communication with Google Apps Script API
 */

#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <secrets.h>

/**
 * Fetch UID database from Google Apps Script
 * @return JSON string containing UID records
 */
String spreadsheet_comm(void) {
  HTTPClient http;
  String url = "https://script.google.com/macros/s/" + String(APP_ID) + "/exec?read";

  Serial.println("Fetching UID database...");

  http.begin(url.c_str());
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

  int httpCode = http.GET();
  String payload = "";

  if (httpCode > 0) {
    payload = http.getString();
    Serial.println("Database request completed - HTTP " + String(httpCode));
  } else {
    Serial.println("Database request failed: " + http.errorToString(httpCode));
  }
  
  http.end();
  return payload;
}

/**
 * Record attendance data via Google Apps Script
 * @param uid RFID card identifier
 * @param access_granted Authorization status (true for valid users)
 * @return Action type string ("time in", "time out", or error message)
 */
void send_scan_data(String uid, bool access_granted) {
  WiFiClientSecure client;
  client.setInsecure();
  client.setTimeout(15000);
  
  HTTPClient http;
  http.begin(client, "https://script.google.com/macros/s/" + String(APP_ID) + "/exec");
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Accept", "application/json");
  
  // Construct attendance record payload
  String jsonPayload = "{\"uid\":\"" + uid + "\",\"access_granted\":" + 
                      (access_granted ? "true" : "false") + "}";
  
  Serial.println("Recording attendance for UID: " + uid);
  
  http.POST(jsonPayload);
  // String response = http.getString();
  
  // if (httpCode == 200) {
  //   Serial.println("Attendance recorded successfully");
  // } else {
  //   Serial.println("Attendance recording failed - HTTP " + String(httpCode));
  // }
  
  // http.end();
  // return response;
}