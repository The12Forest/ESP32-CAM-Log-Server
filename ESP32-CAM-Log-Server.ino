/* 
Tasks
  See Status of all 
*/

#include <WiFi.h>           //Für das Wlan
#include "FS.h"             //Für die SD Karte
#include <SD_MMC.h>         //Für die SD Karte
#include <Arduino.h>        //Für default Arduino
#include <WiFiClient.h>     //Keine Ahnung
#include <WebServer.h>      //Für den Webserver
#include <time.h>           //Für die Zeit
#include <string.h>         //Für das öfnen der Dateien
#include <ESPmDNS.h>        //Für den Hostname               //OTA
#include <WiFiUdp.h>        //Für das UDP over Wifi          //OTA
#include <ArduinoOTA.h>     //Für das akzeptieren der OTA    //OTA
#include "login.h"          //Für Passwörter
#include <DHT.h>            //Für den DHT Sensor

WebServer server(80);
unsigned long previousMillis;
DHT dht(DHT22_PIN, DHT22);
unsigned long lastDHTRead = 0;

void handleDHT() {
  if (millis() - lastDHTRead < 2000) return;  // Alle 2 Sekunden messen!
  lastDHTRead = millis();
  float humi  = dht.readHumidity();
  float tempC = dht.readTemperature();
  WiFiClient client = server.client();

  // if ( isnan(tempC) || isnan(humi)) {
  //   Serial.println("Failed to read from DHT22 sensor!");
  //   server.send(400, "text/plain", String("Failed to read from DHT22 sensor!"));
  // } else {
    Serial.print("Humidity: ");
    Serial.print(humi);
    Serial.print("%");

    Serial.print("  |  ");

    Serial.print("Temperature: ");
    Serial.print(tempC);
    Serial.print("°C");
    if (!client.connected()) {
      client.stop();
      return;
    }
    client.println(F("HTTP/1.1 200 OK"));
    client.println(F("Content-Type: application/json"));
    client.println(F("Connection: close\r\n"));
    String buffer = "{\"ok\":true, \"Temperature\":" + String(tempC) + ", \"Humidity\":" + String(humi) + "}";
    client.println(buffer);  // Send the constructed JSON to the client
    client.stop();
  // }

}

void handleRREADLOG(fs::FS &fs) {
  String fullURI = server.uri();
  String fileContent = "";
  if (fullURI.startsWith("/" URL_PREFIX_READLOG "/")) {
    String Computer = fullURI.substring(URL_PREFIX_LENGTH_READLOG);
    File file = fs.open("/Log/" + Computer + "/log" + Day() + ".txt", FILE_READ);
    if (!file) {
      Serial.println("File not found or can't open the file.");
      server.send(400, "text/plain", String("File not found or can't open the file."));
    } else {
      while (file.available()) {
        fileContent += (char)file.read();
      }
      file.close();
      Serial.println("The log is read");
      server.send(200, "text/plain", String(fileContent));
    }
  } else {
      server.send(400, "text/plain", String("Failed: No directory provided!"));
  }
}

void handleADDPC(fs::FS &fs) {
  int control = 0;
  String fullURI = server.uri();
  if (fullURI.startsWith("/" URL_PREFIX_ADDPC "/")) {
    String foldername = fullURI.substring(URL_PREFIX_LENGTH_ADDPC);
    String fullpath1 = "/Log/" + foldername;
    String fullpath2 = "/Counter/" + foldername;
    if (!SD_MMC.exists(fullpath1)) {
      if (SD_MMC.mkdir(fullpath1)) {
          Serial.println("Verzeichnis erstellt: " + fullpath1);
          control++;
      } else {
          Serial.println("Fehler beim Erstellen von: " + fullpath1);
      }
    } else {
      Serial.println("Fehler beim Erstellen von: " + fullpath1);
    }
    if (!SD_MMC.exists(fullpath2)) {
      if (SD_MMC.mkdir(fullpath2)) {
          Serial.println("Verzeichnis erstellt: " + fullpath2);
          control++;
      } else {
          Serial.println("Fehler beim Erstellen von: " + fullpath2);
      }
    } else {
      Serial.println("Fehler beim Erstellen von: " + fullpath2);
    }
    if ((control = 2)) {
      server.send(200, "text/plain", String("Folder: '" + fullpath1 + "' and '" + fullpath2 + "' created."));
    } else {
      server.send(404, "text/plain", String("Creating folders faild."));
    }
  }
}

void handleRMPC(fs::FS &fs) {
  int control = 0;
  String fullURI = server.uri();
  if (fullURI.startsWith("/" URL_PREFIX_RMPC "/")) {
    String foldername = fullURI.substring(URL_PREFIX_LENGTH_RMPC);
    String fullpath1 = "/Log/" + foldername;
    String fullpath2 = "/Counter/" + foldername;
    if (SD_MMC.exists(fullpath1)) {
      if (fs.rmdir(fullpath1)) {
          Serial.println("Verzeichnis löschen: " + fullpath1);
          control++;
      } else {
          Serial.println("Fehler beim Löschen von: " + fullpath1);
      }
    } else {
      Serial.println("Fehler beim Löschen von: " + fullpath1);
    }
    if (SD_MMC.exists(fullpath2)) {
      if (fs.rmdir(fullpath2)) {
          Serial.println("Verzeichnis löschen: " + fullpath2);
          control++;
      } else {
          Serial.println("Fehler beim Löschen von: " + fullpath2);
      }
    } else {
      Serial.println("Fehler beim Löschen von: " + fullpath2);
    }
    if ((control = 2)) {
      server.send(200, "text/plain", String("Folder: '" + fullpath1 + "' and '" + fullpath2 + "' removed."));
    } else {
      server.send(404, "text/plain", String("Removing folders faild."));
    }
  }
}

void handleMKDIR(fs::FS &fs) {
  String fullURI = server.uri();
  if (fullURI.startsWith("/" URL_PREFIX_MKDIR "/")) {
    String path = fullURI.substring(URL_PREFIX_LENGTH_MKDIR);
    char tempPath[256];
    snprintf(tempPath, sizeof(tempPath), "%s", path.c_str());

    for (char* p = tempPath + 1; *p; p++) {
      if (*p == '/') {
        *p = '\0';
        if (!SD_MMC.exists(tempPath)) {
          if (SD_MMC.mkdir(tempPath)) {
              Serial.printf("Verzeichnis erstellt: %s\n", tempPath);
          } else {
              Serial.printf("Fehler beim Erstellen von: %s\n", tempPath);
          }
        }
        *p = '/';
      }
    }

    // Letztes Verzeichnis im Pfad erstellen
    if (!SD_MMC.exists(tempPath)) {
      if (SD_MMC.mkdir(tempPath)) {
          Serial.println("Verzeichnis erstellt: " + path);
          server.send(200, "text/plain", String("Folder: '" + path + "' created."));
      } else {
          Serial.println("Fehler beim Erstellen von: " + path);
          server.send(404, "text/plain", String("Faild to create folder: '" + path + "'."));
      }
    }
  }
}

void handleRM(fs::FS &fs) {
  String fullURI = server.uri();
  if (fullURI.startsWith("/" URL_PREFIX_RM "/")) {
    String foldername = fullURI.substring(URL_PREFIX_LENGTH_RM);
    if (strstr(foldername.c_str(), ".txt") != NULL) {
      if (!fs.remove(foldername)) {
          Serial.print("Error removing directory: ");
          Serial.println(foldername);
          server.send(200, "text/plain", "Error removing file: " + foldername);
          return;
      }
      Serial.println(String("Folder: " + foldername + " created."));
      server.send(200, "text/plain", String("File: " + foldername + " removed."));
    } else {
      if (!fs.rmdir(foldername)) {
          Serial.print("Error removing directory: ");
          Serial.println(foldername);
          server.send(200, "text/plain", "Error removing directory: " + foldername);
          return;
      }
      Serial.println(String("Folder: " + foldername + " created."));
      server.send(200, "text/plain", String("Folder: " + foldername + " removed."));
    }
  }
}

void handlels(fs::FS &fs) {
  String fullURI = server.uri();
  String fileContent = "";
  if (fullURI.startsWith("/" URL_PREFIX_LS "/")) {
    // Extract the directory path after the prefix
    String dirPath = fullURI.substring(URL_PREFIX_LENGTH_LS);
    if (strstr(dirPath.c_str(), ".txt") != NULL) {
      File file = SD_MMC.open(dirPath, FILE_READ);
      if (!file) {
        Serial.println("File not found or can't open the file.");
        server.send(400, "text/plain", String("File not found or can't open the file."));
      } else {
        while (file.available()) {
          fileContent += (char)file.read();
        }
        file.close();
        Serial.println("The file is read");
        server.send(200, "text/plain", String(fileContent));
      }
    } else {
      if (dirPath.length() > 0) {
          File dir = fs.open(dirPath);
          if (dir && dir.isDirectory()) {
              String fileList = "";
              File file = dir.openNextFile();
              while (file) {
                  fileList += String(file.name()) + "\n";
                  file = dir.openNextFile();
              }
              dir.close();
              server.send(200, "text/plain", fileList);
              Serial.println("Directory listing for: " + dirPath);
          } else {
              server.send(404, "text/plain", "Failed: Directory not found");
              Serial.println("Error: Directory not found - " + dirPath);
          }
      } else {
          server.send(400, "text/plain", String("Failed: No directory provided!"));
      }
    }
  }
}

void handleREAD(fs::FS &fs) {
  String fullURI = server.uri();
  if (fullURI.startsWith("/" URL_PREFIX_READ "/")) {
    String parameters = fullURI.substring(URL_PREFIX_LENGTH_READ);

    int separatorIndex = parameters.indexOf('/');
    if (separatorIndex != -1) {
      String Computer = parameters.substring(0, separatorIndex);  // "t"
      String List = parameters.substring(separatorIndex + 1);     // "12"

      String filePath = "/Counter/" + String(Computer) + "/" + String(List) + ".txt";
      String fileContent = "";

      // Try opening the file for reading
      File file = SD_MMC.open(filePath, FILE_READ);
      if (!file) {
        Serial.println("File not found. Creating a new file.");
        // Create a new file with initial content
        file = SD_MMC.open(filePath, FILE_WRITE);
        if (!file) {
          Serial.println("Read failed, can't acces the file.");
        }
      } else {
        // File exists; read its content
        while (file.available()) {
          fileContent += (char)file.read();
        }
        file.close();
        // Increment the number and write it back
        int intFile = fileContent.toInt();
        server.send(200, "text/plain", String(intFile));
        Serial.println("Count successfully processed from " + Computer + "_ESP for list " + List + " to " + intFile + ".");
      }
    } else {
      server.send(400, "text/plain", "Faild: No syntax after /" URL_PREFIX_COUNTER "/!");
    }
  }
}


void handleCount(fs::FS &fs) {
  server.send(200, "text/plain", "Count is processing.");
  String fullURI = server.uri();
  if (fullURI.startsWith("/" URL_PREFIX_COUNTER "/")) {
    String parameters = fullURI.substring(URL_PREFIX_LENGTH_COUNTER);

    int separatorIndex = parameters.indexOf('/');
    if (separatorIndex != -1) {
      String Computer = parameters.substring(0, separatorIndex);  // "t"
      String List = parameters.substring(separatorIndex + 1);     // "12"

      String filePath = "/Counter/" + String(Computer) + "/" + String(List) + ".txt";
      String fileContent = "";

      // Try opening the file for reading
      File file = SD_MMC.open(filePath, FILE_READ);
      if (!file) {
        Serial.println("File not found. Creating a new file.");
        // Create a new file with initial content
        file = SD_MMC.open(filePath, FILE_WRITE);
        if (file) {
          file.print("0");  // Write initial number
          file.close();
          Serial.println("File created with initial value 1.");
        } else {
          Serial.println("Failed to create file.");
          return;
        }
      } else {
        // File exists; read its content
        while (file.available()) {
          fileContent += (char)file.read();
        }
        file.close();

        // Increment the number and write it back
        int intFile = fileContent.toInt();
        intFile++;
        file = SD_MMC.open(filePath, FILE_WRITE);
        if (file) {
          file.print(intFile);
          file.close();
          // Serial.print("Updated file content: ");
          // Serial.println(intFile);
        } else {
          Serial.println("Failed to open file for writing.");
        }
        Serial.println("Count successfully processed from " + Computer + " for list " + List + " to " + intFile + ".");
      }
    } else {
      // Fehler senden, wenn kein Wert angegeben ist
      server.send(400, "text/plain", "Fehler: Kein Wert nach /" URL_PREFIX_COUNTER "/ angegeben!");
    }
  }
}

void handleLOG(FS &fs) {
  server.send(200, "text/plain", "LOG is Saving.");
  String fullURI = server.uri();
  if (fullURI.startsWith("/" URL_PREFIX_LOG "/")) {
    String parameters = fullURI.substring(URL_PREFIX_LENGTH_LOG);

    int separatorIndex = parameters.indexOf('/');
    if (separatorIndex != -1) {
      String Computer = parameters.substring(0, separatorIndex);  // "t"
      String message = parameters.substring(separatorIndex + 1);  // "12"


      while (message.indexOf("%20") != -1) {
        message.replace("%20", " ");
      }

      File file = fs.open("/Log/" + Computer + "/log" + Day() + ".txt", FILE_APPEND);  // Ändere FILE_WRITE zu FILE_APPEND
      if (!file) {
        Serial.println("Failed to open file for writing");
        // return;

        Serial.println("Generating New file.");
        File file = fs.open("/Log/" + Computer + "/log" + Day() + ".txt", FILE_WRITE);
        if (!file) {
          Serial.println("Generating new file faild.");
          return;
        }
      }
      if (!file.println("[" + Time() + "]     " + message)) {  // Neue Zeile wird automatisch durch println hinzugefügt
        Serial.println("Write failed");
      }
      file.close();


      Serial.println("LOG successfully processed from " + Computer + " with Content " + message + ".");
      // server.send(200, "text/plain", "LOG successfully processed from " + Computer + "_ESP with Content " + message + ".");
    } else {
      // Fehler senden, wenn kein Wert angegeben ist
      server.send(400, "text/plain", "Fehler: Kein Wert nach /" URL_PREFIX_COUNTER "/ angegeben!");
    }
  }
}

void handleReboot() {
  Serial.println(String(ESP32_NAME) + " will reboot now!");
  server.send(200, "text/plain", String(ESP32_NAME) + " will reboot now!");
  ESP.restart();
}

void handleRoot() {
  String message = "API DEFINITION\n==============\n\n";
  message += "Version 1.0:\n";
  message += "http://" + WiFi.localIP().toString() +"/\n";
  message += "http://" + WiFi.localIP().toString() +"/reboot\n";
  message += "http://" + WiFi.localIP().toString() +"/dht\n";
  message += "http://" + WiFi.localIP().toString() +"/" + URL_PREFIX_COUNTER + "/{Computer}/{List}\n";
  message += "http://" + WiFi.localIP().toString() +"/" + URL_PREFIX_LOG + "/{Computer}/{Message}\n";
  message += "http://" + WiFi.localIP().toString() +"/" + URL_PREFIX_READLOG + "/{Computer}\n";
  message += "http://" + WiFi.localIP().toString() +"/" + URL_PREFIX_READ + "/{Computer}/{List}\n";
  message += "http://" + WiFi.localIP().toString() +"/" + URL_PREFIX_MKDIR + "/{Full path}\n";
  message += "http://" + WiFi.localIP().toString() +"/" + URL_PREFIX_RM + "/{full path, alsow working with file.}\n";
  message += "http://" + WiFi.localIP().toString() +"/" + URL_PREFIX_LS + "/{Name of dir '/' for root dir, alsow working with file. (Prints the Content)}\n";
  message += "http://" + WiFi.localIP().toString() +"/" + URL_PREFIX_ADDPC + "/{name of new CAM}\n";
  message += "http://" + WiFi.localIP().toString() +"/" + URL_PREFIX_RMPC + "/{name of old CAM}\n";
  server.send(200, "text/plain", message);
  Serial.println("Client " + server.client().remoteIP().toString() + " connected while searching for Root.");
}


void NotFound() {
  if (server.uri().startsWith("/" URL_PREFIX_COUNTER "/")) {
    handleCount(SD_MMC);  // Funktion aufrufen
  } else if (server.uri().startsWith("/" URL_PREFIX_LOG "/")) {
    handleLOG(SD_MMC);
  } else if (server.uri().startsWith("/" URL_PREFIX_READ "/")) {
    handleREAD(SD_MMC);
  } else if (server.uri().startsWith("/" URL_PREFIX_LS "/")) {
    handlels(SD_MMC);
  } else if (server.uri().startsWith("/" URL_PREFIX_MKDIR "/")) {
    handleMKDIR(SD_MMC);
  } else if (server.uri().startsWith("/" URL_PREFIX_RM "/")) {
    handleRM(SD_MMC);
  } else if (server.uri().startsWith("/" URL_PREFIX_ADDPC "/")) {
    handleADDPC(SD_MMC);
  } else if (server.uri().startsWith("/" URL_PREFIX_RMPC "/")) {
    handleRMPC(SD_MMC);
  } else if (server.uri().startsWith("/" URL_PREFIX_READLOG "/")) {
    handleRREADLOG(SD_MMC);
  } else {
    server.send(404, "text/plain", "Not Found while searching for " + server.uri());
  }
}

String Time() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("No time available (yet)");
    return "Keine Zeit verfügbar";
  }
  char timeStr[60];
  // Verwenden von sprintf, um die Zeit im gewünschten Format zu speichern
  sprintf(timeStr, "%04d,%02d,%02d %02d:%02d:%02d",
          (timeinfo.tm_year + 1900), (timeinfo.tm_mon + 1), timeinfo.tm_mday,
          timeinfo.tm_hour + 1, timeinfo.tm_min, timeinfo.tm_sec);
  // Zeit ausgeben
  // Serial.println(timeStr);
  return String(timeStr);
}

String Day() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("No time available (yet)");
    return "Keine Zeit verfügbar";
  }
  char timeStr[30];
  snprintf(
    timeStr, sizeof(timeStr), "_%04d_%02d_%02d",
    (timeinfo.tm_year + 1900), (timeinfo.tm_mon + 1), timeinfo.tm_mday);
  return String(timeStr);
}

void initSD() {
  if (!SD_MMC.setPins(SD_MMC_CLK, SD_MMC_CMD, SD_MMC_D0)) {
    Serial.println("Set Pins faild");
    return;
  } else {
    Serial.println("Successfully set Pins");
  }
  if (!SD_MMC.begin("/sdcard", true, true, SDMMC_FREQ_DEFAULT, 5)) {
    Serial.println("Card Mount Failed.");
    uint8_t cardType = SD_MMC.cardType();
    if (cardType == CARD_NONE) {
      Serial.println("No SD card attached");
    }
    return;
  } else {
    uint8_t cardType = SD_MMC.cardType();
    Serial.println("Card Mount succesfull");
    Serial.print("SD Card Type: ");
    if (cardType == CARD_MMC) {
      Serial.println("MMC");
    } else if (cardType == CARD_SD) {
      Serial.println("SDSC");
    } else if (cardType == CARD_SDHC) {
      Serial.println("SDHC");
    } else {
      Serial.println("UNKNOWN");
    }
    uint64_t cardSize = SD_MMC.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);
  }
}

void initServer() {
  server.on("/reboot", handleReboot);  // Wenn /run aufgerufen wird, handleRun
  server.on("/dht", handleDHT);
  server.on("/", handleRoot);
  server.onNotFound(NotFound);
  server.begin();
  Serial.println("Webserver gestartet!");
}

void initWifi() {
  int round = 0;
  WiFi.disconnect(true);
  Serial.print("Connecting to ");
  Serial.print(ssid);
  // writeLog(SD_MMC, "Connecting to");
  // writeLog(SD_MMC, ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
    if (round > 35) {
      ESP.restart();
    }
    round++;
  }
  Serial.println("");
  String ip = WiFi.localIP().toString();
  Serial.print("WiFi connected: ");
  Serial.println(WiFi.localIP());
}

void initTime() {
  const char *ntpServer1 = "2.ch.pool.ntp.org";
  const char *ntpServer2 = "3.ch.pool.ntp.org";
  const long gmtOffset_sec = 0;
  const int daylightOffset_sec = 0;
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);
}

void initOTA() {
  // Port defaults to 3232
  ArduinoOTA.setPort(OTA_Port);

  // Hostname defaults to esp3232-[MAC]
  ArduinoOTA.setHostname(ESP32_NAME);

  // No authentication by default
  ArduinoOTA.setPassword(OTA_Password);

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();
}

void setup() {
  pinMode(LEDPINN, OUTPUT);
  digitalWrite(LEDPINN, LOW);
  delay(100);
  digitalWrite(LEDPINN, HIGH);
  delay(100);
  digitalWrite(LEDPINN, LOW);
  Serial.begin(115200);
  initSD();
  initWifi();
  initTime();
  initServer();
  initOTA();
  dht.begin();
  Serial.println("Time is: " + Time());
  Serial.println("Server startup succesfull!");
  delay(2000);
  digitalWrite(LEDPINN, HIGH);
}


void loop() {
  server.handleClient();
  ArduinoOTA.handle();
  delay(200);
  if (WiFi.status() != WL_CONNECTED) {
    Serial.print(F("WiFi connection lost. Going for restart ..."));
    ESP.restart();
  }
  if (millis() / 1000 > 3600) {
    ESP.restart();
  }
}
