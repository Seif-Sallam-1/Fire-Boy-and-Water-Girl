#ifndef DIFFCULTIES_H
#define DIFFCULTIES_H

#include <SFML/Graphics.hpp>

class Diffculties
{
public:
    Diffculties(float width, float height);
    ~Diffculties();
    void draw(sf::RenderWindow& window);
    void moveup();
    void movedown();
    void setselected(int n);
    int pressed() { return selected; }

private:
    int selected;
    sf::Font font;
    sf::Text diffs[3];
};

#endif // DIFFCULTIES_H
