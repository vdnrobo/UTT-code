#pragma once

#include <Arduino.h>  // Только ради определения byte

#include "Config.h"

// TYPES

using Action = void (*)();

struct Menu {
  const char* title;
  const char* names[MAX_MENU_ITEMS];
  Action actions[MAX_MENU_ITEMS];
  Menu* submenus[MAX_MENU_ITEMS];
  int* values[MAX_MENU_ITEMS];
  int vmins[MAX_MENU_ITEMS];
  int vmaxs[MAX_MENU_ITEMS];
  int vsteps[MAX_MENU_ITEMS];
  Menu* parent;
  byte count;

  void addParagraph(const char* name, Action func);
  Menu* addSubmenu(const char* name);
  void addValue(const char* name, int* val, int vmin, int vmax, int vstep);
};

// STATE

extern Menu menus[MAX_MENUS];
extern byte menuCount;
extern Menu* activeMenu;
extern byte cursor;
extern byte scroll;
extern bool editMode;

// BUILDER

void initMenu(const char* title);
void addParagraph(const char* name, Action func);
Menu* addSubmenu(const char* name);
void addValue(const char* name, int* val, int vmin, int vmax, int vstep);

// UI

void drawMenu();
void moveCursor(int d);
void selectItem();
void showMode(byte m, bool done);
void showMessage(const char* line1, const char* line2 = nullptr);
void showResetMessage();
