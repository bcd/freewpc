#!/usr/bin/env python

import re

Root = '/home/bcd/src/git/freewpc'

def tagged (tag, text):
	return "<" + tag + ">" + text + "</" + tag + ">"

class Item:
	def __init__ (self, category, name, level=0):
		self.category = category
		self.attrs = {}
		self.level = level
		self.setName (name)

	def setAttr (self, attr, value):
		if self.category.testBooleanAttr (attr):
			self.attrs[attr] = True
		elif not self.category.setAttr (self, attr, value):
			self.attrs[attr] = value

	def getAttr (self, attr):
		value = self.category.getAttr (self, attr)
		if not value:
			value = self.attrs[attr]
		return value

	def setName (self, name):
		self.name = name
		self.attrs['name'] = name

	def __str__ (self):
		return "(" + self.name + ":" + self.attrs.__str__ () + ")"

class Category:
	static = {
		'switches' : True,
		'lamps' : True,
		'drives' : True,
		'gi' : True,
	}

	def __init__ (self, name):
		self.name = name
		self.items = {}
		self.static = name in Category.static

	def setAttr (self, item, attr, value):
		return None

	def getAttr (self, item, attr):
		return None

	def testBooleanAttr (self, attr):
		return False

	def isStatic (self):
		return self.static

	def isDynamic (self):
		return not self.isDynamic ()

	def print_all (self):
		print '<table border=1 cellpadding=3 cellspacing=4>'

		all_attrs = []
		for itemname in self.items:
			item = self.items[itemname]
			for attrname in item.attrs:
				if attrname == 'name':
					pass
				elif not attrname in all_attrs:
					all_attrs.append (attrname)
		all_attrs.sort ()

		print '<tr>'
		print tagged ('th', 'Name')
		for attrname in all_attrs:
			print tagged ('th', attrname)
		print '</tr>'

		for itemname in self.items:
			print '<tr>'
			print tagged ('td', itemname)
			item = self.items[itemname]
			#if (item.level != 1):
			#	print '(Level ' + str(item.level) + ')'

			for attrname in all_attrs:
			#for attrname in item.attrs:
				if not attrname in item.attrs:
					print tagged ('td', '')
				else:
					attrvalue = item.attrs[attrname]
					print tagged ('td', str(attrvalue))
			print '</tr>'
		print '</table>'


class MatrixCategory(Category):
	def print_matrix (self, cols, rows):
		print '<table border=1 cellpadding=3 cellspacing=4>'
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
					#if (item.level != 1):
					#	print '(Level ' + str(item.level) + ')'
					for attrname in item.attrs:
						# attrvalue = item.attrs[attrname]
						attrvalue = item.getAttr (attrname)
						if attrname == 'name' and attrvalue == itemname:
							pass
						else:
							print "<br>" + attrname + "=" + str(attrvalue)
				print '</td>'
			print '</tr>'
		print '</table>'

	def print_item (self, item):
		pass

class SwitchCategory(MatrixCategory):
	def testBooleanAttr (self, attr):
		return (attr == 'ingame' or
			attr == 'noplay' or
			attr == 'cabinet' or
			attr == 'opto')

	def print_all (self):
		cols = [ 'D', '1', '2', '3', '4', '5', '6', '7', '8', 'F' ]
		rows = [ '1', '2', '3', '4', '5', '6', '7', '8' ]
		self.print_matrix (cols, rows)

class LampCategory(MatrixCategory):
	def print_all (self):
		cols = [ 'D', '1', '2', '3', '4', '5', '6', '7', '8', 'F' ]
		rows = [ '1', '2', '3', '4', '5', '6', '7', '8' ]
		self.print_matrix (cols, rows)

class TriacCategory(MatrixCategory):
	def print_all (self):
		cols = [ '' ]
		rows = [ '0', '1', '2', '3', '4' ]
		self.print_matrix (cols, rows)

class DriverCategory(MatrixCategory):
	def print_all (self):
		cols = [ 'H', 'L', 'G', 'A', 'F', 'X' ]
		rows = [ '1', '2', '3', '4', '5', '6', '7', '8' ]
		self.print_matrix (cols, rows)

class DeffLeffCategory(Category):
	def setAttr (self, item, attr, value):
		r = re.match ("PRI.*", attr)
		if r:
			item.setAttr ('priority', attr)
			return True

		r = re.match ("[DL]_*", attr)
		if r:
			item.setAttr ('flags', attr)
			return True
		return False

class GlobalCategory(Category):
	def setAttr (self, item, attr, value):
		item.attrs['value'] = attr
		return True

class ListCategory(Category):
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
	pass

class ContainerCategory(ListCategory):
	def testBooleanAttr (self, attr):
		return attr == 'trough'

	def setAttr (self, item, attr, value):
		if not 'sol' in item.attrs:
			item.attrs['sol'] = attr;
			return True
		if attr == 'init_max_count':
			return False
		return ListCategory.setAttr (self, item, attr, value, 'switches')

class AdjustmentCategory(Category):
	def setAttr (self, item, attr, value):
		if not 'type' in item.attrs:
			item.attrs['type'] = attr;
			return True
		if not 'default' in item.attrs:
			item.attrs['default'] = attr;
			return True
		return False

class HighScoresCategory(Category):
	def setAttr (self, item, attr, value):
		if not 'initials' in item.attrs:
			item.attrs['initials'] = attr;
			return True
		if not 'score' in item.attrs:
			item.attrs['score'] = attr;
			return True
		return False

CategoryTable = {
	'switches' : SwitchCategory,
	'lamps' : LampCategory,
	'drives' : DriverCategory,
	'gi' : TriacCategory,
	'deffs' : DeffLeffCategory,
	'leffs' : DeffLeffCategory,
	'global' : GlobalCategory,
	'defines' : GlobalCategory,
	'lamplists' : LamplistCategory,
	'containers' : ContainerCategory,
	'adjustments' : AdjustmentCategory,
	'highscores' : HighScoresCategory,
	'system_music' : GlobalCategory,
	'system_sounds' : GlobalCategory,
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
				cat_class = Category
			self.categories[category_name] = cat_class (category_name)
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
				if re.match (",$", r.group (1)):
					prev_lines = prev_lines + ", "
				continue
			else:
				line = prev_lines + line
				prev_lines = ""
				#print "<p>" + line

			# Handle special directives
			r = re.match ("^include (.*)$", line)
			if r:
				self.parse (r.group (1))
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
					# print "#define " + r.group (1)
					continue

				r = re_global.match (line)
				if r:
					self.add (current_category, r.group (1), [ r.group (2) ], self.parselevel)
					# print r.group (1, 2)
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
				print "<p>No match: " + line
		self.parselevel = self.parselevel - 1

	def __init__ (self, filename):
		self.categories = {}
		self.filename = filename
		self.parselevel = 0
		self.parse (filename)

	def save (self):
		pass


