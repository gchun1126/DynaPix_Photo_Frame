#include <SPI.h>
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <Preferences.h>
#include <ESPmDNS.h>
#include "brand_assets.h"
#include "extension_clock_weather.h"
#include "extension_calendar.h"

// ============================================================
// DYNAPIX PHOTO FRAME
// ESP32-S3 N16R8 + Waveshare 4-inch E6
// 600 x 400 landscape UI
// 400 x 600 native panel memory
// ============================================================

// Initial Wi-Fi used until changed from Settings.
const char* BOOTSTRAP_WIFI_SSID = "GaryWireN";
const char* BOOTSTRAP_WIFI_PASS = "garychun";

// Permanent fallback access point.
const char* FALLBACK_AP_SSID = "DynaPix-EPaper";
const char* FALLBACK_AP_PASS = "dynapix6";

// Hardware pins.
#define DEV_POWER_PIN 9
#define EPD_CS_PIN 10
#define EPD_DC_PIN 8
#define EPD_RST_PIN 14
#define EPD_BUSY_PIN 13
#define EPD_MOSI_PIN 11
#define EPD_SCK_PIN 12

// Display.
#define DISPLAY_WIDTH 600
#define DISPLAY_HEIGHT 400
#define PANEL_RAM_WIDTH 400
#define PANEL_RAM_HEIGHT 600
#define EPD_PIXEL_COUNT 240000
#define EPD_IMAGE_BYTES 120000
#define SPI_FREQUENCY 4000000

// Photo library.
#define MAX_PHOTOS 90
#define PHOTO_DIRECTORY "/photos"
#define LIBRARY_FILE "/library.txt"
#define LIBRARY_TEMP_FILE "/library.tmp"
#define LIBRARY_BACKUP_FILE "/library.bak"

#define MAX_CATEGORIES 32
#define CATEGORIES_FILE "/categories.txt"
#define CATEGORIES_TEMP_FILE "/categories.tmp"
#define CATEGORIES_BACKUP_FILE "/categories.bak"

#define UNCATEGORIZED_CATEGORY_ID "__uncategorized__"

// Slideshow.
#define MIN_SLIDE_INTERVAL 30
#define DEFAULT_SLIDE_INTERVAL 300

struct PhotoRecord {
  String id;
  String name;
  String path;
  String categoryIds;

  bool selected;
  bool connectorRight;

  uint32_t hash;
};

PhotoRecord photos[MAX_PHOTOS];
int photoCount = 0;

struct CategoryRecord {
  String id;
  String name;
  String color;
};

CategoryRecord categories[MAX_CATEGORIES];
int categoryCount = 0;

WebServer server(80);
Preferences preferences;

// Device configuration.
String deviceName = "DynaPix Photo Frame";
String hostName = "dynapix";

String activeSSID;
String activePassword;
String panelMount = "left";

// Slideshow.
uint32_t slideIntervalSeconds = DEFAULT_SLIDE_INTERVAL;
String slideshowMode = "sequential";
bool slideshowRunning = false;

String slideshowCategoryIds = "";
String categoryMatchMode = "any";
String libraryCategoryFilter = "all";

String currentPhotoId = "";
unsigned long lastDisplayTime = 0;

// Shuffle queue.
int shuffleOrder[MAX_PHOTOS];
int shuffleCount = 0;
int shufflePosition = 0;

// Display navigation history.
String displayHistory[MAX_PHOTOS];
int historyCount = 0;
int historyPosition = -1;

bool historyNavigationRequest = false;
int requestedHistoryPosition = -1;

// Application status.
String applicationState = "starting";
String applicationMessage = "Starting DynaPix";

bool displayRequested = false;
bool displayRunning = false;

String requestedPhotoId;
String requestedPhotoPath;

// Deferred restart.
bool restartPending = false;
unsigned long restartAt = 0;

// Upload state.
File uploadFile;

String uploadId;
String uploadName;
String uploadTemporaryPath;
String uploadFinalPath;
String uploadError;

bool uploadConnectorRight = false;
bool uploadStarted = false;
bool uploadFinished = false;
bool uploadValid = false;

size_t uploadedBytes = 0;
uint32_t uploadHash = 2166136261UL;

// ============================================================
// GENERAL HELPERS
// ============================================================

String jsonEscape(const String& source) {
  String result;
  result.reserve(source.length() + 16);

  for (size_t i = 0; i < source.length(); i++) {
    char character = source.charAt(i);

    switch (character) {
      case '\\':
        result += "\\\\";
        break;

      case '"':
        result += "\\\"";
        break;

      case '\n':
        result += "\\n";
        break;

      case '\r':
        result += "\\r";
        break;

      case '\t':
        result += "\\t";
        break;

      default:
        if ((uint8_t)character >= 32) {
          result += character;
        }
        break;
    }
  }

  return result;
}

String sanitizeName(String name) {
  name.trim();

  name.replace("|", "-");
  name.replace("\r", " ");
  name.replace("\n", " ");

  while (name.indexOf("  ") >= 0) {
    name.replace("  ", " ");
  }

  if (name.length() == 0) {
    name = "Untitled Photo";
  }

  if (name.length() > 80) {
    name = name.substring(0, 80);
  }

  return name;
}

String sanitizeHostname(String value) {
  value.toLowerCase();

  String result;

  for (size_t i = 0; i < value.length(); i++) {
    char character = value.charAt(i);

    if (
      (character >= 'a' && character <= 'z') ||
      (character >= '0' && character <= '9') ||
      character == '-'
    ) {
      result += character;
    }
  }

  while (result.startsWith("-")) {
    result.remove(0, 1);
  }

  while (result.endsWith("-")) {
    result.remove(result.length() - 1);
  }

  if (result.length() == 0) {
    result = "dynapix";
  }

  if (result.length() > 30) {
    result = result.substring(0, 30);
  }

  return result;
}

String createPhotoId() {
  char buffer[17];

  snprintf(
    buffer,
    sizeof(buffer),
    "%08lx%08lx",
    (unsigned long)esp_random(),
    (unsigned long)esp_random()
  );

  return String(buffer);
}

int findPhotoIndex(const String& id) {
  for (int index = 0; index < photoCount; index++) {
    if (photos[index].id == id) {
      return index;
    }
  }

  return -1;
}

bool photoFileValid(const String& path) {
  File file = LittleFS.open(path, FILE_READ);

  if (!file) {
    return false;
  }

  bool valid = file.size() == EPD_IMAGE_BYTES;
  file.close();

  return valid;
}

uint32_t calculateFileHash(const String& path) {
  File file = LittleFS.open(path, FILE_READ);

  if (!file) {
    return 0;
  }

  uint32_t hash = 2166136261UL;
  uint8_t buffer[512];

  while (file.available()) {
    size_t count = file.read(buffer, sizeof(buffer));

    for (size_t index = 0; index < count; index++) {
      hash ^= buffer[index];
      hash *= 16777619UL;
    }

    yield();
  }

  file.close();

  return hash;
}

int selectedPhotoCount() {
  int count = 0;

  for (int index = 0; index < photoCount; index++) {
    if (photos[index].selected) {
      count++;
    }
  }

  return count;
}

bool hashAlreadyExists(uint32_t hash) {
  for (int index = 0; index < photoCount; index++) {
    if (photos[index].hash == hash) {
      return true;
    }
  }

  return false;
}


// ============================================================
// CATEGORY HELPERS
// ============================================================

String sanitizeCategoryName(String name) {
  name = sanitizeName(name);

  if (name.length() > 32) {
    name = name.substring(0, 32);
  }

  return name;
}

String sanitizeCategoryColor(String color) {
  color.trim();
  color.toLowerCase();

  if (
    color.length() != 7 ||
    color.charAt(0) != '#'
  ) {
    return "#16808a";
  }

  for (int index = 1; index < 7; index++) {
    char character = color.charAt(index);

    bool valid =
      (character >= '0' && character <= '9') ||
      (character >= 'a' && character <= 'f');

    if (!valid) {
      return "#16808a";
    }
  }

  return color;
}

String createCategoryId() {
  char buffer[9];

  snprintf(
    buffer,
    sizeof(buffer),
    "%08lx",
    (unsigned long)esp_random()
  );

  return String(buffer);
}

int findCategoryIndex(const String& id) {
  for (int index = 0; index < categoryCount; index++) {
    if (categories[index].id == id) {
      return index;
    }
  }

  return -1;
}

bool csvContainsId(
  const String& csv,
  const String& id
) {
  if (id.length() == 0) {
    return false;
  }

  String searchable = "," + csv + ",";
  String marker = "," + id + ",";

  return searchable.indexOf(marker) >= 0;
}

String csvAddId(
  String csv,
  const String& id
) {
  csv.trim();

  if (
    id.length() == 0 ||
    csvContainsId(csv, id)
  ) {
    return csv;
  }

  if (csv.length() > 0) {
    csv += ",";
  }

  csv += id;

  return csv;
}

String csvRemoveId(
  const String& csv,
  const String& id
) {
  String result;
  int start = 0;

  while (start <= csv.length()) {
    int comma = csv.indexOf(',', start);

    String token =
      comma < 0
        ? csv.substring(start)
        : csv.substring(start, comma);

    token.trim();

    if (
      token.length() > 0 &&
      token != id
    ) {
      result = csvAddId(result, token);
    }

    if (comma < 0) {
      break;
    }

    start = comma + 1;
  }

  return result;
}

String cleanCategoryCsv(
  const String& csv,
  bool allowUncategorized
) {
  String result;
  int start = 0;

  while (start <= csv.length()) {
    int comma = csv.indexOf(',', start);

    String token =
      comma < 0
        ? csv.substring(start)
        : csv.substring(start, comma);

    token.trim();

    bool valid =
      findCategoryIndex(token) >= 0;

    if (
      allowUncategorized &&
      token == UNCATEGORIZED_CATEGORY_ID
    ) {
      valid = true;
    }

    if (valid) {
      result = csvAddId(result, token);
    }

    if (comma < 0) {
      break;
    }

    start = comma + 1;
  }

  return result;
}

bool photoMatchesConfiguredCategories(
  int photoIndex
) {
  if (
    photoIndex < 0 ||
    photoIndex >= photoCount
  ) {
    return false;
  }

  if (slideshowCategoryIds.length() == 0) {
    return true;
  }

  int requestedCount = 0;
  int matchedCount = 0;
  int start = 0;

  while (
    start <= slideshowCategoryIds.length()
  ) {
    int comma =
      slideshowCategoryIds.indexOf(',', start);

    String categoryId =
      comma < 0
        ? slideshowCategoryIds.substring(start)
        : slideshowCategoryIds.substring(
            start,
            comma
          );

    categoryId.trim();

    if (categoryId.length() > 0) {
      requestedCount++;

      bool matched = false;

      if (
        categoryId ==
        UNCATEGORIZED_CATEGORY_ID
      ) {
        matched =
          photos[photoIndex].categoryIds.length() == 0;
      } else {
        matched = csvContainsId(
          photos[photoIndex].categoryIds,
          categoryId
        );
      }

      if (matched) {
        matchedCount++;
      }
    }

    if (comma < 0) {
      break;
    }

    start = comma + 1;
  }

  if (requestedCount == 0) {
    return true;
  }

  if (categoryMatchMode == "all") {
    return matchedCount == requestedCount;
  }

  return matchedCount > 0;
}

bool photoEligible(int photoIndex) {
  return
    photoIndex >= 0 &&
    photoIndex < photoCount &&
    photos[photoIndex].selected &&
    photoMatchesConfiguredCategories(photoIndex);
}

int eligiblePhotoCount() {
  int count = 0;

  for (int index = 0; index < photoCount; index++) {
    if (photoEligible(index)) {
      count++;
    }
  }

  return count;
}

void pauseSlideshowIfPoolEmpty() {
  if (
    slideshowRunning &&
    eligiblePhotoCount() == 0
  ) {
    slideshowRunning = false;

    preferences.putBool(
      "running",
      false
    );
  }
}


// ============================================================
// LIBRARY STORAGE AND RECOVERY
// ============================================================

void resetShuffleQueue() {
  shuffleCount = 0;
  shufflePosition = 0;
}

void resetDisplayHistory() {
  historyCount = 0;
  historyPosition = -1;
  historyNavigationRequest = false;
  requestedHistoryPosition = -1;
}


// ============================================================
// CATEGORY STORAGE
// ============================================================

bool writeCategoriesFile(const String& path) {
  LittleFS.remove(path);

  File file = LittleFS.open(path, FILE_WRITE);

  if (!file) {
    return false;
  }

  for (int index = 0; index < categoryCount; index++) {
    file.print(categories[index].id);
    file.print("|");

    file.print(categories[index].color);
    file.print("|");

    file.println(
      sanitizeCategoryName(
        categories[index].name
      )
    );
  }

  file.flush();
  file.close();

  return true;
}

bool saveCategories() {
  LittleFS.remove(CATEGORIES_TEMP_FILE);

  if (!writeCategoriesFile(CATEGORIES_TEMP_FILE)) {
    return false;
  }

  LittleFS.remove(CATEGORIES_BACKUP_FILE);

  if (LittleFS.exists(CATEGORIES_FILE)) {
    if (!LittleFS.rename(
      CATEGORIES_FILE,
      CATEGORIES_BACKUP_FILE
    )) {
      LittleFS.remove(CATEGORIES_TEMP_FILE);
      return false;
    }
  }

  if (!LittleFS.rename(
    CATEGORIES_TEMP_FILE,
    CATEGORIES_FILE
  )) {
    if (LittleFS.exists(CATEGORIES_BACKUP_FILE)) {
      LittleFS.rename(
        CATEGORIES_BACKUP_FILE,
        CATEGORIES_FILE
      );
    }

    return false;
  }

  return true;
}

bool loadCategoriesFile(const String& path) {
  File file = LittleFS.open(path, FILE_READ);

  if (!file) {
    return false;
  }

  categoryCount = 0;

  while (
    file.available() &&
    categoryCount < MAX_CATEGORIES
  ) {
    String line = file.readStringUntil('\n');
    line.trim();

    if (line.length() == 0) {
      continue;
    }

    int delimiter1 = line.indexOf('|');
    int delimiter2 =
      line.indexOf('|', delimiter1 + 1);

    if (
      delimiter1 < 0 ||
      delimiter2 < 0
    ) {
      continue;
    }

    CategoryRecord record;

    record.id =
      line.substring(0, delimiter1);

    record.color = sanitizeCategoryColor(
      line.substring(
        delimiter1 + 1,
        delimiter2
      )
    );

    record.name = sanitizeCategoryName(
      line.substring(delimiter2 + 1)
    );

    if (
      record.id.length() > 0 &&
      findCategoryIndex(record.id) < 0
    ) {
      categories[categoryCount++] = record;
    }
  }

  file.close();

  return true;
}

void loadCategories() {
  categoryCount = 0;

  if (!LittleFS.exists(CATEGORIES_FILE)) {
    if (LittleFS.exists(CATEGORIES_TEMP_FILE)) {
      LittleFS.rename(
        CATEGORIES_TEMP_FILE,
        CATEGORIES_FILE
      );
    } else if (
      LittleFS.exists(CATEGORIES_BACKUP_FILE)
    ) {
      LittleFS.rename(
        CATEGORIES_BACKUP_FILE,
        CATEGORIES_FILE
      );
    }
  }

  bool loaded =
    loadCategoriesFile(CATEGORIES_FILE);

  if (
    !loaded &&
    LittleFS.exists(CATEGORIES_BACKUP_FILE)
  ) {
    loaded =
      loadCategoriesFile(
        CATEGORIES_BACKUP_FILE
      );
  }

  if (!loaded) {
    categoryCount = 0;
  }

  saveCategories();

  Serial.printf(
    "[Categories] Loaded %d categories\n",
    categoryCount
  );
}


bool writeLibraryFile(const String& path) {
  LittleFS.remove(path);

  File file = LittleFS.open(path, FILE_WRITE);

  if (!file) {
    return false;
  }

  for (int index = 0; index < photoCount; index++) {
    file.print(photos[index].id);
    file.print("|");

    file.print(
      photos[index].selected ? "1" : "0"
    );
    file.print("|");

    file.print(
      photos[index].connectorRight ? "R" : "L"
    );
    file.print("|");

    file.print(photos[index].hash);
    file.print("|");

    file.print(
      sanitizeName(photos[index].name)
    );
    file.print("|");

    file.println(
      photos[index].categoryIds
    );
  }

  file.flush();
  file.close();

  return true;
}

bool savePhotoLibrary() {
  LittleFS.remove(LIBRARY_TEMP_FILE);

  if (!writeLibraryFile(LIBRARY_TEMP_FILE)) {
    Serial.println(
      "[Library] Failed to write temporary index"
    );

    return false;
  }

  LittleFS.remove(LIBRARY_BACKUP_FILE);

  if (LittleFS.exists(LIBRARY_FILE)) {
    if (!LittleFS.rename(
      LIBRARY_FILE,
      LIBRARY_BACKUP_FILE
    )) {
      Serial.println(
        "[Library] Failed to create backup"
      );

      LittleFS.remove(LIBRARY_TEMP_FILE);
      return false;
    }
  }

  if (!LittleFS.rename(
    LIBRARY_TEMP_FILE,
    LIBRARY_FILE
  )) {
    Serial.println(
      "[Library] Failed to commit index"
    );

    if (LittleFS.exists(LIBRARY_BACKUP_FILE)) {
      LittleFS.rename(
        LIBRARY_BACKUP_FILE,
        LIBRARY_FILE
      );
    }

    return false;
  }

  return true;
}

bool loadLibraryFile(const String& path) {
  File file = LittleFS.open(path, FILE_READ);

  if (!file) {
    return false;
  }

  photoCount = 0;

  while (
    file.available() &&
    photoCount < MAX_PHOTOS
  ) {
    String line = file.readStringUntil('\n');
    line.trim();

    if (line.length() == 0) {
      continue;
    }

    int delimiter1 = line.indexOf('|');
    int delimiter2 =
      line.indexOf('|', delimiter1 + 1);
    int delimiter3 =
      line.indexOf('|', delimiter2 + 1);
    int delimiter4 =
      line.indexOf('|', delimiter3 + 1);
    int delimiter5 =
      line.indexOf('|', delimiter4 + 1);

    if (
      delimiter1 < 0 ||
      delimiter2 < 0 ||
      delimiter3 < 0 ||
      delimiter4 < 0
    ) {
      continue;
    }

    PhotoRecord record;

    record.id =
      line.substring(0, delimiter1);

    record.selected =
      line.substring(
        delimiter1 + 1,
        delimiter2
      ) == "1";

    record.connectorRight =
      line.substring(
        delimiter2 + 1,
        delimiter3
      ) == "R";

    record.hash = strtoul(
      line.substring(
        delimiter3 + 1,
        delimiter4
      ).c_str(),
      nullptr,
      10
    );

    if (delimiter5 < 0) {
      record.name = sanitizeName(
        line.substring(delimiter4 + 1)
      );

      record.categoryIds = "";
    } else {
      record.name = sanitizeName(
        line.substring(
          delimiter4 + 1,
          delimiter5
        )
      );

      record.categoryIds =
        cleanCategoryCsv(
          line.substring(delimiter5 + 1),
          false
        );
    }

    record.path =
      String(PHOTO_DIRECTORY) +
      "/" +
      record.id +
      ".e6";

    if (photoFileValid(record.path)) {
      photos[photoCount++] = record;
    }
  }

  file.close();

  return true;
}

void cleanTemporaryUploads() {
  File directory = LittleFS.open(PHOTO_DIRECTORY);

  if (!directory || !directory.isDirectory()) {
    return;
  }

  File file = directory.openNextFile();

  while (file) {
    String path = file.name();

    if (!path.startsWith("/")) {
      path =
        String(PHOTO_DIRECTORY) +
        "/" +
        path;
    }

    bool shouldRemove =
      !file.isDirectory() &&
      path.endsWith(".tmp");

    file.close();

    if (shouldRemove) {
      Serial.print(
        "[Recovery] Removing incomplete upload: "
      );

      Serial.println(path);
      LittleFS.remove(path);
    }

    file = directory.openNextFile();
  }

  directory.close();
}

String idFromPath(String path) {
  int slash = path.lastIndexOf('/');

  if (slash >= 0) {
    path = path.substring(slash + 1);
  }

  if (path.endsWith(".e6")) {
    path.remove(path.length() - 3);
  }

  return path;
}

void recoverOrphanPhotos() {
  File directory = LittleFS.open(PHOTO_DIRECTORY);

  if (!directory || !directory.isDirectory()) {
    return;
  }

  bool changed = false;
  File file = directory.openNextFile();

  while (file && photoCount < MAX_PHOTOS) {
    String path = file.name();

    if (!path.startsWith("/")) {
      path =
        String(PHOTO_DIRECTORY) +
        "/" +
        path;
    }

    size_t size = file.size();
    bool isDirectory = file.isDirectory();

    file.close();

    if (
      !isDirectory &&
      path.endsWith(".e6") &&
      size == EPD_IMAGE_BYTES
    ) {
      String id = idFromPath(path);

      if (findPhotoIndex(id) < 0) {
        uint32_t hash =
          calculateFileHash(path);

        if (hashAlreadyExists(hash)) {
          LittleFS.remove(path);
        } else {
          PhotoRecord record;

          record.id = id;

          record.name =
            "Recovered " +
            id.substring(
              0,
              min(8, (int)id.length())
            );

          record.path = path;
          record.categoryIds = "";
          record.selected = false;

          record.connectorRight =
            panelMount == "right";

          record.hash = hash;

          photos[photoCount++] = record;
          changed = true;
        }
      }
    }

    file = directory.openNextFile();
  }

  directory.close();

  if (changed) {
    savePhotoLibrary();
  }
}

void loadPhotoLibrary() {
  photoCount = 0;

  cleanTemporaryUploads();

  if (!LittleFS.exists(LIBRARY_FILE)) {
    if (LittleFS.exists(LIBRARY_TEMP_FILE)) {
      LittleFS.rename(
        LIBRARY_TEMP_FILE,
        LIBRARY_FILE
      );
    } else if (
      LittleFS.exists(LIBRARY_BACKUP_FILE)
    ) {
      LittleFS.rename(
        LIBRARY_BACKUP_FILE,
        LIBRARY_FILE
      );
    }
  }

  bool loaded =
    loadLibraryFile(LIBRARY_FILE);

  if (
    !loaded &&
    LittleFS.exists(LIBRARY_BACKUP_FILE)
  ) {
    loaded =
      loadLibraryFile(LIBRARY_BACKUP_FILE);
  }

  if (!loaded) {
    photoCount = 0;
  }

  recoverOrphanPhotos();
  savePhotoLibrary();

  Serial.printf(
    "[Library] Loaded %d photos\n",
    photoCount
  );
}

bool addPhotoToLibrary(
  const String& id,
  const String& name,
  bool connectorRight,
  uint32_t hash
) {
  if (photoCount >= MAX_PHOTOS) {
    return false;
  }

  PhotoRecord record;

  record.id = id;
  record.name = sanitizeName(name);
  record.categoryIds = "";
  record.selected = true;
  record.connectorRight = connectorRight;
  record.hash = hash;

  record.path =
    String(PHOTO_DIRECTORY) +
    "/" +
    id +
    ".e6";

  photos[photoCount++] = record;

  resetShuffleQueue();

  return savePhotoLibrary();
}

bool deletePhotoById(const String& id) {
  int index = findPhotoIndex(id);

  if (index < 0) {
    return false;
  }

  LittleFS.remove(photos[index].path);

  for (
    int moveIndex = index;
    moveIndex < photoCount - 1;
    moveIndex++
  ) {
    photos[moveIndex] =
      photos[moveIndex + 1];
  }

  photoCount--;

  if (currentPhotoId == id) {
    currentPhotoId = "";

    preferences.putString(
      "current",
      ""
    );
  }

  resetShuffleQueue();
  resetDisplayHistory();

  return savePhotoLibrary();
}

// ============================================================
// DISPLAY DRIVER
// ============================================================

void epdSendCommand(uint8_t command) {
  digitalWrite(EPD_DC_PIN, LOW);
  digitalWrite(EPD_CS_PIN, LOW);

  SPI.transfer(command);

  digitalWrite(EPD_CS_PIN, HIGH);
}

void epdSendData(uint8_t data) {
  digitalWrite(EPD_DC_PIN, HIGH);
  digitalWrite(EPD_CS_PIN, LOW);

  SPI.transfer(data);

  digitalWrite(EPD_CS_PIN, HIGH);
}

void epdSendDataBuffer(
  const uint8_t* data,
  size_t length
) {
  digitalWrite(EPD_DC_PIN, HIGH);
  digitalWrite(EPD_CS_PIN, LOW);

  SPI.writeBytes(data, length);

  digitalWrite(EPD_CS_PIN, HIGH);
}

bool epdWaitUntilIdle(
  const char* operation,
  unsigned long timeoutMilliseconds = 120000
) {
  Serial.print("[EPD] Waiting: ");
  Serial.println(operation);

  unsigned long started = millis();

  delay(10);

  while (digitalRead(EPD_BUSY_PIN) == LOW) {
    if (
      millis() - started >=
      timeoutMilliseconds
    ) {
      Serial.print("[EPD] Timeout: ");
      Serial.println(operation);

      return false;
    }

    delay(50);
    yield();
  }

  return true;
}

bool epdReset() {
  digitalWrite(EPD_RST_PIN, HIGH);
  delay(20);

  digitalWrite(EPD_RST_PIN, LOW);
  delay(10);

  digitalWrite(EPD_RST_PIN, HIGH);
  delay(20);

  return epdWaitUntilIdle("reset");
}

bool epdInitE6() {
  if (!epdReset()) {
    return false;
  }

  epdSendCommand(0xAA);
  epdSendData(0x49);
  epdSendData(0x55);
  epdSendData(0x20);
  epdSendData(0x08);
  epdSendData(0x09);
  epdSendData(0x18);

  epdSendCommand(0x01);
  epdSendData(0x3F);
  epdSendData(0x00);
  epdSendData(0x32);
  epdSendData(0x2A);
  epdSendData(0x0E);
  epdSendData(0x2A);

  epdSendCommand(0x00);
  epdSendData(0x5F);
  epdSendData(0x69);

  epdSendCommand(0x03);
  epdSendData(0x00);
  epdSendData(0x54);
  epdSendData(0x00);
  epdSendData(0x44);

  epdSendCommand(0x06);
  epdSendData(PANEL_RAM_WIDTH >> 8);
  epdSendData(PANEL_RAM_WIDTH & 0xFF);
  epdSendData(PANEL_RAM_HEIGHT >> 8);
  epdSendData(PANEL_RAM_HEIGHT & 0xFF);

  epdSendCommand(0x08);
  epdSendData(0x6F);

  epdSendCommand(0x60);
  epdSendData(0x02);
  epdSendData(0x00);

  epdSendCommand(0x61);
  epdSendData(PANEL_RAM_WIDTH >> 8);
  epdSendData(PANEL_RAM_WIDTH & 0xFF);
  epdSendData(PANEL_RAM_HEIGHT >> 8);
  epdSendData(PANEL_RAM_HEIGHT & 0xFF);

  epdSendCommand(0xE3);
  epdSendData(0x2F);

  epdSendCommand(0x84);
  epdSendData(0x01);

  return true;
}

bool epdRefreshDisplay() {
  epdSendCommand(0x04);

  if (!epdWaitUntilIdle("power on")) {
    return false;
  }

  epdSendCommand(0x12);
  epdSendData(0x00);

  if (!epdWaitUntilIdle("refresh")) {
    return false;
  }

  epdSendCommand(0x02);
  epdSendData(0x00);

  return epdWaitUntilIdle("power off");
}

bool sendStoredPhotoToDisplay(
  const String& path
) {
  File image = LittleFS.open(path, FILE_READ);

  if (
    !image ||
    image.size() != EPD_IMAGE_BYTES
  ) {
    if (image) {
      image.close();
    }

    return false;
  }

  if (!epdInitE6()) {
    image.close();
    return false;
  }

  epdSendCommand(0x10);

  uint8_t buffer[1024];
  size_t sent = 0;

  while (image.available()) {
    size_t count =
      image.read(buffer, sizeof(buffer));

    if (count == 0) {
      break;
    }

    epdSendDataBuffer(buffer, count);
    sent += count;

    yield();
  }

  image.close();

  return
    sent == EPD_IMAGE_BYTES &&
    epdRefreshDisplay();
}


// ============================================================
// EXTENSION DISPLAY CALLBACK
// ============================================================

bool displayExtensionFile(const String& path) {
  if (
    displayRunning ||
    displayRequested
  ) {
    return false;
  }

  displayRunning = true;

  applicationState = "refreshing";
  applicationMessage =
    "Refreshing extension dashboard";

  bool successful =
    sendStoredPhotoToDisplay(path);

  displayRunning = false;

  applicationState =
    successful
      ? "complete"
      : "error";

  applicationMessage =
    successful
      ? "Extension dashboard displayed"
      : "Extension dashboard failed";

  return successful;
}

// ============================================================
// SLIDESHOW
// ============================================================

void addDisplayHistory(const String& id) {
  if (
    historyPosition >= 0 &&
    historyPosition < historyCount &&
    displayHistory[historyPosition] == id
  ) {
    return;
  }

  if (historyPosition < historyCount - 1) {
    historyCount = historyPosition + 1;
  }

  if (historyCount >= MAX_PHOTOS) {
    for (
      int index = 1;
      index < historyCount;
      index++
    ) {
      displayHistory[index - 1] =
        displayHistory[index];
    }

    historyCount--;
  }

  displayHistory[historyCount++] = id;
  historyPosition = historyCount - 1;
}

bool requestDisplayByIndex(
  int index,
  bool fromHistory = false,
  int targetHistoryPosition = -1
) {
  if (
    index < 0 ||
    index >= photoCount ||
    displayRunning ||
    displayRequested
  ) {
    return false;
  }

  requestedPhotoId = photos[index].id;
  requestedPhotoPath = photos[index].path;

  historyNavigationRequest = fromHistory;
  requestedHistoryPosition =
    targetHistoryPosition;

  displayRequested = true;

  applicationState = "queued";
  applicationMessage = "Photo queued";

  return true;
}

bool requestDisplayById(const String& id) {
  return requestDisplayByIndex(
    findPhotoIndex(id)
  );
}

int firstSelectedIndex() {
  for (int index = 0; index < photoCount; index++) {
    if (photoEligible(index)) {
      return index;
    }
  }

  return -1;
}

int nextSelectedSequential() {
  if (eligiblePhotoCount() == 0) {
    return -1;
  }

  int currentIndex =
    findPhotoIndex(currentPhotoId);

  for (
    int step = 1;
    step <= photoCount;
    step++
  ) {
    int index =
      (
        currentIndex +
        step +
        photoCount
      ) % photoCount;

    if (photoEligible(index)) {
      return index;
    }
  }

  return firstSelectedIndex();
}

int previousSelectedSequential() {
  if (eligiblePhotoCount() == 0) {
    return -1;
  }

  int currentIndex =
    findPhotoIndex(currentPhotoId);

  if (currentIndex < 0) {
    return firstSelectedIndex();
  }

  for (
    int step = 1;
    step <= photoCount;
    step++
  ) {
    int index =
      (
        currentIndex -
        step +
        photoCount * 2
      ) % photoCount;

    if (photoEligible(index)) {
      return index;
    }
  }

  return firstSelectedIndex();
}

void buildShuffleQueue() {
  shuffleCount = 0;
  shufflePosition = 0;

  for (int index = 0; index < photoCount; index++) {
    if (photoEligible(index)) {
      shuffleOrder[shuffleCount++] = index;
    }
  }

  for (
    int index = shuffleCount - 1;
    index > 0;
    index--
  ) {
    int swapIndex =
      esp_random() % (index + 1);

    int temporary = shuffleOrder[index];

    shuffleOrder[index] =
      shuffleOrder[swapIndex];

    shuffleOrder[swapIndex] =
      temporary;
  }

  if (
    shuffleCount > 1 &&
    photos[shuffleOrder[0]].id ==
      currentPhotoId
  ) {
    int temporary = shuffleOrder[0];

    shuffleOrder[0] =
      shuffleOrder[1];

    shuffleOrder[1] =
      temporary;
  }
}

int nextSelectedShuffle() {
  int eligible = eligiblePhotoCount();

  if (eligible == 0) {
    return -1;
  }

  if (eligible == 1) {
    return firstSelectedIndex();
  }

  if (
    shuffleCount != eligible ||
    shufflePosition >= shuffleCount
  ) {
    buildShuffleQueue();
  }

  while (shufflePosition < shuffleCount) {
    int index =
      shuffleOrder[shufflePosition++];

    if (
      photoEligible(index) &&
      photos[index].id != currentPhotoId
    ) {
      return index;
    }
  }

  buildShuffleQueue();

  if (shuffleCount > 0) {
    return shuffleOrder[shufflePosition++];
  }

  return -1;
}

int chooseNextPhoto() {
  if (slideshowMode == "shuffle") {
    return nextSelectedShuffle();
  }

  return nextSelectedSequential();
}

uint32_t nextPhotoSeconds() {
  if (
    !slideshowRunning ||
    lastDisplayTime == 0
  ) {
    return slideIntervalSeconds;
  }

  uint32_t elapsed =
    (millis() - lastDisplayTime) /
    1000UL;

  if (elapsed >= slideIntervalSeconds) {
    return 0;
  }

  return slideIntervalSeconds - elapsed;
}

// ============================================================
// SETTINGS AND NETWORKING
// ============================================================

void loadSettings() {
  preferences.begin("dynapix", false);

  deviceName = preferences.getString(
    "devname",
    "DynaPix Photo Frame"
  );

  hostName = sanitizeHostname(
    preferences.getString(
      "hostname",
      "dynapix"
    )
  );

  activeSSID = preferences.getString(
    "ssid",
    BOOTSTRAP_WIFI_SSID
  );

  activePassword = preferences.getString(
    "password",
    BOOTSTRAP_WIFI_PASS
  );

  panelMount = preferences.getString(
    "mount",
    "left"
  );

  if (
    panelMount != "left" &&
    panelMount != "right"
  ) {
    panelMount = "left";

    preferences.putString(
      "mount",
      panelMount
    );
  }

  slideIntervalSeconds =
    preferences.getUInt(
      "interval",
      DEFAULT_SLIDE_INTERVAL
    );

  if (
    slideIntervalSeconds <
    MIN_SLIDE_INTERVAL
  ) {
    slideIntervalSeconds =
      MIN_SLIDE_INTERVAL;

    preferences.putUInt(
      "interval",
      slideIntervalSeconds
    );
  }

  slideshowMode =
    preferences.getString(
      "mode",
      "sequential"
    );

  if (
    slideshowMode != "sequential" &&
    slideshowMode != "shuffle"
  ) {
    slideshowMode = "sequential";

    preferences.putString(
      "mode",
      slideshowMode
    );
  }

  slideshowRunning =
    preferences.getBool(
      "running",
      false
    );

  currentPhotoId =
    preferences.getString(
      "current",
      ""
    );

  slideshowCategoryIds =
    preferences.getString(
      "slidecats",
      ""
    );

  categoryMatchMode =
    preferences.getString(
      "catmatch",
      "any"
    );

  if (
    categoryMatchMode != "any" &&
    categoryMatchMode != "all"
  ) {
    categoryMatchMode = "any";
  }

  libraryCategoryFilter =
    preferences.getString(
      "libfilter",
      "all"
    );
}

bool connectStation(
  const String& ssid,
  const String& password,
  unsigned long timeout
) {
  if (ssid.length() == 0) {
    return false;
  }

  WiFi.disconnect(false, false);
  delay(250);

  WiFi.setHostname(hostName.c_str());

  if (password.length() == 0) {
    WiFi.begin(ssid.c_str());
  } else {
    WiFi.begin(
      ssid.c_str(),
      password.c_str()
    );
  }

  unsigned long started = millis();

  while (
    WiFi.status() != WL_CONNECTED &&
    millis() - started < timeout
  ) {
    delay(250);
    yield();
  }

  return WiFi.status() == WL_CONNECTED;
}

void startNetworking() {
  WiFi.mode(WIFI_AP_STA);

  WiFi.softAP(
    FALLBACK_AP_SSID,
    FALLBACK_AP_PASS
  );

  String candidateSSID =
    preferences.getString(
      "candssid",
      ""
    );

  String candidatePassword =
    preferences.getString(
      "candpass",
      ""
    );

  bool candidatePending =
    preferences.getBool(
      "cand",
      false
    );

  bool connected = false;

  if (
    candidatePending &&
    candidateSSID.length() > 0
  ) {
    connected = connectStation(
      candidateSSID,
      candidatePassword,
      15000
    );

    if (connected) {
      preferences.putString(
        "prevssid",
        activeSSID
      );

      preferences.putString(
        "prevpass",
        activePassword
      );

      activeSSID = candidateSSID;
      activePassword = candidatePassword;

      preferences.putString(
        "ssid",
        activeSSID
      );

      preferences.putString(
        "password",
        activePassword
      );
    }

    preferences.putBool(
      "cand",
      false
    );

    preferences.remove("candssid");
    preferences.remove("candpass");
  }

  if (!connected) {
    connected = connectStation(
      activeSSID,
      activePassword,
      15000
    );
  }

  if (!connected) {
    String previousSSID =
      preferences.getString(
        "prevssid",
        ""
      );

    String previousPassword =
      preferences.getString(
        "prevpass",
        ""
      );

    if (previousSSID.length() > 0) {
      connected = connectStation(
        previousSSID,
        previousPassword,
        12000
      );

      if (connected) {
        activeSSID = previousSSID;
        activePassword = previousPassword;

        preferences.putString(
          "ssid",
          activeSSID
        );

        preferences.putString(
          "password",
          activePassword
        );
      }
    }
  }

  if (connected) {
    Serial.print("[Wi-Fi] Connected: ");
    Serial.println(WiFi.localIP());

    if (MDNS.begin(hostName.c_str())) {
      MDNS.addService("http", "tcp", 80);
    }
  }

  Serial.print("[Wi-Fi] Fallback AP: ");
  Serial.println(WiFi.softAPIP());
}

// ============================================================
// WEB INTERFACE
// ============================================================

// ============================================================
// EMBEDDED WEB INTERFACE
// ============================================================

#include "web_ui.h"

// ============================================================
// STATUS API
// ============================================================

void handleStatus() {
  int currentIndex =
    findPhotoIndex(currentPhotoId);

  String currentName =
    currentIndex >= 0
      ? photos[currentIndex].name
      : "";

  bool busy =
    displayRunning ||
    displayRequested;

  String response;
  response.reserve(1000);

  response += "{";

  response += "\"state\":\"";
  response += jsonEscape(applicationState);
  response += "\",";

  response += "\"message\":\"";
  response += jsonEscape(applicationMessage);
  response += "\",";

  response += "\"displayBusy\":";
  response += busy ? "true" : "false";
  response += ",";

  response += "\"deviceName\":\"";
  response += jsonEscape(deviceName);
  response += "\",";

  response += "\"hostname\":\"";
  response += jsonEscape(hostName);
  response += "\",";

  response += "\"stationConnected\":";
  response +=
    WiFi.status() == WL_CONNECTED
      ? "true"
      : "false";
  response += ",";

  response += "\"ssid\":\"";
  response += jsonEscape(
    WiFi.status() == WL_CONNECTED
      ? WiFi.SSID()
      : activeSSID
  );
  response += "\",";

  response += "\"ip\":\"";
  response += WiFi.localIP().toString();
  response += "\",";

  response += "\"apIP\":\"";
  response += WiFi.softAPIP().toString();
  response += "\",";

  response += "\"rssi\":";
  response += String(
    WiFi.status() == WL_CONNECTED
      ? WiFi.RSSI()
      : 0
  );
  response += ",";

  response += "\"photoCount\":";
  response += String(photoCount);
  response += ",";

  response += "\"selectedCount\":";
  response += String(selectedPhotoCount());
  response += ",";

  response += "\"eligibleCount\":";
  response += String(eligiblePhotoCount());
  response += ",";

  response += "\"slideshowRunning\":";
  response +=
    slideshowRunning
      ? "true"
      : "false";
  response += ",";

  response += "\"interval\":";
  response += String(slideIntervalSeconds);
  response += ",";

  response += "\"mode\":\"";
  response += jsonEscape(slideshowMode);
  response += "\",";

  response += "\"slideshowCategoryIds\":\"";
  response += jsonEscape(slideshowCategoryIds);
  response += "\",";

  response += "\"categoryMatch\":\"";
  response += jsonEscape(categoryMatchMode);
  response += "\",";

  response += "\"currentId\":\"";
  response += jsonEscape(currentPhotoId);
  response += "\",";

  response += "\"currentName\":\"";
  response += jsonEscape(currentName);
  response += "\",";

  response += "\"nextSeconds\":";
  response += String(nextPhotoSeconds());

  response += "}";

  server.sendHeader(
    "Cache-Control",
    "no-store"
  );

  server.send(
    200,
    "application/json",
    response
  );
}

// ============================================================
// PHOTO APIs
// ============================================================

void handlePhotoList() {
  size_t total = LittleFS.totalBytes();
  size_t used = LittleFS.usedBytes();

  String response;

  response.reserve(
    900 +
    photoCount * 240 +
    categoryCount * 140
  );

  response += "{";

  response += "\"count\":";
  response += String(photoCount);
  response += ",";

  response += "\"used\":";
  response += String(used);
  response += ",";

  response += "\"free\":";
  response += String(total - used);
  response += ",";

  response += "\"panelMount\":\"";
  response += jsonEscape(panelMount);
  response += "\",";

  response += "\"libraryFilter\":\"";
  response += jsonEscape(
    libraryCategoryFilter
  );
  response += "\",";

  response += "\"slideshowCategoryIds\":\"";
  response += jsonEscape(
    slideshowCategoryIds
  );
  response += "\",";

  response += "\"categoryMatch\":\"";
  response += jsonEscape(
    categoryMatchMode
  );
  response += "\",";

  response += "\"categories\":[";

  for (
    int index = 0;
    index < categoryCount;
    index++
  ) {
    if (index > 0) {
      response += ",";
    }

    int assignedCount = 0;

    for (
      int photoIndex = 0;
      photoIndex < photoCount;
      photoIndex++
    ) {
      if (
        csvContainsId(
          photos[photoIndex].categoryIds,
          categories[index].id
        )
      ) {
        assignedCount++;
      }
    }

    response += "{";

    response += "\"id\":\"";
    response += jsonEscape(
      categories[index].id
    );
    response += "\",";

    response += "\"name\":\"";
    response += jsonEscape(
      categories[index].name
    );
    response += "\",";

    response += "\"color\":\"";
    response += jsonEscape(
      categories[index].color
    );
    response += "\",";

    response += "\"count\":";
    response += String(assignedCount);

    response += "}";
  }

  response += "],";

  response += "\"photos\":[";

  for (
    int index = 0;
    index < photoCount;
    index++
  ) {
    if (index > 0) {
      response += ",";
    }

    response += "{";

    response += "\"id\":\"";
    response += jsonEscape(photos[index].id);
    response += "\",";

    response += "\"name\":\"";
    response += jsonEscape(
      photos[index].name
    );
    response += "\",";

    response += "\"selected\":";
    response +=
      photos[index].selected
        ? "true"
        : "false";
    response += ",";

    response += "\"mount\":\"";
    response +=
      photos[index].connectorRight
        ? "right"
        : "left";
    response += "\",";

    response += "\"categoryIds\":\"";
    response += jsonEscape(
      photos[index].categoryIds
    );
    response += "\",";

    response += "\"size\":";
    response += String(EPD_IMAGE_BYTES);

    response += "}";
  }

  response += "]}";

  server.sendHeader(
    "Cache-Control",
    "no-store"
  );

  server.send(
    200,
    "application/json",
    response
  );
}

void handlePhotoDownload() {
  int index =
    findPhotoIndex(
      server.arg("id")
    );

  if (index < 0) {
    server.send(
      404,
      "text/plain",
      "Photo not found"
    );

    return;
  }

  File file =
    LittleFS.open(
      photos[index].path,
      FILE_READ
    );

  if (!file) {
    server.send(
      500,
      "text/plain",
      "Stored image unavailable"
    );

    return;
  }

  server.sendHeader(
    "Cache-Control",
    "private, max-age=3600"
  );

  server.streamFile(
    file,
    "application/octet-stream"
  );

  file.close();
}

void handlePhotoDelete() {
  if (
    displayRunning ||
    displayRequested
  ) {
    server.send(
      409,
      "text/plain",
      "Display is busy"
    );

    return;
  }

  if (!deletePhotoById(
    server.arg("id")
  )) {
    server.send(
      404,
      "text/plain",
      "Photo not found"
    );

    return;
  }

  if (
    slideshowRunning &&
    selectedPhotoCount() == 0
  ) {
    slideshowRunning = false;

    preferences.putBool(
      "running",
      false
    );
  }

  server.send(
    200,
    "application/json",
    "{\"ok\":true}"
  );
}

void handlePhotoBatchDelete() {
  if (
    displayRunning ||
    displayRequested
  ) {
    server.send(
      409,
      "application/json",
      "{\"ok\":false,\"error\":\"Display is busy\"}"
    );

    return;
  }

  if (
    !server.hasArg("ids") ||
    server.arg("ids").length() == 0
  ) {
    server.send(
      400,
      "application/json",
      "{\"ok\":false,\"error\":\"No photos were selected\"}"
    );

    return;
  }

  String requestedIds =
    "," +
    server.arg("ids") +
    ",";

  int deletedCount = 0;
  bool currentDeleted = false;

  for (
    int index = photoCount - 1;
    index >= 0;
    index--
  ) {
    String marker =
      "," +
      photos[index].id +
      ",";

    if (
      requestedIds.indexOf(marker) < 0
    ) {
      continue;
    }

    LittleFS.remove(
      photos[index].path
    );

    if (
      photos[index].id ==
      currentPhotoId
    ) {
      currentDeleted = true;
    }

    for (
      int moveIndex = index;
      moveIndex < photoCount - 1;
      moveIndex++
    ) {
      photos[moveIndex] =
        photos[moveIndex + 1];
    }

    photoCount--;
    deletedCount++;
  }

  if (deletedCount == 0) {
    server.send(
      404,
      "application/json",
      "{\"ok\":false,\"error\":\"Selected photos were not found\"}"
    );

    return;
  }

  if (currentDeleted) {
    currentPhotoId = "";

    preferences.putString(
      "current",
      ""
    );
  }

  resetShuffleQueue();
  resetDisplayHistory();

  if (
    slideshowRunning &&
    selectedPhotoCount() == 0
  ) {
    slideshowRunning = false;

    preferences.putBool(
      "running",
      false
    );
  }

  if (!savePhotoLibrary()) {
    server.send(
      500,
      "application/json",
      "{\"ok\":false,\"error\":\"Photos were deleted but the library index could not be saved\"}"
    );

    return;
  }

  String response;

  response += "{";
  response += "\"ok\":true,";
  response += "\"deleted\":";
  response += String(deletedCount);
  response += ",";
  response += "\"remaining\":";
  response += String(photoCount);
  response += "}";

  server.sendHeader(
    "Cache-Control",
    "no-store"
  );

  server.send(
    200,
    "application/json",
    response
  );
}

void handlePhotoRename() {
  int index =
    findPhotoIndex(
      server.arg("id")
    );

  if (index < 0) {
    server.send(
      404,
      "text/plain",
      "Photo not found"
    );

    return;
  }

  photos[index].name =
    sanitizeName(
      server.arg("name")
    );

  savePhotoLibrary();

  server.send(
    200,
    "application/json",
    "{\"ok\":true}"
  );
}

void handlePhotoMove() {
  int index =
    findPhotoIndex(
      server.arg("id")
    );

  int direction =
    server.arg("direction").toInt();

  int target =
    index + direction;

  if (
    index < 0 ||
    target < 0 ||
    target >= photoCount
  ) {
    server.send(
      400,
      "text/plain",
      "Cannot move photo"
    );

    return;
  }

  PhotoRecord temporary =
    photos[index];

  photos[index] =
    photos[target];

  photos[target] =
    temporary;

  resetShuffleQueue();
  savePhotoLibrary();

  server.send(
    200,
    "application/json",
    "{\"ok\":true}"
  );
}

void handlePhotoSelection() {
  String ids =
    "," +
    server.arg("ids") +
    ",";

  for (
    int index = 0;
    index < photoCount;
    index++
  ) {
    String marker =
      "," +
      photos[index].id +
      ",";

    photos[index].selected =
      ids.indexOf(marker) >= 0;
  }

  resetShuffleQueue();
  savePhotoLibrary();

  if (
    slideshowRunning &&
    selectedPhotoCount() == 0
  ) {
    slideshowRunning = false;

    preferences.putBool(
      "running",
      false
    );
  }

  server.send(
    200,
    "application/json",
    "{\"ok\":true}"
  );
}


// ============================================================
// CATEGORY API
// ============================================================

void handleCategoryCreate() {
  if (categoryCount >= MAX_CATEGORIES) {
    server.send(
      400,
      "application/json",
      "{\"ok\":false,\"error\":\"Category limit reached\"}"
    );

    return;
  }

  CategoryRecord record;

  record.id = createCategoryId();

  record.name = sanitizeCategoryName(
    server.arg("name")
  );

  record.color = sanitizeCategoryColor(
    server.arg("color")
  );

  categories[categoryCount++] = record;

  if (!saveCategories()) {
    categoryCount--;

    server.send(
      500,
      "application/json",
      "{\"ok\":false,\"error\":\"Could not save category\"}"
    );

    return;
  }

  String response =
    "{\"ok\":true,\"id\":\"" +
    record.id +
    "\"}";

  server.send(
    201,
    "application/json",
    response
  );
}

void handleCategoryUpdate() {
  int index = findCategoryIndex(
    server.arg("id")
  );

  if (index < 0) {
    server.send(
      404,
      "application/json",
      "{\"ok\":false,\"error\":\"Category not found\"}"
    );

    return;
  }

  categories[index].name =
    sanitizeCategoryName(
      server.arg("name")
    );

  categories[index].color =
    sanitizeCategoryColor(
      server.arg("color")
    );

  saveCategories();

  server.send(
    200,
    "application/json",
    "{\"ok\":true}"
  );
}

void handleCategoryDelete() {
  String id = server.arg("id");
  int index = findCategoryIndex(id);

  if (index < 0) {
    server.send(
      404,
      "application/json",
      "{\"ok\":false,\"error\":\"Category not found\"}"
    );

    return;
  }

  for (
    int photoIndex = 0;
    photoIndex < photoCount;
    photoIndex++
  ) {
    photos[photoIndex].categoryIds =
      csvRemoveId(
        photos[photoIndex].categoryIds,
        id
      );
  }

  for (
    int moveIndex = index;
    moveIndex < categoryCount - 1;
    moveIndex++
  ) {
    categories[moveIndex] =
      categories[moveIndex + 1];
  }

  categoryCount--;

  slideshowCategoryIds =
    csvRemoveId(
      slideshowCategoryIds,
      id
    );

  if (libraryCategoryFilter == id) {
    libraryCategoryFilter = "all";
  }

  preferences.putString(
    "slidecats",
    slideshowCategoryIds
  );

  preferences.putString(
    "libfilter",
    libraryCategoryFilter
  );

  saveCategories();
  savePhotoLibrary();

  resetShuffleQueue();
  resetDisplayHistory();
  pauseSlideshowIfPoolEmpty();

  server.send(
    200,
    "application/json",
    "{\"ok\":true}"
  );
}

void handlePhotoCategoryAssignment() {
  String requestedPhotoIds =
    "," + server.arg("ids") + ",";

  String requestedCategories =
    cleanCategoryCsv(
      server.arg("categoryIds"),
      false
    );

  String operation =
    server.arg("operation");

  if (
    operation != "add" &&
    operation != "remove" &&
    operation != "replace"
  ) {
    operation = "replace";
  }

  int changed = 0;

  for (
    int photoIndex = 0;
    photoIndex < photoCount;
    photoIndex++
  ) {
    String marker =
      "," +
      photos[photoIndex].id +
      ",";

    if (
      requestedPhotoIds.indexOf(marker) < 0
    ) {
      continue;
    }

    if (operation == "replace") {
      photos[photoIndex].categoryIds =
        requestedCategories;
    } else {
      int start = 0;

      while (
        start <= requestedCategories.length()
      ) {
        int comma =
          requestedCategories.indexOf(
            ',',
            start
          );

        String categoryId =
          comma < 0
            ? requestedCategories.substring(start)
            : requestedCategories.substring(
                start,
                comma
              );

        categoryId.trim();

        if (categoryId.length() > 0) {
          if (operation == "add") {
            photos[photoIndex].categoryIds =
              csvAddId(
                photos[photoIndex].categoryIds,
                categoryId
              );
          } else {
            photos[photoIndex].categoryIds =
              csvRemoveId(
                photos[photoIndex].categoryIds,
                categoryId
              );
          }
        }

        if (comma < 0) {
          break;
        }

        start = comma + 1;
      }
    }

    changed++;
  }

  savePhotoLibrary();

  resetShuffleQueue();
  resetDisplayHistory();
  pauseSlideshowIfPoolEmpty();

  String response =
    "{\"ok\":true,\"changed\":" +
    String(changed) +
    "}";

  server.send(
    200,
    "application/json",
    response
  );
}

void handleLibraryCategoryFilter() {
  String filter = server.arg("filter");

  bool valid =
    filter == "all" ||
    filter == UNCATEGORIZED_CATEGORY_ID ||
    findCategoryIndex(filter) >= 0;

  if (!valid) {
    filter = "all";
  }

  libraryCategoryFilter = filter;

  preferences.putString(
    "libfilter",
    libraryCategoryFilter
  );

  server.send(
    200,
    "application/json",
    "{\"ok\":true}"
  );
}


// ============================================================
// UPLOAD API
// ============================================================

void handlePhotoUploadData() {
  HTTPUpload& upload =
    server.upload();

  if (
    upload.status ==
    UPLOAD_FILE_START
  ) {
    uploadStarted = true;
    uploadFinished = false;
    uploadValid = true;

    uploadedBytes = 0;
    uploadHash = 2166136261UL;
    uploadError = "";

    if (
      displayRunning ||
      displayRequested
    ) {
      uploadValid = false;
      uploadError = "Display is busy";

      return;
    }

    if (photoCount >= MAX_PHOTOS) {
      uploadValid = false;
      uploadError =
        "Photo library is full";

      return;
    }

    if (
      LittleFS.totalBytes() -
      LittleFS.usedBytes() <
      EPD_IMAGE_BYTES + 8192
    ) {
      uploadValid = false;
      uploadError =
        "Not enough flash storage";

      return;
    }

    uploadId =
      createPhotoId();

    uploadName =
      sanitizeName(
        server.arg("name")
      );

    uploadConnectorRight =
      server.arg("mount") ==
      "right";

    uploadTemporaryPath =
      String(PHOTO_DIRECTORY) +
      "/" +
      uploadId +
      ".tmp";

    uploadFinalPath =
      String(PHOTO_DIRECTORY) +
      "/" +
      uploadId +
      ".e6";

    LittleFS.remove(
      uploadTemporaryPath
    );

    uploadFile =
      LittleFS.open(
        uploadTemporaryPath,
        FILE_WRITE
      );

    if (!uploadFile) {
      uploadValid = false;
      uploadError =
        "Unable to create image file";

      return;
    }

    applicationState =
      "receiving";

    applicationMessage =
      "Receiving photo";
  }

  else if (
    upload.status ==
    UPLOAD_FILE_WRITE
  ) {
    if (
      !uploadValid ||
      !uploadFile
    ) {
      return;
    }

    if (
      uploadedBytes +
      upload.currentSize >
      EPD_IMAGE_BYTES
    ) {
      uploadValid = false;

      uploadError =
        "Image is larger than 120,000 bytes";

      return;
    }

    size_t written =
      uploadFile.write(
        upload.buf,
        upload.currentSize
      );

    if (
      written !=
      upload.currentSize
    ) {
      uploadValid = false;
      uploadError =
        "Flash write failed";

      return;
    }

    for (
      size_t index = 0;
      index < upload.currentSize;
      index++
    ) {
      uploadHash ^= upload.buf[index];
      uploadHash *= 16777619UL;
    }

    uploadedBytes += written;
  }

  else if (
    upload.status ==
    UPLOAD_FILE_END
  ) {
    if (uploadFile) {
      uploadFile.flush();
      uploadFile.close();
    }

    uploadFinished = true;

    if (
      uploadedBytes !=
      EPD_IMAGE_BYTES
    ) {
      uploadValid = false;

      uploadError =
        "Expected 120,000 bytes but received " +
        String(uploadedBytes);
    }

    if (
      uploadValid &&
      hashAlreadyExists(uploadHash)
    ) {
      uploadValid = false;

      uploadError =
        "This converted photo already exists";
    }

    if (uploadValid) {
      LittleFS.remove(
        uploadFinalPath
      );

      if (!LittleFS.rename(
        uploadTemporaryPath,
        uploadFinalPath
      )) {
        uploadValid = false;

        uploadError =
          "Unable to commit image file";
      }
    }

    if (
      uploadValid &&
      !addPhotoToLibrary(
        uploadId,
        uploadName,
        uploadConnectorRight,
        uploadHash
      )
    ) {
      LittleFS.remove(
        uploadFinalPath
      );

      uploadValid = false;

      uploadError =
        "Photo library is full";
    }

    if (!uploadValid) {
      LittleFS.remove(
        uploadTemporaryPath
      );

      LittleFS.remove(
        uploadFinalPath
      );
    }

    applicationState =
      uploadValid
        ? "ready"
        : "error";

    applicationMessage =
      uploadValid
        ? "Photo saved"
        : uploadError;
  }

  else if (
    upload.status ==
    UPLOAD_FILE_ABORTED
  ) {
    if (uploadFile) {
      uploadFile.close();
    }

    LittleFS.remove(
      uploadTemporaryPath
    );

    uploadValid = false;
    uploadFinished = false;

    uploadError =
      "Upload aborted";
  }
}

void handlePhotoUploadComplete() {
  if (
    !uploadStarted ||
    !uploadFinished ||
    !uploadValid
  ) {
    server.send(
      400,
      "text/plain",
      uploadError.length()
        ? uploadError
        : "Upload incomplete"
    );

    return;
  }

  String response =
    "{\"ok\":true,\"id\":\"" +
    uploadId +
    "\",\"bytes\":" +
    String(uploadedBytes) +
    "}";

  server.send(
    201,
    "application/json",
    response
  );
}

// ============================================================
// SLIDESHOW API
// ============================================================

void handleSlideshowSettings() {
  uint32_t requestedInterval =
    strtoul(
      server.arg("interval").c_str(),
      nullptr,
      10
    );

  if (
    requestedInterval <
    MIN_SLIDE_INTERVAL
  ) {
    requestedInterval =
      MIN_SLIDE_INTERVAL;
  }

  String requestedMode =
    server.arg("mode");

  if (
    requestedMode != "sequential" &&
    requestedMode != "shuffle"
  ) {
    requestedMode = "sequential";
  }

  slideIntervalSeconds =
    requestedInterval;

  slideshowMode =
    requestedMode;

  slideshowCategoryIds =
    cleanCategoryCsv(
      server.arg("categoryIds"),
      true
    );

  categoryMatchMode =
    server.arg("categoryMatch");

  if (
    categoryMatchMode != "any" &&
    categoryMatchMode != "all"
  ) {
    categoryMatchMode = "any";
  }

  resetShuffleQueue();
  resetDisplayHistory();

  preferences.putUInt(
    "interval",
    slideIntervalSeconds
  );

  preferences.putString(
    "mode",
    slideshowMode
  );

  preferences.putString(
    "slidecats",
    slideshowCategoryIds
  );

  preferences.putString(
    "catmatch",
    categoryMatchMode
  );

  pauseSlideshowIfPoolEmpty();

  lastDisplayTime = millis();

  String response =
    "{\"ok\":true,\"interval\":" +
    String(slideIntervalSeconds) +
    ",\"mode\":\"" +
    jsonEscape(slideshowMode) +
    "\",\"categoryIds\":\"" +
    jsonEscape(slideshowCategoryIds) +
    "\",\"categoryMatch\":\"" +
    jsonEscape(categoryMatchMode) +
    "\"}";

  server.send(
    200,
    "application/json",
    response
  );
}

void handleSlideshowAction() {
  String action =
    server.arg("action");

  if (
    action != "pause" &&
    (
      displayRunning ||
      displayRequested
    )
  ) {
    server.send(
      409,
      "text/plain",
      "Display is busy"
    );

    return;
  }

  if (action == "start") {
    if (eligiblePhotoCount() == 0) {
      server.send(
        400,
        "text/plain",
        "Select at least one photo"
      );

      return;
    }

    slideshowRunning = true;

    preferences.putBool(
      "running",
      true
    );

    int currentIndex =
      findPhotoIndex(
        currentPhotoId
      );

    if (
      currentIndex < 0 ||
      !photoEligible(currentIndex)
    ) {
      requestDisplayByIndex(
        firstSelectedIndex()
      );
    } else {
      lastDisplayTime = millis();
    }
  }

  else if (action == "pause") {
    slideshowRunning = false;

    preferences.putBool(
      "running",
      false
    );
  }

  else if (action == "next") {
    if (
      historyPosition <
      historyCount - 1
    ) {
      int target =
        historyPosition + 1;

      int index =
        findPhotoIndex(
          displayHistory[target]
        );

      if (index >= 0) {
        requestDisplayByIndex(
          index,
          true,
          target
        );
      }
    } else {
      int next =
        chooseNextPhoto();

      if (next >= 0) {
        requestDisplayByIndex(next);
      }
    }
  }

  else if (action == "previous") {
    if (historyPosition > 0) {
      int target =
        historyPosition - 1;

      int index =
        findPhotoIndex(
          displayHistory[target]
        );

      if (index >= 0) {
        requestDisplayByIndex(
          index,
          true,
          target
        );
      }
    } else {
      int previous =
        previousSelectedSequential();

      if (previous >= 0) {
        requestDisplayByIndex(
          previous
        );
      }
    }
  }

  else if (action == "display") {
    if (!requestDisplayById(
      server.arg("id")
    )) {
      server.send(
        409,
        "text/plain",
        "Photo is invalid or display is busy"
      );

      return;
    }
  }

  else {
    server.send(
      400,
      "text/plain",
      "Unknown action"
    );

    return;
  }

  server.send(
    202,
    "application/json",
    "{\"ok\":true}"
  );
}

// ============================================================
// SETTINGS AND MAINTENANCE API
// ============================================================

void handleDeviceSettings() {
  deviceName =
    sanitizeName(
      server.arg("name")
    );

  hostName =
    sanitizeHostname(
      server.arg("hostname")
    );

  String requestedMount =
    server.arg("mount");

  if (
    requestedMount != "left" &&
    requestedMount != "right"
  ) {
    requestedMount = "left";
  }

  panelMount =
    requestedMount;

  preferences.putString(
    "devname",
    deviceName
  );

  preferences.putString(
    "hostname",
    hostName
  );

  preferences.putString(
    "mount",
    panelMount
  );

  server.send(
    200,
    "application/json",
    "{\"ok\":true}"
  );
}

void handleWiFiSettings() {
  String ssid =
    server.arg("ssid");

  String password =
    server.arg("password");

  bool openNetwork =
    server.arg("open") == "1";

  ssid.trim();

  if (ssid.length() == 0) {
    server.send(
      400,
      "text/plain",
      "Wi-Fi name is required"
    );

    return;
  }

  if (openNetwork) {
    password = "";
  } else if (
    password.length() == 0 &&
    ssid == activeSSID
  ) {
    password =
      activePassword;
  }

  preferences.putString(
    "candssid",
    ssid
  );

  preferences.putString(
    "candpass",
    password
  );

  preferences.putBool(
    "cand",
    true
  );

  restartPending = true;
  restartAt = millis() + 1800;

  server.send(
    202,
    "application/json",
    "{\"ok\":true,\"restarting\":true}"
  );
}

void handleWiFiScan() {
  int count =
    WiFi.scanNetworks(
      false,
      true
    );

  String response =
    "{\"networks\":[";

  for (int index = 0; index < count; index++) {
    if (index > 0) {
      response += ",";
    }

    response += "{\"ssid\":\"";
    response += jsonEscape(
      WiFi.SSID(index)
    );
    response += "\",\"rssi\":";
    response += String(
      WiFi.RSSI(index)
    );
    response += "}";
  }

  response += "]}";

  WiFi.scanDelete();

  server.send(
    200,
    "application/json",
    response
  );
}

void handleCleanup() {
  cleanTemporaryUploads();
  loadPhotoLibrary();

  server.send(
    200,
    "application/json",
    "{\"ok\":true}"
  );
}

void handleRestart() {
  restartPending = true;
  restartAt = millis() + 1200;

  server.send(
    202,
    "application/json",
    "{\"ok\":true,\"restarting\":true}"
  );
}

void handleResetSlideshow() {
  slideIntervalSeconds =
    DEFAULT_SLIDE_INTERVAL;

  slideshowMode =
    "sequential";

  slideshowRunning =
    false;

  resetShuffleQueue();

  preferences.putUInt(
    "interval",
    slideIntervalSeconds
  );

  preferences.putString(
    "mode",
    slideshowMode
  );

  preferences.putBool(
    "running",
    false
  );

  server.send(
    200,
    "application/json",
    "{\"ok\":true}"
  );
}

void handleFactoryReset() {
  server.send(
    202,
    "application/json",
    "{\"ok\":true,\"restarting\":true}"
  );

  delay(250);

  slideshowRunning = false;

  preferences.clear();
  LittleFS.format();

  delay(500);

  ESP.restart();
}

// ============================================================
// WEB SERVER
// ============================================================

// ============================================================
// DYNAPIX BROWSER FAVICON
// ============================================================

const char DYNAPIX_FAVICON_SVG[] PROGMEM = R"DYNFAV(
<svg
 xmlns="http://www.w3.org/2000/svg"
 viewBox="0 0 72 72"
>
 <defs>
  <linearGradient
   id="teal"
   x1="0"
   y1="0"
   x2="1"
   y2="1"
  >
   <stop offset="0" stop-color="#16808a"/>
   <stop offset="1" stop-color="#063f45"/>
  </linearGradient>

  <linearGradient
   id="coral"
   x1="0"
   y1="0"
   x2="1"
   y2="1"
  >
   <stop offset="0" stop-color="#ff735d"/>
   <stop offset="1" stop-color="#fe5945"/>
  </linearGradient>
 </defs>

 <rect
  width="72"
  height="72"
  rx="15"
  fill="#fef8eb"
 />

 <path
  d="M11 9h39a5 5 0 0 1 5 5v6h-5v-5a1 1 0 0 0-1-1H15a4 4 0 0 0-4 4v36a4 4 0 0 0 4 4h33a2 2 0 0 0 2-2v-4h5v5a6 6 0 0 1-6 6H15A9 9 0 0 1 6 54V18a9 9 0 0 1 5-9Z"
  fill="url(#teal)"
 />

 <path
  d="M17 17h30a2 2 0 0 1 2 2v3h-5v-1H18a3 3 0 0 0-3 3v25l13-13a3 3 0 0 1 4 0l8 8 5-5a3 3 0 0 1 4 0l6 6v7H18a8 8 0 0 1-8-8V24a7 7 0 0 1 7-7Z"
  fill="url(#teal)"
 />

 <circle
  cx="24"
  cy="28"
  r="4.7"
  fill="url(#coral)"
 />

 <g fill="url(#teal)">
  <rect x="53" y="17" width="6" height="6" rx="1"/>
  <rect x="60" y="11" width="4" height="4" rx=".8"/>
  <rect x="58" y="27" width="5" height="5" rx=".8"/>
  <rect x="50" y="31" width="4" height="4" rx=".8"/>
  <rect x="63" y="35" width="5" height="5" rx=".8"/>
  <rect x="55" y="42" width="5" height="5" rx=".8"/>
  <rect x="62" y="50" width="4" height="4" rx=".8"/>
 </g>

 <g fill="url(#coral)">
  <rect x="63" y="20" width="5" height="5" rx=".8"/>
  <rect x="66" y="29" width="4" height="4" rx=".8"/>
  <rect x="57" y="36" width="4" height="4" rx=".8"/>
  <rect x="66" y="43" width="5" height="5" rx=".8"/>
  <rect x="58" y="55" width="4" height="4" rx=".8"/>
 </g>
</svg>
)DYNFAV";

// ============================================================
// OFFLINE FONT ASSET SERVER
// ============================================================

void sendWoff2Font(
  const uint8_t* fontData,
  size_t fontLength
) {
  /*
    Fonts do not change while this firmware is installed,
    so the browser can cache them for one year.
  */
  server.sendHeader(
    "Cache-Control",
    "public, max-age=31536000, immutable"
  );

  server.sendHeader(
    "Access-Control-Allow-Origin",
    "*"
  );

  /*
    send_P() reads directly from ESP32 program memory.

    An explicit length is required because WOFF2 is binary
    and contains zero bytes.
  */
  server.send_P(
    200,
    "font/woff2",
    reinterpret_cast<PGM_P>(fontData),
    fontLength
  );
}

void startWebServer() {
  server.on("/", HTTP_GET, []() {
    server.sendHeader(
      "Cache-Control",
      "no-store, no-cache, must-revalidate"
    );

    server.send_P(
      200,
      "text/html",
      INDEX_HTML
    );
  });

  server.on(
    "/api/status",
    HTTP_GET,
    handleStatus
  );

  server.on(
    "/api/photos",
    HTTP_GET,
    handlePhotoList
  );

  server.on(
    "/api/photo",
    HTTP_GET,
    handlePhotoDownload
  );

  server.on(
    "/api/photos/upload",
    HTTP_POST,
    handlePhotoUploadComplete,
    handlePhotoUploadData
  );

  server.on(
    "/api/photos/delete",
    HTTP_POST,
    handlePhotoDelete
  );

  server.on(
    "/api/photos/delete-batch",
    HTTP_POST,
    handlePhotoBatchDelete
  );

  server.on(
    "/api/photos/rename",
    HTTP_POST,
    handlePhotoRename
  );

  server.on(
    "/api/photos/move",
    HTTP_POST,
    handlePhotoMove
  );

  server.on(
    "/api/photos/select",
    HTTP_POST,
    handlePhotoSelection
  );


  server.on(
    "/api/categories/create",
    HTTP_POST,
    handleCategoryCreate
  );

  server.on(
    "/api/categories/update",
    HTTP_POST,
    handleCategoryUpdate
  );

  server.on(
    "/api/categories/delete",
    HTTP_POST,
    handleCategoryDelete
  );

  server.on(
    "/api/photos/categories",
    HTTP_POST,
    handlePhotoCategoryAssignment
  );

  server.on(
    "/api/library/filter",
    HTTP_POST,
    handleLibraryCategoryFilter
  );

  server.on(
    "/api/slideshow/settings",
    HTTP_POST,
    handleSlideshowSettings
  );

  server.on(
    "/api/slideshow/action",
    HTTP_POST,
    handleSlideshowAction
  );

  server.on(
    "/api/settings/device",
    HTTP_POST,
    handleDeviceSettings
  );

  server.on(
    "/api/settings/wifi",
    HTTP_POST,
    handleWiFiSettings
  );

  server.on(
    "/api/wifi/scan",
    HTTP_GET,
    handleWiFiScan
  );

  server.on(
    "/api/maintenance/cleanup",
    HTTP_POST,
    handleCleanup
  );

  server.on(
    "/api/maintenance/restart",
    HTTP_POST,
    handleRestart
  );

  server.on(
    "/api/maintenance/reset-slideshow",
    HTTP_POST,
    handleResetSlideshow
  );

  server.on(
    "/api/maintenance/factory-reset",
    HTTP_POST,
    handleFactoryReset
  );
// ==========================================================
  // OFFLINE FONT ROUTES
  // ==========================================================

  server.on(
    "/assets/audiowide-regular.woff2",
    HTTP_GET,
    []() {
      sendWoff2Font(
        AUDiOWIDE_REGULAR_WOFF2,
        AUDiOWIDE_REGULAR_WOFF2_LEN
      );
    }
  );

  server.on(
    "/assets/montserrat-bold.woff2",
    HTTP_GET,
    []() {
      sendWoff2Font(
        MONTSERRAT_BOLD_WOFF2,
        MONTSERRAT_BOLD_WOFF2_LEN
      );
    }
  );

  server.on(
    "/assets/inter-regular.woff2",
    HTTP_GET,
    []() {
      sendWoff2Font(
        INTER_REGULAR_WOFF2,
        INTER_REGULAR_WOFF2_LEN
      );
    }
  );

  server.on(
    "/assets/inter-medium.woff2",
    HTTP_GET,
    []() {
      sendWoff2Font(
        INTER_MEDIUM_WOFF2,
        INTER_MEDIUM_WOFF2_LEN
      );
    }
  );

  server.on(
    "/assets/inter-semibold.woff2",
    HTTP_GET,
    []() {
      sendWoff2Font(
        INTER_SEMIBOLD_WOFF2,
        INTER_SEMIBOLD_WOFF2_LEN
      );
    }
  );
  

  // ==========================================================
  // DYNAPIX BROWSER FAVICON
  // ==========================================================

  server.on(
    "/favicon.svg",
    HTTP_GET,
    []() {
      server.sendHeader(
        "Cache-Control",
        "public, max-age=604800"
      );

      server.sendHeader(
        "X-Content-Type-Options",
        "nosniff"
      );

      server.send_P(
        200,
        "image/svg+xml",
        DYNAPIX_FAVICON_SVG
      );
    }
  );

  /*
    Older browsers may request /favicon.ico automatically.
    Redirect them to the SVG icon.
  */
  server.on(
    "/favicon.ico",
    HTTP_GET,
    []() {
      server.sendHeader(
        "Location",
        "/favicon.svg?v=2"
      );

      server.send(
        302,
        "text/plain",
        ""
      );
    }
  );

  clockWeatherRegisterRoutes(server);

  calendarExtensionRegisterRoutes(server);

  server.onNotFound([]() {
    server.send(
      404,
      "text/plain",
      "Not found"
    );
  });

  server.begin();

  Serial.println(
    "[Web] Server started"
  );
}

// ============================================================
// SETUP
// ============================================================

void setup() {
  Serial.begin(115200);

  unsigned long serialStart =
    millis();

  while (
    !Serial &&
    millis() - serialStart < 3000
  ) {
    delay(10);
  }

  Serial.println();
  Serial.println("====================================");
  Serial.println(" DynaPix Photo Frame");
  Serial.println(" Neutral-Protected Quality Engine");
  Serial.println("====================================");

  loadSettings();

  bool storageMounted =
    LittleFS.begin(false);

  if (!storageMounted) {
    bool previouslyInitialized =
      preferences.getBool(
        "fsready",
        false
      );

    if (!previouslyInitialized) {
      Serial.println(
        "[Storage] First initialization; formatting LittleFS"
      );

      storageMounted =
        LittleFS.begin(true);
    }
  }

  if (!storageMounted) {
    applicationState = "error";

    applicationMessage =
      "Storage mount failed";

    Serial.println(
      "[Storage] LittleFS mount failed"
    );
  } else {
    preferences.putBool(
      "fsready",
      true
    );

    if (!LittleFS.exists(
      PHOTO_DIRECTORY
    )) {
      LittleFS.mkdir(
        PHOTO_DIRECTORY
      );
    }

    loadCategories();

    slideshowCategoryIds =
      cleanCategoryCsv(
        slideshowCategoryIds,
        true
      );

    if (
      libraryCategoryFilter != "all" &&
      libraryCategoryFilter !=
        UNCATEGORIZED_CATEGORY_ID &&
      findCategoryIndex(
        libraryCategoryFilter
      ) < 0
    ) {
      libraryCategoryFilter = "all";
    }

    loadPhotoLibrary();

    Serial.printf(
      "[Storage] Total: %u bytes\n",
      (unsigned int)
      LittleFS.totalBytes()
    );

    Serial.printf(
      "[Storage] Used: %u bytes\n",
      (unsigned int)
      LittleFS.usedBytes()
    );
  }

  pinMode(
    DEV_POWER_PIN,
    OUTPUT
  );

  digitalWrite(
    DEV_POWER_PIN,
    HIGH
  );

  delay(200);

  pinMode(EPD_CS_PIN, OUTPUT);
  pinMode(EPD_DC_PIN, OUTPUT);
  pinMode(EPD_RST_PIN, OUTPUT);
  pinMode(EPD_BUSY_PIN, INPUT);

  digitalWrite(EPD_CS_PIN, HIGH);
  digitalWrite(EPD_DC_PIN, HIGH);
  digitalWrite(EPD_RST_PIN, HIGH);

  SPI.begin(
    EPD_SCK_PIN,
    -1,
    EPD_MOSI_PIN,
    EPD_CS_PIN
  );

  SPI.setFrequency(
    SPI_FREQUENCY
  );

  SPI.setDataMode(
    SPI_MODE0
  );

  SPI.setBitOrder(
    MSBFIRST
  );

  startNetworking();

  clockWeatherBegin(
    &preferences,
    displayExtensionFile
  );

  calendarExtensionBegin(
    &preferences,
    displayExtensionFile
  );

  startWebServer();

  applicationState = "ready";
  applicationMessage = "Ready";

  /*
    E-paper retains its image through power loss.
    Resume the timer without needlessly refreshing the same image.
  */
  if (
    slideshowRunning &&
    eligiblePhotoCount() > 0
  ) {
    int currentIndex =
      findPhotoIndex(
        currentPhotoId
      );

    if (
      currentIndex >= 0 &&
      photoEligible(currentIndex)
    ) {
      addDisplayHistory(
        currentPhotoId
      );

      lastDisplayTime =
        millis();
    } else {
      requestDisplayByIndex(
        firstSelectedIndex()
      );
    }
  }

  Serial.print(
    "[Web] Router: http://"
  );

  Serial.println(
    WiFi.localIP()
  );

  Serial.print(
    "[Web] Fallback: http://"
  );

  Serial.println(
    WiFi.softAPIP()
  );

  Serial.print(
    "[Web] Hostname: http://"
  );

  Serial.print(hostName);
  Serial.println(".local");
}

// ============================================================
// MAIN LOOP
// ============================================================

void loop() {
  server.handleClient();

  clockWeatherLoop();
  calendarExtensionLoop();

  if (
    restartPending &&
    millis() >= restartAt
  ) {
    delay(200);
    ESP.restart();
  }

  if (
    displayRequested &&
    !displayRunning
  ) {
    displayRequested = false;
    displayRunning = true;

    applicationState =
      "refreshing";

    applicationMessage =
      "Refreshing E-paper display";

    bool successful =
      sendStoredPhotoToDisplay(
        requestedPhotoPath
      );

    displayRunning = false;

    if (successful) {
      currentPhotoId =
        requestedPhotoId;

      preferences.putString(
        "current",
        currentPhotoId
      );

      if (historyNavigationRequest) {
        historyPosition =
          requestedHistoryPosition;
      } else {
        addDisplayHistory(
          currentPhotoId
        );
      }

      historyNavigationRequest =
        false;

      requestedHistoryPosition =
        -1;

      lastDisplayTime =
        millis();

      applicationState =
        "complete";

      applicationMessage =
        "Display refresh complete";
    } else {
      historyNavigationRequest =
        false;

      requestedHistoryPosition =
        -1;

      applicationState =
        "error";

      applicationMessage =
        "Display refresh failed";
    }
  }

  if (
    slideshowRunning &&
    !clockWeatherActive() &&
    !calendarExtensionActive() &&
    !displayRunning &&
    !displayRequested &&
    eligiblePhotoCount() > 1 &&
    lastDisplayTime > 0 &&
    millis() - lastDisplayTime >=
      slideIntervalSeconds * 1000UL
  ) {
    int next =
      chooseNextPhoto();

    if (next >= 0) {
      requestDisplayByIndex(next);
    }
  }

  delay(2);
}