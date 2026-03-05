#pragma once

#include <Arduino.h>  // Только ради определения byte

#include "Config.h"

// TYPES

using Action = void (*)();

struct Menu {
  struct Item {
    const char* name;
    Action action;
    Menu* targetMenu;
    int* sourceValue;
    int valueMin;
    int valueMax;
    int valueAdjustStep;

    void drawEditMode() const;
    void drawCommonMode() const;

    void onValue(int delta);
    void onClick();
  };

  const char* title;
  Menu* parentMenu;

  Item items[MAX_MENU_ITEMS];
  byte _itemCount;

  static Menu& root();

  byte itemsTotal() const;

  void addParagraph(const char* name, Action func);
  Menu* addSubmenu(const char* name);
  void addValue(const char* name, int* val, int vmin, int vmax, int vstep);

  void drawItems(byte cursor, byte scroll, byte end) const;
  void onClick(byte cursor);
};

// STATE

extern Menu menus[MAX_MENUS];
extern byte menuCount;
extern Menu* activeMenu;
extern byte _ui_cursor;
extern byte _ui_scroll;
extern bool editMode;

// BUILDER

void initMenu(const char* title);

// UI

void drawMenu();
void menuOnValue(int delta);
void selectItem();
void showMode(byte m, bool done);
void showMessage(const char* line1, const char* line2 = nullptr);
void showResetMessage();
