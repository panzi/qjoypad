QJoyPad 2.1
-----------

Table of Contents:

   1) What is it?

   2) What is it good for?

   3) What's new in version 2.0?

   4) Installation
      4.1) Customization and configuration
         4.1.1) Device names
         4.1.2) Color

   5) Using QJoyPad (with a GUI)
      5.1) The combo box
      5.2) Add
      5.3) Remove
      5.4) Update
      5.5) Revert
      5.6) Saving
 
      5.7) Joystick buttons
      5.8) Sensativity slider
      5.9) Button buttons. Wait... BUTTON buttons?!
      5.10) Clear
      5.11) Set all keys

   6) Using QJoyPad without a GUI

   7) Changing Layouts from the command line and using QJoyPad in scripts

   8) Getting at your saved layouts

   9) History
 
   10) Problems
      10.1) I can't get my game controller to work in Linux; will QJoyPad help?
      10.2) QJoyPad says it can't find any joysticks?
      10.2) QJoyPad isn't showing all of my joysticks.
      10.2) My joystick has more/fewer keys than that!
      10.3) How does this program handle old analog devices?
      10.4) Why does it say I'm pushing up if I'm not?
      10.4) I keep going in two directions at once instead of just one!
      10.4) I'm pushing up, but nothing's happening!
      10.5) QJoyPad won't start and it doesn't give any error messages.
      10.5) QJoyPad says that there's already a running instance,
              but I don't see anything!
      10.6) I'm getting strange errors about loading layouts when I start
              QJoyPad; what's wrong?
      10.7) What about my Ueber Thrushter Meister Advanced Gaming Joystick (TM)?
      10.8) This program only works for Linux?
      10.8) Does this program work on older versions of Linux?
      10.9) This program only works in XWindows?
      10.10) Why write QJoyPad in QT?
 
   11) GNU General Public Licence
 
   Contact Information





1)     What is it?

	A handy little program with a QT interface that converts button presses on a gamepad or joystick into keypresses in XWindows. It should work with virtually any Linux-supported game device.





2)     Well, uh, what's that good for?

	QJoyPad lets you play any XWindows game that uses the keyboard with a gamepad, even if the game doesn't normally have joystick support. It also is a convenient companion for emulation software as it prevents the need for extra controller plugins and can remember multiple saved layouts. This lets you quickly swap layouts when you change games so you can have the keys just where you want them instead of compromising for the game's defaults or the setting you find most useful in OTHER games.





3)     What's new in version 2.0?

   - Layouts are handled very differently, giving you more flexibility and the
       ability to hand-edit or share your layout files.
   - The number of joysticks and the buttons each supports is autodetected at
       startup, removing clutter and simplifying installation.
   - The layout you last used is remembered and loaded the next time you start.
   - Two features to fascilitate use in scripts and from the command line:
       - QJoyPad can now be run without the graphical interface.
       - QJoyPad can be told which layout to load as an argument and can even
           swap layouts from the command line while running.





4)     Instalation

	You probably want to give a quick read through section 4.1 for a few options you can toy with before you compile, and I'd highly recommend reading the rest of this section, but if you just want to get going, the steps are easy:
	
	tar -xzvf qjoypad-?.?.tgz
	cd qjoypad-?.?
	./configure
	make
	#and if you're root, maybe:
	make install
	
	then run qjoypad to start the program
	
	Bear in mind that ./configure relies on qmake, so you must have qmake on your system. This shouldn't be a problem since it's part of the QT package upon which this program is based. If you don't have QT, you'll need to get it for QJoyPad to work.
	By default, 'make install' puts a copy of the program qjoypad into /usr/local/bin. If you want to put it elsewhere, change the first real line in qjoypad.pro so that target.path = /path/you/want before you run 'make' and 'make install'.
	If your system keeps its joystick devices in some place other than /dev/input/js?, you must make a slight change to "form.h" for QJoyPad to work. Plese read section 4.1.1 for details.	





4.1)     Customization and Configuration

	There are a few features of QJoyPad which you can adjust by editing some quick defines in the source code.
	
4.1.1)
	The most important setting, found in "form.h", is DEVICE; this is where QJoyPad will look for your joystick devices. On most Unix-based systems, joystick devices are listed as /dev/input/js0, /dev/input/js1, etc. If this is the case on your machine, you don't need to change anything, but if yours is any different you will need to replace "/dev/input/js" with the appropriate directory for your system. Don't include a number, QJoyPad adds that in for you.
	If you skip this step when you compile, you can still change the device by passing a '--device' argument to qjoypad. See section 10.2 for details.

4.1.2)
	If you're really into theming, or if you just can't stand blue, you can also change the color that the buttons flash by going into "flash.h" and changing the HIGHLIGHT setting to some other color. The color is defined by three numbers, each ranging from 0 to 255. The first is the red value, the second green, and the third blue. Feel free to change these around, just remember, to see any change you'll have to recompile.





5)     Using QJoyPad (with the Graphical User Interface)

	The QJoyPad window is broken into two parts, the top and the bottom:

	The top consists of a long combo box and four buttons labeled "Add", "Remove", "Update", and "Revert". This section handles the saving of key layouts.
	
5.1)
	The combo box is empty to begin with, but as you add new layouts it will serve as a dropdown list of all the layouts available to you.
	
5.2)
	The [Add] button adds a new layout to the list, asking you for a meaningful name. Make the name short, simple, and easy to remember, just in case you ever want to load the new layout from the command line.
	
5.3)
	The [Remove] button deletes the layout currently selected in the combo box, losing it forever. Be careful, you don't want to re-enter all those keys again, so don't remove something you want to save.
	
5.4)
	The [Update] button changes the current layout to reflect how the keys are set at that moment. Use this to make changes to an already-defined layout.
	
5.5)
	The [Revert] button does about the opposite of Update. If you've gone and changed the keys around, pressing Revert will return them to how they are saved in the current layout.

5.6)
	Every time you close QJoyPad, the layouts are saved, and every time you restart, they are loaded, so you don't need to worry about that. Just remember to put everything you want to save into layouts; the state of all the keys is NOT saved when you exit.
	The layout files are stored in the .qjoypad subdirectory of your home directory, which is hidden so as to not take up space. You can use these layout files with QJoyPad 2.0 on any computer, so remember where they are. Check out section 8 if you ever get the urge to edit your layouts by hand.
	

	The bottom half is where you actually set all the keys in a given layout. 

5.7)
	First, there are several buttons labled "Joystick 1", "Joystick 2", etc. that serve sort of as tabs so you can switch between different controllers to set keys. Whichever of these buttons is pressed is the controller you are currently editing. In general, there will be just one button for every joystick you have plugged in, but if you have, for instance, a joystick one and a joystick three, there will be a gray joystick button representing the missing joystick two.


5.8)
	The little slider right beneath these "JoyStick" buttons is used mainly for joysticks and old analog devices. With joysticks, there really is no absolutely centered state and there are always concerns with callibration, so QJoyPad uses this sensitivity setting to decide when to ignore a small wiggle and when to consider it an action on the directional pad. The further to the right the slider is, the less sensitive QJoyPad will be. Also, some old analog gamepads may need a relatively high sensitivity threshold to get a clear signal. If your joystick is "kinda" working, try adjusting the sensitivity; that should probably help. See section 10.4 for pointers.


5.9)
	Below the slider is a large pile of buttons representing every button on your controller. If you aren't sure which button is which (or which joystick is which, for that matter!) just push the button on the joystick and the corresponding button on the screen will light up for you. To change which key is associated with a button, just click the appropriate button on the screen and a little popup window will ask you to press a key on your keyboard. Just about any key will work, but a handful of special ones like Internet keys, Caps Lock, Scroll Lock, etc., will not. Escape is a special key. Pressing Escape means that you don't want that button on the controller to produce anything at all.
	If you absolutely must use one of the keys QJoyPad doesn't allow, section 8 explains how to do that by hand-editing your layout files.


	At the very bottom of the window, there are two more buttons, [Clear] and [Set all keys].

5.10)
	[Clear] resets every key on the current joystick to nothing, essentially rendering the joystick disabled. You can, of course, then go in and reset the keys if you wish.
	
5.11)
	[Set all keys] goes through asking you for each key, one by one, letting you setup your controller even faster.





6)     Using QJoyPad without the Graphical User Interface.

	What makes QJoyPad so useful is its convenient GUI that lets you quickly identify keys, design layouts, and swap between layouts, but once you have the layouts that you want ready, it might be more convenient not to have all of those buttons on the screen. For this reason, QJoyPad now has the option to run without any interface at all. It will still read your layout files, have the same features, and will even run faster, using fewer system resources, but there are a few things to know before you start.
	To use QJoyPad without a gui, you should probably run it from a console program. Since QJoyPad won't have any physical representation on the screen, it's best to use a console so that you can directly close the program when you're done. You don't have to do it that way, but I would recommend it.
	Starting QJoyPad without an interface is easy! Just run the command 'qjoypad --nogui'. If everything is working okay, you should have a little popup message letting you know that QJoyPad is now running in the background. From now on, QJoyPad is processing input from your joysticks.
	If you know which layout you want to use, try running 'qjoypad --nogui "layout name"' and QJoyPad will start using the layout named in double quotation marks. This also works when you have the gui enabled. Just running 'qjoypad "layout name"' will start up QJoyPad in its regular mode using the named layout.
	The only problem with using QJoyPad without a GUI, is closing it. There is no problem keeping QJoyPad running at all times, but it could be frustrating if you ever forget that it's running or how to stop it. If you are running QJoyPad from some sort of console, just typing Ctrl+C at that console should stop QJoyPad. If you didn't use a console or have lost access to the console, just running the command 'killall qjoypad' should do the trick.





7)     Changing layouts from the command line and using QJoyPad in scripts

	As easy as it is to start QJoyPad with a given layout, it's just as easy to swap layouts from the command line. Whether or not you're using the GUI, running the command 'qjoypad "new layout"' will tell any running instance of QJoyPad to swith to whatever layout is named in the double quotes.
	This is particularly useful for writing small shell scripts! If you wanted to use QJoyPad every time you played the game xgalaga++, for instance, you could use the following script:
	
	#!/bin/sh
	
	qjoypad "xgalaga++" &
	xgalaga++

	That script would start up QJoyPad (if it wasn't already running) and switch to the layout you use for xgalaga++. It would then go ahead and start xgalaga++, using QJoyPad to give you gamepad control. Using this kind of script lets you use QJoyPad in your favorite games easily and automatically.





8)     Getting at your saved layouts

	One of the new features of QJoyPad 2.0 is the new save format that lets you edit and share your layout files. These files are stored in your home directory, under the hidden subdirectory '.qjoypad'. There you will find a file called 'layout' which is used to remember which layout you used last, and several *.lyt files that are the layouts you have defined. These *.lyt files are portable and should work with QJoyPad 2.0 installed on any other computer, no matter what joysticks the other computer has.
	If you ever want to edit the layouts by hand instead of using the QJoyPad interface, the file format is relatively self explanitory. Every option is set in the form of 'Option name = value', and every joystick is denoted by a block of code beginning with 'Joystick X {' and ending with a '}'. If this isn't clear, just take a look at one of the files for yourself.
	Be careful when editing these files; you don't want to accidentally destroy one of your layouts. If you ever change a file so that QJoyPad can't understand it, QJoyPad will either fail to load the layout entirely or warn you that some options were not understood. This is your chance to go back and fix the file before QJoyPad overwrites it. Say that you don't want to load the layout, close QJoyPad, and try to figure out where the file is confusing it.
	The Sensitivity option should be set to a number between 0 and 32766, and each direction or button should be set to a valid keycode between 0 and 122. Setting keycodes manually is fine, but bear in mind that QJoyPad disallows some keys for a reason; the "Caps Lock" keycode will toggle the capslock state on your keyboard every time QJoyPad uses it, and some keycodes that QJoyPad could hypothetically make can't be produced by a normal keyboard. However, the "Escape" key is not allowed just so that you can set a button to nothing. If you really want QJoyPad to generate an "Escape" keypress, just set one of the keycodes to 9. If you need to find other keycodes, the program xev will show you the code for any key that you press. On some keyboards, you might be able to generate keys with values greater than 122 (Internet keys, foreign-language keys, etc), but few games will recognize these keys and they are often not well defined (the keycodes can vary from keyboard to keyboard). If you really need them, send me an email (wren42@users.sourceforge.net) and I can help you get them working.
	A layout can have more or fewer buttons or joysticks than your computer, and it doesn't necesarily have to have a setting for every button or every stick; it's a very flexible format. All in all, though, it's usually easier and safer to change these layouts using QJoyPad instead of by hand.





9)     History

	I've always been a fan of emulation, but I've found that emulation software is often very mediocre when it comes to user input. Many good emulators have hard-coded keys, so you're stuck with the controlls they give you, no matter how uncomfortable. Also, it's common consensus among officionados that a gamepad is essential for playing console or arcade style games, but some emulators don't fully support such gaming devices.
	When I bought two gamepads not long ago, I was relatively happy with them, but there were a few things that frustrated me. For a few games, I didn't have quite enough buttons. This wasn't a big problem, I could almost always get by with just the most important buttons being on the gamepad, but I had to reconfigure my emulator each time I wanted to rearange the buttons. What was worse, is that the common controller plugins for Linux Playstation emulators (I own a Playstation, so no copyright harassment, please ;) ) wouldn't even work right with my gamepads, so I was stuck with keyboard. Really, I was quite frustrated, and I began to doubt the value of my gamepad investment.
	I searched and searched the internet, and, amazingly, I happened to stumble upon a wonderful little program that solved all my problems: xjoypad by Erich Kitzmüller. It was a simple command-line program that converted gamepad input into keypresses, and it solved all of the problems I mentioned above! The only flaw it had was that it was such an incredible pain to configure. Although it was beautiful and lightweight, it had an awful interface that required me to look up keycodes and type them all in at the terminal every time I wanted anything but the standard layout, plus I needed a separate instance for each gamepad. Thanks to the miracle of GPL, though, this only inspired me.
	QJoyPad is the result of me adapting Erich Kitzmüller's code into a more convenient interface, using QT. Although the QT program is certainly bulkier than the original terminal program, it's still small and simple, plus it's much more of an ease to configure, can save and restore, and has a few handy extra features. I give full credit to Erich as my inspiration and the basis for my code, but every bit of this program is original and written by me except for sendevent.h and sendevent.cpp which are directly derived from his work.
	Hopefully this new incarnation of xjoypad's old idea will prove more useful and more accessible to Linux users; I will continue to improve upon it and refine it. Please feel free to send me suggestions. (wren42@users.sourceforge.net)





10)     Problems

10.1)
	I can't get my game controller to work in Linux; will QJoyPad help?
	
	Well, that depends on why you can't get it to work. For the most part, the answer is "No." QJoyPad can only use joysticks and gamepads that are recognized by your kernel and that have the proper drivers loaded. If you can't get your joysticks to work at all in Linux, then, no, QJoyPad can't help. (you might want to check out the joystick.txt file included with your kernel source; if you don't know anything about working with the kernel, check out the kernel HOWTO: http://www.linux.org/docs/ldp/howto/Kernel-HOWTO.html)
	If your joystick is detected and somewhat working, but you can't get it to work in specific programs, then QJoyPad just might be what you're looking for. One of the main reasons I wrote QJoyPad was because my gamepads simply wouldn't work with the input plugins for Linux Playstation emulators, so I know for a fact that sometimes QJoyPad can work around specific software issues.
	The next section, 10.2, has some tips for checking if your joystick is working.


10.2)
	QJoyPad says it can't find any joysticks?
	QJoyPad isn't showing all of my joysticks.
	My joystick has more/fewer keys than that!
	
	QJoyPad automatically recognizes your joysticks using the Linux joystick driver, so all joysticks must be working properly in Linux before they can be used in QJoyPad. If you can't see all of your joysticks or if QJoyPad complains it can't find any, chances are your joystick(s) are not pluged in or are not properly detected by Linux. If that's not it, QJoyPad could also be looking for your joysticks in the wrong directory.
	First, double check that your joysticks are plugged in. If they aren't, plug them, load any modules you might need, and restart QJoyPad.
	If you're still having trouble, QJoyPad might have been compiled with the DEVICE setting in "form.h" pointing to the wrong place. That option had to be set at compile time, and to change it you must recompile (see section 4.1.1); however, if you don't want to bother with that, you can specify the location of your devices as an argument. Using the command 'qjoypad --device /dev/input/js', for example, will start QJoyPad and tell it to look for joysticks in /dev/input/js0, /dev/input/js1, etc.
	If that doesn't work, then you might want to make sure your joysticks are working properly. One way to test this is to do a 'cat /dev/input/js0' (or wherever your joystick device is) and pressing a few buttons on the controller. If you get a bunch of crazy characters, it's working. If not, you'll have to fiddle around with kernel drivers, and should probably look elsewhere for guidance.
	If for some reason QJoyPad is reporting the wrong number of buttons for your device, that means the Linux joystick driver is also reporting the wrong number of buttons. Try using a different driver or check out the documentation for the one you're using, if you're having trouble.
	If your joysticks are working, plugged in, and my program IS looking in the right place, then I'm not sure what to tell you. Unfortunately, I don't have a wealth of different devices and software setups to test on. If you're really stuck, drop me a line and I'll see what I can do. (wren42@users.sourceforge.net)
	
	
10.3)
	How does this program handle old analog devices?
	
	Well, I haven't put it through extensive testing, but QJoyPad seems to do just fine with old devices on a game port as well as new ones on USB. Some devices might need some manual adjustment on the sensitivity, but that's why the slider switch is there.
	
	
10.4)
	Why does it say I'm pushing "Up" if I'm not?
	I keep going in two directions at once instead of just one!
	I'm pushing up, but nothing's happening!
	
	Chances are, this means you're using an overly sensitive or poorly calibrated joystick, that you're using an old analog device, or your sensativity settings are all wrong. Try adjusting the sensativity slider, more to the right if it thinks you're pressing a button when you aren't, more to the left if it thinks you aren't pressing a button when you are. If that doesn't work, try manually adjusting your joystick (if it has adjustment nobbs/sliders), or try calibrating it with jscal.
	

10.5)
	QJoyPad won't start and it doesn't give any error messages.
	QJoyPad says that there's already a running instance, but I don't see anything!
	
	Starting with version 2.0, QJoyPad can be run without any Graphical User Interface (see section 6). Chances are, it's already running somewhere in the background and that's why it's refusing to start up again. To fix this, you can either stop QJoyPad by running the command 'killall qjoypad', or you can just continue using the instance that you can't see. When QJoyPad is running without a GUI, it still works just fine and you can use almost all of its normal features, but if you want it stopped or need to see the buttons, kill it with 'killall qjoypad' and try again.


10.6)
	I'm getting strange errors about loading layouts when I start QJoyPad; what's wrong?
	
	Those errors show that there is something wrong with the layout files themselves. This means the files were corrupted, edited incorrectly, or for some reason QJoyPad didn't save them right (shouldn't ever happen, never seen it happen, but nothing's impossible). In most cases, QJoyPad will be able to load the layout anyway, though some options might be lost. If you don't want to lose that data (ie, you were making changes to the file by hand that you want to keep), tell QJoyPad NOT to load the layout, go back, and try to fix it. If QJoyPad can't load the layout at all, you can try to fix it by hand (see section 8).
	If worse comes to worst and you lose a layout file you created, it shouldn't take you too long to rebuild it from scratch.


10.7)
	What about my Ueber Thrushter Meister Advanced Gaming Joystick (TM)?

	I'm not a wealthy man. I write OpenSource, for crying out loud ;)  I only have access to a few joystick devices. This means that I can't guarantee that your controller will work, all I can say is that if Linux supports it, it SHOULD work. Also, I'm aware that there are some pretty amazing joysticks out there. Some have eight directions instead of four, some have multiple directional controls, throttle devices, and a whole set of other complicated add ons. My gaming devices are simple and small, so I have no familiarity with these features and I'm unlikely ever to add support for them. If you wish to discuss something important to add in or want to help me in doing so, let me know.
	
	
10.8)
	This program only works for Linux?
	Does this program work on older versions of Linux?
	
	That is the case, Linux only. Sorry, if you use Windows, there is no version of this program for you and there never will be; that's just the way it is. QJoyPad is based on the current Linux joystick api, which means it will only work on Linux and only on versions of Linux that have the same basic joystick support. I'm not sure how long the current joystick framework has been around, but it's probably been quite a while; still, I won't guarantee that if you have an ancient version of Linux that this program will work. If you have a really old system and are having problems, let me know and I might be able to help. Also, I know there are several Linux-like operating systems; if you run a system that is very much similar to linux but not quite the same, porting might be pretty easy. If you have a non-Linux system that can run XWindows and QT, then perhaps I can help you out, too. Just ask and I'll see what I can do.
	

10.9)
	This program only works in XWindows?
	
	Yep, I'm afraid so. For all of you out there with old Linux console games, you'll have to try something else. Unfortunately, the old xjoypad also only works with XWindows, but joy2key is a similar program that doesn't have that limitation. Check it out at: http://interreality.org/~tetron/technology/joy2key.
	
	
10.10)
	Why write QJoyPad in QT?
	
	Well, I'm familiar with QT and it's nearly universal :)  I understand that it's not as fast or as compact as some other tool kits, but it's found almost everywhere, it works well, and I haven't yet gotten into the depths of Motif, GTK, FLTK, or any of the miriad others. Give me time, and I will learn more ;)





11)     This software is GPL!

	Check out LICENSE.txt for details! The bare bones of it is, this program is distributed in open-source form so that others may share it freely, learn from it, or modify as they see fit. However, under no circumstances can it be sold!
	This code was written entirely by Nathan Gaylinn (excepting sendevent.cpp and sendevent.h which were taken directly from xjoypad) but is based on the idea of xjoypad by Erich Kitzmüller. Thank you Erich for developing GPL.
	Yay for the power GPL gives to developers!





QJoyPad         (qjoypad.sourceforge.net)
Nathan Gaylinn  (wren42@users.sourceforge.net)
