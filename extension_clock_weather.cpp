#include "extension_clock_weather.h"

#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <LittleFS.h>

#include <time.h>
#include <math.h>

// ============================================================
// CLOCK & WEATHER EXTENSION
// ============================================================

namespace {

#define CW_WIDTH 600
#define CW_HEIGHT 400
#define CW_PANEL_WIDTH 400
#define CW_PANEL_HEIGHT 600
#define CW_BYTES 120000

#define CW_BLACK  0x0
#define CW_WHITE  0x1
#define CW_YELLOW 0x2
#define CW_RED    0x3
#define CW_BLUE   0x5
#define CW_GREEN  0x6

#define CW_DASHBOARD_FILE "/clock_weather.e6"
#define CW_MIN_INTERVAL 180UL
#define CW_DEFAULT_INTERVAL 300UL
#define CW_WEATHER_INTERVAL 1800UL

Preferences* prefs = nullptr;
WebServer* webServer = nullptr;

ClockWeatherDisplayCallback displayCallback = nullptr;

bool active = false;
bool extensionBusy = false;
bool weatherBusy = false;
bool timeConfigured = false;

String layout = "analog-weather";
String locationName = "Hong Kong";
String timezoneSpec =
  "Asia/Hong_Kong";

String temperatureUnit = "fahrenheit";
String windUnit = "mph";
String mountDirection = "left";

double latitude = 22.3193;
double longitude = 114.1694;

uint32_t updateInterval = CW_DEFAULT_INTERVAL;
uint32_t weatherInterval = CW_WEATHER_INTERVAL;

unsigned long lastDisplayMillis = 0;
unsigned long lastWeatherMillis = 0;
unsigned long lastRenderMillis = 0;

struct WeatherData {
  bool valid = false;

  float temperature = 0;
  float apparent = 0;
  float humidity = 0;
  float wind = 0;

  int weatherCode = 0;

  float high[3] = {0, 0, 0};
  float low[3] = {0, 0, 0};
  float rainChance[3] = {0, 0, 0};
  int dailyCode[3] = {0, 0, 0};

  unsigned long updatedMillis = 0;
};

WeatherData weather;

int utcOffsetSeconds = 8 * 60 * 60;
String lastWeatherError = "";

// ============================================================
// SMALL BITMAP FONT
// ============================================================

const uint8_t* glyphFor(char character) {
  static const uint8_t SPACE[5] = {0,0,0,0,0};
  static const uint8_t UNKNOWN[5] = {62,65,93,65,62};

  static const uint8_t A[5] = {126,17,17,17,126};
  static const uint8_t B[5] = {127,73,73,73,54};
  static const uint8_t C[5] = {62,65,65,65,34};
  static const uint8_t D[5] = {127,65,65,34,28};
  static const uint8_t E[5] = {127,73,73,73,65};
  static const uint8_t F[5] = {127,9,9,9,1};
  static const uint8_t G[5] = {62,65,73,73,122};
  static const uint8_t H[5] = {127,8,8,8,127};
  static const uint8_t I[5] = {65,65,127,65,65};
  static const uint8_t J[5] = {32,64,65,63,1};
  static const uint8_t K[5] = {127,8,20,34,65};
  static const uint8_t L[5] = {127,64,64,64,64};
  static const uint8_t M[5] = {127,2,12,2,127};
  static const uint8_t N[5] = {127,4,8,16,127};
  static const uint8_t O[5] = {62,65,65,65,62};
  static const uint8_t P[5] = {127,9,9,9,6};
  static const uint8_t Q[5] = {62,65,81,33,94};
  static const uint8_t R[5] = {127,9,25,41,70};
  static const uint8_t S[5] = {38,73,73,73,50};
  static const uint8_t T[5] = {1,1,127,1,1};
  static const uint8_t U[5] = {63,64,64,64,63};
  static const uint8_t V[5] = {31,32,64,32,31};
  static const uint8_t W[5] = {127,32,24,32,127};
  static const uint8_t X[5] = {99,20,8,20,99};
  static const uint8_t Y[5] = {7,8,112,8,7};
  static const uint8_t Z[5] = {97,81,73,69,67};

  static const uint8_t N0[5] = {62,81,73,69,62};
  static const uint8_t N1[5] = {0,66,127,64,0};
  static const uint8_t N2[5] = {98,81,73,73,70};
  static const uint8_t N3[5] = {34,65,73,73,54};
  static const uint8_t N4[5] = {24,20,18,127,16};
  static const uint8_t N5[5] = {39,69,69,69,57};
  static const uint8_t N6[5] = {60,74,73,73,48};
  static const uint8_t N7[5] = {1,113,9,5,3};
  static const uint8_t N8[5] = {54,73,73,73,54};
  static const uint8_t N9[5] = {6,73,73,41,30};

  static const uint8_t COLON[5] = {0,54,54,0,0};
  static const uint8_t DASH[5] = {8,8,8,8,8};
  static const uint8_t DOT[5] = {0,96,96,0,0};
  static const uint8_t SLASH[5] = {32,16,8,4,2};
  static const uint8_t PLUS[5] = {8,8,62,8,8};
  static const uint8_t DEGREE[5] = {6,9,9,6,0};

  if (character >= 'a' && character <= 'z') {
    character -= 32;
  }

  switch (character) {
    case ' ': return SPACE;

    case 'A': return A;
    case 'B': return B;
    case 'C': return C;
    case 'D': return D;
    case 'E': return E;
    case 'F': return F;
    case 'G': return G;
    case 'H': return H;
    case 'I': return I;
    case 'J': return J;
    case 'K': return K;
    case 'L': return L;
    case 'M': return M;
    case 'N': return N;
    case 'O': return O;
    case 'P': return P;
    case 'Q': return Q;
    case 'R': return R;
    case 'S': return S;
    case 'T': return T;
    case 'U': return U;
    case 'V': return V;
    case 'W': return W;
    case 'X': return X;
    case 'Y': return Y;
    case 'Z': return Z;

    case '0': return N0;
    case '1': return N1;
    case '2': return N2;
    case '3': return N3;
    case '4': return N4;
    case '5': return N5;
    case '6': return N6;
    case '7': return N7;
    case '8': return N8;
    case '9': return N9;

    case ':': return COLON;
    case '-': return DASH;
    case '.': return DOT;
    case '/': return SLASH;
    case '+': return PLUS;
    case '*': return DEGREE;

    default:
      return UNKNOWN;
  }
}

// ============================================================
// DRAWING PRIMITIVES
// ============================================================

uint8_t* canvas = nullptr;

void clearCanvas(uint8_t color) {
  if (!canvas) {
    return;
  }

  memset(
    canvas,
    color,
    CW_WIDTH * CW_HEIGHT
  );
}

void setPixel(
  int x,
  int y,
  uint8_t color
) {
  if (
    !canvas ||
    x < 0 ||
    x >= CW_WIDTH ||
    y < 0 ||
    y >= CW_HEIGHT
  ) {
    return;
  }

  canvas[y * CW_WIDTH + x] = color;
}

void fillRect(
  int x,
  int y,
  int width,
  int height,
  uint8_t color
) {
  for (int row = 0; row < height; row++) {
    for (int column = 0; column < width; column++) {
      setPixel(
        x + column,
        y + row,
        color
      );
    }
  }
}

void drawLine(
  int x0,
  int y0,
  int x1,
  int y1,
  uint8_t color,
  int thickness = 1
) {
  int dx = abs(x1 - x0);
  int sx = x0 < x1 ? 1 : -1;

  int dy = -abs(y1 - y0);
  int sy = y0 < y1 ? 1 : -1;

  int error = dx + dy;

  while (true) {
    int radius = thickness / 2;

    for (int offsetY = -radius;
         offsetY <= radius;
         offsetY++) {
      for (int offsetX = -radius;
           offsetX <= radius;
           offsetX++) {
        setPixel(
          x0 + offsetX,
          y0 + offsetY,
          color
        );
      }
    }

    if (x0 == x1 && y0 == y1) {
      break;
    }

    int twiceError = 2 * error;

    if (twiceError >= dy) {
      error += dy;
      x0 += sx;
    }

    if (twiceError <= dx) {
      error += dx;
      y0 += sy;
    }
  }
}

void drawCircle(
  int centerX,
  int centerY,
  int radius,
  uint8_t color,
  int thickness = 1
) {
  for (int step = 0; step < 360; step++) {
    float angle =
      step * PI / 180.0f;

    int x =
      centerX +
      round(cos(angle) * radius);

    int y =
      centerY +
      round(sin(angle) * radius);

    for (int offset = 0;
         offset < thickness;
         offset++) {
      setPixel(
        centerX +
        round(cos(angle) * (radius - offset)),
        centerY +
        round(sin(angle) * (radius - offset)),
        color
      );
    }
  }
}

void fillCircle(
  int centerX,
  int centerY,
  int radius,
  uint8_t color
) {
  for (int y = -radius; y <= radius; y++) {
    int span =
      sqrt(
        radius * radius -
        y * y
      );

    for (int x = -span; x <= span; x++) {
      setPixel(
        centerX + x,
        centerY + y,
        color
      );
    }
  }
}

int textWidth(
  const String& text,
  int scale
) {
  return text.length() * 6 * scale;
}

void drawCharacter(
  int x,
  int y,
  char character,
  int scale,
  uint8_t color
) {
  const uint8_t* glyph =
    glyphFor(character);

  for (int column = 0; column < 5; column++) {
    uint8_t bits = glyph[column];

    for (int row = 0; row < 7; row++) {
      if (bits & (1 << row)) {
        fillRect(
          x + column * scale,
          y + row * scale,
          scale,
          scale,
          color
        );
      }
    }
  }
}

void drawText(
  int x,
  int y,
  const String& text,
  int scale,
  uint8_t color
) {
  for (int index = 0;
       index < text.length();
       index++) {
    drawCharacter(
      x + index * 6 * scale,
      y,
      text.charAt(index),
      scale,
      color
    );
  }
}

void drawCenteredText(
  int centerX,
  int y,
  const String& text,
  int scale,
  uint8_t color
) {
  drawText(
    centerX -
      textWidth(text, scale) / 2,
    y,
    text,
    scale,
    color
  );
}

// ============================================================
// JSON HELPERS
// ============================================================

bool extractJsonNumber(
  const String& json,
  const String& key,
  float& value
) {
  String marker = "\"" + key + "\":";

  int position = json.indexOf(marker);

  if (position < 0) {
    return false;
  }

  position += marker.length();

  while (
    position < json.length() &&
    (
      json.charAt(position) == ' ' ||
      json.charAt(position) == '\n'
    )
  ) {
    position++;
  }

  int end = position;

  while (
    end < json.length() &&
    (
      isDigit(json.charAt(end)) ||
      json.charAt(end) == '-' ||
      json.charAt(end) == '+' ||
      json.charAt(end) == '.' ||
      json.charAt(end) == 'e' ||
      json.charAt(end) == 'E'
    )
  ) {
    end++;
  }

  if (end <= position) {
    return false;
  }

  value =
    json.substring(position, end).toFloat();

  return true;
}

bool extractJsonArray(
  const String& json,
  const String& key,
  float* values,
  int count
) {
  String marker = "\"" + key + "\":[";

  int position = json.indexOf(marker);

  if (position < 0) {
    return false;
  }

  position += marker.length();

  for (int index = 0; index < count; index++) {
    while (
      position < json.length() &&
      (
        json.charAt(position) == ' ' ||
        json.charAt(position) == ','
      )
    ) {
      position++;
    }

    int end = position;

    while (
      end < json.length() &&
      (
        isDigit(json.charAt(end)) ||
        json.charAt(end) == '-' ||
        json.charAt(end) == '+' ||
        json.charAt(end) == '.' ||
        json.charAt(end) == 'e' ||
        json.charAt(end) == 'E'
      )
    ) {
      end++;
    }

    if (end <= position) {
      return false;
    }

    values[index] =
      json.substring(position, end).toFloat();

    position = end;
  }

  return true;
}

bool extractJsonIntArray(
  const String& json,
  const String& key,
  int* values,
  int count
) {
  float temporary[3];

  if (!extractJsonArray(
    json,
    key,
    temporary,
    count
  )) {
    return false;
  }

  for (int index = 0; index < count; index++) {
    values[index] =
      round(temporary[index]);
  }

  return true;
}


// ============================================================
// GLOBAL LOCATION SEARCH
// ============================================================

String urlEncode(const String& source) {
  String result;

  const char* hex =
    "0123456789ABCDEF";

  for (int index = 0;
       index < source.length();
       index++) {
    uint8_t character =
      static_cast<uint8_t>(
        source.charAt(index)
      );

    bool unreserved =
      (
        character >= 'a' &&
        character <= 'z'
      ) ||
      (
        character >= 'A' &&
        character <= 'Z'
      ) ||
      (
        character >= '0' &&
        character <= '9'
      ) ||
      character == '-' ||
      character == '_' ||
      character == '.' ||
      character == '~';

    if (unreserved) {
      result +=
        static_cast<char>(character);
    } else if (character == ' ') {
      result += "%20";
    } else {
      result += '%';
      result += hex[
        (character >> 4) & 0x0F
      ];
      result += hex[
        character & 0x0F
      ];
    }
  }

  return result;
}

bool secureHttpGet(
  const String& url,
  String& responseBody,
  int& responseCode
) {
  if (WiFi.status() != WL_CONNECTED) {
    responseCode = -1;
    return false;
  }

  WiFiClientSecure secureClient;
  secureClient.setInsecure();

  HTTPClient http;

  if (!http.begin(secureClient, url)) {
    responseCode = -2;
    return false;
  }

  http.setConnectTimeout(15000);
  http.setTimeout(20000);

  http.setUserAgent(
    "DynaPix-Epaper/1.0"
  );

  responseCode = http.GET();

  if (responseCode > 0) {
    responseBody = http.getString();
  } else {
    responseBody =
      http.errorToString(
        responseCode
      );
  }

  http.end();

  return responseCode ==
    HTTP_CODE_OK;
}

void handleLocationSearch() {
  String query =
    webServer->arg("query");

  query.trim();

  if (query.length() < 2) {
    webServer->send(
      400,
      "application/json",
      "{\"error\":true,\"reason\":\"Enter at least two characters\"}"
    );

    return;
  }

  String url =
    "https://geocoding-api.open-meteo.com/"
    "v1/search"
    "?name=" +
    urlEncode(query) +
    "&count=10"
    "&language=en"
    "&format=json";

  String responseBody;
  int responseCode = 0;

  bool success = secureHttpGet(
    url,
    responseBody,
    responseCode
  );

  if (!success) {
    String errorResponse =
      "{\"error\":true,"
      "\"reason\":\"Location search failed\","
      "\"httpCode\":" +
      String(responseCode) +
      "}";

    webServer->send(
      502,
      "application/json",
      errorResponse
    );

    return;
  }

  webServer->sendHeader(
    "Cache-Control",
    "no-store"
  );

  webServer->send(
    200,
    "application/json",
    responseBody
  );
}

// ============================================================
// WEATHER
// ============================================================

String weatherDescription(int code) {
  if (code == 0) {
    return "CLEAR";
  }

  if (code <= 3) {
    return "PARTLY CLOUDY";
  }

  if (code == 45 || code == 48) {
    return "FOG";
  }

  if (code >= 51 && code <= 57) {
    return "DRIZZLE";
  }

  if (code >= 61 && code <= 67) {
    return "RAIN";
  }

  if (code >= 71 && code <= 77) {
    return "SNOW";
  }

  if (code >= 80 && code <= 82) {
    return "SHOWERS";
  }

  if (code >= 85 && code <= 86) {
    return "SNOW SHOWERS";
  }

  if (code >= 95) {
    return "THUNDERSTORM";
  }

  return "CLOUDY";
}

void drawWeatherIcon(
  int x,
  int y,
  int code,
  int size
) {
  if (code == 0) {
    fillCircle(
      x,
      y,
      size / 3,
      CW_YELLOW
    );

    for (int angle = 0;
         angle < 360;
         angle += 45) {
      float radians =
        angle * PI / 180.0f;

      drawLine(
        x + cos(radians) * size / 2,
        y + sin(radians) * size / 2,
        x + cos(radians) * size * 0.72,
        y + sin(radians) * size * 0.72,
        CW_YELLOW,
        3
      );
    }

    return;
  }

  if (code <= 3) {
    fillCircle(
      x - size / 4,
      y - size / 5,
      size / 4,
      CW_YELLOW
    );
  }

  fillCircle(
    x - size / 5,
    y,
    size / 4,
    CW_BLUE
  );

  fillCircle(
    x + size / 12,
    y - size / 7,
    size / 3,
    CW_BLUE
  );

  fillCircle(
    x + size / 3,
    y,
    size / 4,
    CW_BLUE
  );

  fillRect(
    x - size / 2,
    y,
    size,
    size / 4,
    CW_BLUE
  );

  if (
    (code >= 51 && code <= 67) ||
    (code >= 80 && code <= 82)
  ) {
    for (int offset = -20;
         offset <= 20;
         offset += 20) {
      drawLine(
        x + offset,
        y + size / 3,
        x + offset - 6,
        y + size / 2,
        CW_BLUE,
        3
      );
    }
  }

  if (code >= 95) {
    drawLine(
      x,
      y + size / 4,
      x - 9,
      y + size / 2,
      CW_RED,
      5
    );

    drawLine(
      x - 9,
      y + size / 2,
      x + 4,
      y + size / 2,
      CW_RED,
      5
    );

    drawLine(
      x + 4,
      y + size / 2,
      x - 5,
      y + size * 3 / 4,
      CW_RED,
      5
    );
  }
}


/*
  Extracts the contents of a named JSON object.

  Example:

      "current": {
        "temperature_2m": 84.2
      }

  becomes:

      "temperature_2m": 84.2

  This prevents numeric parsers from accidentally matching
  similarly named entries inside current_units or daily_units.
*/
bool extractJsonObject(
  const String& json,
  const String& key,
  String& objectContent
) {
  String marker =
    "\"" + key + "\":";

  int keyPosition =
    json.indexOf(marker);

  if (keyPosition < 0) {
    return false;
  }

  int openingBrace =
    json.indexOf(
      '{',
      keyPosition + marker.length()
    );

  if (openingBrace < 0) {
    return false;
  }

  int depth = 0;

  bool insideString = false;
  bool escaped = false;

  for (
    int index = openingBrace;
    index < json.length();
    index++
  ) {
    char character =
      json.charAt(index);

    if (insideString) {
      if (escaped) {
        escaped = false;
      }

      else if (character == '\\') {
        escaped = true;
      }

      else if (character == '"') {
        insideString = false;
      }

      continue;
    }

    if (character == '"') {
      insideString = true;
      continue;
    }

    if (character == '{') {
      depth++;
    }

    else if (character == '}') {
      depth--;

      if (depth == 0) {
        objectContent =
          json.substring(
            openingBrace + 1,
            index
          );

        return true;
      }
    }
  }

  return false;
}


bool fetchWeather() {
  if (WiFi.status() != WL_CONNECTED) {
    lastWeatherError =
      "ESP32 is not connected to a router";

    return false;
  }

  weatherBusy = true;
  lastWeatherError = "";

  String unitTemperature =
    temperatureUnit == "fahrenheit"
      ? "fahrenheit"
      : "celsius";

  String unitWind =
    windUnit == "mph"
      ? "mph"
      : "kmh";

  String url =
    "https://api.open-meteo.com/v1/forecast"
    "?latitude=" +
    String(latitude, 6) +
    "&longitude=" +
    String(longitude, 6) +
    "&current="
    "temperature_2m,"
    "relative_humidity_2m,"
    "apparent_temperature,"
    "weather_code,"
    "wind_speed_10m"
    "&daily="
    "weather_code,"
    "temperature_2m_max,"
    "temperature_2m_min,"
    "precipitation_probability_max"
    "&temperature_unit=" +
    unitTemperature +
    "&wind_speed_unit=" +
    unitWind +
    "&timezone=auto"
    "&forecast_days=3";

  Serial.println(
    "[Weather] Requesting Open-Meteo"
  );

  Serial.print(
    "[Weather] Location: "
  );

  Serial.print(locationName);

  Serial.print(" (");
  Serial.print(latitude, 6);
  Serial.print(", ");
  Serial.print(longitude, 6);
  Serial.println(")");

  String json;
  int responseCode = 0;

  bool downloaded = secureHttpGet(
    url,
    json,
    responseCode
  );

  if (!downloaded) {
    lastWeatherError =
      "Open-Meteo request failed: " +
      String(responseCode);

    if (json.length() > 0) {
      lastWeatherError +=
        " " +
        json.substring(
          0,
          min(
            120,
            (int)json.length()
          )
        );
    }

    Serial.print(
      "[Weather] Download error: "
    );

    Serial.println(lastWeatherError);

    weatherBusy = false;

    return false;
  }

  Serial.printf(
    "[Weather] Response size: %u bytes\n",
    (unsigned int)json.length()
  );

  /*
    The response contains metadata objects before the actual
    values:

      current_units.temperature_2m = "°C"
      current.temperature_2m       = 28.4

    Parse only inside current and daily so the unit strings
    cannot be mistaken for numeric values.
  */
  String currentJson;
  String dailyJson;

  bool currentObjectFound =
    extractJsonObject(
      json,
      "current",
      currentJson
    );

  bool dailyObjectFound =
    extractJsonObject(
      json,
      "daily",
      dailyJson
    );

  if (
    !currentObjectFound ||
    !dailyObjectFound
  ) {
    lastWeatherError =
      "Current or daily weather data is missing";

    Serial.println(
      "[Weather] JSON object extraction failed"
    );

    Serial.print(
      "[Weather] Current object: "
    );

    Serial.println(
      currentObjectFound
        ? "found"
        : "missing"
    );

    Serial.print(
      "[Weather] Daily object: "
    );

    Serial.println(
      dailyObjectFound
        ? "found"
        : "missing"
    );

    Serial.println(
      json.substring(
        0,
        min(
          700,
          (int)json.length()
        )
      )
    );

    weatherBusy = false;

    return false;
  }

  float currentWeatherCode = 0;
  float offsetValue =
    utcOffsetSeconds;

  bool temperatureParsed =
    extractJsonNumber(
      currentJson,
      "temperature_2m",
      weather.temperature
    );

  bool apparentParsed =
    extractJsonNumber(
      currentJson,
      "apparent_temperature",
      weather.apparent
    );

  bool humidityParsed =
    extractJsonNumber(
      currentJson,
      "relative_humidity_2m",
      weather.humidity
    );

  bool windParsed =
    extractJsonNumber(
      currentJson,
      "wind_speed_10m",
      weather.wind
    );

  bool currentCodeParsed =
    extractJsonNumber(
      currentJson,
      "weather_code",
      currentWeatherCode
    );

  bool highParsed =
    extractJsonArray(
      dailyJson,
      "temperature_2m_max",
      weather.high,
      3
    );

  bool lowParsed =
    extractJsonArray(
      dailyJson,
      "temperature_2m_min",
      weather.low,
      3
    );

  bool rainParsed =
    extractJsonArray(
      dailyJson,
      "precipitation_probability_max",
      weather.rainChance,
      3
    );

  bool dailyCodeParsed =
    extractJsonIntArray(
      dailyJson,
      "weather_code",
      weather.dailyCode,
      3
    );

  /*
    utc_offset_seconds is a top-level value, so it remains
    parsed from the complete response.
  */
  extractJsonNumber(
    json,
    "utc_offset_seconds",
    offsetValue
  );

  bool success =
    temperatureParsed &&
    apparentParsed &&
    humidityParsed &&
    windParsed &&
    currentCodeParsed &&
    highParsed &&
    lowParsed &&
    rainParsed &&
    dailyCodeParsed;

  if (success) {
    weather.weatherCode =
      round(currentWeatherCode);

    utcOffsetSeconds =
      round(offsetValue);

    weather.valid = true;

    weather.updatedMillis =
      millis();

    lastWeatherMillis =
      millis();

    lastWeatherError = "";

    Serial.println(
      "[Weather] Weather parsed successfully"
    );

    Serial.print(
      "[Weather] Temperature: "
    );

    Serial.println(
      weather.temperature,
      1
    );

    Serial.print(
      "[Weather] Description: "
    );

    Serial.println(
      weatherDescription(
        weather.weatherCode
      )
    );

    Serial.print(
      "[Weather] UTC offset: "
    );

    Serial.println(
      utcOffsetSeconds
    );
  }

  else {
    lastWeatherError =
      "Weather response could not be parsed";

    Serial.println(
      "[Weather] Parsing failed"
    );

    Serial.printf(
      "[Weather] temperature=%d apparent=%d "
      "humidity=%d wind=%d currentCode=%d\n",
      temperatureParsed,
      apparentParsed,
      humidityParsed,
      windParsed,
      currentCodeParsed
    );

    Serial.printf(
      "[Weather] high=%d low=%d rain=%d "
      "dailyCode=%d\n",
      highParsed,
      lowParsed,
      rainParsed,
      dailyCodeParsed
    );

    Serial.println(
      "[Weather] Current JSON:"
    );

    Serial.println(
      currentJson.substring(
        0,
        min(
          500,
          (int)currentJson.length()
        )
      )
    );

    Serial.println(
      "[Weather] Daily JSON:"
    );

    Serial.println(
      dailyJson.substring(
        0,
        min(
          700,
          (int)dailyJson.length()
        )
      )
    );
  }

  weatherBusy = false;

  return success;
}

// ============================================================
// CLOCK DRAWING
// ============================================================

bool currentLocalTime(struct tm& timeInfo) {
  time_t now = time(nullptr);

  if (now < 100000) {
    return false;
  }

  /*
    Open-Meteo returns the current UTC offset for the selected
    global location. Adding it before gmtime_r() gives local
    civil time without requiring a complete timezone database
    on the ESP32.
  */
  now += utcOffsetSeconds;

  gmtime_r(
    &now,
    &timeInfo
  );

  return true;
}

String timeText(
  const struct tm& timeInfo,
  bool twelveHour
) {
  int hour = timeInfo.tm_hour;

  if (twelveHour) {
    hour %= 12;

    if (hour == 0) {
      hour = 12;
    }
  }

  char buffer[8];

  snprintf(
    buffer,
    sizeof(buffer),
    "%02d:%02d",
    hour,
    timeInfo.tm_min
  );

  return String(buffer);
}

String dateText(
  const struct tm& timeInfo
) {
  static const char* weekdays[] = {
    "SUN",
    "MON",
    "TUE",
    "WED",
    "THU",
    "FRI",
    "SAT"
  };

  static const char* months[] = {
    "JAN",
    "FEB",
    "MAR",
    "APR",
    "MAY",
    "JUN",
    "JUL",
    "AUG",
    "SEP",
    "OCT",
    "NOV",
    "DEC"
  };

  char buffer[32];

  snprintf(
    buffer,
    sizeof(buffer),
    "%s %s %d %d",
    weekdays[timeInfo.tm_wday],
    months[timeInfo.tm_mon],
    timeInfo.tm_mday,
    timeInfo.tm_year + 1900
  );

  return String(buffer);
}

void drawAnalogClock(
  int centerX,
  int centerY,
  int radius,
  const struct tm& timeInfo,
  bool modern
) {
  drawCircle(
    centerX,
    centerY,
    radius,
    modern ? CW_BLUE : CW_BLACK,
    modern ? 4 : 3
  );

  for (int marker = 0;
       marker < 60;
       marker++) {
    float angle =
      (
        marker * 6.0f -
        90.0f
      ) *
      PI /
      180.0f;

    bool hourMarker =
      marker % 5 == 0;

    int outer =
      radius - 7;

    int inner =
      radius -
      (
        hourMarker
          ? 25
          : 14
      );

    drawLine(
      centerX + cos(angle) * inner,
      centerY + sin(angle) * inner,
      centerX + cos(angle) * outer,
      centerY + sin(angle) * outer,
      hourMarker
        ? CW_BLACK
        : CW_BLUE,
      hourMarker ? 4 : 2
    );
  }

  if (!modern) {
    for (int number = 1;
         number <= 12;
         number++) {
      float angle =
        (
          number * 30.0f -
          90.0f
        ) *
        PI /
        180.0f;

      String label =
        String(number);

      int scale =
        radius > 120 ? 2 : 1;

      int x =
        centerX +
        cos(angle) *
        (radius - 43) -
        textWidth(label, scale) / 2;

      int y =
        centerY +
        sin(angle) *
        (radius - 43) -
        7 * scale / 2;

      drawText(
        x,
        y,
        label,
        scale,
        CW_BLACK
      );
    }
  }

  float minuteAngle =
    (
      timeInfo.tm_min * 6.0f -
      90.0f
    ) *
    PI /
    180.0f;

  float hourAngle =
    (
      (
        timeInfo.tm_hour % 12
      ) *
      30.0f +
      timeInfo.tm_min *
      0.5f -
      90.0f
    ) *
    PI /
    180.0f;

  drawLine(
    centerX,
    centerY,
    centerX +
      cos(hourAngle) *
      radius *
      0.52f,
    centerY +
      sin(hourAngle) *
      radius *
      0.52f,
    CW_BLACK,
    10
  );

  drawLine(
    centerX,
    centerY,
    centerX +
      cos(minuteAngle) *
      radius *
      0.72f,
    centerY +
      sin(minuteAngle) *
      radius *
      0.72f,
    modern ? CW_RED : CW_BLUE,
    6
  );

  fillCircle(
    centerX,
    centerY,
    10,
    CW_RED
  );

  fillCircle(
    centerX,
    centerY,
    4,
    CW_BLACK
  );
}

void drawDigitalClock(
  int centerX,
  int y,
  const struct tm& timeInfo,
  bool twelveHour
) {
  String clockText =
    timeText(
      timeInfo,
      twelveHour
    );

  drawCenteredText(
    centerX,
    y,
    clockText,
    9,
    CW_BLACK
  );
}

// ============================================================
// DASHBOARD RENDERING
// ============================================================

bool writePackedDashboard() {
  File file = LittleFS.open(
    CW_DASHBOARD_FILE,
    FILE_WRITE
  );

  if (!file) {
    return false;
  }

  uint8_t packedByte = 0;
  int outputPixel = 0;

  for (int panelY = 0;
       panelY < CW_PANEL_HEIGHT;
       panelY++) {
    for (int panelX = 0;
         panelX < CW_PANEL_WIDTH;
         panelX++) {
      int sourceX;
      int sourceY;

      if (mountDirection == "right") {
        sourceX = panelY;
        sourceY =
          CW_HEIGHT - 1 - panelX;
      } else {
        sourceX =
          CW_WIDTH - 1 - panelY;

        sourceY = panelX;
      }

      uint8_t color =
        canvas[
          sourceY * CW_WIDTH +
          sourceX
        ] & 0x0F;

      if ((outputPixel & 1) == 0) {
        packedByte =
          color << 4;
      } else {
        packedByte |= color;

        file.write(packedByte);
      }

      outputPixel++;
    }
  }

  file.flush();

  bool success =
    file.size() == CW_BYTES;

  file.close();

  return success;
}

bool renderDashboard() {
  if (!canvas) {
    canvas = static_cast<uint8_t*>(
      ps_malloc(
        CW_WIDTH *
        CW_HEIGHT
      )
    );
  }

  if (!canvas) {
    return false;
  }

  clearCanvas(CW_WHITE);

  struct tm timeInfo;

  bool timeValid =
    currentLocalTime(timeInfo);

  if (!timeValid) {
    memset(
      &timeInfo,
      0,
      sizeof(timeInfo)
    );
  }

  bool twelveHour =
    prefs
      ? prefs->getBool(
          "cw12hour",
          true
        )
      : true;

  bool showWeather =
    layout.indexOf("weather") >= 0 ||
    layout == "weather";

  bool analog =
    layout.startsWith("analog");

  bool modernAnalog =
    layout == "analog-modern";

  if (layout == "weather") {
    drawCenteredText(
      300,
      24,
      locationName,
      3,
      CW_BLACK
    );

    if (weather.valid) {
      drawWeatherIcon(
        165,
        175,
        weather.weatherCode,
        120
      );

      String temperature =
        String(round(weather.temperature)) +
        "*";

      drawCenteredText(
        410,
        118,
        temperature,
        7,
        CW_RED
      );

      drawCenteredText(
        410,
        195,
        weatherDescription(
          weather.weatherCode
        ),
        2,
        CW_BLACK
      );
    }
  }

  else if (analog) {
    int centerX =
      showWeather ? 205 : 300;

    int radius =
      showWeather ? 145 : 160;

    drawAnalogClock(
      centerX,
      190,
      radius,
      timeInfo,
      modernAnalog
    );

    drawCenteredText(
      centerX,
      355,
      dateText(timeInfo),
      2,
      CW_BLACK
    );
  }

  else {
    int centerX =
      showWeather ? 205 : 300;

    drawDigitalClock(
      centerX,
      95,
      timeInfo,
      twelveHour
    );

    drawCenteredText(
      centerX,
      205,
      dateText(timeInfo),
      3,
      CW_BLUE
    );
  }

  if (
    showWeather &&
    layout != "weather"
  ) {
    int left = 410;

    drawLine(
      390,
      35,
      390,
      365,
      CW_BLUE,
      3
    );

    drawCenteredText(
      495,
      32,
      locationName,
      2,
      CW_BLACK
    );

    if (weather.valid) {
      drawWeatherIcon(
        495,
        125,
        weather.weatherCode,
        75
      );

      String temperature =
        String(round(weather.temperature)) +
        "*";

      drawCenteredText(
        495,
        185,
        temperature,
        5,
        CW_RED
      );

      drawCenteredText(
        495,
        235,
        weatherDescription(
          weather.weatherCode
        ),
        1,
        CW_BLACK
      );

      String highLow =
        "H " +
        String(round(weather.high[0])) +
        " L " +
        String(round(weather.low[0]));

      drawCenteredText(
        495,
        270,
        highLow,
        2,
        CW_BLACK
      );

      String humidityText =
        "HUM " +
        String(round(weather.humidity)) +
        "%";

      drawCenteredText(
        495,
        304,
        humidityText,
        1,
        CW_BLUE
      );

      String windText =
        "WIND " +
        String(round(weather.wind));

      drawCenteredText(
        495,
        329,
        windText,
        1,
        CW_GREEN
      );
    } else {
      drawCenteredText(
        495,
        180,
        "WEATHER",
        2,
        CW_BLACK
      );

      drawCenteredText(
        495,
        215,
        "UNAVAILABLE",
        2,
        CW_RED
      );
    }
  }

  if (
    weather.valid &&
    (
      layout == "weather" ||
      layout == "digital-weather"
    )
  ) {
    int startX = 35;
    int top = 300;
    int cellWidth = 176;

    for (int day = 0; day < 3; day++) {
      int cellX =
        startX +
        day * cellWidth;

      drawLine(
        cellX,
        top,
        cellX + cellWidth - 10,
        top,
        CW_BLUE,
        2
      );

      String dayText =
        "D" +
        String(day + 1);

      drawText(
        cellX,
        top + 15,
        dayText,
        2,
        CW_BLACK
      );

      String range =
        String(round(weather.high[day])) +
        "/" +
        String(round(weather.low[day]));

      drawText(
        cellX,
        top + 48,
        range,
        2,
        day == 0
          ? CW_RED
          : CW_BLUE
      );

      String rain =
        "RAIN " +
        String(
          round(
            weather.rainChance[day]
          )
        ) +
        "%";

      drawText(
        cellX,
        top + 77,
        rain,
        1,
        CW_GREEN
      );
    }
  }

  lastRenderMillis = millis();

  return writePackedDashboard();
}

// ============================================================
// SETTINGS
// ============================================================

void loadSettings() {
  if (!prefs) {
    return;
  }

  active =
    prefs->getBool(
      "cwactive",
      false
    );

  layout =
    prefs->getString(
      "cwlayout",
      "analog-weather"
    );

  locationName =
    prefs->getString(
      "cwlocation",
      "Hong Kong"
    );

  latitude =
    prefs->getDouble(
      "cwlat",
      22.3193
    );

  longitude =
    prefs->getDouble(
      "cwlon",
      114.1694
    );

  /*
    Migrate the original development defaults to Hong Kong.
    User-selected non-default locations remain unchanged.
  */
  if (
    locationName == "San Francisco" &&
    fabs(latitude - 37.7749) < 0.01 &&
    fabs(longitude + 122.4194) < 0.01
  ) {
    locationName = "Hong Kong";
    latitude = 22.3193;
    longitude = 114.1694;

    prefs->putString(
      "cwlocation",
      locationName
    );

    prefs->putDouble(
      "cwlat",
      latitude
    );

    prefs->putDouble(
      "cwlon",
      longitude
    );
  }

  timezoneSpec =
    prefs->getString(
      "cwtz",
      "Asia/Hong_Kong"
    );

  temperatureUnit =
    prefs->getString(
      "cwtemp",
      "fahrenheit"
    );

  windUnit =
    prefs->getString(
      "cwwind",
      "mph"
    );

  mountDirection =
    prefs->getString(
      "mount",
      "left"
    );

  updateInterval =
    prefs->getUInt(
      "cwinterval",
      CW_DEFAULT_INTERVAL
    );

  if (updateInterval < CW_MIN_INTERVAL) {
    updateInterval =
      CW_MIN_INTERVAL;
  }

  weatherInterval =
    prefs->getUInt(
      "cwweatherint",
      CW_WEATHER_INTERVAL
    );

  if (weatherInterval < 600) {
    weatherInterval = 600;
  }
}

void saveSettings() {
  if (!prefs) {
    return;
  }

  prefs->putBool(
    "cwactive",
    active
  );

  prefs->putString(
    "cwlayout",
    layout
  );

  prefs->putString(
    "cwlocation",
    locationName
  );

  prefs->putDouble(
    "cwlat",
    latitude
  );

  prefs->putDouble(
    "cwlon",
    longitude
  );

  prefs->putString(
    "cwtz",
    timezoneSpec
  );

  prefs->putString(
    "cwtemp",
    temperatureUnit
  );

  prefs->putString(
    "cwwind",
    windUnit
  );

  prefs->putUInt(
    "cwinterval",
    updateInterval
  );

  prefs->putUInt(
    "cwweatherint",
    weatherInterval
  );
}

void configureTime() {
  /*
    Synchronize the system clock as UTC. Location-specific local
    time is calculated using Open-Meteo's utc_offset_seconds.
  */
  configTime(
    0,
    0,
    "pool.ntp.org",
    "time.nist.gov",
    "time.google.com"
  );

  timeConfigured = true;
}

// ============================================================
// HTTP API
// ============================================================

String jsonEscapeLocal(const String& source) {
  String result;

  for (int index = 0;
       index < source.length();
       index++) {
    char character =
      source.charAt(index);

    if (
      character == '"' ||
      character == '\\'
    ) {
      result += '\\';
    }

    if (
      character == '\n' ||
      character == '\r'
    ) {
      result += ' ';
    } else {
      result += character;
    }
  }

  return result;
}

void handleStatus() {
  struct tm timeInfo;
  bool timeValid =
    currentLocalTime(timeInfo);

  String currentTime =
    timeValid
      ? timeText(
          timeInfo,
          prefs
            ? prefs->getBool(
                "cw12hour",
                true
              )
            : true
        )
      : "Not synchronized";

  String response = "{";

  response += "\"active\":";
  response += active ? "true" : "false";
  response += ",";

  response += "\"busy\":";
  response +=
    (
      extensionBusy ||
      weatherBusy
    )
      ? "true"
      : "false";
  response += ",";

  response += "\"layout\":\"";
  response += jsonEscapeLocal(layout);
  response += "\",";

  response += "\"location\":\"";
  response += jsonEscapeLocal(locationName);
  response += "\",";

  response += "\"latitude\":";
  response += String(latitude, 6);
  response += ",";

  response += "\"longitude\":";
  response += String(longitude, 6);
  response += ",";

  response += "\"timezone\":\"";
  response += jsonEscapeLocal(timezoneSpec);
  response += "\",";

  response += "\"temperatureUnit\":\"";
  response += temperatureUnit;
  response += "\",";

  response += "\"windUnit\":\"";
  response += windUnit;
  response += "\",";

  response += "\"interval\":";
  response += String(updateInterval);
  response += ",";

  response += "\"weatherInterval\":";
  response += String(weatherInterval);
  response += ",";

  response += "\"timeValid\":";
  response += timeValid ? "true" : "false";
  response += ",";

  response += "\"currentTime\":\"";
  response += currentTime;
  response += "\",";

  response += "\"twelveHour\":";
  response +=
    (
      prefs &&
      prefs->getBool(
        "cw12hour",
        true
      )
    )
      ? "true"
      : "false";
  response += ",";

  response += "\"weatherValid\":";
  response += weather.valid
    ? "true"
    : "false";
  response += ",";

  response += "\"weatherError\":\"";
  response += jsonEscapeLocal(
    lastWeatherError
  );
  response += "\",";

  response += "\"utcOffsetSeconds\":";
  response += String(
    utcOffsetSeconds
  );
  response += ",";

  response += "\"temperature\":";
  response += String(weather.temperature, 1);
  response += ",";

  response += "\"weatherCode\":";
  response += String(weather.weatherCode);
  response += ",";

  response += "\"weatherDescription\":\"";
  response += weatherDescription(
    weather.weatherCode
  );
  response += "\",";

  response += "\"lastWeatherSeconds\":";

  if (weather.valid) {
    response += String(
      (
        millis() -
        weather.updatedMillis
      ) /
      1000UL
    );
  } else {
    response += "-1";
  }

  response += "}";

  webServer->sendHeader(
    "Cache-Control",
    "no-store"
  );

  webServer->send(
    200,
    "application/json",
    response
  );
}

void handleSettings() {
  layout =
    webServer->arg("layout");

  if (
    layout != "digital" &&
    layout != "analog" &&
    layout != "analog-modern" &&
    layout != "digital-weather" &&
    layout != "analog-weather" &&
    layout != "weather"
  ) {
    layout = "analog-weather";
  }

  locationName =
    webServer->arg("location");

  locationName.trim();

  if (locationName.length() == 0) {
    locationName = "My Location";
  }

  latitude =
    webServer->arg("latitude").toDouble();

  longitude =
    webServer->arg("longitude").toDouble();

  timezoneSpec =
    webServer->arg("timezone");

  if (timezoneSpec.length() == 0) {
    timezoneSpec = "auto";
  }

  temperatureUnit =
    webServer->arg("temperatureUnit");

  if (
    temperatureUnit != "fahrenheit" &&
    temperatureUnit != "celsius"
  ) {
    temperatureUnit = "fahrenheit";
  }

  windUnit =
    webServer->arg("windUnit");

  if (
    windUnit != "mph" &&
    windUnit != "kmh"
  ) {
    windUnit = "mph";
  }

  updateInterval =
    webServer->arg("interval").toInt();

  if (updateInterval < CW_MIN_INTERVAL) {
    updateInterval = CW_MIN_INTERVAL;
  }

  weatherInterval =
    webServer->arg("weatherInterval").toInt();

  if (weatherInterval < 600) {
    weatherInterval = 600;
  }

  bool twelveHour =
    webServer->arg("twelveHour") == "1";

  prefs->putBool(
    "cw12hour",
    twelveHour
  );

  saveSettings();
  configureTime();

  fetchWeather();
  renderDashboard();

  webServer->send(
    200,
    "application/json",
    "{\"ok\":true}"
  );
}

void handleAction() {
  String action =
    webServer->arg("action");

  if (action == "activate") {
    active = true;

    if (prefs) {
      prefs->putBool(
        "calactive",
        false
      );
    }

    saveSettings();

    fetchWeather();
    renderDashboard();

    bool displayed =
      displayCallback
        ? displayCallback(
            CW_DASHBOARD_FILE
          )
        : false;

    if (displayed) {
      lastDisplayMillis = millis();
    }
  }

  else if (action == "deactivate") {
    active = false;
    saveSettings();
  }

  else if (action == "display-now") {
    fetchWeather();

    if (!renderDashboard()) {
      webServer->send(
        500,
        "text/plain",
        "Could not render dashboard"
      );

      return;
    }

    bool displayed =
      displayCallback
        ? displayCallback(
            CW_DASHBOARD_FILE
          )
        : false;

    if (!displayed) {
      webServer->send(
        409,
        "text/plain",
        "Display is busy"
      );

      return;
    }

    lastDisplayMillis = millis();
  }

  else if (action == "refresh-weather") {
    if (!fetchWeather()) {
      webServer->send(
        502,
        "text/plain",
        "Weather update failed"
      );

      return;
    }

    renderDashboard();
  }

  else if (action == "sync-time") {
    configureTime();
  }

  else {
    webServer->send(
      400,
      "text/plain",
      "Unknown action"
    );

    return;
  }

  webServer->send(
    200,
    "application/json",
    "{\"ok\":true}"
  );
}

void handlePreview() {
  if (
    !LittleFS.exists(
      CW_DASHBOARD_FILE
    ) ||
    lastRenderMillis == 0
  ) {
    renderDashboard();
  }

  File file = LittleFS.open(
    CW_DASHBOARD_FILE,
    FILE_READ
  );

  if (!file) {
    webServer->send(
      500,
      "text/plain",
      "Preview unavailable"
    );

    return;
  }

  webServer->sendHeader(
    "Cache-Control",
    "no-store"
  );

  webServer->streamFile(
    file,
    "application/octet-stream"
  );

  file.close();
}

} // namespace

// ============================================================
// PUBLIC EXTENSION API
// ============================================================

void clockWeatherBegin(
  Preferences* preferences,
  ClockWeatherDisplayCallback callback
) {
  prefs = preferences;
  displayCallback = callback;

  loadSettings();
  configureTime();

  if (WiFi.status() == WL_CONNECTED) {
    fetchWeather();
  }

  renderDashboard();
}

void clockWeatherRegisterRoutes(
  WebServer& server
) {
  webServer = &server;

  server.on(
    "/api/extensions/clock-weather/status",
    HTTP_GET,
    handleStatus
  );

  server.on(
    "/api/extensions/clock-weather/search",
    HTTP_GET,
    handleLocationSearch
  );

  server.on(
    "/api/extensions/clock-weather/settings",
    HTTP_POST,
    handleSettings
  );

  server.on(
    "/api/extensions/clock-weather/action",
    HTTP_POST,
    handleAction
  );

  server.on(
    "/api/extensions/clock-weather/preview",
    HTTP_GET,
    handlePreview
  );
}

void clockWeatherLoop() {
  if (prefs) {
    active = prefs->getBool(
      "cwactive",
      active
    );
  }

  if (!active || extensionBusy) {
    return;
  }

  if (
    WiFi.status() == WL_CONNECTED &&
    (
      !weather.valid ||
      millis() - lastWeatherMillis >=
        weatherInterval * 1000UL
    )
  ) {
    fetchWeather();
  }

  if (
    lastDisplayMillis == 0 ||
    millis() - lastDisplayMillis >=
      updateInterval * 1000UL
  ) {
    extensionBusy = true;

    renderDashboard();

    bool displayed =
      displayCallback
        ? displayCallback(
            CW_DASHBOARD_FILE
          )
        : false;

    if (displayed) {
      lastDisplayMillis = millis();
    }

    extensionBusy = false;
  }
}


bool clockWeatherGetSnapshot(
  ClockWeatherSnapshot& snapshot
) {
  snapshot.valid =
    weather.valid;

  snapshot.temperature =
    weather.temperature;

  snapshot.weatherCode =
    weather.weatherCode;

  snapshot.description =
    weatherDescription(
      weather.weatherCode
    );

  snapshot.location =
    locationName;

  return snapshot.valid;
}

bool clockWeatherActive() {
  return active;
}
