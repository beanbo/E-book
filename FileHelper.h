#ifndef BOARD_HAS_PSRAM
#error "Please enable PSRAM !!!"
#endif

#pragma once
#include <Arduino.h>
#include "FS.h"
#include "SPIFFS.h"

void InitializeFileSystem();

void listDir(const char* dirname, uint8_t levels);

void readFile(const char* path, String& fileText);
void writeFile(const char* path, const char* message);
void appendFile(const char* path, const char* message);
void renameFile(const char* path1, const char* path2);
void deleteFile(const char* path);

void testFileIO(const char* path);
