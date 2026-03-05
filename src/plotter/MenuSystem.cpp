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

static void bindMenu(Menu& menu) {
  activeMenu = &menu;
  cursor = 0;
  scroll = 0;
}

Menu& Menu::root() {
  return menus[0];
}

void Menu::Item::drawEditMode() const {
  oled.setScale(1);
  oled.println(name);
  oled.println();
  oled.setScale(3);
  oled.print("  ");
  oled.println(*sourceValue);
  oled.setScale(1);
  oled.println();
  oled.print("  [");
  oled.print(valueMin);
  oled.print(" .. ");
  oled.print(valueMax);
  oled.println("]");
  oled.println();
  oled.println("  Нажми для выхода");
}

void Menu::Item::drawCommonMode() const {
  oled.print(name);
  if (nullptr != targetMenu) {
    oled.print(" >>");
  } else if (nullptr != sourceValue) {
    oled.print(": ");
    oled.print(*sourceValue);
  }
}

void Menu::Item::onValue(int delta) {
  int* v = sourceValue;
  *v += delta * valueAdjustStep;
  if (*v < valueMin) *v = valueMin;  // todo constrain
  if (*v > valueMax) *v = valueMax;
}

void Menu::Item::onClick() {
  if (nullptr != targetMenu) {
    bindMenu(*targetMenu);
    return;
  }

  if (nullptr != sourceValue) {
    editMode = true;
    return;
  }

  if (nullptr != action) {
    action();
    return;
  }
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

static byte totalItems(const Menu& menu) {
  return menu.item_count + (nullptr != menu.parentMenu ? 1 : 0);
}

static void drawCommonMode(const Menu& menu) {
  oled.setScale(2);
  oled.println(menu.title);
  oled.setScale(1);

  byte total = totalItems(*activeMenu);
  if (0 == total) return;

  if (cursor < scroll) {
    scroll = cursor;
  }
  if (cursor >= scroll + MAX_MENU_ITEMS_VISIBLE) {
    scroll = cursor - MAX_MENU_ITEMS_VISIBLE + 1;
  }

  byte end = min(scroll + MAX_MENU_ITEMS_VISIBLE, total);

  for (byte i = scroll; i < end; i++) {
    oled.print(i == cursor ? "> " : "  ");
    if (i < menu.item_count) {
      menu.items[i].drawCommonMode();
    } else {
      oled.print("<< Назад");  // Квазивиджет
    }
    oled.println();
  }
}

void drawMenu() {
  oled.clear();
  oled.home();

  if (nullptr == activeMenu) return;

  if (editMode) {
    activeMenu->items[cursor].drawEditMode();
  } else {
    drawCommonMode(*activeMenu);
  }
}

static void adjustCursor(int delta) {
  byte total = totalItems(*activeMenu);
  if (0 == total) return;

  if (delta > 0 && cursor < total - 1) cursor++;
  if (delta < 0 && cursor > 0) cursor--;
}

void menuOnValue(int delta) {
  if (editMode) {
    activeMenu->items[cursor].onValue(delta);
  } else {
    adjustCursor(delta);
  }
  drawMenu();
}

static void onPageSelected(Menu& menu) {
  // Квазивиджет, чей action - установка parentMenu
  if (nullptr != menu.parentMenu && cursor == menu.item_count) {
    bindMenu(*menu.parentMenu);
    return;
  }

  menu.items[cursor].onClick();
}

void selectItem() {
  if (editMode) {
    editMode = false;
    settings.save();
  } else if (activeMenu != nullptr) {
    onPageSelected(*activeMenu);
  }

  drawMenu();
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
