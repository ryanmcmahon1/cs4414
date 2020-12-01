#include <iostream>
#include "displayobject.hpp"

char DisplayObject::theFarm[NLINES][LINELEN][NLAYERS];

DisplayObject::DisplayObject(const std::string& str, const int n)
{
	current_x = 0;
	current_y = 0;
	image = str;
	layer = n;
	width = 0;
	height = 1;
	int i = 0, x = 0;
	while(char c = image[i++])
	{
		if(c == '#')
		{
			++height;
			x = 0;
		}
		else
		{
			if(++x > width)
			{
				width = x;
			}
		}
	}
}

DisplayObject::DisplayObject(const DisplayObject& from)
{
	width = from.width;
	height = from.height;
	layer = from.layer;
	current_x = from.current_x;
	current_y = from.current_y;
	image = from.image;
}

DisplayObject::~DisplayObject()
{
	if(current_x > 0 && current_y > 0)
	{
		erase();
		current_x = 0;
		current_y = 0;
	}
}

void DisplayObject::update_contents(const std::string& str)
{
	int x = current_x;
	int y = current_y;
	if(current_x > 0 && current_y > 0)
	{
		erase();
		current_x = 0;
		current_y = 0;
	}
	image = str;
	if(current_x > 0 && current_y > 0)
	{
		draw(x, y);
	}
}

void DisplayObject::draw(int y, int x, char c)
{
	if(y < 1 || x < 1 || y >= NLINES || x >= LINELEN)
		return;
	theFarm[y][x][layer] = c;
}

void DisplayObject::draw(int y, int x)
{
	if(current_x != 0 && current_y != 0)
	{
		erase();
	}
	current_x = x;
	current_y = y;
	int i = 0;
	while(char c = image[i++])
	{
		if(c == '#')
		{
			++y;
			x = current_x;
		}
		else
		{
			DisplayObject::draw(y, x, c);
			++x;
		}
	}
}

void DisplayObject::erase()
{
	int x = current_x;
	int y = current_y;
	int i = 0;
	while(char c = image[i++])
	{
		if(c == '#')
		{
			++y;
			x = current_x;
		}
		else
		{
			if(y >= 1 && x >= 1 && y < NLINES && x < LINELEN)
				theFarm[y][x][layer] = 0;
			++x;
		}
	}
}

void DisplayObject::redisplay()
{
	std::string toDisplay("\033[1;1H");
	toDisplay += '|';
	for(int c = 0; c < LINELEN; c++)
		toDisplay += '_';
	toDisplay += "|\n";
	for(int line = 0; line < NLINES; line++)
	{
		toDisplay += '|';
		for(int col = 0; col < LINELEN; col++)
		{
			int layer = 4;
			char c = 0;
			while(layer--)
			{
				if((c = theFarm[line][col][layer]))
					break;
			}
			toDisplay += c? c: ' ';
		}
		toDisplay += "|\n";
	}
	toDisplay += '|';
	for(int c = 0; c < LINELEN; c++)
		toDisplay += '_';
	toDisplay += "|\n";
	std::cout << toDisplay << std::endl;
}
