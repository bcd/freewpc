#!/usr/bin/env python

from Tkinter import *

class App (Tk):
	def __init__ (self):
		Tk.__init__ (self)
		self.mainmenu = Menu (self)
		self.config (menu=self.mainmenu)

	def new_top_menu (self, name):
		menu = Menu (self.mainmenu)
		self.mainmenu.add_cascade (label=name, menu=menu)
		return menu;

root = App ()

menu = root.new_top_menu ("File")
menu.add_command (label="New")
menu.add_command (label="Open...")
menu.add_command (label="Exit")

menu = root.new_top_menu ("Run")

menu = root.new_top_menu ("Help")
menu.add_command (label="About...")

toolbar = Frame(root)
b = Button (toolbar, text="Open", width=6)
b.pack (side=LEFT, padx=2, pady=2)
b = Button (toolbar, text="Save", width=6)
b.pack (side=LEFT, padx=2, pady=2)
toolbar.pack (side=TOP, fill=X)

w = Label (root, text="Hello, World!")
w.pack ()

statusbar = Label (root, text="This is the status bar.", bd=1, relief=SUNKEN, anchor=W)
statusbar.pack (side=BOTTOM, fill=X)

root.minsize (640, 480)
root.mainloop ()

