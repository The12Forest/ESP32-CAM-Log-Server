#define URL_PREFIX_COUNTER "count"
#define URL_PREFIX_LOG "log"
#define URL_PREFIX_READ "read"
#define URL_PREFIX_LS "ls"
#define URL_PREFIX_MKDIR "mkdir"
#define URL_PREFIX_RM "rm"
#define URL_PREFIX_ADDPC "addpc"
#define URL_PREFIX_RMPC "rmpc"
#define URL_PREFIX_READLOG "readlog"

#define URL_PREFIX_LENGTH_COUNTER (sizeof(URL_PREFIX_COUNTER) + 1)
#define URL_PREFIX_LENGTH_LOG (sizeof(URL_PREFIX_LOG) + 1)
#define URL_PREFIX_LENGTH_READ (sizeof(URL_PREFIX_READ) + 1)
#define URL_PREFIX_LENGTH_LS (sizeof(URL_PREFIX_LS) + 1)
#define URL_PREFIX_LENGTH_MKDIR (sizeof(URL_PREFIX_MKDIR) + 1)
#define URL_PREFIX_LENGTH_RM (sizeof(URL_PREFIX_RM) + 1)
#define URL_PREFIX_LENGTH_ADDPC (sizeof(URL_PREFIX_ADDPC) + 1)
#define URL_PREFIX_LENGTH_RMPC (sizeof(URL_PREFIX_RMPC) + 1)
#define URL_PREFIX_LENGTH_READLOG (sizeof(URL_PREFIX_READLOG) + 1)

#define LEDPINN 48  //ESP32 CAM Pin: 33 ESP32 S3 Dev Module Pin: 48

#define ESP32_NAME "ESP32_Loger"
#define OTA_Password "admin"
#define OTA_Port 3232

#define SD_MMC_CMD  38 //SD Card Pin
#define SD_MMC_CLK  39 //SD Card Pin
#define SD_MMC_D0   40 //SD Card Pin

/*
 _       ____    ___    _   __   ____                                           __           
| |     / / /   /   |  / | / /  / __ \____ ____________      ______  ___  _____/ /____  _____
| | /| / / /   / /| | /  |/ /  / /_/ / __ `/ ___/ ___/ | /| / / __ \/ _ \/ ___/ __/ _ \/ ___/
| |/ |/ / /___/ ___ |/ /|  /  / ____/ /_/ (__  |__  )| |/ |/ / /_/ /  __/ /  / /_/  __/ /    
|__/|__/_____/_/  |_/_/ |_/  /_/    \__,_/____/____/ |__/|__/\____/\___/_/   \__/\___/_/                                                                                             
*/

const char* ssid = "{WLAN Name}";
const char* password = "{Wlan Passwort}";




