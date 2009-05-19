
import os
from Config import Config

def tagged (tag, text):
	return "<" + tag + ">" + text + "</" + tag + ">"

def view (cfg):
	title = cfg.categories['global'].items['Title'].getAttr ('value')
	print tagged('h1', title + ' Configuration')
	for catname in cfg.categories:
		print tagged ('h2', catname)
		cat = cfg.categories[catname]
		cat.print_all ()

config = Config ()
config.loadMD ("machine/tz/tz.md")
config.save ("tz.db")

#config = Config.load ("tz.db")

view (config)
