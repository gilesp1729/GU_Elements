#include "GU_Elements.h"

// Example program for UI elements library and Giga GFX.

// Uses libraries:
// GestureDetector for screen interaction
// GU_Elements for UI elements (included here for the moment)
// Arduino_GigaDisplay_GFX for screen display
// (and all their dependencies)

// Colours in RGB565.
#define CYAN    (uint16_t)0x07FF
#define RED     (uint16_t)0xf800
#define BLUE    (uint16_t)0x001F
#define GREEN   (uint16_t)0x07E0
#define MAGENTA (uint16_t)0xF81F
#define WHITE   (uint16_t)0xffff
#define BLACK   (uint16_t)0x0000
#define YELLOW  (uint16_t)0xFFE0
#define GREY (uint16_t)tft.color565(0x7F, 0x7F, 0x7F)
#define DKGREY (uint16_t)tft.color565(0x3F, 0x3F, 0x3F)

// Construct the graphics and gesture libs
GestureDetector detector;
GigaDisplay_GFX tft;

// Text and UI symbol fonts
#include <fonts/FreeSans18pt7b.h>
#include <fonts/UISymbolSans18pt7b.h>
FontCollection fc(&tft, &FreeSans18pt7b, &UISymbolSans18pt7b, 1, 1);

// Text size multiplier for buttons and menus
const int tsize = 1;

// A standalone button
GU_Button button1(&fc, &detector);

// A button with its associated menu
GU_Button button2(&fc, &detector);
GU_Menu menu(&fc, &detector);
char *items[3] = { "An item", "Another item", "A long item name" };

// A pager with 3 pages. The above buttons and menu are on Page 0.
GU_Pager pager(&tft, &detector);

void Log(char *str, int x = 50, int y = 200)
{
  fc.drawText(str, x, y, WHITE);
  Serial.println(str);
}

// Refresh the screen and redraw the buttons.
void refresh(void)
{
  pager.clearPage(true);
  button1.drawButton();
  button2.drawButton();
}

// callback is called when a button is pressed and released.
void tap_cb(EventType ev, int indx, void *param, int x, int y)
{
  if ((ev & EV_RELEASED) == 0)
    return;   // we only act on the releases
    
  refresh();
  if (ev & EV_LONG_PRESS)
    Log("Long pressed");
  else    
    Log("Tapped");
}

// Callback is called whenever a menu item is selected.
void menu_cb(EventType ev, int indx, void *param, int x, int y)
{
  refresh();
  if ((indx & 0xFF) == 0xFF)
    Log("No selection made");
  else
    Log(items[indx & 0xFF]);
}

// Pager show callback. Take down any UI on the page being left behind,
// and redraw any on the page being shown.
void pager_swipe_cb(EventType ev, int indx, void *param, int x, int y, int dx, int dy)
{
  int old_page = indx >> 8;
  int new_page = indx & 0xFF;

  // Do anything needed to take down an existing page being left behind. This is so that
  // the sensitive areas for buttons and menus are not left on the screen.
  switch (old_page)
  {
  case 0:
    button1.destroyButton();
    menu.destroyMenu();   // button2 will be destroyed with its menu
    break;
  case 1:
    break;
  case 2: 
    break;
  }

  // Construct the UI elements for the page being shown. Clear screen in any event.
  switch (new_page)
  {
  case 0:
    // Set up buttons. button 2 has no callback passed here, as the associated menu 
    // will provide one internally.
    button1.initButtonUL(240, 5, 150, 45, BLACK, YELLOW, BLACK, "Button", tsize, tap_cb, 2, NULL);
    button2.initButtonUL(480, 5, 150, 45, WHITE, DKGREY, WHITE, "Menu", tsize);

    // Set up the menu and its items.
    menu.initMenu(&button2, WHITE, DKGREY, GREY, WHITE, menu_cb, 3, NULL);  // white text on grey
    //menu.initMenu(&button2, WHITE, WHITE, GREY, BLACK, menu_cb, 3, NULL);   // black text on white
    menu.setMenuItem(0, items[0]);
    menu.setMenuItem(1, items[1], false);  // Disable this item
    menu.setMenuItem(2, items[2], true, true);  // Check mark this item

    // Draw buttons
    button1.drawButton();
    button2.drawButton();
    Log("Page 0", 50, 300);
    break;
  case 1:
  // The other pages have nothing on them at present.
    Log("Page 1", 50, 300);
    break;
  case 2: 
    Log("Page 2", 50, 300);
    break;
  }
}

void setup() 
{
  Serial.begin(9600);
  while(!Serial) {}

  tft.begin();
  if (detector.begin()) {
    Serial.println("Touch controller init - OK");
  } else {
    Serial.println("Touch controller init - FAILED");
    while(1) ;
  }

  // Set the rotation. These must occur together.
  tft.setRotation(1);
  detector.setRotation(1);

  // Init the pager to show Page 0 of 3 pages. The callback is responsible
  // for drawing the pages as they are shown and hidden by swiping.
  pager.initPager(3, 0, pager_swipe_cb, NULL, BLACK);
}

void loop() {

  detector.poll();

  delay(10);
}