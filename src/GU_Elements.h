#ifndef GU_ELEMENTS_H
#define GU_ELEMENTS_H

// UI elements for the Giga display.

#include <Arduino_GigaDisplay_GFX.h>
#include <GestureDetector.h>
#include <FontCollection.h>

// ---------------------------------------------------------------------------------

// Provide a class to draw an Adafruit_GFX_Button with a custom font,
// (the Adafruit button only works correctly with system font)
// The custom font is drawn from a font collection, allowing buttons
// to all have consistent fonts and sizes.

// Also uses GestureDetector callbacks to signal button press/release.

class GU_Button
{
public:
  friend class GU_Menu;

  // If fc is NULL, nothing will be drawn, but the button will still pick up taps.
  GU_Button(FontCollection *fc, GestureDetector *gd)
            { _gd = gd; _fc = fc; _gfx = fc != NULL ? fc->_gfx : NULL; }
  ~GU_Button() {  }

  // Set up the placement and appearance of a button.

  // x1           The X coordinate of the top left of the button
  // y1           The Y coordinate of the top left of the button
  // w            Width of the button. Should accommodate the label string.
  // h            Height of the button. Should be at least 10 pixels more than
  //              the advance height of the text font.
  // outline      Color of the outline (16-bit 5-6-5 standard)
  // fill         Color of the button fill (16-bit 5-6-5 standard)
  // textcolor    Color of the button label (16-bit 5-6-5 standard)
  // label        Ascii string of the text inside the button
  // textsize     The font magnification of the label text
  //              (it will be multiplied by the size in the font collection)
  // callback     Tap callback as used by GestureDetector
  //              (may be NULL if a menu will be associated with the button)
  // indx         Priority index of callback in GestureDetector
  // param        User param to pass to callback

  void initButtonUL(int16_t x1, int16_t y1, uint16_t w, uint16_t h,
                    uint16_t outline, uint16_t fill,
                    uint16_t textcolor, char *label,
                    uint8_t textsize,
                    TapCB callback = NULL, int indx = 0, void *param = NULL);

  // Destroy the button.
  void destroyButton(void);

  // Draw the button.
  void drawButton(void);

  // Set the text label of the button
  void setText(char *label);
  // Single character version
  void setText(char ch)
       { char text[2] = {ch, 0}; setText(text); }

  // Set the colors used by a button
  void setColor(uint16_t outline, uint16_t fill, uint16_t textcolor);

  // Get the bounding rect of the button.
  void getButtonRect(int16_t *x, int16_t *y, uint16_t *w, uint16_t *h)
  {
    *x = _x1;
    *y = _y1;
    *w = _w;
    *h = _h;
  }

private:
  Adafruit_GFX *_gfx;
  GestureDetector *_gd;
  FontCollection *_fc;
  int16_t _x1, _y1; // Coordinates of top-left corner
  uint16_t _w, _h;
  uint8_t _textsize;
  uint16_t _outlinecolor, _fillcolor, _textcolor;
  char _label[10];
  bool _is_menu = false;
  int _indx;
};

// ---------------------------------------------------------------------------------

// Max items in a menu
#define MAX_ITEMS   20

// The Menu class:
// - associates a drop-down menu with a button
// - allows multiple menu items to be added
// - allows menu items to be checked or disabled
// - calls a callback when a menu item is selected
// Internal callbacks are at events MAX_EVENTS -1, -2, -3 and -4
// to be at the highest priority when menus are displayed.
class GU_Menu
{
public:
  friend class GU_Button;
  friend void menu_tap_wrapper(EventType ev, int indx, void *param, int x, int y);
  friend void menu_drag_wrapper(EventType ev, int indx, void *param, int x, int y, int dx, int dy);
  friend void menu_item_wrapper(EventType ev, int indx, void *param, int x, int y);
  friend void menu_cancel_wrapper(EventType ev, int indx, void *param, int x, int y);

  GU_Menu(FontCollection *fc, GestureDetector *gd)
          { _gd = gd; _fc = fc; _gfx = fc->_gfx ;}
  ~GU_Menu() {  }

  // Set up a menu associated with a button.
  // The menu item text sizes and height are derived from the button.
  // Provide a callback giving the item selected when tapping or dragging
  // down to the item. The callback is called like a tap callback (with the
  // released flag set to indicate completion)

  // button       The button that will be used to trigger the menu
  // outline      Color of the outline (16-bit 5-6-5 standard)
  // fill         Color of the menu item fill (16-bit 5-6-5 standard)
  // highlight    Color of the fill when highlighted by dragging (16-bit 5-6-5 standard)
  // textcolor    Color of the menu item text (16-bit 5-6-5 standard)
  // callback     Tap callback as used by GestureDetector
  // indx         Priority index of callback. The callback will be called with
  //              this index in its high byte, and the menu item
  //              index in the low byte or 0xFF if menu selection was cancelled
  //              by dragging or tapping outside the menu area.
  // param        User param to pass to callback

  void initMenu(GU_Button *button,
                uint16_t outline, uint16_t fill,
                uint16_t highlight, uint16_t textcolor,
                TapCB callback, int indx, void *param = NULL);

  // Destroy the menu.
  void destroyMenu(void);

  // Set up a menu item at the given index (zero based) within the menu.
  void setMenuItem(int indx, char *itemText, bool enabled = true, bool checked = false);
  // Single character version
  void setMenuItem(int indx, char ch, bool enabled = true, bool checked = false)
                { char item[2] = {ch, 0}; setMenuItem(indx, item, enabled, checked); }

  // Disable/enable a menu item.
  void enableMenuItem(int indx, bool enabled);

  // Set the checkbox in a menu item.
  void checkMenuItem(int indx, bool checked);

  // Ae we displaying a menu? (any menu, not just this instance)
  bool isAnyMenuDisplayed(void) { return _gd->isEventRegistered(MAX_EVENTS - 4); }

private:
  typedef struct GU_MenuItem
  {
    char      label[20];       // String to display on menu item
    uint16_t  itemwidth;       // Width from getTextBounds
    bool      checked;         // Whether checked or enabled/disables
    bool      enabled;
  } GU_MenuItem;

  Adafruit_GFX *_gfx;
  GestureDetector *_gd;
  FontCollection *_fc;
  int16_t _x1, _y1;   // Coordinates of top-left corner of menu area
  uint16_t _w, _h;    // Width/height come from items extents
  uint16_t _itemheight; // Height of a menu item comes from button
  uint8_t _textsize;  // Text size comes from the button
  uint16_t _outlinecolor, _fillcolor, _highlightcolor, _textcolor, _disabledtext;
  GU_Button *_button;    // the associated button
  GU_MenuItem _items[MAX_ITEMS];
  int _n_items;         // total number of items in the menu
  int _n_displayed;     // number actually displayed (if there isn't room for all of them)
  int _first_displayed; // index of top displayed item in menu
  int _max_displayed;    // the max number of items that can be displayed within screen height
  TapCB _callback;
  int _indx;
  void *_param;
  uint16_t _em_width, _em_height;
  int _curr_item = -1;
  long _start_millis = 0; // counter timer for dwelling on a menu item

  // Callback functons that assist with drawing the menu
  void menu_tap_cb(EventType ev, int indx, void *param, int x, int y);
  void menu_drag_cb(EventType ev, int indx, void *param, int x, int y, int dx, int dy);
  void menu_item_cb(EventType ev, int indx, void *param, int x, int y);

  // Menu drawing and navigation
  void drawMenu(int highlight_item);
  void drawIfChanged(int item);
  int determineItem(int x, int y);
  void userCallbackAndCleanUp(int item, int x, int y);
};

// Wrappers to alow member functions to be passed as pointers
void menu_tap_wrapper(EventType ev, int indx, void *param, int x, int y);
void menu_drag_wrapper(EventType ev, int indx, void *param, int x, int y, int dx, int dy);
void menu_item_wrapper(EventType ev, int indx, void *param, int x, int y);
void menu_cancel_wrapper(EventType ev, int indx, void *param, int x, int y);

// ---------------------------------------------------------------------------------

// A class that allows multiple full-screen pages to be swiped between.
// This basic version just clears screen between pages. It can be subclassed
// to display various sorts of page/swipeable indicators. Two subclasses
// (Pager and Sidebar) are given below.
//
// The callback is a DragCB whose index is passed as:
// - page being hidden in high byte (or 0xFF if there isn't one)
// - page being shown in low byte (or 0xFF if leaving the pager)
// Since there is only one pager, no index is passed to init_pager()
// (the real callback is hardcoded at MAX_EVENTS - 5)
class GU_BasicPager
{
public:
  friend void pager_swipe_wrapper(EventType ev, int indx, void *param, int x, int y, int dx, int dy);

  GU_BasicPager(GigaDisplay_GFX *gfx, GestureDetector *gd) { _gfx = gfx; _gd = gd; }
  ~GU_BasicPager() {  }

  // Set up a pager to go from 0 to n_pages-1 pages. Clear screen to
  // the fill color and display the given first page.

  // n_pages      Total number of pages.
  // first_page   The page to display first (0 to n_pages-1)
  // callback     Drag callback function to receive swipes.
  // param        User param to pass to callback.
  // fillcolor    Color to clear screens to (default 0)
  void initPager(int n_pages, int first_page,
                DragCB callback, void *param = NULL, uint16_t fillcolor = 0);

  // Take down the pager.
  void destroyPager(void);

  // Clear the page to fillcolor. Optionally display some sort of indicator
  // that the page can be swiped. This might be dots at bottom (class Pager)
  // or a swipe indicator line at left or right (class Sidebar). This basic version
  // just clears the shole screen.
  virtual void clearPage(bool indicator) { _gfx->fillScreen(_fillcolor); }

  // Go to a given page.
  void gotoPage(int page);

protected:
  Adafruit_GFX *_gfx;
  GestureDetector *_gd;
  int _num_pages = 1;
  int _curr_page = 0;
  DragCB _callback;
  void *_param;
  uint16_t _fillcolor;

  // Callback functons
  void pager_swipe_cb(EventType ev, int indx, void *param, int x, int y, int dx, int dy);
};

// Wrappers
void pager_swipe_wrapper(EventType ev, int indx, void *param, int x, int y, int dx, int dy);
void dotsCB(EventType ev, int indx, void *param, int x, int y);


// ---------------------------------------------------------------------------------

// The Pager class allows swiping between full-page images. A row of dots
// is displayed at the bottom of each page, showing which one is currently
// displayed. Swiping right/left or tapping the dots wll select diferent pages.

class GU_Pager : public GU_BasicPager
{
public:
  friend void dotsCB(EventType ev, int indx, void *param, int x, int y);

  //GU_Pager(GigaDisplay_GFX *gfx, GestureDetector *gd) { _gfx = gfx; _gd = gd; }
  GU_Pager(GigaDisplay_GFX *gfx, GestureDetector *gd) : GU_BasicPager(gfx, gd) { }
  ~GU_Pager() {  }

  // Set up a pager to go from 0 to n_pages-1 pages. Clear screen to
  // the fill color and display the given first page.

  // n_pages      Total number of pages.
  // first_page   The page to display first (0 to n_pages-1)
  // callback     Drag callback function to receive swipes.
  // param        User param to pass to callback.
  // fillcolor    Color to clear screens to (default 0)
  void initPager(int n_pages, int first_page,
                DragCB callback, void *param = NULL, uint16_t fillcolor = 0);

  // This clearPage overrides the basic clearPage to display the dots.
  void clearPage(bool indicator);

private:
  // Display the row of dots at bottom of screen with the current page highlighted.
  void displayDots(bool dots);

  // Points to the button overlaying the row of dots.
  GU_Button *_dots_button;
};

// Wrappers
void dotsCB(EventType ev, int indx, void *param, int x, int y);

// ---------------------------------------------------------------------------------

// The Sidebar class is similar to a Pager, but allows for the sidebars (pages other
// than the initial page) to be less than full width. There is typically only one
// sidebar in addition to the main page, but there can be many.
// The presence of a sidebar is indicated by a swipe indicator line.

// A button covers the rest of the underlying page, allowing the sidebar to
// be cancelled.
class GU_Sidebar : public GU_BasicPager
{
public:
  friend void cancelCB(EventType ev, int indx, void *param, int x, int y);
  GU_Sidebar(GigaDisplay_GFX *gfx, GestureDetector *gd) : GU_BasicPager(gfx, gd) { }
  ~GU_Sidebar() {  }

  // Set up a pager to go from 0 to n_pages-1 pages. Clear screen to
  // the fill color and display the given first page at full screen.

  // n_pages      Total number of pages.
  // first_page   The page to display first (0 to n_pages-1)
  // sidewidth    The width of the sidebars.
  // sidecolor    Color to fill sidebars.
  // sideborder   Color to outline sidebars. This is also the color
  //              used for the swipe indicator.
  // callback     Drag callback function to receive swipes.
  // param        User param to pass to callback.
  // fillcolor    Color to clear main page to (default 0)
  void initSidebar(int n_pages, int first_page,
                  uint16_t sidewidth, uint16_t sidecolor, uint16_t sideborder,
                  DragCB callback, void *param = NULL, uint16_t fillcolor = 0);

  // This clearPage overrides the basic clearPage to display the swipe indicator.
  void clearPage(bool indicator);

private:
  GU_Button *_cancel_button;
  int _main_page;
  uint16_t _sidewidth;
  uint16_t _sidecolor;
  uint16_t _sideborder;
};

void cancelCB(EventType ev, int indx, void *param, int x, int y);


// ---------------------------------------------------------------------------------

// Useful colour stuff not belonging to any class in particular

uint16_t rgb565_average(uint16_t color1, uint16_t color2);
void rgb565_unpack(uint16_t color, uint8_t *red, uint8_t *green, uint8_t *blue);
uint16_t rgb565_pack(uint8_t red, uint8_t green, uint8_t blue);

// Colours in RGB565.
#define RGB565_PACK(red, green, blue) ((red & 0xF8) << 8) | ((green & 0xFC) << 3) | (blue >> 3)

#define CYAN    (uint16_t)0x07FF
#define RED     (uint16_t)0xf800
#define BLUE    (uint16_t)0x001F
#define GREEN   (uint16_t)0x07E0
#define MAGENTA (uint16_t)0xF81F
#define WHITE   (uint16_t)0xffff
#define BLACK   (uint16_t)0x0000
#define YELLOW  (uint16_t)0xFFE0
#define GREY (uint16_t)RGB565_PACK(0x7F, 0x7F, 0x7F)
#define DKGREY (uint16_t)RGB565_PACK(0x3F, 0x3F, 0x3F)

#endif // def GU_ELEMENTS_H
