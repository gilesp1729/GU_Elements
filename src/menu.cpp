#include "Arduino.h"
#include "GU_Elements.h"

// Set up a menu.
void GU_Menu::initMenu(GU_Button *button,
              uint16_t outline, uint16_t fill,
              uint16_t highlight, uint16_t textcolor,
              TapCB callback, int indx, void *param)
{
  int16_t x, y;

  _button = button;
  _outlinecolor = outline;
  _fillcolor = fill;
  _highlightcolor = highlight;
  _textcolor = textcolor;
  _disabledtext = rgb565_average(_fillcolor, _textcolor);
  _textsize = _button->_textsize;
  _w = 0;             // will be accumulated from menu items
  _h = 0;
  _x1 = _button->_x1; // may change if too close to edge of screen
  _y1 = _button->_y1 + _button->_h;
  _n_items = 0;
  _n_displayed = 0;
  _first_displayed = 0;
  _callback = callback;
  _indx = indx;
  _param = param;

  // Item height is derived from button, but may have a little extra to stop
  // crowding based on the font.
  _fc->getTextBounds("M", 0, 0, &x, &y, &_em_width, &_em_height, _textsize);
  _itemheight = max(_button->_h, 2 * _em_height);
  _max_displayed = (_gfx->height() - _y1) / _itemheight;
}

// Set up a menu item at the given index (zero based) within the menu.
void GU_Menu::setMenuItem(int indx, char *itemText, bool enabled, bool checked)
{
  int16_t x, y;
  uint16_t w, h;

  if (indx < 0 || indx > MAX_ITEMS - 1)
    return;   // out of range

  if (indx >= _n_items)
    _n_items = indx + 1;
  if (_n_items <= _max_displayed)
    _n_displayed = _n_items;

  _items[indx].enabled = enabled;
  _items[indx].checked = checked;
  strncpy(_items[indx].label, itemText, 19);
  _items[indx].label[19] = 0;

  // Accumulate the item into the menu area bounds.
  // Give it a little extra room on left and right, esp for check marks
  _fc->getTextBounds(_items[indx].label, 0, 0, &x, &y, &_items[indx].itemwidth, &h, _textsize);
  _items[indx].itemwidth += 3 * _em_width;

  if (_items[indx].itemwidth > _w)
  {
    _w = _items[indx].itemwidth;
    if (_x1 + _w >= _gfx->width())
      _x1 = _gfx->width() - _w - 1;   // push it back onto the screen
  }

  h = (indx + 1) * _itemheight;
  if (h > _h)
    _h = h;

#if 0
  Serial.print("SetMenuItem: xywh ");
  Serial.print(_x1);
  Serial.print(" ");
  Serial.print(_y1);
  Serial.print(" ");
  Serial.print(_w);
  Serial.print(" ");
  Serial.println(_h);
#endif

  // Set a tap on the associated button using internal callbacks. Use the menu's event index
  // as there may be more than one of these going at the same time.
  _gd->onTap(_button->_x1, _button->_y1, _button->_w, _button->_h, menu_tap_wrapper, _indx, (void *)this);
}

// Disable/enable a menu item.
void GU_Menu::enableMenuItem(int indx, bool enabled)
{
  if (indx < 0 || indx > MAX_ITEMS - 1)
    return;   // out of range

  _items[indx].enabled = enabled;
}

// Set the checkbox in a menu item.
void GU_Menu::checkMenuItem(int indx, bool checked)
{
  if (indx < 0 || indx > MAX_ITEMS - 1)
    return;   // out of range

  _items[indx].checked = checked;
}

// Draw the menu with (optionally) one item highlighted.
void GU_Menu::drawMenu(int highlight_item)
{
  int16_t x, y;
  uint16_t w, h, color;
  int16_t item_y1, item_text_y;

  item_y1 = _y1;
  for (int i = _first_displayed; i < _first_displayed + _n_displayed; i++)
  {
    if (i == highlight_item)
      _gfx->fillRect(_x1, item_y1, _w, _itemheight, _highlightcolor);
    else
      _gfx->fillRect(_x1, item_y1, _w, _itemheight, _fillcolor);

    if (_items[i].enabled)
      color = _textcolor;
    else
      color = _disabledtext;
    _fc->getTextBounds(_items[i].label, _x1, item_y1, &x, &y, &w, &h, _textsize);

    // X placement allows for checkmarks, Y placement is as for button with font adjustment.
    item_text_y = item_y1 + (_itemheight / 2) - (h / 2) + (item_y1 - y);

    // If there are more items before the beginning or after the end,
    // put in a little arrow indicator (instead of any check mark)
    if (i == _first_displayed && _first_displayed > 0)
    {
      // Draw a solid up arrow
      _fc->drawText((char)13, _x1 + (_em_width / 2), item_text_y, color, _textsize);
    }
    else if (i == _first_displayed + _n_displayed - 1 && i < _n_items - 1)
    {
      // Draw a solid down arrow
      _fc->drawText((char)14, _x1 + (_em_width / 2), item_text_y, color, _textsize);
    }
    else if (_items[i].checked)
    {
      // Draw a tick mark
      _fc->drawText((char)25, _x1 + (_em_width / 2), item_text_y, color, _textsize);
    }

    _fc->drawText(_items[i].label, _x1 + 2 * _em_width, item_text_y, color, _textsize);

#if 0
    Serial.print(_x1);
    Serial.print(" ");
    Serial.print(item_y1);
    Serial.print(" adjust ");
    Serial.print(item_y1 - y);
    Serial.print(" Bounds h ");
    Serial.print(h);
    Serial.print(" ");
    Serial.println(_items[i].label);
#endif

    item_y1 += _itemheight;
  }
  _gfx->drawRect(_x1, _y1, _w, _h, _outlinecolor);
}

// Redraw the menu if the highlight has changed.
void GU_Menu::drawIfChanged(int item)
{
  if (item != _curr_item)
  {
    drawMenu(item);
    _curr_item = item;
  }
}

// Determine which item the x/y are in, or -1 if it's outside the menu.
int GU_Menu::determineItem(int x, int y)
{
  int i;

  if (x < _x1 || x > _x1 + _w)
    return -1;
  if (y < _y1 || y > _y1 + _h)
    return -1;

  // Check to make sure we aren't right on the bottom line
  i = min((int)(y - _y1) / _itemheight, _n_displayed - 1) + _first_displayed;

  // If we spend time in the first (or last) item, and there is more to
  // display in that direction, alter _first_displayed to suit (this will
  // cause the menu to be scrolled).
  if (1)  // TODO put a time delay in here
  {
    if (i == _first_displayed && _first_displayed > 0)
    {
      _first_displayed--;
      i--;
    }
    else if (i == _first_displayed + _n_displayed - 1 && i < _n_items - 1)
    {
      _first_displayed++;
      i++;
    }
  }

  // If the item is enabled, return its index
  if (_items[i].enabled)
    return i;

  return -1;  // disabled item
}

// Call user's callback function and clean up internal tap and drag events.
void GU_Menu::userCallbackAndCleanUp(int item, int x, int y)
{
  // Call user's calback with user's supplied index and param.
  // The user's index in the high byte, the menu item index in the low byte.
  // the x/y are not important but need to be passed anyway.
  (*_callback)(EV_TAP, (_indx << 8) | item, _param, x, y);

  // Clean up the other menu callbacks.
  _gd->cancelEvent(MAX_EVENTS - 4);
  _gd->cancelEvent(MAX_EVENTS - 3);
  _gd->cancelEvent(MAX_EVENTS - 2);
  _gd->cancelEvent(MAX_EVENTS - 1);
}

void GU_Menu::destroyMenu(void)
{
  _gd->cancelEvent(_indx);

  // Clean up the other menu callbacks.
  _gd->cancelEvent(MAX_EVENTS - 4);
  _gd->cancelEvent(MAX_EVENTS - 3);
  _gd->cancelEvent(MAX_EVENTS - 2);
  _gd->cancelEvent(MAX_EVENTS - 1);
}

// Callback rountines for menu selection.
void GU_Menu::menu_tap_cb(EventType ev, int indx, void *param, int xtap, int ytap)
{
  // Display the menu on tap down. No highlighted items (yet)
  if (ev & EV_RELEASED)
    return;

  _curr_item = -1;    // nothing is selected yet
  drawMenu(-1);

  // Set a drag on the button to allow highlighting when dragged down into the menu.
  // These use fixed index numbers (only one menu is ever active) and are at
  // the highest priority.
  _gd->onDrag(_button->_x1, _button->_y1, _button->_w, _button->_h, menu_drag_wrapper, MAX_EVENTS - 1, (void *)this);

  // Set a tap and a drag on the menu area.
  _gd->onTap(_x1, _y1, _w, _h, menu_item_wrapper, MAX_EVENTS - 2, (void *)this);
  _gd->onDrag(_x1, _y1, _w, _h, menu_drag_wrapper, MAX_EVENTS - 3, (void *)this);

  // Finally, a catch-all tap at lower priority to cancel the menu.
  _gd->onTap(0, 0, 0, 0, menu_cancel_wrapper, MAX_EVENTS - 4, (void *)this);
}

// Wrappers outside the class so they can be passed as function pointers.

// Handle a tap on the associated button to bring down the menu, and set up
// other events on the menu area.
void menu_tap_wrapper(EventType ev, int indx, void *param, int x, int y)
{
  GU_Menu *menu = (GU_Menu *)param;

  // Not very nice in C++ I know, but at least it works to get back
  // into a member function.
  menu->menu_tap_cb(ev, indx, param, x, y);
}

// Handle a tap on (or a drag into) a menu item. Return the selection when released.
void menu_item_wrapper(EventType ev, int indx, void *param, int x, int y)
{
  GU_Menu *menu = (GU_Menu *)param;
  int item;

  item = menu->determineItem(x, y);
#if 0
  Serial.print("Menu item ");
  Serial.print(item);
  Serial.print(" ");
  Serial.print(x);
  Serial.print(" ");
  Serial.println(y);
#endif
  if (ev & EV_RELEASED)
    menu->userCallbackAndCleanUp(menu->_curr_item, x, y);
  else
    menu->drawIfChanged(item);
}

// Handle a drag, starting in either the menu or its associated button,
// to highlight the items and return the selection when released.
void menu_drag_wrapper(EventType ev, int indx, void *param, int x, int y, int dx, int dy)
{
  menu_item_wrapper(ev, indx, param, x + dx, y + dy);
}

// Handle a tap outside the menu area to cancel the menu and return
// a selection result of -1.
void menu_cancel_wrapper(EventType ev, int indx, void *param, int x, int y)
{
  GU_Menu *menu = (GU_Menu *)param;

  menu->userCallbackAndCleanUp(-1, x, y);
}

// Pack and unpack a RGB565 color.
void rgb565_unpack(uint16_t color, uint8_t *red, uint8_t *green, uint8_t *blue)
{
  *red = (color >> 8) & 0xF8;
  *green = (color >> 3) & 0xFC;
  *blue = (color << 3) & 0xF8;
}

uint16_t rgb565_pack(uint8_t red, uint8_t green, uint8_t blue)
{
  return ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3);
}

// Take the average of two RGB565 colors.
uint16_t rgb565_average(uint16_t color1, uint16_t color2)
{
  uint8_t r1, g1, b1, r2, g2, b2;

  rgb565_unpack(color1, &r1, &g1, &b1);
  rgb565_unpack(color2, &r2, &g2, &b2);
  return rgb565_pack((r1 + r2) / 2, (g1 + g2) / 2, (b1 + b2) / 2);
}


