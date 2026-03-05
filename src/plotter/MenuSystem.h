#pragma once

#include <Arduino.h>  // Только ради определения byte

#include "ArrayList.hpp"
#include "Config.h"

// TYPES

using Action = void (*)();

struct Menu;

struct Item {
  const char* name;
  Action action;
  Menu* targetMenu;
  int* valueSource;
  int valueMin;
  int valueMax;
  int valueAdjustStep;

  void drawEditMode() const;
  void drawCommonMode() const;

  void onValue(int delta);
  void onClick();
};

struct Menu {
  const char* title;
  Menu* parentMenu;

  ArrayList<Item, MAX_MENU_ITEMS> items{};

  static Menu& root();

  void addParagraph(const char* name, Action func);
  Menu* addSubmenu(const char* name);
  void addValue(const char* name, int* val, int vmin, int vmax, int vstep);

  void drawItems(byte cursor) const;
  void onClick(byte cursor);

  byte itemsTotal() const { return items.size() + (nullptr != parentMenu ? 1 : 0); }

  byte maxScroll() const { return max(0, itemsTotal() - MAX_MENU_ITEMS_VISIBLE); }

  byte calcScroll(byte cursor) const {
    const byte max_scroll = maxScroll();
    return min(max(cursor - (MAX_MENU_ITEMS_VISIBLE - 1), 0), max_scroll);
  }
};

// BUILDER

void initMenu(const char* title);

// UI

void drawMenu();
void menuOnValue(int delta);
void selectItem();
void showMode(byte m, bool done);
void showMessage(const char* line1, const char* line2 = nullptr);
void showResetMessage();
