QJoyPad 1.3
-----------

Table of Contents:

   1) What is it?

   2) What is it good for?

   3) Installation
      3.1) Customization and configuration
         3.1.1) Device names
         3.1.2) Joysticks and buttons
         3.1.3) Color

   4) Using qjoypad
      4.1) The combo box
      4.2) Add
      4.3) Remove
      4.4) Update
      4.5) Revert
		4.6) Saving
 
      4.7) Joystick buttons
		4.8) Sensativity slider
      4.9) Button buttons. Wait... BUTTON buttons?!
      4.10) Clear
      4.11) Set all keys
 
   5) History
 
   6) Problems
      6.1) How come my joystick is disabled?
      6.1) How come all of the buttons are grayed out?
      6.2) I'm getting this strange error about different versions, 
           save files, and data loss, what's wrong?
      6.3) How does this program handle old analog devices?
		6.4) Why does it say I'm pushing up if I'm not?
		     I keep going in two directions at once instead of just one!
			  I'm pushing up, but nothing's happening!
		6.5) What about my Ueber Thrushter Meister Advanced Gaming Joystick (TM)?
      6.6) This program only works in XWindows?
      6.7) Why write QJoyPad in QT?
 
   7) GNU General Public Licence
 
   Contact Information





1)     What is it?

	A handy little program with a QT interface that converts button presses on a gamepad or joystick into keypresses in XWindows. It seems to also work for analog controllers, but this has not been extensively tested.





2)     Well, uh, what's that good for?

	QJoyPad lets you play any XWindows game that uses the keyboard with a gamepad, even if the game doesn't normally have joystick support. It also is a convenient companion for emulation software as it prevents the need for extra controller plugins and can remember multiple saved layouts. This lets you quickly swap layouts when you change games so you can have the keys just where you want them instead of compromising for the game's defaults or the setting you find most useful in OTHER games.





3)      Instalation

	You probably want to give a quick read through section 3.1 for a few options you can toy with before you compile, and I'd highly recommend reading the rest of this section, but if you just want to get going, the steps are easy:
	
	tar -xzvf qjoypad-?.?.tgz
	cd qjoypad-?.?
	./configure
	make
	#and if you're root, maybe:
	make install
	
	then run qjoypad to start the program
	
	Bear in mind that ./configure relies on qmake, so you must have qmake on your system. This shouldn't be a problem since it's part of the QT package upon which this program is based. If you don't have QT, you'll need to get it for QJoyPad to work.
	By default, "make install" puts a copy of the program qjoypad into /usr/local/bin. If you want to put it elsewhere, change the first real line in qjoypad.pro so that target.path = /path/you/want before you do the above steps.
	If your system keeps its joystick devices in some place other than /dev/input/js?, you must make a slight change to form.h for QJoyPad to work. Plese read section 3.1.1 for details.
	If you have more than four joysticks/gamepads on your computer, have a controller with more than ten buttons (not counting one directional pad), or if you suspect that in the future you will have either of these things, please read section 3.1.2 for special instructions.
	




3.1)     Customization and Configuration

	There are a few features of QJoyPad which you can adjust by editing a few quick defines in "form.h".
	
3.1.1)
	The most important setting is DEVICE; this is where QJoyPad will look for your joystick devices. On most Unix-based systems, joystick devices are listed as /dev/input/js0, or /dev/input/js1, etc. If this is the case on your machine, you don't need to change the DEVICE setting, but if yours is any different you will need to replace "/dev/input/js" with the appropriate directory for your system. Don't include a number, QJoyPad adds that in for you.

3.1.2)
	The other settings are mainly for ascetic purposes, unless you have a lot of big complicated joysticks. For most people, four joysticks with at most ten buttons each will be plenty, but if you have more than four joysticks or if any of your joysticks has more than ten buttons, change the BUTTONS and JOYSTICKS setting appropriately. Also, if you're like me and you know you'll never be using more than two joysticks, or if your controllers just don't have ten buttons, feel free to lower these numbers to fit your needs. The lower these are set, the less crouded the QJoyPad window will be, but if you ever need to add more buttons or more controllers, you'll have to go back in, change these settings, and recompile, so be sure to set these reasonably! Hopefully, you'll only need to compile once. Just as a warning, though, if you do decide to go back and change these values later, after you recompile your saved layouts might get a little confused. Just double check that they are like you remember before using them.
	
3.1.3)
	If you're really into theming, or if you just can't stand blue, you can also change the color that the buttons' flash by going into "flash.h" and changing the HIGHLIGHT setting to some other color. The color is defined by three numbers, each ranging from 0 to 255. The first is the red value, the second green, and the third blue. Feel free to change these around, just remember, to see any change you'll have to recompile.





4)     How it works

	The QJoyPad window is broken into two parts, the top and the bottom:

	The top consists of a long combo box and four buttons labeled "Add", "Remove", "Update", and "Revert". This section handles the saving of key layouts.
	
4.1)
	The combo box is empty to begin with, but as you add new layouts it will serve as a dropdown list of all the layouts available to you.
	
4.2)
	The [Add] button adds a new layout to the list, asking you for a meaningful name.
	
4.3)
	The [Remove] button deletes the layout currently selected in the combo box, losing it forever. Be careful, you don't want to re-enter all those keys again, so don't remove something you want to save.
	
4.4)
	The [Update] button changes the current layout to reflect how the keys are set at that moment. Use this to make changes to a predefined layout.
	
4.5)
	The [Revert] button does about the opposite of Update. If you've gone and changed the keys around, pressing Revert will return them to how they are saved in the current layout.

4.6)
	Every time you close QJoyPad, the layouts are saved, and every time you restart, they are loaded, so you don't need to worry about that. Just remember to put everything you want to save into layouts; the state of all the keys is NOT saved when you exit.
	

	The bottom half is where you actually set all the keys in a given layout. 

4.7)
	First there are several buttons labled "Joystick 1", "Joystick 2", etc. that serve sort of as tabs so you can switch between different controllers to set keys. Whichever of these buttons is pressed is the controller you are currently editing. If one or more of these joystick buttons is disabled, that means the program is having a problem connection with that joystick; chances are, it's not plugged in. If you want to plug in another joystick, you'll have to restart the program after you do. (See 6.1 if you still have trouble)


4.8)
	The little slider right beneath these four joystick buttons is used mainly for joysticks and old analog devices. With joysticks, there really is no absolutely centered state and there are always concerns with callibration, so QJoyPad uses this sensitivity setting to decide when to ignore a small wiggle and when to consider it an action on the directional pad. The further to the right the slider is, the less sensitive QJoyPad will be. Also, some old analog gamepads may need a relatively high sensitivity threshold to get a clear signal. If your joystick is "kinda" working, try adjusting the sensitivity. See section 6.4 for pointers.


4.9)
	Below the slider is a large pile of buttons representing every button on your controller. If you aren't sure which button is which (or which joystick is which, for that matter!) just push the button on the joystick and the corresponding button on the screen will light up for you. To change which key is associated with a button, just click the appropriate button on the screen and a little popup window will ask you to press a key on your keyboard. Just about any key will work, but a handful of special ones like the Windows key, or Caps Lock, Scroll Lock, etc., will not. Escape is a special key. Pressing Escape means that you don't want that button on the controller to produce anything at all.

	At the very bottom of the window, there are two more buttons, [Clear] and [Set all keys].

4.10)
	[Clear] resets every key on the current joystick to nothing, essentially rendering the joystick disabled. You can, of course, then go in and reset the keys if you wish.
	
4.11)
	[Set all keys] goes through asking you for each key, one by one, letting you setup your controller even faster.





5)     History

	I've always been a fan of emulation, but I've found that emulation software is often very mediocre when it comes to user input. Many good emulators have hard-coded keys, so you're stuck with what they give you, no matter how uncomfortable. Also, it's common consensus among officionados that a gamepad is essential for playing console or arcade style games, but some emulators don't fully support such gaming devices.
	When I bought two gamepads not long ago, I was relatively happy with them, but there were a few things that frustrated me. For a few games, I didn't have quite enough buttons. This wasn't a big problem, I could almost always get by with just the most important buttons being on the gamepad, but I had to reconfigure my emulator each time I wanted to rearange the buttons. What was worse, is that the common controller plugins for Linux Playstation emulators (I own a playstation, so no copyright harassment, please ;) ) wouldn't even work right with my gamepads, so I was stuck with keyboard. Really, I was quite frustrated, and I began to doubt the value of my gamepad investment.
	I searched and searched the internet, and amazingly I happened to stumble upon a wonderful little program that solved all my problems: xjoypad by Erich Kitzmüller. It was a simple command-line program that converted gamepad input into keypresses, and it solved all of the problems I mentioned above! The only flaw it had was that it was such an incredible pain to configure. Although it was beautiful and lightweight, it had an awful interface that required me to look up keycodes and type them all in at the terminal every time I wanted anything but the standard layout, plus I needed a separate instance for each gamepad. Thanks to the miracle of GPL, though, this only inspired me.
	QJoyPad is the result of me adapting Erich Kitzmüller's beautiful code into a more convenient interface, using QT. Although the QT program is certainly bulkier than the original terminal program, it's still very small and very simple, and this version is an ease to configure, can save and restore, and has a few handy extra features. I give full credit to Erich as my inspiration and the basis for my code, but every bit of this program is original and written by me except for sendevent.h and sendevent.cpp which are directly derived from his work.
	Hopefully this new incarnation of xjoypad will prove more useful and more accessible to Linux users; I will continue to improve upon it and refine it. Please feel free to send me suggestions. (wren42@users.sourceforge.net)





6)     Problems

6.1)
	How come my joystick is disabled?
	How come all of the buttons are grayed out?
	
	When one of the joystick buttons is disabled and you can't click on it, that means that QJoyPad had a problem connecting to that joystick. Chances are, that means that your joystick(s) are not pluged in or are not properly detected by your operating system, or it means that QJoyPad was compiled with the wrong settings, looking for your joysticks in the wrong place.
	First, double check that your joysticks are plugged in. If they aren't, plug them in and restart the program.
	If you're still having trouble, double check that the DEVICE setting in "form.h" is pointing at your joystick devices. If not, change DEVICE, recompile, and try again.
	If that doesn't work, then you might want to make sure your joysticks are working properly. One way to test this is to do a "cat /dev/input/js0" (or wherever your joystick device is) and pressing a few keys on the controller. If you get a bunch of crazy characters, it's working. If not, you'll have to fiddle around with kernel drivers, and should probably look elsewhere for guidance.
	If your joysticks are working, plugged in, and my program IS looking in the right place, then I'm not sure what to tell you. Unfortunately, I don't have a wealth of different devices and software setups to test on. If you're really stuck, drop me a line and I'll see what I can do. (wren42@users.sourceforge.net)
	
	
6.2)
	I'm getting this strange error about different versions, save files, and data loss; what's wrong?
	
	This means that the last time QJoyPad was run, it was configured differntly to support a different number of joysticks or buttons. This isn't a big problem, but I can't guarantee that the layouts will be preserved. I would suggest either deleting .qjoypadrc from your home directory and starting over with no layouts at all, or going through all the layouts and double checking that there's nothing wrong with them; they might be just fine. You won't have to deal with this issue unless you recompiled QJoyPad.
	

6.3)
	How does this program handle old analog devices?
	
	Well, I haven't put it through extensive testing, but QJoyPad seems to do just fine with old devices on a game port as well as USB. Some devices might need some manual adjustment on the sensitivity, but that's why the slider switch is there.
	
	
6.4)
	Why does it say I'm pushing "Up" if I'm not?
	I keep going in two directions at once instead of just one!
	I'm pushing up, but nothing's happening!
	
	Chances are, this means you're using an overly sensitive or poorly calibrated joystick, that you're using an old analog device, or your sensativity settings are all wrong. Try adjusting the sensativity slider, more to the right if it thinks you're pressing a button when you aren't, more to the left if it thinks you aren't pressing a button but you are. If that doesn't work, try manually adjusting your joystick (if it has adjustment nobbs/sliders), or try calibrating it with jscal.
	

6.5)
	What about my Ueber Thrushter Meister Advanced Gaming Joystick (TM)?

	I'm not a wealthy man. I write OpenSource, for crying out loud ;)  I only have access to a few joystick devices. This means that I can't guarantee that your controller will work, all I can say is that linux supports it, it SHOULD work. Also, I'm aware that there are some pretty amazing joysticks out there. Some have eight directions instead of four, some have multiple directional controls, throttle devices, and a whole set of other complicated add ons. My gaming devices are simple and small, so I have no familiarity with these features and I'm unlikely ever to add support for them. If you wish to discuss something important to add in or help me in doing so, let me know.
	
	
6.6)
	This program only works in XWindows?
	
	Yep, I'm afraid so. For all of you out there with old Linux console games, you'll have to try something else. Unfortunately, the old xjoypad also only works with XWindows, but joy2key is a similar program that doesn't have that limitation. Check it out at: http://interreality.org/~tetron/technology/joy2key.
	
	
6.7)
	Why write QJoyPad in QT?
	
	Well, I'm familiar with QT and it's nearly universal :)  I understand that it's not as fast or as compact as some other tool kits, but it's found almost everywhere, it works well, and I haven't yet gotten into the depths of Motif, GTK, FLTK, or any of the miriad others. Give me time, and I will learn more ;)





7)     This software is GPL!

	Check out LICENSE.txt for details! The bare bones of it is, this program is distributed in open-source form so that others may share it freely, learn from it, or modify as they see fit. However, under no circumstances can it be sold!
	This code was written entirely by Nathan Gaylinn (excepting sendevent.cpp and sendevent.h which were taken directly from xjoypad) but is based on the idea of xjoypad by Erich Kitzmüller. Thank you Erich for developing GPL.
	Yay for the power GPL gives to developers!





Nathan Gaylinn (wren42@users.sourceforge.net)
