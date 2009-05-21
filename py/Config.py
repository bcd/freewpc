#!/usr/bin/env python

import re
import cPickle as pickle
import os

Root = os.environ["HOME"] + '/src/git/freewpc'

def tagged (tag, text):
	return "<" + tag + ">" + text + "</" + tag + ">"

def smaller (text):
	return "<font size=-1>" + text + "</font>"

#---------------------------------------------------------------------
#
# Item - the parent class for an object instance
#
#---------------------------------------------------------------------
class Item:
	def __init__ (self, category, name, level=0):
		self.category = category
		self.attrs = {}
		# TODO - level should be per-attribute
		self.level = level
		self.setName (name)

	def __cmp__ (self, other):
		return cmp (self.name, other.name)

	def setAttrValue (self, attr, value):
		"""A low-level set of an item attribute.
		This should not be overriden by subclasses."""
		self.attrs[attr] = value

		globalID = self.category.getGlobalID (attr)
		if globalID:
			id = self.category.getID (self.name)
			self.category.config.add ('implicit-defines', globalID, [ id ], 0)

		if self.category.testBooleanAttr (attr) and value is True:
			inherits = self.category.getInheritedBooleanAttrs (attr)
			if inherits:
				for parent in inherits:
					self.setAttrValue (parent, inherits[parent])

	def setAttr (self, attr, value):
		"""Set an item's attribute value.  The category may provide an override."""

		attr = re.sub ('^ *', '', attr)
		attr = re.sub (' *$', '', attr)
		if self.category.testBooleanAttr (attr):
			self.setAttrValue (attr, True)
		elif not self.category.setAttr (self, attr, value):
			self.setAttrValue (attr, value)

	def getAttr (self, attr):
		"""Return the value of an attribute, or None if it is not set"""
		return self.attrs[attr]

	def getAttrDefault (self, attr):
		if self.category.testBooleanAttr (attr):
			return False
		return self.category.getAttrDefault (self, attr)

	def setName (self, name, level=0):
		self.name = name
		self.attrs['name'] = name
		if 'notinstalled' in self.attrs:
			del self.attrs['notinstalled']

	def __str__ (self):
		return "(" + self.name + ":" + self.attrs.__str__ () + ")"

#---------------------------------------------------------------------
#
# Category - the parent class of all object categories
#
#---------------------------------------------------------------------
class Category:
	static_flag = True
	prefix = None
	suffix = None

	def __init__ (self, name, config):
		self.name = name
		self.items = {}
		self.config = config

	def setAttr (self, item, attr, value):
		return False

	def setAttrOnce (self, item, attr, value):
		"""Set an attribute value only if it is undefined.
		Returns True if the attribute was changed.
		This should not be overwritten, and should only be called from
		category-specific setAttr() hooks."""
		if not attr in item.attrs:
			item.attrs[attr] = value;
			return True
		return False

	def getGlobalID (self, attr):
		return None

	def getLongName (self, name):
		if self.suffix:
			name = name + "_" + self.suffix
		name = re.sub ('[\. ]+', '_', name)
		return name

	def getCDecl (self, name):
		return self.getLongName (name).lower ()

	def getID (self, name):
		if self.prefix:
			name = self.prefix + "_" + name
		return self.getLongName (name).upper ()

	def getAttrDefault (self, item, attr):
		if attr == 'c_decl':
			return self.getCDecl (item.name)
		elif attr == 'id':
			return self.getID (item.name)
		return None

	def getInheritedBooleanAttrs (self, attr):
		"""Returns a list of inherited boolean attributes.
		When the given attribute is True, it says that additional attributes
		are well-defined, and may be automatically set to True or False.
		When attr is False, nothing is assumed."""
		return None

	def testBooleanAttr (self, attr):
		return False

	def isStatic (self):
		return self.static_flag

	def isDynamic (self):
		return not self.isStatic ()

	def getAllAttrs (self):
		"""Return a list of all attributes that are valid for this category.
		Not all objects may actually have the attribute set."""
		# TODO - cached the result of this calculation so it is not done every time.
		all_attrs = []
		for itemname in self.items:
			item = self.items[itemname]
			for attrname in item.attrs:
				if attrname == 'name':
					pass
				elif not attrname in all_attrs:
					all_attrs.append (attrname)
		all_attrs.sort ()
		return all_attrs

	def getDefaultAttrHTML (self, item, attr, printAttr = False):
		if printAttr:
			s = attr + "="
		else:
			s = ""

		value = item.getAttrDefault (attr)
		if value is None:
			s = '<font color=#b0b0c0>' + s + 'N/A' + '</font>'
		else:
			s = '<font color=#b0b0c0>' + s + str(value) + '</font>'
		return s

	def print_all (self):
		print '<table border=1 cellpadding=3 cellspacing=4>'
		all_attrs = self.getAllAttrs ()

		print '<tr>'
		print tagged ('th', 'Name')
		for attrname in all_attrs:
			print tagged ('th', attrname)
		print '</tr>'

		items = self.items.keys ()
		items.sort ()
		for itemname in items:
			print '<tr>'
			print tagged ('td', itemname)
			item = self.items[itemname]

			for attrname in all_attrs:
				if not attrname in item.attrs:
					print tagged ('td', self.getDefaultAttrHTML (item, attrname))
				else:
					attrvalue = item.getAttr (attrname)
					print tagged ('td', str(attrvalue))
			print '</tr>'
		print '</table>'


class MatrixCategory(Category):
	def print_matrix (self, cols, rows):
		print '<table border=1 cellpadding=3 cellspacing=4>'
		all_attrs = self.getAllAttrs ()
		for row in rows:
			print '<tr>'
			for col in cols:
				key = col + row
				print '<td>'
				if key in self.items:
					item = self.items[key]
					itemname = item.attrs['name']
					print key
					self.print_item (item)
					print '<br>' + itemname

					for attrname in all_attrs:
						if not attrname in item.attrs:
							if not self.getGlobalID (attrname):
								print "<br>" + smaller (self.getDefaultAttrHTML (item, attrname, True))
						else:
							attrvalue = item.getAttr (attrname)
							if attrname == 'name' and attrvalue == itemname:
								pass
							else:
								print "<br>" + smaller (attrname + "=" + str(attrvalue))
				print '</td>'
			print '</tr>'
		print '</table>'

	def print_item (self, item):
		pass

class SwitchCategory(MatrixCategory):
	prefix = "SW"

	def testBooleanAttr (self, attr):
		return (attr == 'ingame' or attr == 'noplay' or attr == 'intest' or
			attr == 'cabinet' or attr == 'opto' or attr == 'standup' or
			attr == 'edge' or attr == 'service' or attr == 'virtual' or
			attr == 'button' or attr == 'noscore')

	def print_all (self):
		cols = [ 'D', '1', '2', '3', '4', '5', '6', '7', '8', 'F' ]
		rows = [ '1', '2', '3', '4', '5', '6', '7', '8' ]
		self.print_matrix (cols, rows)

	def getGlobalID (self, attr):
		if attr == 'shooter': return 'MACHINE_SHOOTER_SWITCH'
		elif attr == 'tilt': return 'MACHINE_TILT_SWITCH'
		elif attr == 'slam-tilt': return 'MACHINE_SLAM_TILT_SWITCH'
		elif attr == 'start-button': return 'MACHINE_START_SWITCH'
		elif attr == 'buyin-button': return 'MACHINE_BUYIN_SWITCH'
		elif attr == 'launch-button': return 'MACHINE_LAUNCH_SWITCH'
		elif attr == 'outhole': return 'MACHINE_OUTHOLE_SWITCH'
		return None

class LampCategory(MatrixCategory):
	prefix = "LM"

	def testBooleanAttr (self, attr):
		return (attr == 'cabinet')

	def print_all (self):
		cols = [ 'D', '1', '2', '3', '4', '5', '6', '7', '8', 'F' ]
		rows = [ '1', '2', '3', '4', '5', '6', '7', '8' ]
		self.print_matrix (cols, rows)

	def getGlobalID (self, attr):
		if attr == 'start': return 'MACHINE_START_LAMP'
		elif attr == 'buyin': return 'MACHINE_BUYIN_LAMP'
		elif attr == 'shoot-again': return 'MACHINE_SHOOT_AGAIN_LAMP'
		elif attr == 'extra-ball': return 'MACHINE_EXTRA_BALL_LAMP'
		return None

	def setAttr (self, item, attr, value):
		if attr in [ 'blue', 'red', 'green', 'orange', 'purple',
			'white', 'yellow' ]:
			item.setAttrValue ('color', attr)
			return True
		return False

class TriacCategory(MatrixCategory):
	prefix = "TRIAC"

	def print_all (self):
		cols = [ '' ]
		rows = [ '0', '1', '2', '3', '4' ]
		self.print_matrix (cols, rows)

class DriverCategory(MatrixCategory):
	prefix = "SOL"

	def testBooleanAttr (self, attr):
		return (attr == 'motor' or attr == 'nosearch' or attr == 'flash')

	def print_all (self):
		cols = [ 'H', 'L', 'G', 'A', 'F', 'X' ]
		rows = [ '1', '2', '3', '4', '5', '6', '7', '8' ]
		self.print_matrix (cols, rows)

	def getGlobalID (self, attr):
		if attr == 'launch': return 'MACHINE_LAUNCH_SOLENOID'
		elif attr == 'knocker': return 'MACHINE_KNOCKER_SOLENOID'
		elif attr == 'ballserve': return 'MACHINE_BALL_SERVE_SOLENOID'
		return None

	def getAttrDefault (self, item, attr):
		if attr == 'duty': return 'SOL_DUTY_DEFAULT'
		elif attr == 'time': return 'SOL_TIME_DEFAULT'
		else: return Category.getAttrDefault (self, item, attr)


class DeffLeffCategory(Category):
	static_flag = False

	def setAttr (self, item, attr, value):
		r = re.match ("PRI.*", attr)
		if r or attr == '0':
			item.setAttr ('priority', attr)
			return True

		r = re.match (r"([^+]*)\+(.*)", attr)
		if r:
			item.setAttr (r.group (1), value)
			self.setAttr (item, r.group (2), value)
			return True

		if attr == 'D_QUEUED':
			item.setAttr ('queued', True)
			return True

		if attr == 'D_ABORTABLE':
			item.setAttr ('abortable', True)
			return True

		if attr == 'D_RESTARTABLE':
			item.setAttr ('restartable', True)
			return True

		if attr == 'D_PAUSE':
			item.setAttr ('pause', True)
			return True

		if attr == 'D_SCORE':
			item.setAttr ('score', True)
			return True

		if attr == 'L_NORMAL' or attr == 'D_NORMAL':
			return True

		return False

	def testBooleanAttr (self, attr):
		return (attr == 'runner' or attr == 'shared' or
			attr == 'queued' or attr == 'abortable' or
			attr == 'pause' or attr == 'restartable' or
			attr == 'score')

	def getAttrDefault (self, item, attr):
		if attr == 'page':
			return -1
		return Category.getAttrDefault (self, item, attr)


class DeffCategory(DeffLeffCategory):
	suffix = "deff"
	prefix = "DEFF"

class LeffCategory(DeffLeffCategory):
	suffix = "leff"
	prefix = "LEFF"

	def getAttrDefault (self, item, attr):
		if attr == 'GI':
			return 'L_NOGI'
		if attr == 'LAMPS':
			return 'L_NOLAMPS'
		return DeffLeffCategory.getAttrDefault (self, item, attr)

	def setAttr (self, item, attr, value):
		if attr == 'GI' and value == 'ALL':
			item.setAttrValue (attr, 'L_ALL_GI')
			return True
		return DeffLeffCategory.setAttr (self, item, attr, value)

class GlobalCategory(Category):
	static_flag = False
	def setAttr (self, item, attr, value):
		item.attrs['value'] = attr
		return True

class ListCategory(Category):
	static_flag = False
	def setAttr (self, item, attr, value, listattr = 'entries'):
		if not listattr in item.attrs:
			item.attrs[listattr] = []

		if attr == 'set':
			return False

		r = re.match ("([A-Z]):(.*)", attr)
		if r:
			item.attrs[r.group(1)] = r.group(2)
			return True

		entries = item.attrs[listattr]
		entries.append ( attr )
		return True

class LamplistCategory(ListCategory):
	prefix = "LAMPLIST"

	def testBooleanAttr (self, attr):
		return (attr == 'set')


class ContainerCategory(ListCategory):
	static_flag = False
	prefix = "DEV"

	def testBooleanAttr (self, attr):
		return attr == 'trough'

	def getAttrDefault (self, item, attr):
		if attr == 'init_max_count':
			return 0
		return Category.getAttrDefault (self, item, attr)

	def setAttr (self, item, attr, value):
		if not 'sol' in item.attrs:
			item.attrs['sol'] = attr;
			return True
		if attr == 'init_max_count':
			return False
		return ListCategory.setAttr (self, item, attr, value, 'switches')

class AdjustmentCategory(Category):
	static_flag = False
	def setAttr (self, item, attr, value):
		if self.setAttrOnce (item, 'type', attr):
			return True
		if self.setAttrOnce (item, 'default', attr):
			return True
		return False

class HighScoresCategory(Category):
	static_flag = False
	def setAttr (self, item, attr, value):
		if self.setAttrOnce (item, 'initials', attr):
			return True
		if self.setAttrOnce (item, 'score', attr):
			return True
		return False

class TemplateCategory(Category):
	static_flag = False

class FontCategory(Category):
	static_flag = False
	prefix = 'FON'

class FlagCategory(Category):
	static_flag = False
	prefix = 'FLAG'

class GlobalFlagCategory(Category):
	static_flag = False
	prefix = 'GLOBAL_FLAG'

CategoryTable = {
	'switches' : SwitchCategory,
	'lamps' : LampCategory,
	'drives' : DriverCategory,
	'gi' : TriacCategory,
	'deffs' : DeffCategory,
	'leffs' : LeffCategory,
	'global' : GlobalCategory,
	'defines' : GlobalCategory,
	'implicit-defines' : GlobalCategory,
	'lamplists' : LamplistCategory,
	'containers' : ContainerCategory,
	'adjustments' : AdjustmentCategory,
	'highscores' : HighScoresCategory,
	'system_music' : GlobalCategory,
	'system_sounds' : GlobalCategory,
	'templates' : TemplateCategory,
	'fonts' : FontCategory,
	'flags' : FlagCategory,
	'globalflags' : GlobalFlagCategory,
	'timers' : Category,
	'tests' : Category,
	'scores' : Category,
}

#---------------------------------------------------------------------
#
# Config - parser for the .md file format
#
#---------------------------------------------------------------------
class Config:
	def add (self, category_name, key, attributes, level=0):
		# Find the category with the given name.  Create it if it
		# does not already exist.
		if not category_name in self.categories:
			if category_name in CategoryTable:
				cat_class = CategoryTable[category_name]
			else:
				raise RuntimeError ("undefined category " + category_name)
			self.categories[category_name] = cat_class (category_name, self)
		category = self.categories[category_name]

		# For static categories, the name is taken as the first
		# attributes.  For others, the key is the name.
		if attributes and category.isStatic ():
			name = attributes[0]
			attributes = attributes[1:]
		else:
			name = key

		# Create/edit the item
		if not key in category.items:
			item = category.items[key] = Item (category, name, level)
		else:
			item = category.items[key]
			item.setName (name)

		# Add/override each attribute to the item
		for attr in attributes:
			r = re.match ("(.*)\((.*)\)", attr)
			if r:
				(attrname, attrvalue) = r.group (1, 2)
			else:
				r = re.match ("(.*)=(.*)", attr)
				if r:
					(attrname, attrvalue) = r.group (1, 2)
				else:
					attrname = attr
					attrvalue = True
			item.setAttr (attrname, attrvalue)

	def parse (self, filename):
		self.parselevel = self.parselevel + 1
		filename = Root + '/' + filename
		re_blank = re.compile ('^$')
		re_comment = re.compile ('^#')
		re_category = re.compile ('^\[([a-z_]*)\]$')
		re_global = re.compile ('^([^:]+):[ \t]*(.*)$')

		current_category = 'global'
		prev_lines = ""

		f = open (filename, 'r')
		for line in f:
			line = line.strip ()
			if re_blank.match (line):
				continue
			if re_comment.match (line):
				continue

			# Handle merge of multiple lines
			r = re.match (r"(.*)[,\\]$", line)
			if r:
				prev_lines = prev_lines + r.group (1)
				if not re.match (",$", r.group (1)):
					prev_lines = prev_lines + ", "
				continue
			else:
				line = prev_lines + line
				prev_lines = ""

			# Handle special directives
			r = re.match ("^include (.*)$", line)
			if r:
				self.parse (r.group (1))
				continue

			r = re.match ("^perlinclude (.*)$", line)
			if r:
				continue

			# Handle a category change, e.g. [switches]
			r = re_category.match (line)
			if r:
				current_category = r.group (1)
				continue

			# Handle definitions given before a category name; these
			# are termed globals.
			if current_category == 'global':
				r = re.match ("^define ([^ ]*)(.*)$", line)
				if r:
					self.add ('defines', r.group (1), [ r.group (2) ], self.parselevel)
					continue

				r = re_global.match (line)
				if r:
					self.add (current_category, r.group (1), [ r.group (2) ], self.parselevel)
					continue

			# Handle ordinary definitions inside a category.
			r = re_global.match (line)
			if r:
				name = r.group (1)
				attrs = r.group (2)
				if attrs:
					attrlist = re.split (',[ \t]*', attrs)
				else:
					attrlist = []
				self.add (current_category, name, attrlist, self.parselevel)
			else:
				raise Error ("No match: " + line)
		self.parselevel = self.parselevel - 1

	def __init__ (self):
		self.categories = {}
		self.parselevel = 0

	def loadMD (self, filename):
		self.filename = filename
		self.parse (filename)

	def saveMD (self, filename):
		pass

	def load (filename):
		f = file (filename, 'r')
		return pickle.load (f)
	load = staticmethod (load)

	def save (self, filename):
		f = file (filename, 'w')
		pickle.dump (self, f)

