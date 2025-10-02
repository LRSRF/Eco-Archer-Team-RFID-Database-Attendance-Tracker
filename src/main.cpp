/**
 * RFID Office Attendance System
 * Author: Raphael C. Murillo
 * Date: October 1, 2024
 * License: GNU GENERAL PUBLIC LICENSE
 */

// Standard Libraries
#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <Wire.h>
#include <vector>
#include <string.h>

// Third-party Libraries
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MFRC522.h>
#include <ArduinoJson.h>

// Project Headers
#include <animations.h>
#include <buzz_tones.h>
#include <data_map.h>
#include <requests.h>
#include <secrets.h>
#include <discord.h>
#include <discord_embeds.h>

// Hardware Pin Definitions
#define RST_PIN 22
#define SS_PIN 5
#define BUZZER_PIN 25
#define OLED_SDA 21
#define OLED_SCL 14
#define OLED_RESET -1

// Display Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Audio Configuration
const int frequency = 2000;
const int duration = 1000;

// Hardware Instances
MFRC522 mfrc522(SS_PIN, RST_PIN);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Network Configuration
const char wifi_ssid[] = WIFI_SSID;
const char wifi_pass[] = WIFI_PW;

// Global Variables
static std::vector<String> uid_db;
UserInfo *users;
int userCount = 0;
int frame = 0;

// Function Declarations
void connect_wifi();
bool check_uid(const String &target_uid);

void setup() {
  // Initialize hardware interfaces
  Wire.begin(OLED_SDA, OLED_SCL);
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init();
  delay(100);

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true); // System halt on display failure
  }
  display.clearDisplay();

  Serial.println("\nRFID Attendance System - Initializing...");

  // Establish network connection
  connect_wifi();
  display.clearDisplay();

  // Download UID database with retry mechanism
  bool uidsDownloaded = false;
  int retryCount = 0;
  const int maxRetries = 3;
  const int retryDelay = 1000;

  while (!uidsDownloaded && retryCount < maxRetries) {
    // Ensure WiFi connection before database download
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi disconnected. Reconnecting...");
      connect_wifi();
    }

    display.clearDisplay();
    // Display loading animation and status
    display.drawBitmap(48, 11, gears[frame], FRAME_WIDTH, FRAME_HEIGHT, 1);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(20, 45);

    if (retryCount == 0) {
      display.print("Downloading UIDs");
    } else {
      display.print("Retry Attempt: " + String(retryCount + 1));
    }
    display.display();

    // Request UID database from Google Apps Script
    String json = spreadsheet_comm();
    Serial.println("Database Response: " + String(json.length()) + " characters");

    // Validate JSON response format
    if (json.length() > 0 && json.startsWith("[") && json.endsWith("]")) {
      jsonToHashmap(json, users, userCount);
      
      if (userCount > 0) {
        Serial.println("UID Database Downloaded Successfully:");
        Serial.println("Total Users: " + String(userCount));
        
        // Populate UID lookup vector for fast authorization checks
        uid_db.clear();
        for (int i = 0; i < userCount; i++) {
          uid_db.push_back(users[i].uid);
          Serial.println("  " + users[i].name + " (" + users[i].uid + ")");
        }

        uidsDownloaded = true;
        
        // Display success status
        display.clearDisplay();
        display.drawBitmap(48, 16, authorized[frame], FRAME_WIDTH, FRAME_HEIGHT, 1);
        display.setTextSize(1);
        display.setTextColor(WHITE);
        display.setCursor(20, 50);
        display.print("Database Ready!");
        display.display();
        break;
        
      } else {
        Serial.println("No users found in database response");
      }
    } else {
      Serial.println("Invalid database response format");
    }
    
    retryCount++;
    if (retryCount < maxRetries) {
      Serial.println("Retrying in " + String(retryDelay) + "ms...");
      delay(retryDelay);
    }
  }

  // Handle database download failure
  if (!uidsDownloaded) {
    Serial.println("CRITICAL: Failed to download UID database after " + String(maxRetries) + " attempts");
    display.clearDisplay();
    display.drawBitmap(48, 11, denied[0], FRAME_WIDTH, FRAME_HEIGHT, 1);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(15, 45);
    display.print("Database Failed");
    display.setCursor(25, 55);
    display.print("Check Network");
    display.display();
    
    while (true) { // System halt - requires restart
      delay(1000);
    }
  }

  // Initialize buzzer and audio feedback
  pinMode(BUZZER_PIN, OUTPUT);
  ledcSetup(0, frequency, 8);
  ledcAttachPin(BUZZER_PIN, 0);
  success_buzz(BUZZER_PIN);

  Serial.println("System Ready - RFID Scanner Active");
  send_discord_message("Eco Archers Team Gatepass System Online. 📡");
}

void loop() {
  // Display scanning animation
  display.clearDisplay();
  display.drawBitmap(48, 16, scan_display[frame], FRAME_WIDTH, FRAME_HEIGHT, 1);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(25, 50);
  display.print("Ready to scan...");
  display.display();
  
  // Update animation frame
  unsigned int frame_count = (sizeof(scan_display) / sizeof(scan_display[0]));
  frame = (frame + 1) % frame_count;
  delay(FRAME_DELAY);

  // Check for new RFID card
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Extract UID from scanned card
  String uid = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uid.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    uid.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  uid.toUpperCase();
  uid.trim();
  
  Serial.println("Card Scanned - UID: " + uid);
  scan_buzz(BUZZER_PIN);

  // Display verification status
  display.clearDisplay();
  display.drawBitmap(48, 16, gears[0], FRAME_WIDTH, FRAME_HEIGHT, 1);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(25, 50);
  display.print("Verifying...");
  display.display();

  // Lookup user in local database
  UserInfo *user = nullptr;
  for (int i = 0; i < userCount; i++) {
    if (users[i].uid == uid) {
      user = &users[i];
      break;
    }
  }

  if (user != nullptr) {
    // Authorized user found in database
    Serial.println("ACCESS GRANTED: " + user->name + " (" + user->discord_username + ")");

    send_discord_embeds(authorized_message(user->name, user->discord_username, "attendance"));

    // Record attendance and determine action type
    send_scan_data(uid, true);
    // String actionType = response;
    // actionType.trim();
    
    // Serial.println("Attendance Action: " + actionType);
    // delay(500);

    // // Send appropriate Discord notification
    // if (actionType == "time in") {
    //   send_discord_embeds(authorized_message(user->name, user->discord_username, "time in"));
    // } else if (actionType == "time out") {
    //   send_discord_embeds(authorized_message(user->name, user->discord_username, "time out"));
    // } else {
    //   Serial.println("Unknown action type: " + actionType + " - Using default");
    //   send_discord_embeds(authorized_message(user->name, user->discord_username, "attendance"));
    // }

    // Display success feedback
    display.clearDisplay();
    display.drawBitmap(48, 16, authorized[0], FRAME_WIDTH, FRAME_HEIGHT, 1);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(25, 50);
    display.print(user->name);
    display.display();
    success_buzz(BUZZER_PIN);

  } else {
    // Unauthorized card scanned
    Serial.println("ACCESS DENIED: Unknown UID " + uid);

    // Log unauthorized attempt
    send_scan_data(uid, false);
    delay(500);

    // Send security alert to Discord
    send_discord_embeds(denied_message());

    // Display denial feedback
    display.clearDisplay();
    display.drawBitmap(48, 16, denied[0], FRAME_WIDTH, FRAME_HEIGHT, 1);
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(25, 50);
    display.print("Access Denied");
    display.display();
    error_buzz(BUZZER_PIN);
  }

  // Reset MFRC522
  mfrc522.PICC_HaltA();
  delay(250);
}

void connect_wifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(wifi_ssid, wifi_pass);

  // Display connection status
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 25);
  display.print("Connecting to " + String(wifi_ssid));
  display.display();

  Serial.print("Connecting to WiFi network: " + String(wifi_ssid));

  // Wait for connection with timeout
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi Connected Successfully");
    Serial.println("IP Address: " + WiFi.localIP().toString());
  } else {
    Serial.println("\nWiFi Connection Failed");
  }
}

bool check_uid(const String &target_uid) {
  for (const String &uid : uid_db) {
    if (uid == target_uid) {
      return true;
    }
  }
  return false;
}