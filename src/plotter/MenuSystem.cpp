#include "MenuSystem.h"

#include <Arduino.h>

#include "Oled.hpp"
#include "Settings.h"

// STATE

Menu menus[MAX_MENUS];
byte menuCount = 0;
Menu* activeMenu = nullptr;
byte cursor = 0;
byte scroll = 0;
bool editMode = 0;

Menu& Menu::root() {
  return menus[0];
}

// BUILDER

void initMenu(const char* title) {
  Menu::root().title = title;
  Menu::root().parentMenu = nullptr;
  Menu::root().item_count = 0;
  menuCount = 1;
  activeMenu = &Menu::root();
}

void Menu::addParagraph(const char* name, Action func) {
  if (item_count >= MAX_MENU_ITEMS) return;
  byte i = item_count++;
  auto& item = items[i];
  item.name = name;
  item.action = func;
  item.targetMenu = nullptr;
  item.sourceValue = nullptr;
}

Menu* Menu::addSubmenu(const char* name) {
  if (menuCount >= MAX_MENUS || item_count >= MAX_MENU_ITEMS) return nullptr;
  byte id = menuCount++;
  menus[id].title = name;
  menus[id].parentMenu = parentMenu;
  menus[id].item_count = 0;

  byte i = item_count++;
  auto& item = items[i];
  item.name = name;
  item.action = nullptr;
  item.targetMenu = &menus[id];
  item.sourceValue = nullptr;
  return &menus[id];
}

void Menu::addValue(const char* name, int* val, int vmin, int vmax, int vstep) {
  if (item_count >= MAX_MENU_ITEMS) return;
  byte i = item_count++;
  auto& item = items[i];
  item.name = name;
  item.action = nullptr;
  item.targetMenu = nullptr;
  item.sourceValue = val;
  item.valueMin = vmin;
  item.valueMax = vmax;
  item.valueAdjustStep = vstep;
}

// UI

static byte totalItems() {
  return activeMenu->item_count + (activeMenu->parentMenu ? 1 : 0);
}

void drawMenu() {
  oled.clear();
  oled.home();

  if (editMode) {
    oled.setScale(1);
    oled.println(activeMenu->items[cursor].name);
    oled.println();
    oled.setScale(3);
    oled.print("  ");
    oled.println(*activeMenu->items[cursor].sourceValue);
    oled.setScale(1);
    oled.println();
    oled.print("  [");
    oled.print(activeMenu->items[cursor].valueMin);
    oled.print(" .. ");
    oled.print(activeMenu->items[cursor].valueMax);
    oled.println("]");
    oled.println();
    oled.println("  Нажми для выхода");
    return;
  }

  oled.setScale(2);
  oled.println(activeMenu->title);
  oled.setScale(1);

  byte total = totalItems();
  if (!total) return;

  if (cursor < scroll) scroll = cursor;
  if (cursor >= scroll + MAX_MENU_ITEMS_VISIBLE) scroll = cursor - MAX_MENU_ITEMS_VISIBLE + 1;

  byte end = min(scroll + MAX_MENU_ITEMS_VISIBLE, total);

  for (byte i = scroll; i < end; i++) {
    oled.print(i == cursor ? "> " : "  ");
    if (i < activeMenu->item_count) {
      oled.print(activeMenu->items[i].name);
      if (activeMenu->items[i].targetMenu) {
        oled.print(" >>");
      } else if (activeMenu->items[i].sourceValue) {
        oled.print(": ");
        oled.print(*activeMenu->items[i].sourceValue);
      }
    } else {
      oled.print("<< Назад");
    }
    oled.println();
  }
}

void moveCursor(int d) {
  if (editMode) {
    int* v = activeMenu->items[cursor].sourceValue;
    *v += d * activeMenu->items[cursor].valueAdjustStep;
    if (*v < activeMenu->items[cursor].valueMin) *v = activeMenu->items[cursor].valueMin;
    if (*v > activeMenu->items[cursor].valueMax) *v = activeMenu->items[cursor].valueMax;
    drawMenu();
    return;
  }

  byte total = totalItems();
  if (!total) return;
  if (d > 0 && cursor < total - 1) cursor++;
  if (d < 0 && cursor > 0) cursor--;
  drawMenu();
}

void selectItem() {
  if (editMode) {
    editMode = false;
    settings.save();
    drawMenu();
    return;
  }

  if (activeMenu->parentMenu && cursor == activeMenu->item_count) {
    activeMenu = activeMenu->parentMenu;
    cursor = scroll = 0;
    drawMenu();
    return;
  }

  if (activeMenu->items[cursor].targetMenu) {
    activeMenu = activeMenu->items[cursor].targetMenu;
    cursor = scroll = 0;
    drawMenu();
    return;
  }

  if (activeMenu->items[cursor].sourceValue) {
    editMode = true;
    drawMenu();
    return;
  }

  if (activeMenu->items[cursor].action) {
    activeMenu->items[cursor].action();
    drawMenu();
  }
}

void showMode(byte m, bool done) {
  oled.clear();
  oled.home();
  oled.setScale(2);
  oled.print("Режим ");
  oled.println(m);
  oled.setScale(1);
  oled.println();
  oled.println(done ? "Выполнен" : "Выполняется");
}

void showMessage(const char* line1, const char* line2) {
  oled.clear();
  oled.home();
  oled.setScale(2);
  oled.println(line1);
  oled.setScale(1);
  if (line2) {
    oled.println();
    oled.println(line2);
  }
}

void showResetMessage() {
  oled.clear();
  oled.home();
  oled.setScale(2);
  oled.println("Сброс!");
  oled.setScale(1);
  oled.println();
  oled.println("Настройки по");
  oled.println("умолчанию");
}
