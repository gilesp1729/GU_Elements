#include "GU_Elements.h"

// Example program for UI elements library and Giga GFX.

// Uses libraries:
// GestureDetector for screen interaction
// GU_Elements for UI elements
// Arduino_GigaDisplay_GFX for screen display
// (and all their dependencies)

// Construct the graphics and gesture libs
GestureDetector detector;
GigaDisplay_GFX tft;

// Text and UI symbol fonts go into a font collection.
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

void Log(char *str, int x = 50, int y = 200)
{
  fc.drawText(str, x, y, WHITE);
  Serial.println(str);
}

// Refresh the screen and draw the buttons.
void refresh(void)
{
  tft.fillScreen(0);
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

  // Set up buttons. button 2 has no callback passed here, as the associated menu
  // will provide one internally.
  button1.initButtonUL(240, 5, 150, 45, BLACK, YELLOW, BLACK, "Button", tsize, tap_cb, 2, NULL);

  // Uncomment the second line to put the button at the bottom of the screen
  // so its menu goes up instead of down.
  button2.initButtonUL(480, 5, 150, 45, WHITE, DKGREY, WHITE, "Menu", tsize);
  //button2.initButtonUL(480, 430, 150, 45, WHITE, DKGREY, WHITE, "Menu", tsize);

  // Set up the menu and its items. Uncomment the second line for black text on white.
  menu.initMenu(&button2, WHITE, DKGREY, GREY, WHITE, menu_cb, 3, NULL);
  //menu.initMenu(&button2, WHITE, WHITE, GREY, BLACK, menu_cb, 3, NULL);
  
  menu.setMenuItem(0, items[0]);
  menu.setMenuItem(1, items[1], false);  // Disable this item
  menu.setMenuItem(2, items[2], true, true);  // Check mark this item

  // Clear the screen and draw the buttons
  refresh();
}

void loop() {

  detector.poll();

  delay(10);
}