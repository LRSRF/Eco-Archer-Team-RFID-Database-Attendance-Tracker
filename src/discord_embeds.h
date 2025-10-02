/**
 * Discord Embed Message Generator
 * Creates formatted notification messages for Discord webhook
 */

#include <Arduino.h>
#include <cJSON.h>
#include <secrets.h>

/**
 * Create Discord embed JSON structure
 * @param title Message title
 * @param description Message content
 * @param color Embed border color (decimal format)
 * @return JSON string for Discord embed
 */
String embed_message(const char *title, const char *description, int color) {
  cJSON *embed = cJSON_CreateObject();
  cJSON *fields = cJSON_CreateArray();

  cJSON_AddItemToObject(embed, "title", cJSON_CreateString(title));
  cJSON_AddItemToObject(embed, "description", cJSON_CreateString(description));
  cJSON_AddItemToObject(embed, "color", cJSON_CreateNumber(color));
  cJSON_AddItemToObject(embed, "fields", fields);

  char *json_string = cJSON_Print(embed);
  String result = String(json_string);
  
  cJSON_Delete(embed);
  free(json_string);
  
  return result;
}

/**
 * Generate authorized access notification message
 * @param name Employee full name
 * @param username Discord username
 * @param action_type Attendance action ("time in", "time out", or "attendance")
 * @return Discord embed JSON string
 */
String authorized_message(String name, String username, String action_type) {
  const char *title = "✅ [ATTENDANCE RECORDED] Automated Gatepass Message";
  String description;
  int color;

  // Generate appropriate message based on action type
  if (action_type == "time in") {
    description = "Greetings @" + username + "!\n\n" +
                  "**" + name + "** has successfully **timed in**. ✅";
    color = 0x0099FF; // Blue
  } else if (action_type == "time out") {
    description = "Goodbye @" + username + "!\n\n" +
                  "**" + name + "** has successfully **timed out**. 👋";
    color = 0x00FF00; // Green
  } else {
    description = "Hello @" + username + "!\n\n" +
                  "Attendance recorded for **" + name + "**. ✅";
    color = 0x00FF00; // Green
  }

  // Convert String to const char* for embed_message
  unsigned char content[description.length() + 1];
  description.getBytes(content, description.length() + 1);
  
  return embed_message(title, (const char *)content, color);
}

/**
 * Generate unauthorized access security alert message
 * @return Discord embed JSON string for security notification
 */
String denied_message() {
  const char *title = "❌ [ACCESS DENIED] Automated Gatepass Message";
  String description = "**UNAUTHORIZED ACCESS ATTEMPT**\n\n" +
                      String("An unregistered RFID card was used to attempt ") +
                      String("facility access.\n The request has been **DENIED** and logged.");

  // Convert String to const char* for embed_message
  unsigned char content[description.length() + 1];
  description.getBytes(content, description.length() + 1);
  
  return embed_message(title, (const char *)content, 0xFF0000); // Red
}