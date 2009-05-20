
from array import *

def compress (list, size):
	"""Compress a list of integers into a bitset."""
	bitset = {}
	for i in range (size):
		bitset[i] = 0;
	for item in list:
		index = item / 8
		mask = 1 << (item % 8)
		bitset[index] |= mask
	return bitset

def toString (bitset):
	s = '{ '
	indices = bitset.keys ()
	indices.sort ()
	for index in indices:
		byte = bitset[index]
		s += ("0x%02X, " % byte)
	s += ' }'
	return s

def decompress (bitset):
	"""Decompress a bitset into a list of integers."""
	list = []
	indices = bitset.keys ()
	indices.sort ()
	for index in indices:
		byte = bitset[index]
		for bitpos in range (8):
			if byte & (1 << bitpos):
				list.append ( (index * 8) + bitpos )
	return list

bitset = compress ([1, 4, 16, 20], 8)
print toString (bitset)
list = decompress (bitset)
print list
