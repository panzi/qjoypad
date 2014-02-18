QJoyPad 4 Documentation

Distributed with QJoyPad 4.0; available online at
http://qjoypad.sourceforge.net/doc/doc_index.html

John Toman

   virtuoussin13@users.sourceforge.net
   http://qjoypad.sourceforge.net
     _________________________________________________________

   Table of Contents
   1. Introduction

        1.1. What is QJoyPad?
        1.2. What's it good for?
        1.3. Features

   2. Getting Started

        2.1. Requirements
        2.2. Installation

   3. Using QJoyPad

        3.1. The Tray Icon
        3.2. The Popup Menu
        3.3. The Setup Dialog

              3.3.1. The Layout Selection combo box
              3.3.2. The Add button
              3.3.3. The Remove button
              3.3.4. The Update button
              3.3.5. The Revert button
              3.3.6. The Joystick buttons
              3.3.7. The Joystick Component buttons
              3.3.8. The Clear button
              3.3.9. Quick Set

        3.4. Configuring axes

              3.4.1. The Axis Position Indicator
              3.4.2. Making an axis "Gradient"
              3.4.3. Switching between keyboard and mouse control
              3.4.4. Adjusting mouse speed
              3.4.5. Setting keys
              3.4.6. Throttle Settings

        3.5. Configuring buttons

              3.5.1. Choosing a key / mouse button
              3.5.2. Making a button "Sticky"
              3.5.3. Using Rapid Fire

        3.6. Command-line use and scripting

   4. Layout Files
   5. Problems

        5.1. I can't get my game controller to work in Linux;
                will QJoyPad help?

        5.2. Joystick recognition

              5.2.1. QJoyPad says it can't find any joysticks?
              5.2.2. QJoyPad isn't showing all of my joysticks.
              5.2.3. My joystick has more/fewer buttons/axes than
                      that!

        5.3. Joystick adjustment

              5.3.1. Why does it say I'm moving if I'm not?
              5.3.2. I keep going in two directions at once
                      instead of just one!

              5.3.3. I'm pushing up, but nothing's happening!

        5.4. QJoyPad won't start!
        5.5. I have two versions of QJoyPad open at once and
                they're getting in each other's way!

        5.6. I'm getting strange errors when I change layouts;
                what's wrong?

        5.7. This program only works in XWindows?
        5.8. But my window manager doesn't HAVE a system tray!
        5.9. I hate the QJoyPad icon. Is there any way to change
                it?

        5.10. Why do I have to tell QJoyPad to "update joystick
                devices"? Why can't it do that on its own?

        5.11. When QJoyPad checks for new joysticks, it doesn't
                find mine!

        5.12. Why are both Up and Down treated as the same axis?
        5.13. All of this is too complicated. Why isn't there a
                button for Up?

        5.14. Features and suggestions

              5.14.1. Why can't I click with an axis, or move the
                      mouse with a button?

              5.14.2. Why doesn't QJoyPad do _____?

   6. Credits
   7. This software is GPL!
     _________________________________________________________

Chapter 1. Introduction

1.1. What is QJoyPad?

   QJoyPad  is  a  convenient  little program with a QT interface
   that  converts  movement  and  button  presses on a gamepad or
   joystick into key presses, mouse clicks, and mouse movement in
   XWindows. It should work on almost every Linux system and with
   any Linux-supported gaming device.
     _________________________________________________________

1.2. What's it good for?

   QJoyPad  lets  you play any XWindows game that uses input from
   the  keyboard  and  mouse  with a joystick device, even if the
   game doesn't normally have joystick support. In addition, it's
   a  convenient  companion for emulation software as it prevents
   the  need  for  extra  controller  plugins  and  can  remember
   multiple saved layouts. Also, QJoyPad can quickly swap between
   layouts  whenever  you change games, so you'll always have the
   controls right where you want them instead of compromising for
   the  game's  defaults  or the settings you find most useful in
   other  games.  Now  with  version  3,  QJoyPad  also  supports
   features  like  rapid  fire  and  sticky  buttons (see Section
   3.5.2) that can improve your gaming experience.

   Not  a  gamer?  Then QJoyPad can still be pretty useful if you
   would  find  it more comfortable or convenient to control your
   computer  with a joystick or game pad. It may be designed with
   gaming  in  mind,  but it's a useful program for virtually any
   purpose.
     _________________________________________________________

1.3. Features

     - Incorporates your gaming devices into any XWindows program
     - Move and click the mouse with your joystick
     - Auto-detects  how  many  joysticks  you  have and how many
       buttons and axes each supports
     - Can detect joystick devices on the fly without restarting
     - Support for devices with more than two axes
     - Save  as  many layouts as you want and switch between them
       quickly
     - Swap  layouts  on  the fly from the command line or from a
       script
     - Share  layout files with your friends or even edit them by
       hand for greater control
     - Color  cues quickly show you which buttons you're pressing
       and which joystick you're using
     - Set or reset all the keys at once in a flash
     - Adjust the sensitivity of every axis independently
     - Quietly   hides  in  your  system  tray,  running  in  the
       background without taking up space
     - For window managers without a system tray, QJoyPad can run
       without the tray icon.
     - Make an axis "Gradient" so that a light push does a little
       and a harder push does more
     - Support for throttle controls
     - Make  a  button "Sticky" if you don't feel like holding it
       down all the time
     - Turn on Rapid Fire so you don't wear out your gamepad!
     _________________________________________________________

Chapter 2. Getting Started

2.1. Requirements

     - A Linux computer and a Linux-compatible gaming device
     - A Linux kernel with joystick support (see the Linux Kernel
       HOWTO  [http://www.linuxdocs.org/HOWTOs/Kernel-HOWTO.html]
       and the Linux joystick driver website
       http://atrey.karlin.mff.cuni.cz/~vojtech/joystick/)
     - XWindows (see www.xfree86.org)
     - Trolltech's QT (see www.trolltech.com)
     _________________________________________________________

2.2. Installation

   Installing QJoyPad should be a quick and painless process. The
   basic procedure is:

   ./config
   make
   make install

   however,  there  are  some  settings  that  might  need  to be
   changed.

    1. Device  directory:  By  default,  QJoyPad  will  look  for
       joystick  devices  in  /dev/input, but if your system puts
       them  somewhere else, you'll need to run ./config with the
       argument  --devdir=DIR,  where  DIR is where your joystick
       devices reside. For instance, if your joystick devices are
       /dev/js0, /dev/js1, etc., instead of running ./config, run
       ./config --devdir="/dev"
    2. Install  directory:  By default, QJoyPad will try to put a
       copy   of   itself   in   /usr/local/bin,  some  icons  in
       /usr/local/share/pixmaps,     and     this    readme    in
       /usr/local/doc/qjoypad3 so that they will be accessible to
       all  users.  If you want these files to go somewhere other
       than  /usr/local (or if you don't have permission to write
       to   /usr/local),   you'll   need  to  pass  the  argument
       --prefix=DIR  to  ./config.  For example, if you wanted to
       install  QJoyPad just for yourself in your home directory,
       you  could  run  ./config --prefix="/home/user" instead of
       ./config.
    3. Use  Plain  Keys:  Normally,  QJoyPad doesn't use standard
       XWindows  key names to describe your keyboard, but instead
       uses  names  that  look nicer and are easier to recognize.
       For   instance,   instead   of  "KP_Begin",  "Prior",  and
       "Shift_L", QJoyPad uses "KP 5", "PageDown", and "L Shift".
       If  you  think  generating  these  names  is  a  waste  of
       processor  power, or if you don't think you're getting the
       right  name for the key you're pressing, pass the argument
       --plain_keys  to  ./config  and  QJoyPad will just use the
       XWindows default names.

   Of course, you can mix use as many of these options at once as
   you    like.    For    instance    ./config    --devdir="/dev"
   --prefix="/home/user" is completely valid.
     _________________________________________________________

Chapter 3. Using QJoyPad

3.1. The Tray Icon

   QJoyPad  3  is  centered  around  the idea of a "tray icon", a
   little icon that usually sits on your taskbar, out of the way;
   when  you  first  run QJoyPad, this is how it will be, just an
   icon.  If  your  window  manager  doesn't  support system tray
   icons, then you'll see QJoyPad as a tiny 24x24 window floating
   around  like  any  other window (see Section 5.8 ). since this
   might  be  hard  to  work with, QJoyPad (starting with version
   3.3)  gives  you  the  option  of having a larger icon to work
   with;  just run qjoypad --notray and QJoyPad will use a larger
   floating icon instead of a system tray icon.

   By  right clicking on the QJoyPad icon (it should look like an
   old  gamepad), you will get a pop-up menu that lets you switch
   layouts  (when  you  first  install  QJoyPad, there will be no
   layouts  available) and see some important information. To add
   or  modify  layouts,  left  click  the  icon to open the Setup
   Dialog.
     _________________________________________________________

3.2. The Popup Menu

   When  you  right click the QJoyPad icon, a menu should pop up.
   The  top  of  this  menu  shows you which joystick devices are
   currently  available  (see Section 5.2 if not all your devices
   are  listed).  Below that are the options to update the layout
   list or the joystick devices; use these if you have just put a
   new  layout  in  ~/.qjoypad3 by hand or if you've plugged in a
   new  joystick  device.  Below  even  farther  is a list of the
   available layouts and the option to quit.
     _________________________________________________________

3.3. The Setup Dialog

   The  following sections describe the parts of the Setup Dialog
   going from the top down and from left to right.
     _________________________________________________________

3.3.1. The Layout Selection combo box

   At  the  top  of the Setup Dialog is a combo box that says [NO
   LAYOUT]  to  begin  with,  but  as you add new layouts it will
   serve as a drop-down list of all the layouts available to you.
     _________________________________________________________

3.3.2. The Add button

   The Add button adds a new layout to the list, asking you for a
   meaningful  name.  Make  the  name  short, simple, and easy to
   remember,  just  in  case you ever want to load the new layout
   from  the  command  line.  You  can use any name you like that
   would  be a legal filename on your computer (see Chapter 4 for
   details.).
     _________________________________________________________

3.3.3. The Remove button

   The Remove button deletes the layout currently selected in the
   combo box, losing it forever.
     _________________________________________________________

3.3.4. The Update button

   The  Update button saves how the keys are currently set to the
   current layout. Use this to make changes to an already-defined
   layout.  Don't  forget  to  use Update to save any changes you
   make  before  quitting  or switching to another layout, or all
   the changes will be forgotten!
     _________________________________________________________

3.3.5. The Revert button

   The Revert button does about the opposite of Update. If you've
   gone  and changed the keys around, pressing Revert will return
   them to how they are saved in the current layout.
     _________________________________________________________

3.3.6. The Joystick buttons

   Immediately below the Add, Remove, Update, and Revert buttons,
   there are several buttons labeled Joystick 1, Joystick 2, etc.
   that  serve  as  tabs  so  you  can  switch  between different
   controllers  to  set  keys.  Whichever one of these buttons is
   pressed  is the controller you are currently editing. Pressing
   any  button or moving any axis on a given controller will make
   its associated button flash blue to let you know which it is.
     _________________________________________________________

3.3.7. The Joystick Component buttons

   Beneath  the  Joystick  Buttons  is  a  large  pile of buttons
   representing   every  axis  and  button  on  your  controller.
   Whenever  you  move  the  axis  or push the button that one of
   these  buttons represents, it will flash blue so that you know
   which  it  is.  To  setup  any  of  these,  just  click on the
   appropriate  button and a dialog will pop up to let you choose
   your settings.
     _________________________________________________________

3.3.8. The Clear button

   The  Clear  button resets all the axes and buttons to nothing,
   essentially  rendering  the  joystick disabled. From here it's
   easy enough to set the buttons you need, starting from a clean
   slate.
     _________________________________________________________

3.3.9. Quick Set

   The  Quick  Set button does exactly what you'd expect, it lets
   you  set  all the buttons and axes quickly! When you click it,
   it brings up a little window with a Done button, and until you
   click  Done it's watching the controller. Whenever you press a
   button  or move an axis, it will ask you which button you want
   associated  with  that  movement.  You can't set all the extra
   options  this way, but it's much faster than going through all
   the dialogs!
     _________________________________________________________

3.4. Configuring axes

   In  QJoyPad 2, you were allowed one key to be assigned to each
   of  four  directions, Up, Down, Left, and Right. In version 3,
   there  is  support  for  many axes and each one can do fancier
   things  than  just press a key. Unfortunately, since different
   controllers  do  things  differently,  it's not as easy as Up,
   Down,  Left,  and  Right. Up-Down is an axis, Left-Right is an
   axis,  and if you have a nicer controller, you might have many
   more axes on top of that.

   The  first step in configuring axes is to figure out which one
   you  want to set. If you have a joystick, try moving it around
   and  seeing  which  buttons  flash  blue  on the QJoyPad Setup
   Dialog.  If you have a gamepad, try pressing different buttons
   on  the  Directional-Pad  or  moving  around any mini joystick
   controls  it  might have. Once you know which axis you want to
   set,  click  on  its  associated  button  to open the Set Axis
   dialog.
     _________________________________________________________

3.4.1. The Axis Position Indicator

   In  the  middle  of  this  dialog,  you  will see a white bar,
   divided  in  two,  that represents the current position of the
   axis you're editing. Try moving that axis to see how it works.
   This  is  so you know which direction is considered "positive"
   and which is "negative"; it might not be what you'd expect. If
   this  axis  is  a D-Pad, then it is either off or on, but most
   other  axes  are sensitive to how far they are depressed and a
   colored bar here will show you how far it is at the moment.

   Along the white bar, you will also see small blue and red tabs
   that  you  can  drag.  These  adjust  the  "Dead Zone" and the
   "Extreme  Zone" of the axis. When the colored bar representing
   the  axis'  position  passes  one of the blue markers, the bar
   will  turn blue meaning that when the axis is this far QJoyPad
   will consider it moved, and when the bar passes one of the red
   markers  it  will turn red and QJoyPad will consider that axis
   fully  depressed.  When  the  bar is gray, that means that you
   haven't  moved  the  axis  out of its Dead Zone and QJoyPad is
   ignoring  its  movement.  To adjust where the Dead and Extreme
   Zones  are,  just  slide the blue and red markers to where you
   think they should be.

   You  probably  won't need to adjust the sensitivity unless you
   are  having  trouble  getting  QJoyPad to generate key presses
   when you want it to (see Section 5.3).
     _________________________________________________________

3.4.2. Making an axis "Gradient"

   On  the  upper  half  of  this dialog, you will see a checkbox
   marked  Gradient. Checking this box means that instead of just
   generating  one key press when the axis is moved, QJoyPad will
   start  flickering  that  key on and off as soon as the axis is
   out  of  the  Dead Zone (when the colored bar turns blue). How
   far the axis is pushed determines what percent of the time the
   simulated  key  will  be depressed. As soon as the axis enters
   its  Extreme  Zone  (when  the colored bar turns red), the key
   will  be  down  100%  of  the time. Making an axis Gradient is
   useful  if  you  want to use it as an accelerator in a game so
   how  fast  you  go is controlled by how far the axis is moved.
   Also,  it's  nice to use this when the axis is set to move the
   mouse  because it can give you finer control of the mouse when
   you push the axis just a little but still let you move quickly
   when you push it all the way.
     _________________________________________________________

3.4.3. Switching between keyboard and mouse control

   On  the  upper  half  of the dialog, there is a combo box that
   lets  you  choose  between keyboard control and mouse control.
   There  are  four  different  mouse options that let you choose
   whether the mouse will move vertically (Up-Down) when the axis
   moves  or  horizontally (Left-Right). You can also reverse the
   direction  of the mouse if you want moving the axis up to move
   the mouse down or visa versa.

   Tip

   Usually you want an axis to be Gradient if it's going to move
   the mouse.
     _________________________________________________________

3.4.4. Adjusting mouse speed

   When  using  one  of the mouse modes, you can set the speed of
   the mouse by adjusting the number in the upper right corner.
     _________________________________________________________

3.4.5. Setting keys

   When using keyboard mode, you can set which key corresponds to
   which   direction   of   the  axis  by  clicking  the  buttons
   immediately  below the Axis Position Indicator. The one on the
   left  will  be  pressed when the axis is moved in the negative
   direction  (when  the colored bar is on the left side) and the
   one  on  the  right when it is in the positive direction (when
   the colored bar is on the right side).
     _________________________________________________________

3.4.6. Throttle Settings

   Between  these  two  buttons is another combo box that changes
   the throttle settings. This is meant for gamepads which have a
   specific  type of throttle control. What it does is instead of
   having  two keys for when the axis is positive or negative, it
   has just one and treats the way the axis moves differently. In
   one  of  the  throttle  modes,  the  axis  will  be considered
   centered when it is all the way to one direction or the other.
     _________________________________________________________

3.5. Configuring buttons

   Similarly  to  the  buttons corresponding to axes in the Setup
   Dialog,   the  ones  corresponding  to  the  buttons  on  your
   controller  also  light  up to let you know which is which. To
   figure  out which button you want, press it on the game device
   and  then  click on the button on screen that flashed. A small
   settings dialog will pop up.
     _________________________________________________________

3.5.1. Choosing a key / mouse button

   At  the  top  of this dialog is a button that you can click to
   set  which key or mouse button you want to associate with this
   button  on  your  controller.  Just  click on it, and the rest
   should be self-explanatory.
     _________________________________________________________

3.5.2. Making a button "Sticky"

   Below this and to the left is a little checkbox marked Sticky.
   When  a  button is set as Sticky, that means that pressing the
   button  once  will make QJoyPad simulate a key press (or mouse
   button press) and pressing that button again will make QJoyPad
   release  the  simulated  key.  This is useful for racing games
   where  you're  almost  always  pouring on the gas, or for RPGs
   that  have  a  button  used  for  run, even though it's always
   better  to  be  running. This way, all you have to do is press
   the  button  once and it's like you're holding it down. To let
   the button back up, just press it a second time.
     _________________________________________________________

3.5.3. Using Rapid Fire

   Just  next  to the Sticky checkbox is another one marked Rapid
   Fire.  When  this is enabled for a button, holding that button
   down  means  that QJoyPad will flicker the associated key very
   fast.  This is great for space shooters where you want to fire
   quickly  but  you  don't  want  to  break your button (or your
   thumb!) from pressing over and over again.

   Tip

   Keep in mind that any button can be set both Sticky AND Rapid
   Fire. This is even better for space shooters because this way
   all you need to do is press the button once and from then
   until you press it again you will be shooting Rapid Fire.
     _________________________________________________________

3.6. Command-line use and scripting

   Although  QJoyPad only works in XWindows, it supports changing
   axes  on the fly from the command line. If you want to load up
   the layout named "Tetris", all you have to do is run:

   qjoypad "Tetris"

   and  one of two things will happen. If QJoyPad isn't currently
   open, it will start running and load the "Tetris" layout (this
   is  case  sensitive!  see  Chapter  4).  If QJoyPad is already
   running, it will just silently switch to the requested layout.

   What's so great about this is it lets you forget about QJoyPad
   once  you've  made all your layouts, and just worry about your
   games!  It's  very easy to write short little shell scripts to
   automatically  load  the  layout  you need when you start up a
   game.  For  instance, if you wanted to run the game xgalaga++,
   using  QJoyPad  for  joystick support, you could create a text
   file called run-xgalaga with the following lines in it:

   #!/bin/sh

   qjoypad "XGalaga" &
   xgalaga++

   Then  with  the command "chmod a+x run-xgalaga" you could make
   that  text  file an executable shell script; once that's done,
   all  you need to to do is execute run-xgalaga and QJoyPad will
   load  the  appropriate layout and your game will start. To use
   this  script for any other program, just change "XGalaga" to a
   different  layout  name  and  "xgalaga++"  to the name of some
   other program and you're done.
     _________________________________________________________

Chapter 4. Layout Files

   When  QJoyPad  saves  a  layout,  it creates a file using that
   layout's  name.  Because  of  this,  layout names must also be
   valid  filenames.  This  shouldn't  be  very limiting, it just
   means that names can't contain certain special characters such
   as  '/',  '*',  etc. Remember that most Linux file systems are
   case  sensitive, so a layout named "Layout" will be considered
   distinct  from  a  layout  named "layout". On most modern file
   systems,  spaces should be okay and there should be no serious
   limits on name length.

   Whenever  you  create  a  new layout, QJoyPad makes a new file
   called  Name.lyt  in  ~/.qjoypad3, where Name is the name that
   you  provided.  Whenever you update that layout, it overwrites
   that  file  to  reflect  your changes, whenever you revert, it
   rereads that file, and if you ever remove that layout, it will
   erase that file from your hard drive.

   The  format  of  these files isn't difficult to figure out, so
   you  can  edit  them  by hand if you like. The numbers used to
   represent keys are standard X11 keycodes.

   It's  also  easy to share QJoyPad layout files; just copy them
   from  one  user's  ~/.qjoypad3 directory to another and either
   tell  QJoyPad  to  update the layout list by right clicking on
   the  tray icon, or just restart QJoyPad. If you switch layouts
   through the command line, you don't even need to do that.
     _________________________________________________________

Chapter 5. Problems

5.1. I can't get my game controller to work in Linux; will QJoyPad
help?

   Well,  that  depends  on why you can't get it to work. For the
   most  part, the answer is "No." QJoyPad can only use joysticks
   and  gamepads that are recognized by your kernel and that have
   the  proper drivers loaded. If you can't get your joysticks to
   work at all in Linux, then, no, QJoyPad can't help. (you might
   want  to  check  out  the joystick.txt file included with your
   kernel  source;  if you don't know anything about working with
   the    kernel,    check    out    the   Linux   Kernel   HOWTO
   [http://www.linuxdocs.org/HOWTOs/Kernel-HOWTO.html] )

   If  your  joystick  is  detected and somewhat working, but you
   can't  get  it to work in specific programs, then QJoyPad just
   might  be  what  you're looking for. One of the main reasons I
   wrote  QJoyPad  was  because  my gamepads simply wouldn't work
   right  with the input plugins for Linux Playstation emulators,
   so  I  know  for a fact that sometimes QJoyPad can work around
   specific software issues.

   Check  out  Section  5.2  for  some  tips for checking if your
   joystick is working.
     _________________________________________________________

5.2. Joystick recognition

5.2.1. QJoyPad says it can't find any joysticks?

5.2.2. QJoyPad isn't showing all of my joysticks.

5.2.3. My joystick has more/fewer buttons/axes than that!

   QJoyPad  automatically  recognizes  your  joysticks  using the
   Linux  joystick  driver,  so  all  joysticks  must  be working
   properly  in  Linux before they can be used in QJoyPad. If you
   can't  see  all  of  your joysticks or if QJoyPad complains it
   can't  find  any, chances are your joystick(s) are not plugged
   in  or  are  not properly detected by Linux. If that's not it,
   QJoyPad  could also be looking for your joysticks in the wrong
   directory.

   First,  double  check  that  your joysticks are plugged in. If
   they  aren't,  plug them, load any modules you might need, and
   tell  QJoyPad  to  Update joystick devices with the popup menu
   (remember,  this menu is only accessible when the Setup Dialog
   is closed).

   If  you're  still  having  trouble,  QJoyPad  might  have been
   compiled  with the devdir setting pointing to the wrong place.
   That  option  had  to be set at compile time, and to change it
   you  must  recompile  (see Section 2.2); however, if you don't
   want to bother with that, you can specify the location of your
   devices  as  an  argument. Using the command "qjoypad --device
   /dev/input",  for  example,  will start QJoyPad and tell it to
   look for joysticks in /dev/input/js0, /dev/input/js1, etc.

   If  that  doesn't  work, then you might want to make sure your
   joysticks  are working properly. One way to test this is to do
   a  "cat  /dev/input/js0" (or wherever your joystick device is)
   and  press a few buttons on the controller. If you get a bunch
   of  crazy  characters,  it's  working.  If not, you'll have to
   fiddle  around  with  kernel drivers, and should probably look
   elsewhere for guidance.

   If  for  some  reason QJoyPad is reporting the wrong number of
   buttons or axes for your device, that means the Linux joystick
   driver  is  also  reporting the wrong number. Unless you can't
   get  to  buttons  or  axes  that you need, this shouldn't be a
   problem,  but if you want to get the number right, Try using a
   different  driver  or  check out the documentation for the one
   you're using.

   If  your  joysticks are working, plugged in, and my program is
   looking  in  the  right  place, then I'm not sure what to tell
   you. Unfortunately, I don't have a wealth of different devices
   and  software  setups to test on. If you're really stuck, drop
   me a line and I'll see what I can do.
   <wren42@users.sourceforge.net>
     _________________________________________________________

5.3. Joystick adjustment

5.3.1. Why does it say I'm moving if I'm not?

5.3.2. I keep going in two directions at once instead of just one!

5.3.3. I'm pushing up, but nothing's happening!

   Chances  are,  this  means you're using an overly sensitive or
   poorly  calibrated  joystick  or your sensitivity settings are
   all  wrong.  Try  adjusting the Dead Zone of the axes that are
   giving  you  trouble  (move  the  blue  tab  in  the Axis Edit
   dialog),  more  away  from  the  center  if  it  thinks you're
   pressing  a  button when you aren't, more toward the center if
   it  thinks  you aren't pressing a button when you are. If that
   doesn't  work, try manually adjusting your joystick (if it has
   adjustment knobs/sliders), or try calibrating it with jscal.
     _________________________________________________________

5.4. QJoyPad won't start!

   There  are  two  reasons  why  QJoyPad  won't  start. For one,
   QJoyPad  won't  start is if it's already running! To make sure
   QJoyPad  doesn't  interfere  with  itself, only one version of
   QJoyPad  is  allowed  to  run  at  a time. If you can't see an
   already open version, look for the icon in the system tray. If
   you  really  can't  find  it  anywhere,  try  running "killall
   qjoypad"  and  then  "rm  -f ~/lock.pid" and then try starting
   QJoyPad again. It should work this time.

   Finally, QJoyPad won't actually run if one of its arguments is
   --h or --help. When it sees one of those arguments, it outputs
   usage  information  to  the  console and then quits. If you're
   running  QJoyPad  away  from  a console or want it to run like
   normal, don't give one of these arguments.
     _________________________________________________________

5.5. I have two versions of QJoyPad open at once and they're getting
in each other's way!

   QJoyPad doesn't work well when there are two or more instances
   open;  for  that reason, it uses a file to tell whether or not
   it's  already running. Every version of QJoyPad has done this,
   but  in  version  3.4,  where that file is kept was changed to
   make the program more compatible with certain distributions.

   If  you're  seeing  two versions of QJoyPad open at once, that
   means that either one of those is QJoyPad 3.4 and the other is
   an  older  version, or that you're running an older version of
   QJoyPad  on  a  system  where  you  don't have write access to
   /var/run.  In  either case, you should just make sure that you
   are  running  the newest version of QJoyPad and that there are
   no other versions installed on your system.

   If you really want to keep earlier versions of QJoyPad, that's
   fine!  Just remember that if you do, it's possible to have two
   instancesrunning at once and that that can cause problems.
     _________________________________________________________

5.6. I'm getting strange errors when I change layouts; what's wrong?

   Those  errors  show  that  there  is  something wrong with the
   layout  files themselves. This means the files were corrupted,
   edited  incorrectly,  or  for  some reason QJoyPad didn't save
   them  right  (shouldn't ever happen, never seen it happen, but
   nothing's  impossible). Unless the file QJoyPad is looking for
   is  completely  missing or mangled, it's quite likely that the
   file  can  be repaired by hand. If you need help with the save
   format,  just  send me an email <wren42@users.sourceforge.net>
   and I'll see if I can't help.

   If  worse  comes  to  worst  and  you  lose  a layout file you
   created,  it  shouldn't  take  you too long to rebuild it from
   scratch.
     _________________________________________________________

5.7. This program only works in XWindows?

   Yep,  I'm  afraid  so. For all of you out there with old Linux
   console  games  that won't run in an xterm, you'll have to try
   something  else.  If  you really must find a way, joy2key is a
   program  that  is  similar  to QJoyPad but without a graphical
   interface  or  many of the fancier features, but which doesn't
   have that limitation. Check it out at:
   http://interreality.org/~tetron/technology/joy2key.
     _________________________________________________________

5.8. But my window manager doesn't HAVE a system tray!

   I'm  well  aware  that every Linux setup is different and that
   there  are a million different window managers that range from
   beautiful, feature-full, and bloated to stark, minimalist, and
   lightning-fast.  Unfortunately,  after  a few people suggested
   that  I  have a tray icon for the no-gui mode, I realized that
   it  was  a very, very good idea. The new version of QJoyPad is
   built  up  around  the system tray conceptually, and to make a
   version that doesn't use it would be a lot of work, so for now
   I plan to keep things as they are.

   However,  to  accommodate those of you who don't have a system
   tray  and  can't  stand  that  little icon, using the argument
   --notray  makes a floating icon that is much bigger instead of
   the  little tray icon. It still behaves exactly as the smaller
   icon  would, except it is larger and cannot be captured by the
   system tray.
     _________________________________________________________

5.9. I hate the QJoyPad icon. Is there any way to change it?

   Absolutely!  Starting  with  version  3.3,  QJoyPad stores its
   icons  in  $PATH/share/pixmaps/qjoypad  (where  $PATH  is  the
   install  path  used  by  given  at install time, /usr/local by
   default)  and  actually  comes  with  a few different icons to
   choose   from.   In  that  directory,  there  are  two  files,
   icon24.png  and  icon64.png  which correspond to the small and
   large icons that qjoypad will use. These files are just links,
   and  can be relinked to any png image of the appropriate size.
   Feel  free  to switch between the provided icons, change them,
   or make your own! QJoyPad should look how you want it to.
     _________________________________________________________

5.10. Why do I have to tell QJoyPad to "update joystick devices"?
Why can't it do that on its own?

   It  can!  With  the  hotplug  options of the more recent Linux
   kernels,  not  only is it easy to automatically load the right
   modules  for  a  joystick  when  it is plugged in, but also to
   notify  QJoyPad  and  have  it update its list. Unfortunately,
   this  is  a  little complex, still not available on everyone's
   computer,  and still in development. If you'd like to get this
   setup, I'd love to help you figure things out and perhaps I'll
   make  it  a  standard  feature of QJoyPad once I have a better
   idea  of  what  needs  to  be done on various systems. Please,
   either  contact  me  for  help  getting started, or send me an
   email after you get it working explaining how you did it ;) To
   make  QJoyPad  search for new devices, use the command qjoypad
   --update.
     _________________________________________________________

5.11. When QJoyPad checks for new joysticks, it doesn't find mine!

   When  you  plug  in  a joystick, there are certain things that
   have  to  happen  for  the  joystick  to  become  available to
   programs   like  QJoyPad.  Mainly,  the  joystick  has  to  be
   recognized and drivers have to be loaded. Even if this process
   is  automatic on your computer it could take a few seconds, so
   if  QJoyPad can't find your device right away, try again a few
   moments  later. If driver modules aren't loaded automatically,
   don't  forget  to load them before you ask QJoyPad to look for
   new devices. If you keep having troubles, see Section 5.2.
     _________________________________________________________

5.12. Why are both Up and Down treated as the same axis?

   That's because they are the same axis. An "axis" on a joystick
   (or  gamepad)  isn't  actually a direction, but a dimension. A
   standard  simple  joystick can move along two axes, the X-axis
   (side to side) and the Y-axis (up and down); when you move the
   stick  to  the  left, you're moving it along the X-axis in the
   negative  direction,  and when you move it to the right you're
   moving it in the positive direction along the same axis.

   What  really matters is that in QJoyPad, every axis represents
   two  opposing  directions and can therefore have two different
   keys.  I  only  do  it  that  way because thats how the device
   itself works. I'd make the labels a little more intuitive, but
   unfortunately  what  each  axis  corresponds  to  changes from
   controller  to  controller  and  there's no way for me to know
   which  is which. If you don't know which axis to set for which
   direction, move in the direction you want and see which button
   lights up, or try using Quick Set instead.
     _________________________________________________________

5.13. All of this is too complicated. Why isn't there a button for
Up?

   Unfortunately, adding new features means increasing complexity
   and  making  things more confusing. That's just how things go.
   If  you  just  want  to  have one key pressed when you press a
   button  on your joystick, try using just the quick set feature
   of  QJoyPad 3. There all you need to do is press what you want
   to  press  on the joystick and then type the key you want that
   button to trigger.

   Also,  if  you  preferred  the simplicity of QJoyPad 2.1, it's
   still  available  and  quite  functional, it just doesn't have
   quite  as many options and doesn't use a system tray icon. The
   two  versions of QJoyPad are compatible and can both be run on
   the same computer without getting in each others' way (as long
   as  you  rename  one  of  them  so  they  aren't  both  called
   "qjoypad"), just not at the same time.
     _________________________________________________________

5.14. Features and suggestions

5.14.1. Why can't I click with an axis, or move the mouse with a
     _________________________________________________________

5.14.2. Why doesn't QJoyPad do _____?

   For  the  sake  of  my sanity, I didn't program every possible
   thing  I  could  imagine someone wanting to do into QJoyPad. I
   added  in  the  features that people were asking for and which
   made  sense,  and  I set somewhat arbitrary limits on what the
   user can and can't do. Why set limits? Because if I didn't the
   program  would  get  far  too  bulky and too time consuming to
   write.  I  tried  to  draw  the  line  at  what  I thought was
   reasonable  use.  No,  you can't make the mouse click whenever
   you move an axis... but why would you want to?

   If  there's  something that you feel QJoyPad should be able to
   do that it can't, let me know and I'll considering adding that
   in future versions.
     _________________________________________________________

Chapter 6. Credits

   Thank  you  to  Erich  Kitzmüller,  author  of xjoypad for the
   inspiration  to write QJoyPad and for the code that started me
   off.

   The  development  team  for Psi, the Jabber client, also get a
   lot  of  thanks for writing the tray icon code that I borrowed
   and  tweaked.  Thank  you  for  developing GPL and for helping
   other    developers!    (Check    out    the    Psi    Website
   [http://psi.affinix.com/])

   Thank  you  also  to  everyone  who has sent me an email about
   QJoyPad. Knowing that my program is used and appreciated means
   a  lot,  especially  since  that's  about  all I get out of my
   programming. Open source is like teaching; it's very important
   and means a lot for young and developing programmers, but it's
   a time consuming and underpaid profession ;)

   Finally,  I  need  to  offer  a  very  warm  thank you to Mark
   Hannessen   who   graciously   donated  one  Logitech  Wingman
   Rumblepad  to  the  cause  of  QJoyPad. Without that, I simply
   would  not  have been able to add support for multiple axes or
   throttle  controls,  so  version 3 might have never been made.
   Thank you for your interest and for your support, Mark.
     _________________________________________________________

Chapter 7. Licensing
    
   This software is licensed under the terms of the GNU GPLv2.
   Please see LICENSE.txt for a full text of the license.

