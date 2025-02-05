#include "menu.h"
#include <SFML/Graphics.hpp>
#include <iostream>
using namespace std;
using namespace sf;
menu::menu(float width, float height)
{
	selected = 0;
	font.loadFromFile("C:/Windows/Fonts/Arial.ttf");
	mainmenu[0].setFont(font);
	mainmenu[0].setFillColor(Color::Red);
	mainmenu[0].setString("Play");
	mainmenu[0].setPosition(800, 400);
	mainmenu[0].setCharacterSize(100);

	mainmenu[1].setFont(font);
	mainmenu[1].setFillColor(Color::White);
	mainmenu[1].setString("History");
	mainmenu[1].setPosition(800, 550);
	mainmenu[1].setCharacterSize(100);

	mainmenu[2].setFont(font);
	mainmenu[2].setFillColor(Color::White);
	mainmenu[2].setString("Exit");
	mainmenu[2].setPosition(800, 700);
	mainmenu[2].setCharacterSize(100);

}
menu::~menu() {};
void menu::setselected(int n)
{
	selected = n;
}
void menu::draw(RenderWindow& window)
{
	for (int i = 0; i < 3; i++)
	{
		window.draw(mainmenu[i]);
	}
}
void menu::movedown() {
	if (selected < 2)
	{
		mainmenu[selected].setFillColor(Color::White);
		selected++;
		mainmenu[selected].setFillColor(Color::Red);
	}
	else
	{
		mainmenu[2].setFillColor(Color::White);
		selected = 0;
		mainmenu[selected].setFillColor(Color::Red);
	}
}
void menu::moveup()
{
	if (selected > 0)
	{
		mainmenu[selected].setFillColor(Color::White);
		selected--;
		mainmenu[selected].setFillColor(Color::Red);
	}
	else
	{
		mainmenu[0].setFillColor(Color::White);
		selected = 2;
		mainmenu[selected].setFillColor(Color::Red);
	}
}

