#include "MenuSystem.h"

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
    ui.bind(*targetMenu);
    return;
  }

  if (nullptr != valueSource) {
    ui.editMode = true;
    return;
  }

  if (nullptr != action) {
    action();
    return;
  }
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
  if (ui._menus.full() || items.full()) return nullptr;
  Menu sub_menu;
  sub_menu.title = name;
  sub_menu.parentMenu = parentMenu;
  ui._menus.push(sub_menu);

  Item item;
  item.name = name;
  item.action = nullptr;
  item.targetMenu = &ui._menus.back();
  item.valueSource = nullptr;
  items.push(item);
  return &ui._menus.back();
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
    ui.bind(*parentMenu);
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
