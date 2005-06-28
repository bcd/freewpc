/* Copyright (C) 1995 DJ Delorie, see COPYING.DJ for details */
#include <inline/ctype.inl>

unsigned char __ctype_flags[] = {
  __ISCNTRL,						/* CTRL+@, 0x00 */
  __ISCNTRL,						/* CTRL+A, 0x01 */
  __ISCNTRL,						/* CTRL+B, 0x02 */
  __ISCNTRL,						/* CTRL+C, 0x03 */
  __ISCNTRL,						/* CTRL+D, 0x04 */
  __ISCNTRL,						/* CTRL+E, 0x05 */
  __ISCNTRL,						/* CTRL+F, 0x06 */
  __ISCNTRL,						/* CTRL+G, 0x07 */
  __ISCNTRL,						/* CTRL+H, 0x08 */
  __ISCNTRL | __ISSPACE,				/* CTRL+I, 0x09 */
  __ISCNTRL | __ISSPACE,				/* CTRL+J, 0x0a */
  __ISCNTRL | __ISSPACE,				/* CTRL+K, 0x0b */
  __ISCNTRL | __ISSPACE,				/* CTRL+L, 0x0c */
  __ISCNTRL | __ISSPACE,				/* CTRL+M, 0x0d */
  __ISCNTRL,						/* CTRL+N, 0x0e */
  __ISCNTRL,						/* CTRL+O, 0x0f */
  __ISCNTRL,						/* CTRL+P, 0x10 */
  __ISCNTRL,						/* CTRL+Q, 0x11 */
  __ISCNTRL,						/* CTRL+R, 0x12 */
  __ISCNTRL,						/* CTRL+S, 0x13 */
  __ISCNTRL,						/* CTRL+T, 0x14 */
  __ISCNTRL,						/* CTRL+U, 0x15 */
  __ISCNTRL,						/* CTRL+V, 0x16 */
  __ISCNTRL,						/* CTRL+W, 0x17 */
  __ISCNTRL,						/* CTRL+X, 0x18 */
  __ISCNTRL,						/* CTRL+Y, 0x19 */
  __ISCNTRL,						/* CTRL+Z, 0x1a */
  __ISCNTRL,						/* CTRL+[, 0x1b */
  __ISCNTRL,						/* CTRL+\, 0x1c */
  __ISCNTRL,						/* CTRL+], 0x1d */
  __ISCNTRL,						/* CTRL+^, 0x1e */
  __ISCNTRL,						/* CTRL+_, 0x1f */
  __ISPRINT | __ISSPACE,				/* ` ', 0x20 */
  __ISGRAPH | __ISPRINT ,				/* `!', 0x21 */
  __ISGRAPH | __ISPRINT ,				/* 0x22 */
  __ISGRAPH | __ISPRINT ,				/* `#', 0x23 */
  __ISGRAPH | __ISPRINT ,				/* `$', 0x24 */
  __ISGRAPH | __ISPRINT ,				/* `%', 0x25 */
  __ISGRAPH | __ISPRINT ,				/* `&', 0x26 */
  __ISGRAPH | __ISPRINT ,				/* 0x27 */
  __ISGRAPH | __ISPRINT ,				/* `(', 0x28 */
  __ISGRAPH | __ISPRINT ,				/* `)', 0x29 */
  __ISGRAPH | __ISPRINT ,				/* `*', 0x2a */
  __ISGRAPH | __ISPRINT ,				/* `+', 0x2b */
  __ISGRAPH | __ISPRINT ,				/* `,', 0x2c */
  __ISGRAPH | __ISPRINT ,				/* `-', 0x2d */
  __ISGRAPH | __ISPRINT ,				/* `.', 0x2e */
  __ISGRAPH | __ISPRINT ,				/* `/', 0x2f */
  __ISDIGIT | __ISGRAPH | __ISPRINT | __ISXDIGIT,	/* `0', 0x30 */
  __ISDIGIT | __ISGRAPH | __ISPRINT | __ISXDIGIT,	/* `1', 0x31 */
  __ISDIGIT | __ISGRAPH | __ISPRINT | __ISXDIGIT,	/* `2', 0x32 */
  __ISDIGIT | __ISGRAPH | __ISPRINT | __ISXDIGIT,	/* `3', 0x33 */
  __ISDIGIT | __ISGRAPH | __ISPRINT | __ISXDIGIT,	/* `4', 0x34 */
  __ISDIGIT | __ISGRAPH | __ISPRINT | __ISXDIGIT,	/* `5', 0x35 */
  __ISDIGIT | __ISGRAPH | __ISPRINT | __ISXDIGIT,	/* `6', 0x36 */
  __ISDIGIT | __ISGRAPH | __ISPRINT | __ISXDIGIT,	/* `7', 0x37 */
  __ISDIGIT | __ISGRAPH | __ISPRINT | __ISXDIGIT,	/* `8', 0x38 */
  __ISDIGIT | __ISGRAPH | __ISPRINT | __ISXDIGIT,	/* `9', 0x39 */
  __ISGRAPH | __ISPRINT ,				/* `:', 0x3a */
  __ISGRAPH | __ISPRINT ,				/* `;', 0x3b */
  __ISGRAPH | __ISPRINT ,				/* `<', 0x3c */
  __ISGRAPH | __ISPRINT ,				/* `=', 0x3d */
  __ISGRAPH | __ISPRINT ,				/* `>', 0x3e */
  __ISGRAPH | __ISPRINT ,				/* `?', 0x3f */
  __ISGRAPH | __ISPRINT ,				/* `@', 0x40 */
  __ISGRAPH | __ISPRINT | __ISUPPER | __ISXDIGIT,	/* `A', 0x41 */
  __ISGRAPH | __ISPRINT | __ISUPPER | __ISXDIGIT,	/* `B', 0x42 */
  __ISGRAPH | __ISPRINT | __ISUPPER | __ISXDIGIT,	/* `C', 0x43 */
  __ISGRAPH | __ISPRINT | __ISUPPER | __ISXDIGIT,	/* `D', 0x44 */
  __ISGRAPH | __ISPRINT | __ISUPPER | __ISXDIGIT,	/* `E', 0x45 */
  __ISGRAPH | __ISPRINT | __ISUPPER | __ISXDIGIT,	/* `F', 0x46 */
  __ISGRAPH | __ISPRINT | __ISUPPER,			/* `G', 0x47 */
  __ISGRAPH | __ISPRINT | __ISUPPER,			/* `H', 0x48 */
  __ISGRAPH | __ISPRINT | __ISUPPER,			/* `I', 0x49 */
  __ISGRAPH | __ISPRINT | __ISUPPER,			/* `J', 0x4a */
  __ISGRAPH | __ISPRINT | __ISUPPER,			/* `K', 0x4b */
  __ISGRAPH | __ISPRINT | __ISUPPER,			/* `L', 0x4c */
  __ISGRAPH | __ISPRINT | __ISUPPER,			/* `M', 0x4d */
  __ISGRAPH | __ISPRINT | __ISUPPER,			/* `N', 0x4e */
  __ISGRAPH | __ISPRINT | __ISUPPER,			/* `O', 0x4f */
  __ISGRAPH | __ISPRINT | __ISUPPER,			/* `P', 0x50 */
  __ISGRAPH | __ISPRINT | __ISUPPER,			/* `Q', 0x51 */
  __ISGRAPH | __ISPRINT | __ISUPPER,			/* `R', 0x52 */
  __ISGRAPH | __ISPRINT | __ISUPPER,			/* `S', 0x53 */
  __ISGRAPH | __ISPRINT | __ISUPPER,			/* `T', 0x54 */
  __ISGRAPH | __ISPRINT | __ISUPPER,			/* `U', 0x55 */
  __ISGRAPH | __ISPRINT | __ISUPPER,			/* `V', 0x56 */
  __ISGRAPH | __ISPRINT | __ISUPPER,			/* `W', 0x57 */
  __ISGRAPH | __ISPRINT | __ISUPPER,			/* `X', 0x58 */
  __ISGRAPH | __ISPRINT | __ISUPPER,			/* `Y', 0x59 */
  __ISGRAPH | __ISPRINT | __ISUPPER,			/* `Z', 0x5a */
  __ISGRAPH | __ISPRINT ,				/* `[', 0x5b */
  __ISGRAPH | __ISPRINT ,				/* 0x5c */
  __ISGRAPH | __ISPRINT ,				/* `]', 0x5d */
  __ISGRAPH | __ISPRINT ,				/* `^', 0x5e */
  __ISGRAPH | __ISPRINT ,				/* `_', 0x5f */
  __ISGRAPH | __ISPRINT ,				/* 0x60 */
  __ISGRAPH | __ISLOWER | __ISPRINT | __ISXDIGIT,	/* `a', 0x61 */
  __ISGRAPH | __ISLOWER | __ISPRINT | __ISXDIGIT,	/* `b', 0x62 */
  __ISGRAPH | __ISLOWER | __ISPRINT | __ISXDIGIT,	/* `c', 0x63 */
  __ISGRAPH | __ISLOWER | __ISPRINT | __ISXDIGIT,	/* `d', 0x64 */
  __ISGRAPH | __ISLOWER | __ISPRINT | __ISXDIGIT,	/* `e', 0x65 */
  __ISGRAPH | __ISLOWER | __ISPRINT | __ISXDIGIT,	/* `f', 0x66 */
  __ISGRAPH | __ISLOWER | __ISPRINT,			/* `g', 0x67 */
  __ISGRAPH | __ISLOWER | __ISPRINT,			/* `h', 0x68 */
  __ISGRAPH | __ISLOWER | __ISPRINT,			/* `i', 0x69 */
  __ISGRAPH | __ISLOWER | __ISPRINT,			/* `j', 0x6a */
  __ISGRAPH | __ISLOWER | __ISPRINT,			/* `k', 0x6b */
  __ISGRAPH | __ISLOWER | __ISPRINT,			/* `l', 0x6c */
  __ISGRAPH | __ISLOWER | __ISPRINT,			/* `m', 0x6d */
  __ISGRAPH | __ISLOWER | __ISPRINT,			/* `n', 0x6e */
  __ISGRAPH | __ISLOWER | __ISPRINT,			/* `o', 0x6f */
  __ISGRAPH | __ISLOWER | __ISPRINT,			/* `p', 0x70 */
  __ISGRAPH | __ISLOWER | __ISPRINT,			/* `q', 0x71 */
  __ISGRAPH | __ISLOWER | __ISPRINT,			/* `r', 0x72 */
  __ISGRAPH | __ISLOWER | __ISPRINT,			/* `s', 0x73 */
  __ISGRAPH | __ISLOWER | __ISPRINT,			/* `t', 0x74 */
  __ISGRAPH | __ISLOWER | __ISPRINT,			/* `u', 0x75 */
  __ISGRAPH | __ISLOWER | __ISPRINT,			/* `v', 0x76 */
  __ISGRAPH | __ISLOWER | __ISPRINT,			/* `w', 0x77 */
  __ISGRAPH | __ISLOWER | __ISPRINT,			/* `x', 0x78 */
  __ISGRAPH | __ISLOWER | __ISPRINT,			/* `y', 0x79 */
  __ISGRAPH | __ISLOWER | __ISPRINT,			/* `z', 0x7a */
  __ISGRAPH | __ISPRINT ,				/* `{', 0x7b */
  __ISGRAPH | __ISPRINT ,				/* `|', 0x7c */
  __ISGRAPH | __ISPRINT ,				/* `}', 0x7d */
  __ISGRAPH | __ISPRINT ,				/* `~', 0x7e */
  __ISCNTRL,						/* 0x7f */
  0,							/* 0x80 */
  0,							/* 0x81 */
  0,							/* 0x82 */
  0,							/* 0x83 */
  0,							/* 0x84 */
  0,							/* 0x85 */
  0,							/* 0x86 */
  0,							/* 0x87 */
  0,							/* 0x88 */
  0,							/* 0x89 */
  0,							/* 0x8a */
  0,							/* 0x8b */
  0,							/* 0x8c */
  0,							/* 0x8d */
  0,							/* 0x8e */
  0,							/* 0x8f */
  0,							/* 0x90 */
  0,							/* 0x91 */
  0,							/* 0x92 */
  0,							/* 0x93 */
  0,							/* 0x94 */
  0,							/* 0x95 */
  0,							/* 0x96 */
  0,							/* 0x97 */
  0,							/* 0x98 */
  0,							/* 0x99 */
  0,							/* 0x9a */
  0,							/* 0x9b */
  0,							/* 0x9c */
  0,							/* 0x9d */
  0,							/* 0x9e */
  0,							/* 0x9f */
  0,							/* 0xa0 */
  0,							/* 0xa1 */
  0,							/* 0xa2 */
  0,							/* 0xa3 */
  0,							/* 0xa4 */
  0,							/* 0xa5 */
  0,							/* 0xa6 */
  0,							/* 0xa7 */
  0,							/* 0xa8 */
  0,							/* 0xa9 */
  0,							/* 0xaa */
  0,							/* 0xab */
  0,							/* 0xac */
  0,							/* 0xad */
  0,							/* 0xae */
  0,							/* 0xaf */
  0,							/* 0xb0 */
  0,							/* 0xb1 */
  0,							/* 0xb2 */
  0,							/* 0xb3 */
  0,							/* 0xb4 */
  0,							/* 0xb5 */
  0,							/* 0xb6 */
  0,							/* 0xb7 */
  0,							/* 0xb8 */
  0,							/* 0xb9 */
  0,							/* 0xba */
  0,							/* 0xbb */
  0,							/* 0xbc */
  0,							/* 0xbd */
  0,							/* 0xbe */
  0,							/* 0xbf */
  0,							/* 0xc0 */
  0,							/* 0xc1 */
  0,							/* 0xc2 */
  0,							/* 0xc3 */
  0,							/* 0xc4 */
  0,							/* 0xc5 */
  0,							/* 0xc6 */
  0,							/* 0xc7 */
  0,							/* 0xc8 */
  0,							/* 0xc9 */
  0,							/* 0xca */
  0,							/* 0xcb */
  0,							/* 0xcc */
  0,							/* 0xcd */
  0,							/* 0xce */
  0,							/* 0xcf */
  0,							/* 0xd0 */
  0,							/* 0xd1 */
  0,							/* 0xd2 */
  0,							/* 0xd3 */
  0,							/* 0xd4 */
  0,							/* 0xd5 */
  0,							/* 0xd6 */
  0,							/* 0xd7 */
  0,							/* 0xd8 */
  0,							/* 0xd9 */
  0,							/* 0xda */
  0,							/* 0xdb */
  0,							/* 0xdc */
  0,							/* 0xdd */
  0,							/* 0xde */
  0,							/* 0xdf */
  0,							/* 0xe0 */
  0,							/* 0xe1 */
  0,							/* 0xe2 */
  0,							/* 0xe3 */
  0,							/* 0xe4 */
  0,							/* 0xe5 */
  0,							/* 0xe6 */
  0,							/* 0xe7 */
  0,							/* 0xe8 */
  0,							/* 0xe9 */
  0,							/* 0xea */
  0,							/* 0xeb */
  0,							/* 0xec */
  0,							/* 0xed */
  0,							/* 0xee */
  0,							/* 0xef */
  0,							/* 0xf0 */
  0,							/* 0xf1 */
  0,							/* 0xf2 */
  0,							/* 0xf3 */
  0,							/* 0xf4 */
  0,							/* 0xf5 */
  0,							/* 0xf6 */
  0,							/* 0xf7 */
  0,							/* 0xf8 */
  0,							/* 0xf9 */
  0,							/* 0xfa */
  0,							/* 0xfb */
  0,							/* 0xfc */
  0,							/* 0xfd */
  0,							/* 0xfe */
  0,							/* 0xff */
};
