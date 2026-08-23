#include "extension_calendar.h"
#include "extension_clock_weather.h"

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <LittleFS.h>
#include <time.h>
#include <math.h>

namespace {

#define CAL_WIDTH 600
#define CAL_HEIGHT 400
#define CAL_PANEL_WIDTH 400
#define CAL_PANEL_HEIGHT 600
#define CAL_BYTES 120000

#define CAL_BLACK  0x0
#define CAL_WHITE  0x1
#define CAL_YELLOW 0x2
#define CAL_RED    0x3
#define CAL_BLUE   0x5
#define CAL_GREEN  0x6

#define CAL_MAX_FEEDS 4
#define CAL_MAX_EVENTS 64

#define CAL_FILE "/calendar_dashboard.e6"
#define CAL_CACHE "/calendar_cache.txt"

#define CAL_MIN_DISPLAY_INTERVAL 180UL
#define CAL_DEFAULT_SYNC_INTERVAL 900UL

Preferences* prefs = nullptr;
WebServer* serverInstance = nullptr;
CalendarDisplayCallback displayCallback = nullptr;

bool active = false;
bool busy = false;

String layout = "agenda-month";
String privacyMode = "full";

int rangeDays = 2;
int maximumEvents = 6;
int utcOffsetMinutes = 480;

uint32_t syncInterval = CAL_DEFAULT_SYNC_INTERVAL;

unsigned long lastSyncMillis = 0;
unsigned long lastRenderCheckMillis = 0;
unsigned long lastDisplayMillis = 0;

uint32_t lastDisplayedHash = 0;

String lastSyncError = "";

struct CalendarFeed {
  bool enabled = false;
  String name;
  String url;
  String color;
};

struct CalendarEvent {
  time_t start = 0;
  time_t end = 0;

  bool allDay = false;

  String summary;
  String location;
  String color;
};

CalendarFeed feeds[CAL_MAX_FEEDS];
CalendarEvent events[CAL_MAX_EVENTS];

int eventCount = 0;
uint8_t* canvas = nullptr;

// ============================================================
// FONT
// ============================================================

const uint8_t* glyph(char character) {
  static const uint8_t EMPTY[5] =
    {0,0,0,0,0};

  static const uint8_t UNKNOWN[5] =
    {62,65,93,65,62};

  static const uint8_t LETTERS[26][5] = {
    {126,17,17,17,126},
    {127,73,73,73,54},
    {62,65,65,65,34},
    {127,65,65,34,28},
    {127,73,73,73,65},
    {127,9,9,9,1},
    {62,65,73,73,122},
    {127,8,8,8,127},
    {65,65,127,65,65},
    {32,64,65,63,1},
    {127,8,20,34,65},
    {127,64,64,64,64},
    {127,2,12,2,127},
    {127,4,8,16,127},
    {62,65,65,65,62},
    {127,9,9,9,6},
    {62,65,81,33,94},
    {127,9,25,41,70},
    {38,73,73,73,50},
    {1,1,127,1,1},
    {63,64,64,64,63},
    {31,32,64,32,31},
    {127,32,24,32,127},
    {99,20,8,20,99},
    {7,8,112,8,7},
    {97,81,73,69,67}
  };

  static const uint8_t NUMBERS[10][5] = {
    {62,81,73,69,62},
    {0,66,127,64,0},
    {98,81,73,73,70},
    {34,65,73,73,54},
    {24,20,18,127,16},
    {39,69,69,69,57},
    {60,74,73,73,48},
    {1,113,9,5,3},
    {54,73,73,73,54},
    {6,73,73,41,30}
  };

  static const uint8_t COLON[5] =
    {0,54,54,0,0};

  static const uint8_t DASH[5] =
    {8,8,8,8,8};

  static const uint8_t DOT[5] =
    {0,96,96,0,0};

  static const uint8_t SLASH[5] =
    {32,16,8,4,2};

  if (character >= 'a' && character <= 'z') {
    character -= 32;
  }

  if (character >= 'A' && character <= 'Z') {
    return LETTERS[character - 'A'];
  }

  if (character >= '0' && character <= '9') {
    return NUMBERS[character - '0'];
  }

  switch (character) {
    case ' ': return EMPTY;
    case ':': return COLON;
    case '-': return DASH;
    case '.': return DOT;
    case '/': return SLASH;
    default: return UNKNOWN;
  }
}

// ============================================================
// DRAWING
// ============================================================

void setPixel(
  int x,
  int y,
  uint8_t color
) {
  if (
    !canvas ||
    x < 0 ||
    x >= CAL_WIDTH ||
    y < 0 ||
    y >= CAL_HEIGHT
  ) {
    return;
  }

  canvas[y * CAL_WIDTH + x] = color;
}

void clearCanvas(uint8_t color) {
  if (canvas) {
    memset(
      canvas,
      color,
      CAL_WIDTH * CAL_HEIGHT
    );
  }
}

void fillRectangle(
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

    for (
      int offsetY = -radius;
      offsetY <= radius;
      offsetY++
    ) {
      for (
        int offsetX = -radius;
        offsetX <= radius;
        offsetX++
      ) {
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

    int twiceError = error * 2;

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
  const uint8_t* bitmap =
    glyph(character);

  for (int column = 0; column < 5; column++) {
    uint8_t bits = bitmap[column];

    for (int row = 0; row < 7; row++) {
      if (bits & (1 << row)) {
        fillRectangle(
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
  String text,
  int scale,
  uint8_t color
) {
  text.toUpperCase();

  for (
    int index = 0;
    index < text.length();
    index++
  ) {
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
  String text,
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

String clippedText(
  String text,
  int maximumCharacters
) {
  text.trim();

  if (text.length() <= maximumCharacters) {
    return text;
  }

  return text.substring(
    0,
    maximumCharacters - 3
  ) + "...";
}

// ============================================================
// NETWORK
// ============================================================

bool secureGet(
  const String& url,
  String& response,
  int& statusCode
) {
  if (WiFi.status() != WL_CONNECTED) {
    statusCode = -1;
    response = "No router internet connection";
    return false;
  }

  WiFiClientSecure secureClient;
  secureClient.setInsecure();

  HTTPClient http;

  if (!http.begin(secureClient, url)) {
    statusCode = -2;
    response = "Could not initialize HTTPS";
    return false;
  }

  http.setConnectTimeout(15000);
  http.setTimeout(25000);

  http.setUserAgent(
    "DynaPix-Calendar/1.0"
  );

  statusCode = http.GET();

  if (statusCode > 0) {
    response = http.getString();
  } else {
    response =
      http.errorToString(statusCode);
  }

  http.end();

  return statusCode == HTTP_CODE_OK;
}

// ============================================================
// ICS PARSING
// ============================================================

String decodeIcsText(String value) {
  value.replace("\\n", " ");
  value.replace("\\N", " ");
  value.replace("\\,", ",");
  value.replace("\\;", ";");
  value.replace("\\\\", "\\");

  value.trim();

  return value;
}

String unfoldedIcs(const String& source) {
  String result;
  String previous;

  int position = 0;

  while (position <= source.length()) {
    int newline = source.indexOf('\n', position);

    String line =
      newline < 0
        ? source.substring(position)
        : source.substring(position, newline);

    line.replace("\r", "");

    if (
      line.startsWith(" ") ||
      line.startsWith("\t")
    ) {
      previous += line.substring(1);
    } else {
      if (previous.length() > 0) {
        result += previous;
        result += "\n";
      }

      previous = line;
    }

    if (newline < 0) {
      break;
    }

    position = newline + 1;
  }

  if (previous.length() > 0) {
    result += previous;
    result += "\n";
  }

  return result;
}

String propertyLine(
  const String& block,
  const String& property
) {
  int position = 0;

  while (position <= block.length()) {
    int newline = block.indexOf('\n', position);

    String line =
      newline < 0
        ? block.substring(position)
        : block.substring(position, newline);

    if (
      line.startsWith(property + ":") ||
      line.startsWith(property + ";")
    ) {
      return line;
    }

    if (newline < 0) {
      break;
    }

    position = newline + 1;
  }

  return "";
}

String propertyValue(
  const String& block,
  const String& property
) {
  String line =
    propertyLine(block, property);

  int colon = line.indexOf(':');

  if (colon < 0) {
    return "";
  }

  return decodeIcsText(
    line.substring(colon + 1)
  );
}

time_t makeUtc(struct tm& value) {
  /*
    Convert a civil calendar date directly to Unix epoch
    seconds without changing the ESP32 global timezone.

    tm_mon values outside 0 through 11 are normalized, which
    is needed when expanding monthly recurring events.
  */

  int year =
    value.tm_year + 1900;

  int month =
    value.tm_mon + 1;

  while (month <= 0) {
    month += 12;
    year--;
  }

  while (month > 12) {
    month -= 12;
    year++;
  }

  int day =
    value.tm_mday;

  int adjustedYear =
    year -
    (
      month <= 2
        ? 1
        : 0
    );

  int era =
    (
      adjustedYear >= 0
        ? adjustedYear
        : adjustedYear - 399
    ) / 400;

  unsigned int yearOfEra =
    static_cast<unsigned int>(
      adjustedYear -
      era * 400
    );

  unsigned int adjustedMonth =
    static_cast<unsigned int>(
      month +
      (
        month > 2
          ? -3
          : 9
      )
    );

  /*
    Day number of the first day of the requested month.
    Adding day - 1 also normalizes out-of-range month days.
  */
  unsigned int dayOfYear =
    (
      153 *
      adjustedMonth +
      2
    ) / 5;

  unsigned int dayOfEra =
    yearOfEra * 365 +
    yearOfEra / 4 -
    yearOfEra / 100 +
    dayOfYear;

  int64_t epochDays =
    static_cast<int64_t>(era) *
    146097LL +
    static_cast<int64_t>(dayOfEra) -
    719468LL +
    static_cast<int64_t>(day - 1);

  int64_t epochSeconds =
    epochDays * 86400LL +
    static_cast<int64_t>(
      value.tm_hour
    ) * 3600LL +
    static_cast<int64_t>(
      value.tm_min
    ) * 60LL +
    static_cast<int64_t>(
      value.tm_sec
    );

  return static_cast<time_t>(
    epochSeconds
  );
}

bool parseIcsDate(
  const String& line,
  time_t& timestamp,
  bool& allDay
) {
  int colon = line.indexOf(':');

  if (colon < 0) {
    return false;
  }

  String parameters =
    line.substring(0, colon);

  String value =
    line.substring(colon + 1);

  value.trim();

  allDay =
    parameters.indexOf("VALUE=DATE") >= 0 ||
    value.length() == 8;

  if (value.length() < 8) {
    return false;
  }

  struct tm parsed = {};

  parsed.tm_year =
    value.substring(0, 4).toInt() - 1900;

  parsed.tm_mon =
    value.substring(4, 6).toInt() - 1;

  parsed.tm_mday =
    value.substring(6, 8).toInt();

  if (!allDay && value.length() >= 15) {
    parsed.tm_hour =
      value.substring(9, 11).toInt();

    parsed.tm_min =
      value.substring(11, 13).toInt();

    parsed.tm_sec =
      value.substring(13, 15).toInt();
  }

  time_t raw =
    makeUtc(parsed);

  bool utcValue =
    value.endsWith("Z");

  if (
    !utcValue &&
    !allDay
  ) {
    raw -=
      utcOffsetMinutes * 60;
  }

  if (allDay) {
    raw -=
      utcOffsetMinutes * 60;
  }

  timestamp = raw;

  return timestamp > 0;
}

int rruleInteger(
  const String& rule,
  const String& key,
  int defaultValue
) {
  String marker = key + "=";

  int position = rule.indexOf(marker);

  if (position < 0) {
    return defaultValue;
  }

  position += marker.length();

  int end = rule.indexOf(';', position);

  String value =
    end < 0
      ? rule.substring(position)
      : rule.substring(position, end);

  int result = value.toInt();

  return result > 0
    ? result
    : defaultValue;
}

String rruleValue(
  const String& rule,
  const String& key
) {
  String marker = key + "=";

  int position = rule.indexOf(marker);

  if (position < 0) {
    return "";
  }

  position += marker.length();

  int end = rule.indexOf(';', position);

  return end < 0
    ? rule.substring(position)
    : rule.substring(position, end);
}

time_t addMonths(
  time_t start,
  int months
) {
  struct tm value = {};

  time_t localValue =
    start +
    utcOffsetMinutes * 60;

  gmtime_r(
    &localValue,
    &value
  );

  value.tm_mon += months;

  time_t result =
    makeUtc(value) -
    utcOffsetMinutes * 60;

  return result;
}

bool eventInRange(
  time_t start,
  time_t end
) {
  time_t now = time(nullptr);

  if (now < 100000) {
    now = 0;
  }

  time_t windowStart =
    now - 86400;

  time_t windowEnd =
    now +
    (
      max(rangeDays, 3) + 35
    ) *
    86400L;

  return
    end >= windowStart &&
    start <= windowEnd;
}

void addEvent(
  time_t start,
  time_t end,
  bool allDay,
  String summary,
  String location,
  const String& color
) {
  if (
    eventCount >= CAL_MAX_EVENTS ||
    !eventInRange(start, end)
  ) {
    return;
  }

  summary = decodeIcsText(summary);
  location = decodeIcsText(location);

  if (summary.length() == 0) {
    summary = "Busy";
  }

  CalendarEvent& event =
    events[eventCount++];

  event.start = start;
  event.end = end;
  event.allDay = allDay;
  event.summary = summary;
  event.location = location;
  event.color = color;
}

void expandEvent(
  time_t start,
  time_t end,
  bool allDay,
  const String& summary,
  const String& location,
  const String& color,
  const String& rule
) {
  addEvent(
    start,
    end,
    allDay,
    summary,
    location,
    color
  );

  if (
    rule.length() == 0 ||
    eventCount >= CAL_MAX_EVENTS
  ) {
    return;
  }

  String frequency =
    rruleValue(rule, "FREQ");

  int interval =
    rruleInteger(rule, "INTERVAL", 1);

  int count =
    rruleInteger(rule, "COUNT", 100);

  String untilText =
    rruleValue(rule, "UNTIL");

  time_t until = 0;

  if (untilText.length() > 0) {
    bool ignoredAllDay = false;

    parseIcsDate(
      "DTSTART:" + untilText,
      until,
      ignoredAllDay
    );
  }

  time_t duration =
    max((time_t)1, end - start);

  for (
    int occurrence = 1;
    occurrence < count &&
    eventCount < CAL_MAX_EVENTS;
    occurrence++
  ) {
    time_t nextStart = start;

    if (frequency == "DAILY") {
      nextStart +=
        occurrence *
        interval *
        86400L;
    }

    else if (frequency == "WEEKLY") {
      nextStart +=
        occurrence *
        interval *
        7 *
        86400L;
    }

    else if (frequency == "MONTHLY") {
      nextStart =
        addMonths(
          start,
          occurrence * interval
        );
    }

    else {
      break;
    }

    if (
      until > 0 &&
      nextStart > until
    ) {
      break;
    }

    if (
      nextStart >
      time(nullptr) +
      45L * 86400L
    ) {
      break;
    }

    addEvent(
      nextStart,
      nextStart + duration,
      allDay,
      summary,
      location,
      color
    );
  }
}

void parseCalendar(
  const String& source,
  const CalendarFeed& feed
) {
  String unfolded =
    unfoldedIcs(source);

  int searchPosition = 0;

  while (
    eventCount < CAL_MAX_EVENTS
  ) {
    int begin =
      unfolded.indexOf(
        "BEGIN:VEVENT",
        searchPosition
      );

    if (begin < 0) {
      break;
    }

    int finish =
      unfolded.indexOf(
        "END:VEVENT",
        begin
      );

    if (finish < 0) {
      break;
    }

    String block =
      unfolded.substring(
        begin,
        finish
      );

    String startLine =
      propertyLine(
        block,
        "DTSTART"
      );

    String endLine =
      propertyLine(
        block,
        "DTEND"
      );

    time_t start = 0;
    time_t end = 0;

    bool allDay = false;
    bool endAllDay = false;

    if (
      parseIcsDate(
        startLine,
        start,
        allDay
      )
    ) {
      if (
        !parseIcsDate(
          endLine,
          end,
          endAllDay
        )
      ) {
        end =
          start +
          (
            allDay
              ? 86400
              : 3600
          );
      }

      expandEvent(
        start,
        end,
        allDay,
        propertyValue(
          block,
          "SUMMARY"
        ),
        propertyValue(
          block,
          "LOCATION"
        ),
        feed.color,
        propertyValue(
          block,
          "RRULE"
        )
      );
    }

    searchPosition =
      finish +
      strlen("END:VEVENT");
  }
}

void sortEvents() {
  for (
    int outer = 0;
    outer < eventCount - 1;
    outer++
  ) {
    for (
      int inner = outer + 1;
      inner < eventCount;
      inner++
    ) {
      if (
        events[inner].start <
        events[outer].start
      ) {
        CalendarEvent temporary =
          events[outer];

        events[outer] =
          events[inner];

        events[inner] =
          temporary;
      }
    }
  }
}

// ============================================================
// CACHE
// ============================================================

String escapeCache(String text) {
  text.replace("\\", "\\\\");
  text.replace("|", "\\p");
  text.replace("\n", " ");
  text.replace("\r", " ");

  return text;
}

String unescapeCache(String text) {
  text.replace("\\p", "|");
  text.replace("\\\\", "\\");

  return text;
}

void saveCache() {
  File file =
    LittleFS.open(
      CAL_CACHE,
      FILE_WRITE
    );

  if (!file) {
    return;
  }

  for (int index = 0;
       index < eventCount;
       index++) {
    file.print(
      (long long)events[index].start
    );
    file.print("|");

    file.print(
      (long long)events[index].end
    );
    file.print("|");

    file.print(
      events[index].allDay
        ? "1"
        : "0"
    );
    file.print("|");

    file.print(
      escapeCache(
        events[index].summary
      )
    );
    file.print("|");

    file.print(
      escapeCache(
        events[index].location
      )
    );
    file.print("|");

    file.println(
      events[index].color
    );
  }

  file.flush();
  file.close();
}

void loadCache() {
  eventCount = 0;

  File file =
    LittleFS.open(
      CAL_CACHE,
      FILE_READ
    );

  if (!file) {
    return;
  }

  while (
    file.available() &&
    eventCount < CAL_MAX_EVENTS
  ) {
    String line =
      file.readStringUntil('\n');

    line.trim();

    if (line.length() == 0) {
      continue;
    }

    int p1 = line.indexOf('|');
    int p2 = line.indexOf('|', p1 + 1);
    int p3 = line.indexOf('|', p2 + 1);
    int p4 = line.indexOf('|', p3 + 1);
    int p5 = line.indexOf('|', p4 + 1);

    if (
      p1 < 0 ||
      p2 < 0 ||
      p3 < 0 ||
      p4 < 0 ||
      p5 < 0
    ) {
      continue;
    }

    CalendarEvent event;

    event.start =
      strtoll(
        line.substring(0, p1).c_str(),
        nullptr,
        10
      );

    event.end =
      strtoll(
        line.substring(
          p1 + 1,
          p2
        ).c_str(),
        nullptr,
        10
      );

    event.allDay =
      line.substring(
        p2 + 1,
        p3
      ) == "1";

    event.summary =
      unescapeCache(
        line.substring(
          p3 + 1,
          p4
        )
      );

    event.location =
      unescapeCache(
        line.substring(
          p4 + 1,
          p5
        )
      );

    event.color =
      line.substring(p5 + 1);

    if (
      eventInRange(
        event.start,
        event.end
      )
    ) {
      events[eventCount++] =
        event;
    }
  }

  file.close();

  sortEvents();
}

// ============================================================
// SYNCHRONIZATION
// ============================================================

bool syncCalendars() {
  if (WiFi.status() != WL_CONNECTED) {
    lastSyncError =
      "No router internet connection";

    return false;
  }

  busy = true;
  lastSyncError = "";

  CalendarEvent previous[CAL_MAX_EVENTS];
  int previousCount = eventCount;

  for (int index = 0;
       index < previousCount;
       index++) {
    previous[index] = events[index];
  }

  eventCount = 0;

  bool anySuccess = false;

  for (int index = 0;
       index < CAL_MAX_FEEDS;
       index++) {
    if (
      !feeds[index].enabled ||
      feeds[index].url.length() == 0
    ) {
      continue;
    }

    String response;
    int statusCode = 0;

    bool success =
      secureGet(
        feeds[index].url,
        response,
        statusCode
      );

    if (!success) {
      lastSyncError =
        "Feed " +
        String(index + 1) +
        " failed: " +
        String(statusCode);

      continue;
    }

    parseCalendar(
      response,
      feeds[index]
    );

    anySuccess = true;
  }

  if (anySuccess) {
    sortEvents();
    saveCache();

    lastSyncMillis = millis();
  } else {
    eventCount = previousCount;

    for (int index = 0;
         index < previousCount;
         index++) {
      events[index] = previous[index];
    }

    if (lastSyncError.length() == 0) {
      lastSyncError =
        "No enabled calendar feeds";
    }
  }

  busy = false;

  return anySuccess;
}

// ============================================================
// TIME HELPERS
// ============================================================

bool localTimeValue(
  time_t timestamp,
  struct tm& value
) {
  timestamp +=
    utcOffsetMinutes * 60;

  gmtime_r(
    &timestamp,
    &value
  );

  return true;
}

String eventTimeText(
  const CalendarEvent& event
) {
  if (event.allDay) {
    return "ALL DAY";
  }

  struct tm value = {};

  localTimeValue(
    event.start,
    value
  );

  char buffer[8];

  snprintf(
    buffer,
    sizeof(buffer),
    "%02d:%02d",
    value.tm_hour,
    value.tm_min
  );

  return String(buffer);
}

String dateHeader(time_t timestamp) {
  static const char* weekdays[] = {
    "SUN","MON","TUE","WED",
    "THU","FRI","SAT"
  };

  static const char* months[] = {
    "JAN","FEB","MAR","APR",
    "MAY","JUN","JUL","AUG",
    "SEP","OCT","NOV","DEC"
  };

  struct tm value = {};

  localTimeValue(
    timestamp,
    value
  );

  char buffer[32];

  snprintf(
    buffer,
    sizeof(buffer),
    "%s %s %d %d",
    weekdays[value.tm_wday],
    months[value.tm_mon],
    value.tm_mday,
    value.tm_year + 1900
  );

  return String(buffer);
}

time_t localMidnight(time_t timestamp) {
  timestamp +=
    utcOffsetMinutes * 60;

  struct tm value = {};

  gmtime_r(
    &timestamp,
    &value
  );

  value.tm_hour = 0;
  value.tm_min = 0;
  value.tm_sec = 0;

  return
    makeUtc(value) -
    utcOffsetMinutes * 60;
}

// ============================================================
// RENDERING
// ============================================================

uint8_t colorCode(String color) {
  color.toLowerCase();

  if (
    color.indexOf("ff") >= 0 &&
    color.indexOf("00") >= 0
  ) {
    return CAL_RED;
  }

  if (
    color.indexOf("16") >= 0 ||
    color.indexOf("0a") >= 0
  ) {
    return CAL_GREEN;
  }

  return CAL_BLUE;
}

String visibleSummary(
  const CalendarEvent& event
) {
  if (privacyMode == "busy") {
    return "Busy";
  }

  if (privacyMode == "time-only") {
    return "";
  }

  return event.summary;
}

void drawAgenda(
  int left,
  int top,
  int width,
  int height,
  int limit
) {
  time_t now = time(nullptr);
  time_t end =
    localMidnight(now) +
    rangeDays * 86400L;

  int y = top;
  int displayed = 0;

  for (int index = 0;
       index < eventCount &&
       displayed < limit;
       index++) {
    CalendarEvent& event =
      events[index];

    if (
      event.end < now ||
      event.start >= end
    ) {
      continue;
    }

    uint8_t markerColor =
      colorCode(event.color);

    fillRectangle(
      left,
      y,
      7,
      48,
      markerColor
    );

    drawText(
      left + 16,
      y,
      eventTimeText(event),
      2,
      CAL_BLACK
    );

    String summary =
      visibleSummary(event);

    if (summary.length() > 0) {
      drawText(
        left + 130,
        y,
        clippedText(
          summary,
          max(8, (width - 145) / 12)
        ),
        2,
        CAL_BLACK
      );
    }

    if (
      privacyMode == "full" &&
      event.location.length() > 0
    ) {
      drawText(
        left + 130,
        y + 27,
        clippedText(
          event.location,
          max(8, (width - 145) / 6)
        ),
        1,
        CAL_BLUE
      );
    }

    y += 55;
    displayed++;

    if (y + 45 > top + height) {
      break;
    }
  }

  if (displayed == 0) {
    drawCenteredText(
      left + width / 2,
      top + height / 2,
      "NO UPCOMING EVENTS",
      2,
      CAL_BLUE
    );
  }
}

void drawMiniMonth(
  int left,
  int top,
  int width,
  int height
) {
  time_t now =
    time(nullptr) +
    utcOffsetMinutes * 60;

  struct tm current = {};

  gmtime_r(
    &now,
    &current
  );

  static const char* months[] = {
    "JAN", "FEB", "MAR", "APR",
    "MAY", "JUN", "JUL", "AUG",
    "SEP", "OCT", "NOV", "DEC"
  };

  static const char* weekdays[] = {
    "S", "M", "T", "W", "T", "F", "S"
  };

  /*
    Keep internal padding on both sides. The caller supplies a
    160-pixel-wide panel, ending at x=580 and leaving a
    20-pixel physical right margin.
  */
  const int horizontalPadding = 4;

  const int gridLeft =
    left +
    horizontalPadding;

  const int gridWidth =
    width -
    horizontalPadding * 2;

  const int columnWidth =
    gridWidth / 7;

  const int weekdayTop =
    top + 32;

  const int calendarTop =
    top + 58;

  const int rowHeight = 31;

  drawCenteredText(
    left + width / 2,
    top,
    String(months[current.tm_mon]) +
      " " +
      String(current.tm_year + 1900),
    2,
    CAL_BLACK
  );

  /*
    Draw each weekday independently at the exact center of its
    calendar column. This guarantees alignment with the dates.
  */
  for (
    int column = 0;
    column < 7;
    column++
  ) {
    int centerX =
      gridLeft +
      column * columnWidth +
      columnWidth / 2;

    drawCenteredText(
      centerX,
      weekdayTop,
      weekdays[column],
      1,
      CAL_BLUE
    );
  }

  struct tm firstDay = current;

  firstDay.tm_mday = 1;
  firstDay.tm_hour = 0;
  firstDay.tm_min = 0;
  firstDay.tm_sec = 0;

  time_t firstTimestamp =
    makeUtc(firstDay);

  gmtime_r(
    &firstTimestamp,
    &firstDay
  );

  int daysInMonth = 31;

  int month =
    current.tm_mon;

  if (
    month == 3 ||
    month == 5 ||
    month == 8 ||
    month == 10
  ) {
    daysInMonth = 30;
  }

  if (month == 1) {
    int year =
      current.tm_year + 1900;

    bool leapYear =
      (
        year % 4 == 0 &&
        year % 100 != 0
      ) ||
      year % 400 == 0;

    daysInMonth =
      leapYear
        ? 29
        : 28;
  }

  for (
    int day = 1;
    day <= daysInMonth;
    day++
  ) {
    int cell =
      firstDay.tm_wday +
      day -
      1;

    int column =
      cell % 7;

    int row =
      cell / 7;

    int centerX =
      gridLeft +
      column * columnWidth +
      columnWidth / 2;

    int y =
      calendarTop +
      row * rowHeight;

    String dayText =
      String(day);

    /*
      Centering by measured text width keeps one- and two-digit
      dates aligned to the same column center.
    */
    drawText(
      centerX -
        textWidth(dayText, 1) / 2,
      y,
      dayText,
      1,
      day == current.tm_mday
        ? CAL_RED
        : CAL_BLACK
    );
  }
}

void drawWeek(
  time_t today
) {
  int columnWidth = 118;

  for (int day = 0; day < 5; day++) {
    time_t dayStart =
      today +
      day * 86400L;

    int left =
      5 +
      day * columnWidth;

    drawCenteredText(
      left + columnWidth / 2,
      15,
      dateHeader(dayStart).substring(0, 6),
      1,
      day == 0
        ? CAL_RED
        : CAL_BLACK
    );

    drawLine(
      left,
      42,
      left + columnWidth - 4,
      42,
      CAL_BLUE,
      2
    );

    int y = 58;
    int shown = 0;

    for (int index = 0;
         index < eventCount &&
         shown < 4;
         index++) {
      CalendarEvent& event =
        events[index];

      if (
        event.start >= dayStart &&
        event.start < dayStart + 86400L
      ) {
        drawText(
          left + 3,
          y,
          eventTimeText(event),
          1,
          CAL_BLUE
        );

        drawText(
          left + 3,
          y + 16,
          clippedText(
            visibleSummary(event),
            17
          ),
          1,
          CAL_BLACK
        );

        y += 60;
        shown++;
      }
    }
  }
}

void drawMonthView(time_t now) {
  struct tm current = {};

  localTimeValue(
    now,
    current
  );

  drawCenteredText(
    300,
    16,
    dateHeader(now).substring(4),
    3,
    CAL_BLACK
  );

  int top = 65;
  int cellWidth = 85;
  int cellHeight = 52;

  drawText(
    20,
    50,
    "SUN   MON   TUE   WED   THU   FRI   SAT",
    1,
    CAL_BLUE
  );

  struct tm first = current;

  first.tm_mday = 1;
  first.tm_hour = 0;
  first.tm_min = 0;
  first.tm_sec = 0;

  time_t firstTime =
    makeUtc(first);

  gmtime_r(
    &firstTime,
    &first
  );

  for (int day = 1; day <= 31; day++) {
    struct tm test = current;

    test.tm_mday = day;
    test.tm_hour = 0;
    test.tm_min = 0;
    test.tm_sec = 0;

    time_t testTime =
      makeUtc(test);

    struct tm normalized = {};

    gmtime_r(
      &testTime,
      &normalized
    );

    if (normalized.tm_mon != current.tm_mon) {
      break;
    }

    int cell =
      first.tm_wday +
      day - 1;

    int column = cell % 7;
    int row = cell / 7;

    int left =
      3 +
      column * cellWidth;

    int y =
      top +
      row * cellHeight;

    drawLine(
      left,
      y,
      left + cellWidth - 3,
      y,
      CAL_BLUE,
      1
    );

    drawText(
      left + 4,
      y + 5,
      String(day),
      1,
      day == current.tm_mday
        ? CAL_RED
        : CAL_BLACK
    );

    time_t dayUtc =
      testTime -
      utcOffsetMinutes * 60;

    int dots = 0;

    for (int index = 0;
         index < eventCount;
         index++) {
      if (
        events[index].start >= dayUtc &&
        events[index].start <
          dayUtc + 86400L
      ) {
        fillRectangle(
          left + 28 + dots * 10,
          y + 8,
          6,
          6,
          colorCode(
            events[index].color
          )
        );

        dots++;

        if (dots >= 4) {
          break;
        }
      }
    }
  }
}

uint32_t canvasHash() {
  uint32_t hash = 2166136261UL;

  for (
    int index = 0;
    index < CAL_WIDTH * CAL_HEIGHT;
    index++
  ) {
    hash ^= canvas[index];
    hash *= 16777619UL;
  }

  return hash;
}

bool writePackedFile() {
  File file =
    LittleFS.open(
      CAL_FILE,
      FILE_WRITE
    );

  if (!file) {
    return false;
  }

  String mount =
    prefs
      ? prefs->getString(
          "mount",
          "left"
        )
      : "left";

  uint8_t packed = 0;
  int outputPixel = 0;

  for (int panelY = 0;
       panelY < CAL_PANEL_HEIGHT;
       panelY++) {
    for (int panelX = 0;
         panelX < CAL_PANEL_WIDTH;
         panelX++) {
      int sourceX;
      int sourceY;

      if (mount == "right") {
        sourceX = panelY;
        sourceY =
          CAL_HEIGHT - 1 - panelX;
      } else {
        sourceX =
          CAL_WIDTH - 1 - panelY;

        sourceY = panelX;
      }

      uint8_t color =
        canvas[
          sourceY * CAL_WIDTH +
          sourceX
        ] & 0x0F;

      if ((outputPixel & 1) == 0) {
        packed = color << 4;
      } else {
        packed |= color;
        file.write(packed);
      }

      outputPixel++;
    }
  }

  file.flush();

  bool valid =
    file.size() == CAL_BYTES;

  file.close();

  return valid;
}

uint32_t renderDashboard() {
  if (!canvas) {
    canvas =
      static_cast<uint8_t*>(
        ps_malloc(
          CAL_WIDTH *
          CAL_HEIGHT
        )
      );
  }

  if (!canvas) {
    return 0;
  }

  clearCanvas(CAL_WHITE);

  time_t now = time(nullptr);

  if (now < 100000) {
    now = 0;
  }

  time_t today =
    localMidnight(now);

  drawCenteredText(
    300,
    12,
    dateHeader(now),
    3,
    CAL_BLACK
  );

  drawLine(
    15,
    48,
    585,
    48,
    CAL_BLUE,
    2
  );

  if (layout == "agenda") {
    drawAgenda(
      20,
      68,
      560,
      310,
      maximumEvents
    );
  }

  else if (layout == "agenda-month") {
    drawAgenda(
      15,
      68,
      390,
      310,
      maximumEvents
    );

    drawLine(
      410,
      58,
      410,
      385,
      CAL_BLUE,
      2
    );

    drawMiniMonth(
      420,
      70,
      160,
      300
    );
  }

  else if (layout == "week") {
    drawWeek(today);
  }

  else if (layout == "month") {
    drawMonthView(now);
  }

  else if (
    layout == "clock-agenda" ||
    layout == "clock-weather-agenda"
  ) {
    struct tm value = {};

    localTimeValue(
      now,
      value
    );

    char timeBuffer[8];

    snprintf(
      timeBuffer,
      sizeof(timeBuffer),
      "%02d:%02d",
      value.tm_hour,
      value.tm_min
    );

    drawCenteredText(
      120,
      125,
      String(timeBuffer),
      5,
      CAL_BLACK
    );

    if (layout == "clock-weather-agenda") {
      ClockWeatherSnapshot weather = {};

      if (
        clockWeatherGetSnapshot(weather) &&
        weather.valid
      ) {
        drawCenteredText(
          120,
          205,
          String(round(weather.temperature)) +
            " " +
            weather.description,
          1,
          CAL_RED
        );
      }
    }

    drawLine(
      235,
      60,
      235,
      380,
      CAL_BLUE,
      2
    );

    drawAgenda(
      250,
      70,
      335,
      300,
      maximumEvents
    );
  }

  else {
    layout = "agenda-month";

    drawAgenda(
      15,
      68,
      390,
      310,
      maximumEvents
    );

    drawMiniMonth(
      420,
      70,
      160,
      300
    );
  }

  uint32_t hash = canvasHash();

  if (!writePackedFile()) {
    return 0;
  }

  return hash;
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
      "calactive",
      false
    );

  layout =
    prefs->getString(
      "callayout",
      "agenda-month"
    );

  privacyMode =
    prefs->getString(
      "calprivacy",
      "full"
    );

  rangeDays =
    prefs->getInt(
      "calrange",
      2
    );

  maximumEvents =
    prefs->getInt(
      "calmax",
      6
    );

  syncInterval =
    prefs->getUInt(
      "calsync",
      CAL_DEFAULT_SYNC_INTERVAL
    );

  utcOffsetMinutes =
    prefs->getInt(
      "caloffset",
      480
    );

  for (int index = 0;
       index < CAL_MAX_FEEDS;
       index++) {
    String suffix =
      String(index);

    feeds[index].enabled =
      prefs->getBool(
        (
          "calen" + suffix
        ).c_str(),
        false
      );

    feeds[index].name =
      prefs->getString(
        (
          "calname" + suffix
        ).c_str(),
        "Calendar " +
          String(index + 1)
      );

    feeds[index].url =
      prefs->getString(
        (
          "calurl" + suffix
        ).c_str(),
        ""
      );

    feeds[index].color =
      prefs->getString(
        (
          "calcolor" + suffix
        ).c_str(),
        index == 0
          ? "#fe5945"
          : "#16808a"
      );
  }
}

void saveGlobalSettings() {
  prefs->putBool(
    "calactive",
    active
  );

  prefs->putString(
    "callayout",
    layout
  );

  prefs->putString(
    "calprivacy",
    privacyMode
  );

  prefs->putInt(
    "calrange",
    rangeDays
  );

  prefs->putInt(
    "calmax",
    maximumEvents
  );

  prefs->putUInt(
    "calsync",
    syncInterval
  );

  prefs->putInt(
    "caloffset",
    utcOffsetMinutes
  );
}

void saveFeed(int index) {
  if (
    index < 0 ||
    index >= CAL_MAX_FEEDS
  ) {
    return;
  }

  String suffix = String(index);

  prefs->putBool(
    (
      "calen" + suffix
    ).c_str(),
    feeds[index].enabled
  );

  prefs->putString(
    (
      "calname" + suffix
    ).c_str(),
    feeds[index].name
  );

  prefs->putString(
    (
      "calurl" + suffix
    ).c_str(),
    feeds[index].url
  );

  prefs->putString(
    (
      "calcolor" + suffix
    ).c_str(),
    feeds[index].color
  );
}

// ============================================================
// API
// ============================================================

String jsonEscape(String value) {
  value.replace("\\", "\\\\");
  value.replace("\"", "\\\"");
  value.replace("\r", " ");
  value.replace("\n", " ");

  return value;
}

void handleStatus() {
  String response = "{";

  response += "\"active\":";
  response += active
    ? "true"
    : "false";
  response += ",";

  response += "\"busy\":";
  response += busy
    ? "true"
    : "false";
  response += ",";

  response += "\"layout\":\"";
  response += jsonEscape(layout);
  response += "\",";

  response += "\"privacy\":\"";
  response += jsonEscape(privacyMode);
  response += "\",";

  response += "\"rangeDays\":";
  response += String(rangeDays);
  response += ",";

  response += "\"maximumEvents\":";
  response += String(maximumEvents);
  response += ",";

  response += "\"syncInterval\":";
  response += String(syncInterval);
  response += ",";

  response += "\"utcOffsetMinutes\":";
  response += String(utcOffsetMinutes);
  response += ",";

  response += "\"eventCount\":";
  response += String(eventCount);
  response += ",";

  response += "\"lastError\":\"";
  response += jsonEscape(lastSyncError);
  response += "\",";

  response += "\"feeds\":[";

  for (int index = 0;
       index < CAL_MAX_FEEDS;
       index++) {
    if (index > 0) {
      response += ",";
    }

    response += "{";

    response += "\"index\":";
    response += String(index);
    response += ",";

    response += "\"enabled\":";
    response += feeds[index].enabled
      ? "true"
      : "false";
    response += ",";

    response += "\"name\":\"";
    response += jsonEscape(
      feeds[index].name
    );
    response += "\",";

    response += "\"color\":\"";
    response += jsonEscape(
      feeds[index].color
    );
    response += "\",";

    response += "\"configured\":";
    response +=
      feeds[index].url.length() > 0
        ? "true"
        : "false";

    response += "}";
  }

  response += "]}";

  serverInstance->sendHeader(
    "Cache-Control",
    "no-store"
  );

  serverInstance->send(
    200,
    "application/json",
    response
  );
}

void handleSettings() {
  layout =
    serverInstance->arg("layout");

  if (
    layout != "agenda" &&
    layout != "agenda-month" &&
    layout != "week" &&
    layout != "month" &&
    layout != "clock-agenda" &&
    layout != "clock-weather-agenda"
  ) {
    layout = "agenda-month";
  }

  privacyMode =
    serverInstance->arg("privacy");

  if (
    privacyMode != "full" &&
    privacyMode != "busy" &&
    privacyMode != "time-only"
  ) {
    privacyMode = "full";
  }

  rangeDays =
    constrain(
      serverInstance
        ->arg("rangeDays")
        .toInt(),
      1,
      7
    );

  maximumEvents =
    constrain(
      serverInstance
        ->arg("maximumEvents")
        .toInt(),
      1,
      10
    );

  long requestedSyncInterval =
    serverInstance
      ->arg("syncInterval")
      .toInt();

  if (
    requestedSyncInterval <
    300L
  ) {
    requestedSyncInterval =
      300L;
  }

  syncInterval =
    static_cast<uint32_t>(
      requestedSyncInterval
    );

  utcOffsetMinutes =
    constrain(
      serverInstance
        ->arg("utcOffsetMinutes")
        .toInt(),
      -720,
      840
    );

  saveGlobalSettings();

  uint32_t hash =
    renderDashboard();

  serverInstance->send(
    hash > 0 ? 200 : 500,
    "application/json",
    hash > 0
      ? "{\"ok\":true}"
      : "{\"ok\":false}"
  );
}

void handleFeedSave() {
  int index =
    serverInstance
      ->arg("index")
      .toInt();

  if (
    index < 0 ||
    index >= CAL_MAX_FEEDS
  ) {
    serverInstance->send(
      400,
      "text/plain",
      "Invalid calendar index"
    );

    return;
  }

  feeds[index].enabled =
    serverInstance
      ->arg("enabled") == "1";

  feeds[index].name =
    serverInstance
      ->arg("name");

  feeds[index].name.trim();

  if (feeds[index].name.length() == 0) {
    feeds[index].name =
      "Calendar " +
      String(index + 1);
  }

  String submittedUrl =
    serverInstance->arg("url");

  submittedUrl.trim();

  if (submittedUrl.length() > 0) {
    feeds[index].url =
      submittedUrl;
  }

  feeds[index].color =
    serverInstance->arg("color");

  if (
    feeds[index].color.length() != 7
  ) {
    feeds[index].color =
      "#16808a";
  }

  saveFeed(index);

  serverInstance->send(
    200,
    "application/json",
    "{\"ok\":true}"
  );
}

void handleFeedDelete() {
  int index =
    serverInstance
      ->arg("index")
      .toInt();

  if (
    index < 0 ||
    index >= CAL_MAX_FEEDS
  ) {
    serverInstance->send(
      400,
      "text/plain",
      "Invalid calendar index"
    );

    return;
  }

  feeds[index].enabled = false;
  feeds[index].url = "";

  saveFeed(index);

  serverInstance->send(
    200,
    "application/json",
    "{\"ok\":true}"
  );
}

void handleAction() {
  String action =
    serverInstance->arg("action");

  if (action == "activate") {
    active = true;

    prefs->putBool(
      "cwactive",
      false
    );

    saveGlobalSettings();

    syncCalendars();

    uint32_t hash =
      renderDashboard();

    if (
      hash > 0 &&
      displayCallback &&
      displayCallback(CAL_FILE)
    ) {
      lastDisplayedHash = hash;
      lastDisplayMillis = millis();
    }
  }

  else if (action == "deactivate") {
    active = false;
    saveGlobalSettings();
  }

  else if (action == "sync") {
    if (!syncCalendars()) {
      serverInstance->send(
        502,
        "text/plain",
        lastSyncError
      );

      return;
    }

    renderDashboard();
  }

  else if (action == "display-now") {
    uint32_t hash =
      renderDashboard();

    if (
      hash == 0 ||
      !displayCallback ||
      !displayCallback(CAL_FILE)
    ) {
      serverInstance->send(
        409,
        "text/plain",
        "Display is busy"
      );

      return;
    }

    lastDisplayedHash = hash;
    lastDisplayMillis = millis();
  }

  else {
    serverInstance->send(
      400,
      "text/plain",
      "Unknown action"
    );

    return;
  }

  serverInstance->send(
    200,
    "application/json",
    "{\"ok\":true}"
  );
}

void handlePreview() {
  if (!LittleFS.exists(CAL_FILE)) {
    renderDashboard();
  }

  File file =
    LittleFS.open(
      CAL_FILE,
      FILE_READ
    );

  if (!file) {
    serverInstance->send(
      500,
      "text/plain",
      "Preview unavailable"
    );

    return;
  }

  serverInstance->sendHeader(
    "Cache-Control",
    "no-store"
  );

  serverInstance->streamFile(
    file,
    "application/octet-stream"
  );

  file.close();
}

} // namespace

void calendarExtensionBegin(
  Preferences* preferences,
  CalendarDisplayCallback callback
) {
  prefs = preferences;
  displayCallback = callback;

  loadSettings();
  loadCache();

  renderDashboard();
}

void calendarExtensionRegisterRoutes(
  WebServer& server
) {
  serverInstance = &server;

  server.on(
    "/api/extensions/calendar/status",
    HTTP_GET,
    handleStatus
  );

  server.on(
    "/api/extensions/calendar/settings",
    HTTP_POST,
    handleSettings
  );

  server.on(
    "/api/extensions/calendar/feed",
    HTTP_POST,
    handleFeedSave
  );

  server.on(
    "/api/extensions/calendar/feed/delete",
    HTTP_POST,
    handleFeedDelete
  );

  server.on(
    "/api/extensions/calendar/action",
    HTTP_POST,
    handleAction
  );

  server.on(
    "/api/extensions/calendar/preview",
    HTTP_GET,
    handlePreview
  );
}

void calendarExtensionLoop() {
  if (prefs) {
    active =
      prefs->getBool(
        "calactive",
        active
      );
  }

  if (!active || busy) {
    return;
  }

  if (
    WiFi.status() == WL_CONNECTED &&
    (
      lastSyncMillis == 0 ||
      millis() - lastSyncMillis >=
        syncInterval * 1000UL
    )
  ) {
    syncCalendars();
  }

  if (
    millis() -
    lastRenderCheckMillis <
    60000UL
  ) {
    return;
  }

  lastRenderCheckMillis = millis();

  uint32_t hash =
    renderDashboard();

  if (
    hash > 0 &&
    hash != lastDisplayedHash &&
    (
      lastDisplayMillis == 0 ||
      millis() - lastDisplayMillis >=
        CAL_MIN_DISPLAY_INTERVAL * 1000UL
    )
  ) {
    busy = true;

    if (
      displayCallback &&
      displayCallback(CAL_FILE)
    ) {
      lastDisplayedHash = hash;
      lastDisplayMillis = millis();
    }

    busy = false;
  }
}

bool calendarExtensionActive() {
  if (prefs) {
    active =
      prefs->getBool(
        "calactive",
        active
      );
  }

  return active;
}
