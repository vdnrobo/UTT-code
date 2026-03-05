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
  Menu::root().parent = nullptr;
  Menu::root().item_count = 0;
  menuCount = 1;
  activeMenu = &Menu::root();
}

void Menu::addParagraph(const char* name, Action func) {
  if (item_count >= MAX_MENU_ITEMS) return;
  byte i = item_count++;
  auto& item = items[i];
  item.names = name;
  item.actions = func;
  item.submenus = nullptr;
  item.values = nullptr;
}

Menu* Menu::addSubmenu(const char* name) {
  if (menuCount >= MAX_MENUS || item_count >= MAX_MENU_ITEMS) return nullptr;
  byte id = menuCount++;
  menus[id].title = name;
  menus[id].parent = parent;
  menus[id].item_count = 0;

  byte i = item_count++;
  auto& item = items[i];
  item.names = name;
  item.actions = nullptr;
  item.submenus = &menus[id];
  item.values = nullptr;
  return &menus[id];
}

void Menu::addValue(const char* name, int* val, int vmin, int vmax, int vstep) {
  if (item_count >= MAX_MENU_ITEMS) return;
  byte i = item_count++;
  auto& item = items[i];
  item.names = name;
  item.actions = nullptr;
  item.submenus = nullptr;
  item.values = val;
  item.vmins = vmin;
  item.vmaxs = vmax;
  item.vsteps = vstep;
}

// UI

static byte totalItems() {
  return activeMenu->item_count + (activeMenu->parent ? 1 : 0);
}

void drawMenu() {
  oled.clear();
  oled.home();

  if (editMode) {
    oled.setScale(1);
    oled.println(activeMenu->items[cursor].names);
    oled.println();
    oled.setScale(3);
    oled.print("  ");
    oled.println(*activeMenu->items[cursor].values);
    oled.setScale(1);
    oled.println();
    oled.print("  [");
    oled.print(activeMenu->items[cursor].vmins);
    oled.print(" .. ");
    oled.print(activeMenu->items[cursor].vmaxs);
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
      oled.print(activeMenu->items[i].names);
      if (activeMenu->items[i].submenus) {
        oled.print(" >>");
      } else if (activeMenu->items[i].values) {
        oled.print(": ");
        oled.print(*activeMenu->items[i].values);
      }
    } else {
      oled.print("<< Назад");
    }
    oled.println();
  }
}

void moveCursor(int d) {
  if (editMode) {
    int* v = activeMenu->items[cursor].values;
    *v += d * activeMenu->items[cursor].vsteps;
    if (*v < activeMenu->items[cursor].vmins) *v = activeMenu->items[cursor].vmins;
    if (*v > activeMenu->items[cursor].vmaxs) *v = activeMenu->items[cursor].vmaxs;
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

  if (activeMenu->parent && cursor == activeMenu->item_count) {
    activeMenu = activeMenu->parent;
    cursor = scroll = 0;
    drawMenu();
    return;
  }

  if (activeMenu->items[cursor].submenus) {
    activeMenu = activeMenu->items[cursor].submenus;
    cursor = scroll = 0;
    drawMenu();
    return;
  }

  if (activeMenu->items[cursor].values) {
    editMode = true;
    drawMenu();
    return;
  }

  if (activeMenu->items[cursor].actions) {
    activeMenu->items[cursor].actions();
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
