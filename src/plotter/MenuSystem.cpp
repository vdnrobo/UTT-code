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

// BUILDER

void initMenu(const char* title) {
  menuCount = 1;
  menus[0].title = title;
  menus[0].parent = nullptr;
  menus[0].count = 0;
  activeMenu = &menus[0];
}

void Menu::addParagraph(const char* name, Action func) {
  auto m = this;
  if (m->count >= MAX_MENU_ITEMS) return;
  byte i = m->count++;
  m->names[i] = name;
  m->actions[i] = func;
  m->submenus[i] = nullptr;
  m->values[i] = nullptr;
}

void addParagraph(const char* name, Action func) {
  menus[0].addParagraph(name, func);
}

Menu* Menu::addSubmenu(const char* name) {
  auto parent = this;
  if (menuCount >= MAX_MENUS || parent->count >= MAX_MENU_ITEMS) return nullptr;
  byte id = menuCount++;
  menus[id].title = name;
  menus[id].parent = parent;
  menus[id].count = 0;

  byte i = parent->count++;
  parent->names[i] = name;
  parent->actions[i] = nullptr;
  parent->submenus[i] = &menus[id];
  parent->values[i] = nullptr;
  return &menus[id];
}

Menu* addSubmenu(const char* name) {
  return menus[0].addSubmenu(name);
}

void Menu::addValue(const char* name, int* val, int vmin, int vmax, int vstep) {
  auto m = this;
  if (m->count >= MAX_MENU_ITEMS) return;
  byte i = m->count++;
  m->names[i] = name;
  m->actions[i] = nullptr;
  m->submenus[i] = nullptr;
  m->values[i] = val;
  m->vmins[i] = vmin;
  m->vmaxs[i] = vmax;
  m->vsteps[i] = vstep;
}

void addValue(const char* name, int* val, int vmin, int vmax, int vstep) {
  menus[0].addValue(name, val, vmin, vmax, vstep);
}

// UI

static byte totalItems() {
  return activeMenu->count + (activeMenu->parent ? 1 : 0);
}

void drawMenu() {
  oled.clear();
  oled.home();

  if (editMode) {
    oled.setScale(1);
    oled.println(activeMenu->names[cursor]);
    oled.println();
    oled.setScale(3);
    oled.print("  ");
    oled.println(*activeMenu->values[cursor]);
    oled.setScale(1);
    oled.println();
    oled.print("  [");
    oled.print(activeMenu->vmins[cursor]);
    oled.print(" .. ");
    oled.print(activeMenu->vmaxs[cursor]);
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
    if (i < activeMenu->count) {
      oled.print(activeMenu->names[i]);
      if (activeMenu->submenus[i]) {
        oled.print(" >>");
      } else if (activeMenu->values[i]) {
        oled.print(": ");
        oled.print(*activeMenu->values[i]);
      }
    } else {
      oled.print("<< Назад");
    }
    oled.println();
  }
}

void moveCursor(int d) {
  if (editMode) {
    int* v = activeMenu->values[cursor];
    *v += d * activeMenu->vsteps[cursor];
    if (*v < activeMenu->vmins[cursor]) *v = activeMenu->vmins[cursor];
    if (*v > activeMenu->vmaxs[cursor]) *v = activeMenu->vmaxs[cursor];
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

  if (activeMenu->parent && cursor == activeMenu->count) {
    activeMenu = activeMenu->parent;
    cursor = scroll = 0;
    drawMenu();
    return;
  }

  if (activeMenu->submenus[cursor]) {
    activeMenu = activeMenu->submenus[cursor];
    cursor = scroll = 0;
    drawMenu();
    return;
  }

  if (activeMenu->values[cursor]) {
    editMode = true;
    drawMenu();
    return;
  }

  if (activeMenu->actions[cursor]) {
    activeMenu->actions[cursor]();
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
