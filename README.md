# UI library for the Arduino Giga R1 and Giga Display.

Classes providing UI elements (buttons, menus, etc) for the Arduino Giga Display.
Built on top of GestureDetector, it provides drop-down menus and buttons, as well
as multiple pages accessible by swiping left or right containing any user content.
The callbacks are those provided in GestureDetector.

The caller is notified of:
- button presses via tap callbacks
- menu selections via tap callbacks
- page changes via swipe callbacks

# Buttons and drop-down menus
Menus may be activated by tapping an assocated button, or by dragging down from the button.
Highlighting and dragging is handled internally until the desired selection is released
or the selection cancelled by dragging or tapping outside the menu area.

The button and menu item strings may contain symbols as well as ascii text. They use the
symbol fonts provided in the FontCollection library.

# Pager
Multiple pages can be accessed by swiping left and right, or by tapping one of the dots
in the row at the bottom of the screen. A swipe callback is given to the caller telling it 
the page being left, and the page being displayed. The caller must write code to:
- clear and remove sensitive aeas from the page being left, and
- draw content and UI elements on the page being displayed.

# Sidebar
This is like the pager but only the initial page is full-screen. Subsequent page(s) may
be narrower and are used for a slide-out sidebar. A swipe indicator line is displayed on the
side having a sidebar available.

Dependencies:
- Gesture detector library (gilesp1729/GestureDetector)
- Font collection library (gilesp1729/FontCollection)
- Giga GFX library
- Giga touch library

Works in progress:
- keyboard
- progress bars and sliders
