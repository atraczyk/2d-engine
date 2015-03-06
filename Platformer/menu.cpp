//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15) GPL
//	http://atraczyk.co.nf/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	menus and items
//-----------------------------------------------------------------------------

#include "menu.h"

MenuItem::MenuItem()
{
	boolValue = NULL;
	name = NULL;
	function = NULL;
}

MenuItem::MenuItem(float _x,float _y, bool is_slider, char* _name,void (*_function)())
{
	x = _x;
	y = _y;
	w = 0;
	h = 0;
	boolValue = NULL;
	floatValue = NULL;
	name = _name;
	function = _function;
	isSlider = is_slider;
}

MenuList::MenuList()
{
	name = NULL; 
}

MenuList::MenuList(char* name)
{
	name = name; 
}

void MenuList::addItem(float _x, float _y, bool is_slider,  char* _name, void (*_function)())
{
	items.push_back(MenuItem(_x, _y, is_slider, _name, _function));
}