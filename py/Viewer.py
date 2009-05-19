
from Config import Config

def test ():
	config = Config ("machine/wcs/wcs.md")
	print config.categories['global'].items['Title']
	print config.categories['switches'].items['31']
	print config.categories['deffs'].items['Ultra Collect']

def tagged (tag, text):
	return "<" + tag + ">" + text + "</" + tag + ">"

def view_matrix (cat, cols, rows):
	print '<table border=1 cellpadding=3 cellspacing=4>'
	for row in rows:
		print '<tr>'
		for col in cols:
			key = col + row
			print '<td>'
			if key in cat.items:
				item = cat.items[key]
				itemname = item.attrs['name']
				print key
				print '<br>' + itemname
				#if (item.level != 1):
				#	print '(Level ' + str(item.level) + ')'
				for attrname in item.attrs:
					attrvalue = item.attrs[attrname]
					if attrname == 'name' and attrvalue == itemname:
						pass
			print '</td>'
		print '</tr>'
	print '</table>'

def view_switch_matrix (cat):
	cols = [ 'D', '1', '2', '3', '4', '5', '6', '7', '8', 'F' ]
	rows = [ '1', '2', '3', '4', '5', '6', '7', '8' ]
	view_matrix (cat, cols, rows)

def view_lamp_matrix (cat):
	cols = [ '1', '2', '3', '4', '5', '6', '7', '8' ]
	rows = [ '1', '2', '3', '4', '5', '6', '7', '8' ]
	view_matrix (cat, cols, rows)

def view_driver_matrix (cat):
	cols = [ 'H', 'L', 'G', 'A', 'F', 'X' ]
	rows = [ '1', '2', '3', '4', '5', '6', '7', '8' ]
	view_matrix (cat, cols, rows)

def view_list (cat):
	print '<table>'
	for itemname in cat.items:
		print '<tr>'
		print tagged ('td', attrname + "=" + str(attrvalue))
		print '</tr>'
	print '</table>'


def view (cfg):
	print tagged('h1', 'Configuration')
	for catname in cfg.categories:
		print tagged ('h2', catname)
		cat = cfg.categories[catname]
		cat.print_all ()
		#if catname == 'switches':
		#	view_switch_matrix(cat)
		#elif catname == 'lamps':
		#	view_lamp_matrix(cat)
		#elif catname == 'drives':
		#	view_driver_matrix(cat)
		#else:
		#	view_list (cat)

# view (Config ("machine/wcs/wcs.md"))
view (Config ("machine/tz/tz.md"))
