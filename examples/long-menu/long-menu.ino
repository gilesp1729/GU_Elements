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

// A button with its associated menu. This menu is longer than will
// fit on the screen, to demonstrate menu scrolling when dragged through.
GU_Button button2(&fc, &detector);
GU_Menu menu(&fc, &detector);
char *items[10] = { "An item", "Another item", "A long item name",
                  "Item 3", "Item 4", "Item 5", "Item 6", "Item 7", "Item 8", "Item 9" };

void Log(char *str, int x = 50, int y = 200)
{
  fc.drawText(str, x, y, WHITE);
  Serial.println(str);
}

// Refresh the screen and draw the buttons.
void refresh(void)
{
  tft.fillScreen(0);
  button2.drawButton();
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
  // will provide one internally. Uncomment the second line to put the button
  // at the bottom.
  button2.initButtonUL(480, 5, 200, 45, WHITE, DKGREY, WHITE, "Long Menu", tsize);
  //button2.initButtonUL(480, 430, 200, 45, WHITE, DKGREY, WHITE, "Long Menu", tsize);

  // Set up the menu and its items.
  menu.initMenu(&button2, WHITE, DKGREY, GREY, WHITE, menu_cb, 3, NULL);
  //menu.initMenu(&button2, WHITE, WHITE, GREY, BLACK, menu_cb, 3, NULL);   // black text on white
  menu.setMenuItem(0, items[0]);
  menu.setMenuItem(1, items[1], false);  // Disable this item
  menu.setMenuItem(2, items[2], true, true, true);  // Check mark and underline this item
  menu.setMenuItem(3, items[3]);
  menu.setMenuItem(4, items[4]);
  menu.setMenuItem(5, items[5]);
  menu.setMenuItem(6, items[6]);
  menu.setMenuItem(7, items[7]);
  menu.setMenuItem(8, items[8]);
  menu.setMenuItem(9, items[9]);

  // A help tip
  menu.setTip("This is a long (scrolling) menu");

  // Clear the screen and draw the buttons
  refresh();
}

void loop() {

  detector.poll();

  delay(10);
}