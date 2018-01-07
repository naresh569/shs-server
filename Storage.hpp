
#pragma once
#include <EEPROM.h>
#include "Utilities.hpp"

class Storage {
  int index;
  public:
    Storage();
    bool put(char*);
    char* get();
    void clear();
    void write(const char*);
    void write(char);
    void write(int);
};

Storage :: Storage() {
  index = 0;
}

bool Storage :: put(char* data) {
  int maxLength = EEPROM.length();
  int len = length(data);

  if (len >= maxLength) {
    return false;
  }

  int i;
  for (i = 0; i < len; i++) {
    if (EEPROM[i] == data[i])
    continue;
    EEPROM[i] = data[i];
  }
  EEPROM[i] = 0;
  return true;
}

char* Storage :: get() {
  int maxLength = EEPROM.length();
  char* data = new char[1000];
  int i;
  for (i = 0; i < maxLength; i++) {
    char c = EEPROM[i];
    if (c == 0) {
      break;
    }
    data[i] = c;
  }
  data[i] = 0;



  return data;
}

void Storage :: clear() {
    EEPROM[0] = 0;
}

void Storage :: write(const char* data) {
  if (!data[0])
  EEPROM[index] = 0;
  for (int i = 0; data[i] != 0; i++) {
    EEPROM[index++] = data[i];
  }
}

void Storage :: write(char data) {
  if (!data)
  EEPROM[index] = 0;
  EEPROM[index++] = data;
}

void Storage :: write(int data) {
  char temp[SIZESHORT] = "";
  concat(temp, data);
  write(temp);
}