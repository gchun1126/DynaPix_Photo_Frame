#pragma once

#include <Arduino.h>
#include <WebServer.h>
#include <Preferences.h>

typedef bool (*ClockWeatherDisplayCallback)(
  const String& path
);

void clockWeatherBegin(
  Preferences* preferences,
  ClockWeatherDisplayCallback displayCallback
);

void clockWeatherRegisterRoutes(
  WebServer& server
);

void clockWeatherLoop();

bool clockWeatherActive();


// ============================================================
// CLOCK & WEATHER SNAPSHOT FOR OTHER EXTENSIONS
// ============================================================

struct ClockWeatherSnapshot {
  bool valid;
  float temperature;
  int weatherCode;
  String description;
  String location;
};

bool clockWeatherGetSnapshot(
  ClockWeatherSnapshot& snapshot
);
