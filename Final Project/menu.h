#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
using namespace std;
using namespace sf;
class menu
{
public:
	menu(float width, float height);
	Text mainmenu[3];
	void draw(RenderWindow& window);
	void moveup();
	void movedown();
	void setselected(int n);
	int pressed() {
		return selected;
	}
	~menu();
private:
	int selected;
	Font font;
};

