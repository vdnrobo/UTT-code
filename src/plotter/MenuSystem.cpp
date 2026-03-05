#include "MenuSystem.h"

#include <Arduino.h>

#include "Oled.hpp"
#include "Settings.h"

// STATE

ArrayList<Menu, MAX_MENUS> menus{};
Menu* activeMenu = nullptr;
byte _ui_cursor = 0;
bool editMode = 0;

static void bindMenu(Menu& menu) {
  activeMenu = &menu;
  _ui_cursor = 0;
}

void initMenu(const char* title) {
  Menu root_menu;
  root_menu.title = title;
  root_menu.parentMenu = nullptr;
  root_menu.items.clear();

  menus.push(root_menu);
  activeMenu = &Menu::root();
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

void Item::drawEditMode() const {
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

void Item::drawCommonMode() const {
  oled.print(name);
  if (nullptr != targetMenu) {
    oled.print(" >>");
    oled.print(targetMenu->title);
  } else if (nullptr != valueSource) {
    oled.print(": ");
    oled.print(*valueSource);
  }
}

void Item::onValue(int delta) {
  if (nullptr == valueSource) return;
  *valueSource += delta * valueAdjustStep;
  *valueSource = constrain(*valueSource, valueMin, valueMax);
}

void Item::onClick() {
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

Menu& Menu::root() {
  return menus[0];
}

void Menu::addParagraph(const char* name, Action func) {
  if (items.full()) return;
  Item item;
  item.name = name;
  item.action = func;
  item.targetMenu = nullptr;
  item.valueSource = nullptr;
  items.push(item);
}

Menu* Menu::addSubmenu(const char* name) {
  if (menus.full() || items.full()) return nullptr;
  Menu sub_menu;
  sub_menu.title = name;
  sub_menu.parentMenu = parentMenu;
  menus.push(sub_menu);

  Item item;
  item.name = name;
  item.action = nullptr;
  item.targetMenu = &menus.back();
  item.valueSource = nullptr;
  items.push(item);
  return &menus.back();
}

void Menu::addValue(const char* name, int* val, int vmin, int vmax, int vstep) {
  if (items.full()) return;
  Item item;
  item.name = name;
  item.action = nullptr;
  item.targetMenu = nullptr;
  item.valueSource = val;
  item.valueMin = vmin;
  item.valueMax = vmax;
  item.valueAdjustStep = vstep;
  items.push(item);
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
    if (i < items.size()) {
      items[i].drawCommonMode();
    } else {
      oled.print("<< Назад");  // Квазивиджет
    }
    oled.println();
  }
}

void Menu::onClick(byte cursor) {
  // Квазивиджет, чей action - установка parentMenu
  if (nullptr != parentMenu && cursor == items.size()) {
    bindMenu(*parentMenu);
    return;
  }

  items[cursor].onClick();
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
