#include "Arduino.h"
#include "GU_Elements.h"

const int dotsize = 20;
const int spacing = 12;

// Pager routines.


// Start up a basic pager.
void GU_BasicPager::initPager(int n_pages, int first_page, DragCB callback, void *param, uint16_t fillcolor)
{
  _num_pages = n_pages;
  _curr_page = first_page;
  _callback = callback;
  _param = param;
  _fillcolor = fillcolor;

  // Show the first page. Set the page being left to 0xFF as we haven't been on a page.
  clearPage(true);
  (*_callback)(EV_SWIPE, (0xFF << 8) | first_page, param, 0, 0, 0, 0);

  // Trap left and right swipes.
  _gd->onSwipe(0, 0, 0, 0, pager_swipe_wrapper, MAX_EVENTS - 5, (void *)this, CO_HORIZ, 3);
}

void GU_BasicPager::destroyPager(void)
{
  // Leave the current page and go to page 0xFF (no page displayed).
  // This will also cancel the dots button
  clearPage(false);
  (*_callback)(EV_SWIPE, (_curr_page << 8) | 0xFF, _param, 0, 0, 0, 0);

  // cancel the swipe event
  _gd->cancelEvent(MAX_EVENTS - 5);
}

void GU_BasicPager::pager_swipe_cb(EventType ev, int indx, void *param, int x, int y, int dx, int dy)
{
  int leaving_page = _curr_page;

  // Detect whether swiping left (to higher numbered pages) or right (lower)
  if (dx > 0)
  {
    if (_curr_page > 0)
    {
      _curr_page--;
      clearPage(true);
      (*_callback)(EV_SWIPE, (leaving_page << 8) | _curr_page, param, x, y, dx, dy);
    }
  }
  else
  {
    if (_curr_page < _num_pages - 1)
    {
      _curr_page++;
      clearPage(true);
      (*_callback)(EV_SWIPE, (leaving_page << 8) | _curr_page, param, x, y, dx, dy);
    }
  }
}

void pager_swipe_wrapper(EventType ev, int indx, void *param, int x, int y, int dx, int dy)
{
  GU_BasicPager *pager = (GU_BasicPager *)param;

  pager->pager_swipe_cb(ev, indx, param, x, y, dx, dy);
}


// ---------------------------------------------------------------------------------

// Pager (row of dots)

// Init the pager's dots button, and call the base class for further
// initialisation.

void GU_Pager::initPager(int n_pages, int first_page, DragCB callback, void *param, uint16_t fillcolor)
{
  // Button used for selecting pages by tapping dots.
  static GU_Button dots_button(NULL, _gd);
  _dots_button = &dots_button;

  GU_BasicPager::initPager(n_pages, first_page, callback, param, fillcolor);
}

// Clear the page to fillcolor.
void GU_Pager::clearPage(bool dots)
{
  _gfx->fillScreen(_fillcolor);
  displayDots(dots);
}

// Callback for hitting the dots button.
void dotsCB(EventType ev, int indx, void *param, int x, int y)
{
  GU_Pager *pager = (GU_Pager *)param;
  int16_t start_x, start_y;
  uint16_t w, h;
  int dot;

  // Decide which dot has been touched based on the x value.
  pager->_dots_button->getButtonRect(&start_x, &start_y, &w, &h);
  dot = (x - start_x) / (dotsize + spacing);

  // Issue a swipe CB to the caller to select which page to go to.
  if (dot != pager->_curr_page && dot < pager->_num_pages)
  {
    int leaving_page = pager->_curr_page;
    pager->_curr_page = dot;
    pager->clearPage(true);
    (*pager->_callback)(EV_SWIPE, (leaving_page << 8) | pager->_curr_page, param, x, y, 0, 0);
  }
}

// Display the row of dots at bottom of screen with the current page highlighted.
// Dot color is the bitwise negation of the fillcolor, so it shows up on any background.
// Create an invisible button over the dots to pick up taps to select pages.
// If dots is false, don't display the dots, and cancel the invisible button.
void GU_Pager::displayDots(bool dots)
{
  int radius = dotsize / 2;
  int x = (_gfx->width() / 2) - _num_pages * (dotsize + spacing) / 2;
  int y = _gfx->height() - dotsize - spacing;
  if (dots)
  {
    // Create the button. The callback will generate swipe callbacks to
    // tell the user to switch pages.
    _dots_button->initButtonUL(x - radius, y - radius,
                            _num_pages * (dotsize + spacing), dotsize + spacing,
                            0, 0, 0, "\0", 1,
                            dotsCB, MAX_EVENTS - 6, (void *)this);

    // Draw the dots. The dot for the current page is filled.
    for (int i = 0; i < _num_pages; i++)
    {
      if (i == _curr_page)
        _gfx->fillCircle(x, y, radius, ~_fillcolor);
      else
        _gfx->drawCircle(x, y, radius, ~_fillcolor);
      x += dotsize + spacing;
    }
  }
  else
  {
    // Cancel the button, since there are no dots.
  _gd->cancelEvent(MAX_EVENTS - 6);
  }
}

