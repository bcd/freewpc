#!/usr/bin/env python

from Tkinter import *

class App (Tk):
	def __init__ (self):
		Tk.__init__ (self)
		self.mainmenu = Menu (self)
		self.config (menu=self.mainmenu)

	def new_top_menu (self, name, **args):
		menu = Menu (self.mainmenu)
		self.mainmenu.add_cascade (label=name, menu=menu, **args)
		return menu;

root = App ()

menu = root.new_top_menu ("File", underline=0)
menu.add_command (label="Open...", underline=0)
menu.add_command (label="Exit", command=root.quit, underline=1)

menu = root.new_top_menu ("Edit", underline=0)
menu.add_command (label="Preferences...", underline=0)

menu = root.new_top_menu ("Run", underline=0)
menu.add_command (label="Compile", underline=0)
menu.add_separator ()
menu.add_command (label="PinMAME", underline=0)
menu.add_command (label="PinMAME Test", underline=8)
menu.add_command (label="Visual Pinball", underline=0)

menu = root.new_top_menu ("Help")
menu.add_command (label="About...")

toolbar = Frame(root)
#b = Button (toolbar, text="Open", width=6)
#b.pack (side=LEFT, padx=2, pady=2)
#b = Button (toolbar, text="Save", width=6)
#b.pack (side=LEFT, padx=2, pady=2)
toolbar.pack (side=TOP, fill=X)

#w = Label (root, text="Hello, World!")
#w.pack ()

statusbar = Label (root, text="This is the status bar.", bd=1, relief=SUNKEN, anchor=W)
statusbar.pack (side=BOTTOM, fill=X)

root.title ("FreeWPC")
root.minsize (640, 480)
root.mainloop ()

