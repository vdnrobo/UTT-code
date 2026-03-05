#include "MenuSystem.h"

#include <Arduino.h>

#include "Oled.hpp"
#include "Settings.h"

// STATE

Menu menus[MAX_MENUS];
byte menuCount = 0;
Menu* activeMenu = nullptr;
byte _ui_cursor = 0;
bool editMode = 0;

static void bindMenu(Menu& menu) {
  activeMenu = &menu;
  _ui_cursor = 0;
}

Menu& Menu::root() {
  return menus[0];
}

byte Menu::itemsTotal() const {
  return _itemCount + (nullptr != parentMenu ? 1 : 0);
}

void Menu::Item::drawEditMode() const {
  oled.setScale(1);
  oled.println(name);
  oled.println();
  oled.setScale(3);
  oled.print("  ");
  if (nullptr == valueSource) {
    oled.println("null");
  } else {
    oled.println(*valueSource);
  }
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
    oled.print(targetMenu->title);
  } else if (nullptr != valueSource) {
    oled.print(": ");
    oled.print(*valueSource);
  }
}

void Menu::Item::onValue(int delta) {
  if (nullptr == valueSource) return;
  *valueSource += delta * valueAdjustStep;
  *valueSource = constrain(*valueSource, valueMin, valueMax);
}

void Menu::Item::onClick() {
  if (nullptr != targetMenu) {
    bindMenu(*targetMenu);
    return;
  }

  if (nullptr != valueSource) {
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
  Menu::root()._itemCount = 0;
  menuCount = 1;
  activeMenu = &Menu::root();
}

void Menu::addParagraph(const char* name, Action func) {
  if (_itemCount >= MAX_MENU_ITEMS) return;
  byte i = _itemCount++;
  auto& item = items[i];
  item.name = name;
  item.action = func;
  item.targetMenu = nullptr;
  item.valueSource = nullptr;
}

Menu* Menu::addSubmenu(const char* name) {
  if (menuCount >= MAX_MENUS || _itemCount >= MAX_MENU_ITEMS) return nullptr;
  byte id = menuCount++;
  menus[id].title = name;
  menus[id].parentMenu = parentMenu;
  menus[id]._itemCount = 0;

  byte i = _itemCount++;
  auto& item = items[i];
  item.name = name;
  item.action = nullptr;
  item.targetMenu = &menus[id];
  item.valueSource = nullptr;
  return &menus[id];
}

void Menu::addValue(const char* name, int* val, int vmin, int vmax, int vstep) {
  if (_itemCount >= MAX_MENU_ITEMS) return;
  byte i = _itemCount++;
  auto& item = items[i];
  item.name = name;
  item.action = nullptr;
  item.targetMenu = nullptr;
  item.valueSource = val;
  item.valueMin = vmin;
  item.valueMax = vmax;
  item.valueAdjustStep = vstep;
}

void Menu::drawItems(byte cursor) const {
  oled.setScale(2);
  oled.println(title);
  oled.setScale(1);

  const byte total = itemsTotal();
  if (0 == total) return;

  const byte scroll = calcScroll(cursor);
  const byte end = min(scroll + MAX_MENU_ITEMS_VISIBLE, total);

  for (byte i = scroll; i < end; i++) {
    oled.write(i == cursor ? '>' : ' ');
    oled.write(' ');
    if (i < _itemCount) {
      items[i].drawCommonMode();
    } else {
      oled.print("<< Назад");  // Квазивиджет
    }
    oled.println();
  }
}

void Menu::onClick(byte cursor) {
  // Квазивиджет, чей action - установка parentMenu
  if (nullptr != parentMenu && cursor == _itemCount) {
    bindMenu(*parentMenu);
    return;
  }

  items[cursor].onClick();
}

void drawMenu() {
  oled.clear();
  oled.home();

  if (nullptr == activeMenu) return;

  if (editMode) {
    activeMenu->items[_ui_cursor].drawEditMode();
  } else {
    activeMenu->drawItems(_ui_cursor);
  }
}

static void adjustCursor(int delta) {
  const byte total = activeMenu->itemsTotal();
  if (0 == total) return;

  if (delta > 0 && _ui_cursor < total - 1) _ui_cursor++;
  if (delta < 0 && _ui_cursor > 0) _ui_cursor--;
}

void menuOnValue(int delta) {
  if (editMode) {
    activeMenu->items[_ui_cursor].onValue(delta);
  } else {
    adjustCursor(delta);
  }
  drawMenu();
}

void selectItem() {
  if (editMode) {
    editMode = false;
    settings.save();
  } else if (activeMenu != nullptr) {
    activeMenu->onClick(_ui_cursor);
  }

  drawMenu();
}

void showMode(byte mode, bool done) {
  oled.clear();
  oled.home();
  oled.setScale(2);
  oled.print("Режим ");
  oled.println(mode);
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
  if (nullptr != line2) {
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
