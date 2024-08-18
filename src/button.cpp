#include "Arduino.h"
#include "GU_Elements.h"

// Set up a button.
void GU_Button::initButtonUL(int16_t x1, int16_t y1, uint16_t w, uint16_t h,
                            uint16_t outline, uint16_t fill,
                            uint16_t textcolor, char *label,
                            uint8_t textsize,
                            TapCB callback, int indx, void *param)

{
  _x1 = x1;
  _y1 = y1;
  _w = w;
  _h = h;
  _outlinecolor = outline;
  _fillcolor = fill;
  _textcolor = textcolor;
  _textsize = textsize;
  strncpy(_label, label, 9);
  _label[9] = 0; // strncpy does not place a null at the end.
                // When 'label' is >9 characters, _label is not terminated.
  _indx = indx;
  if (callback != NULL)
    _gd->onTap(_x1, _y1, _w, _h, callback, indx, param);
  else
    // We expect a menu to be triggered by this button, and will call its callback instead
    _is_menu = true;
}

// Destroy the button.
void GU_Button::destroyButton(void)
{
  if (!_is_menu)
    _gd->cancelEvent(_indx);
}

// Draw a button.
void GU_Button::drawButton(void)
{
  int16_t x, y;
  uint16_t w, h;

  // If there is no FC, there is no GFX, and we cannot display anything.
  if (_fc == NULL)
    return;

  // If button is associated with a menu, draw it square
  if (_is_menu)
  {
    _gfx->fillRect(_x1, _y1, _w, _h, _fillcolor);
    _gfx->drawRect(_x1, _y1, _w, _h, _outlinecolor);
  }
  else
  {
    uint8_t r = min(_w, _h) / 4; // Corner radius
    _gfx->fillRoundRect(_x1, _y1, _w, _h, r, _fillcolor);
    _gfx->drawRoundRect(_x1, _y1, _w, _h, r, _outlinecolor);
  }

  // Original code for system font only
  //_gfx->setCursor(_x1 + (_w / 2) - (strlen(_label) * 3 * _textsize_x),
  //                _y1 + (_h / 2) - (4 * _textsize_y));

  _fc->getTextBounds(_label, _x1, _y1, &x, &y, &w, &h, _textsize);
#if 0
  {
    char buf[64];
    sprintf(buf, "x/y %d %d xywh %d %d %d %d", _x1, _y1, x, y, w, h);
    Serial.println(buf);
  }
#endif

  // System font is drawn from the upper left, but custom fonts are
  // drawn from the lower left. Adjust by the Y returned from getTextBounds().
  _fc->drawText(_label, _x1 + (_w / 2) - (w / 2), _y1 + (_h / 2) - (h / 2) + (_y1 - y), _textcolor, _textsize);
}

void GU_Button::setText(char *label)
{
  strncpy(_label, label, 9);
  _label[9] = 0; // strncpy does not place a null at the end.
  drawButton();
}

void GU_Button::setColor(uint16_t outline, uint16_t fill, uint16_t textcolor)
{
  _outlinecolor = outline;
  _fillcolor = fill;
  _textcolor = textcolor;
  drawButton();
}