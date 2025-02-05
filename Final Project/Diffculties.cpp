#include "Diffculties.h"
#include "menu.h"
#include <SFML/Graphics.hpp>
#include <iostream>
using namespace std;
using namespace sf;
Diffculties::Diffculties(float width, float height)
{
	selected = 0;
	font.loadFromFile("C:/Windows/Fonts/Arial.ttf");
	diffs[0].setFont(font);
	diffs[0].setFillColor(Color::Red);
	diffs[0].setString("Easy");
	diffs[0].setPosition(800, 400);
	diffs[0].setCharacterSize(100);

	diffs[1].setFont(font);
	diffs[1].setFillColor(Color::White);
	diffs[1].setString("Medium");
	diffs[1].setPosition(800, 550);
	diffs[1].setCharacterSize(100);

	diffs[2].setFont(font);
	diffs[2].setFillColor(Color::White);
	diffs[2].setString("Hard");
	diffs[2].setPosition(800, 700);
	diffs[2].setCharacterSize(100);

}
Diffculties::~Diffculties() {};
void Diffculties::setselected(int n)
{
	selected = n;
}
void Diffculties::draw(RenderWindow& window)
{
	for (int i = 0; i < 3; i++)
	{
		window.draw(diffs[i]);
	}
}
void Diffculties::movedown() {
	if (selected < 2)
	{
		diffs[selected].setFillColor(Color::White);
		selected++;
		diffs[selected].setFillColor(Color::Red);
	}
	else
	{
		diffs[2].setFillColor(Color::White);
		selected = 0;
		diffs[selected].setFillColor(Color::Red);
	}
}
void Diffculties::moveup()
{
	if (selected > 0)
	{
		diffs[selected].setFillColor(Color::White);
		selected--;
		diffs[selected].setFillColor(Color::Red);
	}
	else
	{
		diffs[0].setFillColor(Color::White);
		selected = 2;
		diffs[selected].setFillColor(Color::Red);
	}
}

