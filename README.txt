==================
Requirements:
==================

-Windows Vista/7/8/8.1

==================
Description:
==================

-A 2d platform game engine with:

	-(static/dynamic/sloped) tile and sprite collision resolution. 
	-keyboard/mouse/joystick support for game and menus

-A map editor to edit

	-3 layers 
	 -background
	 -foreground
	 -collisions (optional)
		
==================
Controls:
==================

-Game
	movement 	- joy hat,      W/A/S/D
	run		- joy button 2, M	
	jump/swim	- joy button 0, N
	attack		- joy button 3, Y
	
	unlock camera	- T
	move camera	- Up/Left/Down/Right
	
	menu toggle	- joy button 7, Space, RightClick
	menu select	- joy button 0, Return, LeftClick
	quit		- Escape, joy button 6
	
	fullscreen 	- F1
	decrease size	- J
	increase size	- K

	slow/fast	- joy button 4 / joy button 5 
	layers toggle	- 1-7
	debug		- F2
	show collisions	- F3
	show hit boxes 	- F4
	doublebuffer	- F6
	bilinear and
	scan lines 	- F7
	curvature 	- F8
	adjust curvature- 8 / 9
	
-Editor

	move camera	- W/A/S/D, RightDown drag
	select		- LeftClick
	paint		- LeftControl + LeftClick
	edit front	- M
	edit back	- N
	edit collisions	- C
	edit image	- V
	
	draw front	- 1
	draw back	- 2
	draw collisions	- 3

	fullscreen 	- F1
	decrease size	- J
	increase size	- K
	doublebuffer	- F6	
	quit		- Escape, joy button 6

==================
Installation:
==================
==================
Contact Details:
==================

-andreastraczyk@gmail.com

==================
Tools Used:
==================

-Microsoft Visual Studio Express 2012
-GIMP 2.8.10

==================
Licensing/Legal:
==================

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

==================
TODO:
==================

0%	- refactor

==================
Naming Conventions:
==================

function prefixes in main.cpp

GL - opengl
P - program
R - raster
S - sound
I - input
M - menu
C - enemy/player
L - tile/level
O - collision
T - time
U - utility/misc
E - event
G - game

macros and enumerations
 -all upper case seperated by underscores
  -example	#define MATH_PI

*************
* variables *
*************

 -globals and locals
  -lower case seperated by underscores, preferably with 
   descriptive prefixes
  -examples	int 	last_position;
		int	velocity;
		Menu	menu_first_item;

 -members
  -lower case first letter, following words with upper case
   first letter 
  -examples	int 	lastPosition;
		int	velocity;

*************
* functions *
*************

 -globals
  -function group title upper case then underscore - 
  -upper case first letter of each word no seperations
  -example	double 	T_TimeFunction(void) {};

 -members
  -lower case first letter, following words with upper case
   first letter 
  -example	double 	Time::getCurrentTime(void) {};

*************
*  classes  *
*************

  -upper case first letter of each word no seperations
  -example	class 	HumanBeing {};
  
	