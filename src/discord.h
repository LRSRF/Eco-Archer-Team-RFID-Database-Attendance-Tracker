/**
 * Discord Webhook Integration
 * Handles secure HTTP communication with Discord API
 */

#include <Arduino.h>
#include <HTTPClient.h>
#include <secrets.h>

// Discord webhook configuration
const String discord_webhook = DISCORD_API;
const String discord_tts = DISCORD_TTS;

const char *DISCORD_CERT = R"(
-----BEGIN CERTIFICATE-----
MIIDejCCAmKgAwIBAgIQf+UwvzMTQ77dghYQST2KGzANBgkqhkiG9w0BAQsFADBX
MQswCQYDVQQGEwJCRTEZMBcGA1UEChMQR2xvYmFsU2lnbiBudi1zYTEQMA4GA1UE
CxMHUm9vdCBDQTEbMBkGA1UEAxMSR2xvYmFsU2lnbiBSb290IENBMB4XDTIzMTEx
NTAzNDMyMVoXDTI4MDEyODAwMDA0MlowRzELMAkGA1UEBhMCVVMxIjAgBgNVBAoT
GUdvb2dsZSBUcnVzdCBTZXJ2aWNlcyBMTEMxFDASBgNVBAMTC0dUUyBSb290IFI0
MHYwEAYHKoZIzj0CAQYFK4EEACIDYgAE83Rzp2iLYK5DuDXFgTB7S0md+8Fhzube
Rr1r1WEYNa5A3XP3iZEwWus87oV8okB2O6nGuEfYKueSkWpz6bFyOZ8pn6KY019e
WIZlD6GEZQbR3IvJx3PIjGov5cSr0R2Ko4H/MIH8MA4GA1UdDwEB/wQEAwIBhjAd
BgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUHAwIwDwYDVR0TAQH/BAUwAwEB/zAd
BgNVHQ4EFgQUgEzW63T/STaj1dj8tT7FavCUHYwwHwYDVR0jBBgwFoAUYHtmGkUN
l8qJUC99BM00qP/8/UswNgYIKwYBBQUHAQEEKjAoMCYGCCsGAQUFBzAChhpodHRw
Oi8vaS5wa2kuZ29vZy9nc3IxLmNydDAtBgNVHR8EJjAkMCKgIKAehhxodHRwOi8v
Yy5wa2kuZ29vZy9yL2dzcjEuY3JsMBMGA1UdIAQMMAowCAYGZ4EMAQIBMA0GCSqG
SIb3DQEBCwUAA4IBAQAYQrsPBtYDh5bjP2OBDwmkoWhIDDkic574y04tfzHpn+cJ
odI2D4SseesQ6bDrarZ7C30ddLibZatoKiws3UL9xnELz4ct92vID24FfVbiI1hY
+SW6FoVHkNeWIP0GCbaM4C6uVdF5dTUsMVs/ZbzNnIdCp5Gxmx5ejvEau8otR/Cs
kGN+hr/W5GvT1tMBjgWKZ1i4//emhA1JG1BbPzoLJQvyEotc03lXjTaCzv8mEbep
8RqZ7a2CPsgRbuvTPBwcOMBBmuFeU88+FSBX6+7iP0il8b4Z0QFqIwwMHfs/L6K1
vepuoxtGzi4CZ68zJpiq1UvSqTbFJjtbD4seiMHl
-----END CERTIFICATE-----
)";

/**
 * Send message to Discord webhook
 * @param content Plain text message content
 * @param embed_json JSON string for Discord embed (optional)
 */
void send_discord(String content, String embed_json) {
  WiFiClientSecure *client = new WiFiClientSecure;
  if (!client) {
    Serial.println("Failed to create secure client for Discord");
    return;
  }
  
  client->setCACert(DISCORD_CERT);
  HTTPClient https;
  
  if (https.begin(*client, discord_webhook)) {
    https.addHeader("Content-Type", "application/json");

    // Construct Discord webhook payload
    String json_payload = "{\"content\":\"" + content + 
                         "\",\"tts\":" + discord_tts;
    
    if (embed_json.length() > 0) {
      json_payload += ",\"embeds\":[" + embed_json + "]";
    }
    json_payload += "}";

    int http_code = https.POST(json_payload);

    if (http_code == HTTP_CODE_OK || http_code == HTTP_CODE_NO_CONTENT) {
      Serial.println("Discord notification sent successfully");
    } else {
      Serial.println("Discord notification failed - HTTP " + String(http_code));
    }

    https.end();
  } else {
    Serial.println("Failed to connect to Discord webhook");
  }
  
  delete client;
}

/**
 * Send plain text message to Discord
 * @param content Message text
 */
void send_discord_message(String content) { 
  send_discord(content, ""); 
}

/**
 * Send embed message to Discord
 * @param embeds JSON embed string
 */
void send_discord_embeds(String embeds) {
  send_discord("", embeds);
}
