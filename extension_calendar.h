#pragma once

#include <Arduino.h>
#include <Preferences.h>
#include <WebServer.h>

typedef bool (*CalendarDisplayCallback)(
  const String& filePath
);

void calendarExtensionBegin(
  Preferences* preferences,
  CalendarDisplayCallback callback
);

void calendarExtensionRegisterRoutes(
  WebServer& server
);

void calendarExtensionLoop();

bool calendarExtensionActive();
