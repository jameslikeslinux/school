Author: James Lee
Email:  jameslee@gwu.edu
Class:  CSCI 6907
Date:   20120220
Lab:    1

Description: This lab displays strings of ASCII characters on the LED array
built into the ZNEO dev board.  Strings longer than the number of LEDs will
scroll either one LED at a time or one column at a time as chosen at
runtime.  A timer generates an interrupt every millisecond that drives the
entire operation of the program through a simple event-driven callback
model.  Each LED is represented by an object that registers itself with the
timer to draw a single row every two milliseconds.  The LEDs are tied
together by the ledarray class, which maps strings of characters to
individual LEDs and defines the behavior of inter-LED activity such as
scrolling, which is also driven by callbacks registered with the timer.
A button class is defined to capture single and double presses, driven by
the timer interrupt.  The button class is instatiated three times in the
main function for each of the three hardware buttons on the ZNEO dev board.
The main function registers callbacks with each button event to display
different strings on the LED array.

Other files: button.{c,h} led.{c,h} ledarray.{c,h} matrix.h timer.{c,h}
Problems:    none

Enhancements: I implemented "smooth scrolling" to shift characters from
right to left one column at a time.  This can be turned on and off at
runtime.  I purposely let characters appear to scroll through the empty
space between the LED panels which gives it a smoother look.  Double press
button 3 to see the effect at slower speed.  I also took care to ensure
the string could be updated even if it is shifted halfway between LED
panels.  Double press button 1 (or button 2 if you wait 255 seconds) to see
that effect.

Questions:
1. Did you order the display characters by column or row?  Does it matter?

I ordered scanned the pixels across the LED array by rows.  I did it that
way because the supplied matrix.h was populated with character data in rows.
It probably doesn't matter either way, except that you could extract more
efficiency by scanning column-wise, as a column could still fit in the byte
sized character data and there are fewer columns than rows.

2. Did you find that yoru display blinked or flickered, or that the
characters seemed dim, or that one row or column of dots was brighter than
the rest?  If you solved thath, what was the problem?  If not, what do you
think the problem is?

When I was first developing the lab, I did not know about timers, so I
scanned the LEDs in a tight loop.  This gave each individual LED a
persistent, uniform look and brightness.  When I changed the program to be
timer driven, it took some fine tuning to find the correct refresh interval
the LEDs didn't flicker.  I found that about 2 ms per row is just about
right.

3. Briefly describe how did you de-bounce the switches?

Basically, I ignore anything that happens for 20 ms after a valid button
event.  So when I first push a button, I register it as valid and don't
bother checking anything else about it for 20 ms.  Then when I release the
button, again, I ignore everything for 20 ms.  I've found this to be
highly effective at eliminating button bounces without affecting
interactivity.
