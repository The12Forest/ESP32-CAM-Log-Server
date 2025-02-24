# ESP32 Webserver with SD Card Logging

This project is an ESP32-based web server that interacts with an SD card. It allows users to create and delete directories, log messages, and retrieve log files using HTTP requests.

## Features

- Create and delete directories on the SD card
- Log messages with timestamps
- Read log files
- List directory contents
- Remote reboot functionality
- OTA update capability

## Requirements

- ESP32 microcontroller (i use ESP32-S3 Wroom CAM with integrated SD Card) 
- SD card and SD card module (SD\_MMC)
- WiFi connection
- Arduino IDE

## Installation and Setup

### 1. Install Arduino IDE and ESP32 Board Package

1. Download and install the latest [Arduino IDE](https://www.arduino.cc/en/software).
2. Open Arduino IDE and go to **File** → **Preferences**.
3. In the "Additional Board Manager URLs" field, add:
   ```
   https://dl.espressif.com/dl/package_esp32_index.json
   ```
4. Go to **Tools** → **Board** → **Board Manager** and install "ESP32 by Espressif Systems". Use ther version 2.0.14.

### 2. Install Required Libraries

Install the following libraries via the **Library Manager** (Sketch → Include Library → Manage Libraries):

- `WiFi.h`
- `SD_MMC.h`
- `ArduinoOTA.h`
- `WebServer.h`

### 3. Flashing the Code

1. Connect the ESP32 to your PC via USB.
2. Open the provided `.ino` file in Arduino IDE.
3. Modify `login.h` to include your WiFi credentials:
   ```cpp
   const char* ssid = "YOUR_WIFI_SSID";
   const char* password = "YOUR_WIFI_PASSWORD";
   ```
4. Select the correct **Board** (ESP32 Dev Module) and **Port** under the "Tools" menu.
5. Click the **Upload** button.

## API Endpoints

Once the ESP32 is connected to WiFi, you can interact with it using HTTP requests:

| Endpoint                     | Description                                                 |
| ---------------------------- | ----------------------------------------------------------- |
| `/`                          | Displays available API endpoints                            |
| `/reboot`                    | Reboots the ESP32                                           |
| `/addpc/{name}`              | Creates a new directory for a PC in `/Log/` and `/Counter/` |
| `/rmpc/{name}`               | Deletes a PC directory from `/Log/` and `/Counter/`         |
| `/mkdir/{path}`              | Creates a directory at the specified path                   |
| `/rm/{path}`                 | Deletes a file or directory                                 |
| `/ls/{dir}`                  | Lists contents of a directory                               |
| `/read/{computer}/{list}`    | Reads a log file from the counter directory                 |
| `/log/{computer}/{message}`  | Logs a message in the respective log file                   |
| `/counter/{computer}/{list}` | Increments the counter for a specific list                  |

## Example Usage

Using `curl`:

```sh
curl http://esp32.local/reboot
curl http://esp32.local/mkdir//new_folder
curl http://esp32.local/log/computer1/"System started"
```

or use the other ESP32-CAM Code

## Troubleshooting

- Ensure the correct COM port and board are selected in Arduino IDE.
- Check the serial monitor for debugging information.
- Ensure the SD card is formatted correctly (FAT32 recommended).
- Verify WiFi credentials in `login.h`.
- Use partition scheme Minimal SPIFFS

## License

This project is open-source and can be modified as needed.

