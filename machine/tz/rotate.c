/*
 * Copyright 2011 by Ewan Meadows <sonny_jim@hotmail.com>
 *
 * This file is part of FreeWPC.
 *
 * FreeWPC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * FreeWPC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with FreeWPC; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */
#include <stdio.h>

#define RADIUS	10

static void print_pixel_coords (int x, int y)
{
	printf ("%d %d\n", x, y);
}
int main (void)
{

  int f = 1 - RADIUS;
  int ddF_x = 1;
  int ddF_y = -2 * RADIUS;
  int x = 0;
  int y = RADIUS;
 
  print_pixel_coords(64, 16 + RADIUS);
  print_pixel_coords(64, 16 - RADIUS);
  print_pixel_coords(64 + RADIUS, 16);
  print_pixel_coords(64 - RADIUS, 16);
 
  while(x < y)
  {
    // ddF_x == 2 * x + 1;
    // ddF_y == -2 * y;
    // f == x*x + y*y - RADIUS*radius + 2*x - y + 1;
    if(f >= 0) 
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;    
    print_pixel_coords(64 + x, 16 + y);
    print_pixel_coords(64 - x, 16 + y);
    print_pixel_coords(64 + x, 16 - y);
    print_pixel_coords(64 - x, 16 - y);
    print_pixel_coords(64 + y, 16 + x);
    print_pixel_coords(64 - y, 16 + x);
    print_pixel_coords(64 + y, 16 - x);
    print_pixel_coords(64 - y, 16 - x);
  }
  return;
}

