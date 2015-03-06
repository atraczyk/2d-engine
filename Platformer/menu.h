//-----------------------------------------------------------------------------
//	2d platform game engine and level editor by Andreas Traczyk (2014-15) GPL
//	http://atraczyk.co.nf/	email: andreastraczyk@gmail.com
//
//	DESCRIPTION:	menus and items
//-----------------------------------------------------------------------------

#ifndef MENU_H
#define MENU_H

#include <vector>
#include <stdio.h>
#include "objects.h"
#include "point.h"

#define		BOOLTOSTRING(x)			x?"on":"off"

class MenuItem
{
public:
	float	x, y, w, h;
	char*	name;
	void	(*function)();
	bool*	boolValue;
	bool	isSlider;
	float*  floatValue;
	
	MenuItem();
	MenuItem(float _x,float _y, bool is_slider, char* _name,void (*_function)());
	~MenuItem(){};
};

class MenuList
{
public:
	std::vector<MenuItem> items;
	char* name;

	MenuList();
	MenuList(char* listname);
	~MenuList(){};

	void addItem(float _x, float _y, bool is_slider, char* _name, void (*_function)());
};

#endif