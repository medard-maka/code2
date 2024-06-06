

#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <SPI.h>
#include <MFRC522.h>

const char *ssid = "McMeder";
const char *password = "mcmeder91";

const int RST_PIN = 22;
const int SS_PIN = 21;

MFRC522 mfrc522(SS_PIN, RST_PIN); // Créer une instance du module RFID-RC522

AsyncWebServer server(80);

void setup()
{
  Serial.begin(115200);

  // Initialiser le système de fichiers SPIFFS
  if (!SPIFFS.begin())
  {
    Serial.println("Erreur SPIFFS...");
    return;
  }

  // Initialiser le module RFID-RC522
  SPI.begin();
  mfrc522.PCD_Init();

  // Connecter le WiFi
  WiFi.begin(ssid, password);
  Serial.print("Tentative de connexion...");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
  }
  Serial.println("\nConnexion établie!");
  Serial.print("Adresse IP: ");
  Serial.println(WiFi.localIP());

  // Définir les routes du serveur web
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/readRFID", HTTP_GET, [](AsyncWebServerRequest *request)
  {
    if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial())
    {
      String cardID = "";
      for (byte i = 0; i < mfrc522.uid.size; i++)
      {
        cardID += String(mfrc522.uid.uidByte[i], HEX);
      }
      request->send(200, "text/plain", cardID);
    }
    else
    {
      request->send(200, "text/plain", "");
    }
  });

  server.begin();
  Serial.println("Serveur actif!");
}

void loop()
{
  // Vérifier si une nouvelle carte RFID est présente
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial())
  {
    // Récupérer l'ID de la carte
    String cardID = "";
    for (byte i = 0; i < mfrc522.uid.size; i++)
    {
      cardID += String(mfrc522.uid.uidByte[i], HEX);
    }
    Serial.println("ID de la carte : " + cardID);
  }
}
