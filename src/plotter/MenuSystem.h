#pragma once

#include <Arduino.h>  // Только ради определения byte

#include "ArrayList.hpp"
#include "Config.h"
#include "Oled.hpp"
#include "Settings.h"

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

struct UI {
  Menu& root() { return _menus[0]; }

  void initMenu(const char* title) {
    Menu root_menu;
    root_menu.title = title;
    root_menu.parentMenu = nullptr;
    root_menu.items.clear();

    _menus.push(root_menu);
    _activeMenu = &root();
  }

  void draw() {
    oled.clear();
    oled.home();

    if (nullptr == _activeMenu) return;

    if (editMode) {
      _activeMenu->items[_cursor].drawEditMode();
    } else {
      _activeMenu->drawItems(_cursor);
    }
  }

  void onValue(int delta) {
    if (editMode) {
      _activeMenu->items[_cursor].onValue(delta);
    } else {
      adjustCursor(delta);
    }
    draw();
  }

  void onClick() {
    if (editMode) {
      editMode = false;
      settings.save();
    } else if (_activeMenu != nullptr) {
      _activeMenu->onClick(_cursor);
    }
    draw();
  }

  void bind(Menu& menu) {
    _activeMenu = &menu;
    _cursor = 0;
  }

  ArrayList<Menu, MAX_MENUS> _menus{};

private:
  Menu* _activeMenu{nullptr};
  byte _cursor{0};

public:
  bool editMode{false};

private:
  void adjustCursor(int delta) {
    const byte total = _activeMenu->itemsTotal();
    if (0 == total) return;

    if (delta > 0 && _cursor < total - 1) _cursor++;
    if (delta < 0 && _cursor > 0) _cursor--;
  }
};

extern UI ui;

// UI

void showMode(byte m, bool done);
void showMessage(const char* line1, const char* line2 = nullptr);
void showResetMessage();
