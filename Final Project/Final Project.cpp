#include <SFML/Graphics.hpp>
#include <iostream>
#include "menu.h"  
#include "Diffculties.h"
#include <time.h>
#include <cstdlib>
#include <vector>
#include<fstream>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>
#include <SFML/Window/Event.hpp>
#include <algorithm>
using namespace sf;
using namespace std;
// Structure for game objects
const string img_path = "C:/Users/Lenovo/source/repos/Final Project/Final Project/images/";
const string font_path = "C:/Users/Lenovo/source/repos/Final Project/Final Project/fonts/"; struct GameObject
{
	Sprite sprite;
	Vector2f position;

	// Check collision with another GameObject
	bool checkCollision(const GameObject& other) const
	{
		return sprite.getGlobalBounds().intersects(other.sprite.getGlobalBounds());
	}

	GameObject() = default;

	GameObject(const Sprite& sprite, const Vector2f& position)
		: sprite(sprite), position(position)
	{
	}

	bool operator==(const GameObject& other) const
	{
		return false;
	}
};
FloatRect getHillBounds(const GameObject& hill)
{
	FloatRect bounds = hill.sprite.getGlobalBounds();

	// bounds of the hill at the bottom center of the map
	float paddingLeft = 5.0f;  // Example adjustment for left side
	float paddingTop = 5.0f;   // Example adjustment for top side
	float paddingRight = 5.0f; // Example adjustment for right side
	float paddingBottom = 5.0f; // Example adjustment for bottom side

	bounds.left += paddingLeft;
	bounds.top += paddingTop;
	bounds.width -= (paddingLeft + paddingRight);
	bounds.height -= (paddingTop + paddingBottom);

	return bounds;
}
const int SCREEN_WIDTH = 1921;
const int SCREEN_HEIGHT = 1080;
// Texture Manager to prevent duplicate texture loading
class TextureManager
{
private:
	map<string, Texture> textures;

public:
	const Texture& getTexture(const string& textureFile)
	{
		if (textures.find(textureFile) == textures.end())
		{
			Texture texture;
			if (!texture.loadFromFile(textureFile))
			{
				cerr << "Failed to load texture: " << textureFile << endl;
				exit(EXIT_FAILURE);
			}
			textures[textureFile] = texture;
		}
		return textures[textureFile];
	}
};

// GameObject initialization
void initializeGameObject(GameObject& object, const Texture& texture, Vector2f position, Vector2f scale = { 1.0f, 1.0f })
{
	object.sprite.setTexture(texture);
	object.sprite.setPosition(position);
	object.sprite.setScale(scale);
	object.position = position;
}


enum PLAYER_ANIMATION_STATES { IDLE = 0, MOVING_LEFT, MOVING_RIGHT, JUMPING };
enum WATERGIRL_ANIMATION_STATES { WATERGIRL_IDLE = 0, WATERGIRL_MOVING_LEFT, WATERGIRL_MOVING_RIGHT, WATERGIRL_JUMPING };

class Character {
public:
	RectangleShape shape;
	Texture texture;
	IntRect currentFrame;
	Vector2f velocity;
	bool isGrounded;
	Clock animationTimer;
	short animState;
	std::string type; // Add type property

	Character(const string& texturePath, const Vector2f& size, const Vector2f& position, const std::string& characterType) {
		shape.setSize(size);
		if (!texture.loadFromFile(texturePath)) {
			cerr << "Error loading texture from " << texturePath << endl;
		}
		shape.setTexture(&texture);
		shape.setPosition(position);
		currentFrame = IntRect(0, 400, 100, 100); // Default frame
		shape.setTextureRect(currentFrame);
		velocity = Vector2f(0.f, 0.f);
		isGrounded = true;
		animState = IDLE; // Start in IDLE state
		type = characterType; // Initialize type
	}

	void move(float dirX, float dirY) {
		shape.move(dirX, dirY);
	}

	void jump() {
		if (isGrounded) {
			velocity.y = -10.f; // Jump strength
			isGrounded = false;
		}
	}

	void update(float deltaTime) {
		// Apply gravity
		if (!isGrounded) {
			velocity.y += 0.5f; // Gravity strength
		}

		// Move the character
		shape.move(velocity);

		// Check for ground collision
		if (shape.getPosition().y + shape.getSize().y >= SCREEN_HEIGHT) { // Assuming ground level is at y = 720
			shape.setPosition(shape.getPosition().x, SCREEN_HEIGHT - shape.getSize().y - 60);
			isGrounded = true;
			velocity.y = 0; // Reset vertical velocity
			animState = IDLE; // Reset to IDLE state when on the ground
		}
	}
};

class Fireboy : public Character {
public:
	Fireboy(const string& texturePath, const Vector2f& size, const Vector2f& position)
		: Character(texturePath, size, position, "Fireboy") { // Pass "Fireboy" as type
		currentFrame = IntRect(0, 400, 100, 100);
		shape.setTextureRect(currentFrame);
	}

	void updateAnimations() {
		if (animState == IDLE) {
			if (animationTimer.getElapsedTime().asSeconds() >= 0.1f) {
				currentFrame.top = 400.f; // IDLE row (4th row, starting from 0)
				currentFrame.left += 100.f;
				if (currentFrame.left >= 100.f) // Wrap around after 4 frames
					currentFrame.left = 0;

				animationTimer.restart();
				shape.setTextureRect(currentFrame);
			}
		}
		else if (animState == MOVING_RIGHT) {
			if (animationTimer.getElapsedTime().asSeconds() >= 0.1f) {
				currentFrame.top = 0.f; // MOVING_RIGHT row for Fireboy
				currentFrame.height = 100.f;
				currentFrame.left += 100.f;
				if (currentFrame.left >= 500.f) // Assuming 4 frames in the row
					currentFrame.left = 0;

				animationTimer.restart();
				shape.setTextureRect(currentFrame);
			}
		}
		else if (animState == MOVING_LEFT) {
			if (animationTimer.getElapsedTime().asSeconds() >= 0.1f) {
				currentFrame.top = 95.f; // MOVING_LEFT row for Fireboy
				currentFrame.left += 100.f;
				if (currentFrame.left >= 400.f) // Assuming 4 frames in the row
					currentFrame.left = 0;

				animationTimer.restart();
				shape.setTextureRect(currentFrame);
			}
		}
		else if (animState == JUMPING) {
			if (animationTimer.getElapsedTime().asSeconds() >= 0.1f) {
				currentFrame.top = 200.f; // JUMPING row for Fireboy
				currentFrame.left += 100.f;
				if (currentFrame.left >= 500.f) // Assuming 3 frames in the row
					currentFrame.left = 0;

				animationTimer.restart();
				shape.setTextureRect(currentFrame);
			}
		}
		else {
			animationTimer.restart();
		}
	}
};

class Watergirl : public Character {
public:
	Watergirl(const string& texturePath, const Vector2f& size, const Vector2f& position)
		: Character(texturePath, size, position, "Watergirl") { // Pass "Watergirl" as type
		currentFrame = IntRect(0, 0, 100, 100); // Initial frame for Watergirl
		shape.setTextureRect(currentFrame);
	}

	void updateAnimation() {
		if (animState == WATERGIRL_IDLE) {
			if (animationTimer.getElapsedTime().asSeconds() >= 0.1f) {
				currentFrame.top = 400.f; // IDLE row (4th row, starting from 0)
				currentFrame.left += 100.f;
				if (currentFrame.left >= 100.f) // Wrap around after 4 frames
					currentFrame.left = 0;

				animationTimer.restart();
				shape.setTextureRect(currentFrame);
			}
		}
		else if (animState == WATERGIRL_MOVING_RIGHT) {
			if (animationTimer.getElapsedTime().asSeconds() >= 0.1f) {
				currentFrame.top = 95.f; // MOVING_RIGHT row for Watergirl
				currentFrame.height = 92.f; // MOVING_RIGHT row for Watergirl
				currentFrame.left += 100.f;
				if (currentFrame.left >= 500.f) // Assuming 4 frames in the row
					currentFrame.left = 0;

				animationTimer.restart();
				shape.setTextureRect(currentFrame);
			}
		}
		else if (animState == WATERGIRL_MOVING_LEFT) {
			if (animationTimer.getElapsedTime().asSeconds() >= 0.1f) {
				currentFrame.top = 0; // MOVING_LEFT row for Watergirl
				currentFrame.left += 100.f;
				if (currentFrame.left >= 400.f) // Assuming 4 frames in the row
					currentFrame.left = 0;

				animationTimer.restart();
				shape.setTextureRect(currentFrame);
			}
		}
		else if (animState == WATERGIRL_JUMPING) {
			if (animationTimer.getElapsedTime().asSeconds() >= 0.1f) {
				currentFrame.top = 191.f; // JUMPING row for Watergirl
				currentFrame.height = 93.f;
				currentFrame.left += 100.f;
				if (currentFrame.left >= 500.f) // Assuming 3 frames in the row
					currentFrame.left = 0;

				animationTimer.restart();
				shape.setTextureRect(currentFrame);
			}
		}
		else {
			animationTimer.restart();
		}
	}
};
void enterthename(RenderWindow& window, string& name1, string& name2);
int state = -1;
int difficulty = 0; // 0: Easy, 1: Medium, 2: Hard
void resolveCollision(Character& player, const FloatRect& objectBounds, Vector2f& movement) {
	FloatRect playerBounds = player.shape.getGlobalBounds();

	float overlapLeft = playerBounds.left + playerBounds.width - objectBounds.left - 30;
	float overlapRight = objectBounds.left + objectBounds.width - playerBounds.left - 30;
	float overlapTop = playerBounds.top + playerBounds.height - objectBounds.top - 10;
	float overlapBottom = objectBounds.top + objectBounds.height - playerBounds.top - 10;

	// Resolve collision based on the direction of movement
	if (movement.x > 0 && overlapLeft < overlapRight && overlapLeft < overlapTop && overlapLeft < overlapBottom) {
		player.shape.move(-overlapLeft, 0); // Move left
	}
	else if (movement.x < 0 && overlapRight < overlapLeft && overlapRight < overlapTop && overlapRight < overlapBottom) {
		player.shape.move(overlapRight, 0); // Move right
	}
	else if (movement.y > 0 && overlapTop < overlapLeft && overlapTop < overlapRight && overlapTop < overlapBottom) {
		player.shape.move(0, -overlapTop); // Move up
		player.isGrounded = true; // Set grounded state
		player.velocity.y = 0; // Reset vertical velocity
	}
	else if (movement.y < 0 && overlapBottom < overlapLeft && overlapBottom < overlapRight && overlapBottom < overlapTop) {
		player.shape.move(0, overlapBottom); // Move down
	}
}


void score(Sprite Red[], RectangleShape& player1, Sprite Blue[], RectangleShape& player2, int& ScR, int& ScB, Text& ScboardR, Text& ScboardB)
{
	//red daimonds
	for (int i = 0; i < 5; ++i)
	{
		if (player1.getGlobalBounds().intersects(Red[i].getGlobalBounds()))
		{
			Red[i].setScale(0, 0);
			++ScR;
			ScboardR.setString("Score: " + to_string(ScR));
		}
	}
	//blue daimonds
	for (int i = 0; i < 5; ++i)
	{
		if (player2.getGlobalBounds().intersects(Blue[i].getGlobalBounds()))
		{
			Blue[i].setScale(0, 0);
			++ScB;
			ScboardB.setString("Score: " + to_string(ScB));
		}
	}
}


GameObject TimerBox;

int main() {
	string name1;
	string name2;
	bool ongoing = true;
	Font font;
	font.loadFromFile("C:/Windows/Fonts/Arial.ttf");

	Music music1, music2;
	music1.openFromFile("videoplayback.mp3");
	music2.openFromFile("videoplayback.ogg.mp3");
	while (ongoing) {
		if (state < 4) {
			music2.stop();
			music1.setLoop(true);
			music1.play();
		}
		else if (state > 3) {
			music1.stop();
			music2.setLoop(true);
			music2.play();
		}

		if (state == -1) {
			RenderWindow window(VideoMode(1920, 1080), "Game");
			menu Menu(1921, 1080);
			Texture photo;
			photo.loadFromFile("bgr.jpg");
			Sprite ph;
			ph.setTexture(photo);

			while (window.isOpen() && state == -1) {
				Event event;
				while (window.pollEvent(event)) {
					if (event.type == Event::Closed) {
						window.close();
						ongoing = false;
					}

					if (event.type == Event::KeyPressed) {
						if (event.key.code == Keyboard::Up)
							Menu.moveup();
						if (event.key.code == Keyboard::Down)
							Menu.movedown();
						if (event.key.code == Keyboard::Enter) {
							if (Menu.pressed() == 0) // play
								state = 0;
							if (Menu.pressed() == 1) // history
								state = 1;
							if (Menu.pressed() == 2) // exit
								state = 2;
						}
					}
				}

				window.clear();
				window.draw(ph);
				Menu.draw(window);
				window.display();

				if (state == 2) {
					window.close();
					break;
				}
				if (state == 0) {
					enterthename(window, name1, name2);

					if (window.isOpen()) {  // Check if the window is still open 
						window.clear();
						state = 3; // Move to difficulty selection state
					}
				}

			}
		}
		if (state == 1) {

			RenderWindow window(VideoMode(1921, 1080), "History");
			Texture his;
			his.loadFromFile("bgr.jpg");
			Sprite history;
			history.setTexture(his);
			Font font;
			font.loadFromFile("C:/Windows/Fonts/Arial.ttf");
			Text text[100];
			for (int i = 0; i < 100; i++) {
				text[i].setFont(font);
				text[i].setCharacterSize(50);
				text[i].setFillColor(Color::Red);
			}
			ifstream infile("History.txt");
			vector<pair<string, int>> entries;
			string line;
			while (getline(infile, line, '*')) {
				size_t delimPos = line.find_last_of(' '); // Split by last space
				if (delimPos != string::npos) {
					string name = line.substr(0, delimPos); // Text part
					int score = stoi(line.substr(delimPos + 1)); // Convert to number
					entries.emplace_back(name, score); // Store as pair
				}
			}
			infile.close();

			// Sort entries by score in ascending order
			sort(entries.begin(), entries.end(), [](pair<string, int> a, pair<string, int> b) {
				return a.second < b.second;
				});

			/// Set text objects with the sorted entries
			for (int i = 0; i < entries.size() && i < 100; i++) {
				text[i].setString(entries[i].first + " " + to_string(entries[i].second) + " seconds");
				text[i].setPosition(100, 150 + 150 * i);
			}


			while (window.isOpen()) {
				Event event;
				while (window.pollEvent(event)) {
					if (event.type == Event::Closed) {
						window.close();
						ongoing = false;
					}
					if (Keyboard::isKeyPressed(Keyboard::Escape)) {
						window.close();
						state = -1;
					}
				}

				// Scroll logic
				if (Keyboard::isKeyPressed(Keyboard::Up)) {
					if (text[0].getPosition().y <= 5) {
						for (int i = 0; i < entries.size(); i++) {
							text[i].move(0, 10);
						}
					}
				}
				if (Keyboard::isKeyPressed(Keyboard::Down)) {
					if (text[entries.size() - 1].getPosition().y >= window.getPosition().y + 1080) {
						for (int i = 0; i < entries.size(); i++) {
							text[i].move(0, -10);
						}
					}
				}

				// Rendering
				window.clear();
				window.draw(history); // Draw the background
				for (size_t i = 0; i < entries.size(); i++) {
					window.draw(text[i]);
				}
				window.display();
			}





		}

		if (state == 3) {
			RenderWindow window(VideoMode(1921, 1080), "Select Difficulty");
			Diffculties Diffculties(1921, 1080);
			Texture photo1;
			photo1.loadFromFile("bgr.jpg");
			Sprite ph1;
			ph1.setTexture(photo1);

			while (window.isOpen()) {
				Event event;
				while (window.pollEvent(event)) {
					if (event.type == Event::Closed) {
						window.close();
						ongoing = false;
						break;
					}
					if (event.type == Event::KeyPressed) {
						if (event.key.code == Keyboard::Up)
							Diffculties.moveup();
						if (event.key.code == Keyboard::Down)
							Diffculties.movedown();
						if (event.key.code == Keyboard::Enter) {
							if (Diffculties.pressed() == 0) // easy
								state = 4;
							if (Diffculties.pressed() == 1) // medium
								state = 5;
							if (Diffculties.pressed() == 2) // hard
								state = 6;
							window.close();
							break;
						}
					}
				}

				window.clear();
				window.draw(ph1);
				Diffculties.draw(window);
				window.display();
			}
		}
		if (state == 4)
		{
			RenderWindow window(VideoMode(1921, 1080), "Fireboy and WaterGirl");
			window.setFramerateLimit(90);
			TextureManager textureManager;
			// Create Fireboy and Watergirl
			Fireboy fireboy("fireboy.png", Vector2f(100, 100), Vector2f(700, 720 - 100)); // 720 - height
			Watergirl watergirl("watergirl_sprite.png", Vector2f(100, 100), Vector2f(700, 720 - 100)); // 720 - height
			Clock clock;
			GameObject pauseButton;
			initializeGameObject(pauseButton, textureManager.getTexture("Pause_Button.jpeg"), { 22, 22 }, { 0.2f, 0.2f });

			// Pause overlay
			RectangleShape pauseOverlay(Vector2f(1921, 1080));
			pauseOverlay.setFillColor(Color(0, 0, 0, 150)); // Semi-transparent black
			bool isPaused = false;
			Text homeButtonText;
			homeButtonText.setFont(font);
			homeButtonText.setString("Home");
			homeButtonText.setCharacterSize(60);
			homeButtonText.setFillColor(Color::Red);
			homeButtonText.setPosition(1921 / 2 - 100, 1080 / 2 - 100);

			// Resume button as text
			Text resumeButtonText;
			resumeButtonText.setFont(font);
			resumeButtonText.setString("Resume");
			resumeButtonText.setCharacterSize(60);
			resumeButtonText.setFillColor(Color::Green);
			resumeButtonText.setPosition(1921 / 2 - 100, 1080 / 2 + 50);
			while (window.isOpen()) {
				Event event;

				while (window.pollEvent(event)) {
					if (event.type == Event::Closed) {
						window.close();
						ongoing = false;
					}
					if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Left) {
						Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

						// If clicked on pause button
						if (pauseButton.sprite.getGlobalBounds().contains(mousePos)) {
							isPaused = !isPaused;
						}

						// If clicked on "Resume" button
						if (isPaused && resumeButtonText.getGlobalBounds().contains(mousePos)) {
							isPaused = false;
						}

						// If clicked on "Home" button (set state to -1)
						if (homeButtonText.getGlobalBounds().contains(mousePos)) {
							window.close();
							state = -1;
						}
					}
				}

				// If the game is not paused, process input and update characters
				if (!isPaused) {
					// Fireboy movement and animation
					if (Keyboard::isKeyPressed(Keyboard::Key::A)) {
						fireboy.move(-2.f, 0.f);
						fireboy.animState = MOVING_LEFT;
					}
					else if (Keyboard::isKeyPressed(Keyboard::Key::D)) {
						fireboy.move(2.f, 0.f);
						fireboy.animState = MOVING_RIGHT;
					}

					// Watergirl movement and animation
					if (Keyboard::isKeyPressed(Keyboard::Key::Left)) { // Move left
						watergirl.move(-2.f, 0.f);
						watergirl.animState = WATERGIRL_MOVING_LEFT;
					}
					else if (Keyboard::isKeyPressed(Keyboard::Key::Right)) { // Move right
						watergirl.move(2.f, 0.f);
						watergirl.animState = WATERGIRL_MOVING_RIGHT;
					}

					// Jumping logic for both characters
					if (Keyboard::isKeyPressed(Keyboard::Key::Up)) { // Jump
						watergirl.jump();
						if (watergirl.velocity.y < 0) // Jumping upward
							watergirl.animState = WATERGIRL_JUMPING;
					}

					// Reset to idle state if no movement is detected
					if (!Keyboard::isKeyPressed(Keyboard::Key::Left) &&
						!Keyboard::isKeyPressed(Keyboard::Key::Right) &&
						watergirl.isGrounded) {
						watergirl.animState = WATERGIRL_IDLE;
					}

					// Update character positions and animations
					float deltaTime = clock.restart().asSeconds();
					fireboy.updateAnimations();
					watergirl.updateAnimation();
					fireboy.update(deltaTime);
					watergirl.update(deltaTime);
				}

				// Clear the window
				window.clear();

				// Draw pause button
				window.draw(pauseButton.sprite);

				// If the game is paused, show the pause overlay and buttons
				if (isPaused) {
					window.draw(pauseOverlay);        // Draw the overlay (e.g., a semi-transparent screen)
					window.draw(homeButtonText);      // Draw the "Home" button
					window.draw(resumeButtonText);    // Draw the "Resume" button
				}

				window.draw(fireboy.shape);
				window.draw(watergirl.shape);
				window.display();
			}
		}


		else if (state == 5)
		{
			int ScR = 0;
			int ScB = 0;
			// Create red and blue diamond sprites
			Sprite Red[5]; // Assume these are initialized properly
			Sprite Blue[5]; // Assume these are initialized properly

			RenderWindow window(VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Fireboy and Watergirl", Style::Titlebar | Style::Close);
			window.setFramerateLimit(70);
			Fireboy fireboy("fireboy.png", Vector2f(100, 100), Vector2f(1680, 1010));
			Watergirl watergirl("watergirl_sprite.png", Vector2f(100, 100), Vector2f(150, 1010));
			Clock clock; // For delta time

			TextureManager textureManager;

			// Background initialization
			vector<GameObject> backgrounds;
			vector<Vector2f> positions = {
				{0, 0}, {250, 0}, {500, 0}, {750, 0}, {1000, 0}, {1250, 0}, {1500, 0}, {1650, 0},
				{0, 193.5}, {250, 193.5}, {500, 193.5}, {750, 193.5}, {1000, 193.5}, {1250, 193.5}, {1500, 193.5},
				{1650, 193.5}, {0, 386.5}, {250, 386.5}, {500, 386.5}, {750, 386.5}, {1000, 386.5}, {1250, 386.5}, {1500, 386.5},
				{1650, 386.5}, {0, 579.5}, {250, 579.5}, {500, 579.5}, {750, 579.5}, {1000, 579.5}, {1250, 579.5}, {1500, 579.5},
				{1650, 579.5}, {0, 719}, {250, 719}, {500, 719}, {750, 719}, {1000, 719}, {1250, 719}, {1500, 719}, {1650, 719},
				{0, 912.5}, {250, 912.5}, {500, 912.5}, {750, 912.5}, {1000, 912.5}, {1250, 912.5}, {1500, 912.5}, {1650, 912.5}
			};

			const Texture& backgroundTexture = textureManager.getTexture(img_path + "background2.png");
			for (const auto& position : positions) {
				GameObject background;
				initializeGameObject(background, backgroundTexture, position);
				backgrounds.push_back(background);
			}
			// Top & Bottom Background Blocks
			vector<GameObject> Top_bottom_backgrounds;
			vector<Vector2f> tp_bt_positions =
			{
						{0, 0}, {400, 0}, {800, 0}, {1200, 0}, {1600, 0}, {0, 1020}, {400, 1020}, {800, 1020}, {1200, 1020}, {1600, 1020},
				{0, 700}, {1540, 700}, {470, 550}, {1150, 550}, {450, 820}, {1340, 820}

			};
			const Texture& Top_bottom_backgroundsTexture = textureManager.getTexture(img_path + "Tb8.png");
			for (const auto& pos : tp_bt_positions)
			{
				GameObject Top_bottom_background;
				initializeGameObject(Top_bottom_background, Top_bottom_backgroundsTexture, pos);
				Top_bottom_backgrounds.push_back(Top_bottom_background);
			}

			// Small columns
			vector<GameObject> small_cols;
			vector<Vector2f> scols_positions =
			{
				 {1530, 300},{1530, 420},{1335, 360}, {870, 675}, {1210, 800}, {1020, 675}, {1355, 200}, {1000, 300},
				 {686, 990}, {1200, 990}, {1730, 960}, {200, 960}

			};
			const Texture& small_colsTexture = textureManager.getTexture(img_path + "Tb9.png");
			for (const auto& j : scols_positions)
			{
				GameObject small_col;
				initializeGameObject(small_col, small_colsTexture, j);
				small_cols.push_back(small_col);
			}

			// Columns
			vector<GameObject> cols;
			vector<Vector2f> col_positions =
			{

				{-3, 0}, {-3, 400}, {-3, 700}, {1884, 0}, {1884, 400}, {1884, 700}, {650, 20},{ 650, 150 }, { 1330, 620 }, { 1330, 250 },
				{1330, 150}, {990, 620}, {990, 300}


			};
			const Texture& colsTexture = textureManager.getTexture(img_path + "col2.png");
			for (const auto& p : col_positions)
			{
				GameObject col_background;
				initializeGameObject(col_background, colsTexture, p);
				cols.push_back(col_background);
			}

			// red diamonds
			vector<GameObject> rdiamonds;
			vector<Vector2f> rdiamonds_positions =
			{
				{1430, 460}, {1600,750}, { 1210, 900 },  {1470, 940}
			};
			const Texture& rdiamondsTexture = textureManager.getTexture(img_path + "red_diamond.png");
			for (const auto& p : rdiamonds_positions)
			{
				GameObject rdiamond;
				initializeGameObject(rdiamond, rdiamondsTexture, p);
				rdiamonds.push_back(rdiamond);
			}
			for (int i = 0; i < rdiamonds_positions.size(); ++i) {
				Red[i].setTexture(rdiamondsTexture);
				Red[i].setPosition(rdiamonds_positions[i]);
			}

			// blue diamonds
			vector<GameObject> bdiamonds;
			vector<Vector2f> bdiamonds_positions =
			{

				{505, 940}, {750, 900}, {540, 460}, {65, 650}


			};
			const Texture& bdiamondsTexture = textureManager.getTexture(img_path + "blue_diamond.png");
			for (const auto& p : bdiamonds_positions)
			{
				GameObject bdiamond;
				initializeGameObject(bdiamond, bdiamondsTexture, p);
				bdiamonds.push_back(bdiamond);
			}
			for (int i = 0; i < bdiamonds_positions.size(); ++i) {
				Blue[i].setTexture(bdiamondsTexture);
				Blue[i].setPosition(bdiamonds_positions[i]);
			}


			GameObject hill;
			initializeGameObject(hill, textureManager.getTexture(img_path + "background1.png"), Vector2f(880, 880), { 1.0f, 1.0f });

			GameObject lake;
			initializeGameObject(lake, textureManager.getTexture(img_path + "w.png"), Vector2f(475, 535), { 0.5f, 0.5f });

			GameObject lake3;
			initializeGameObject(lake3, textureManager.getTexture(img_path + "w.png"), Vector2f(400, 1005), { 0.7f, 0.7f });

			GameObject lake5;
			initializeGameObject(lake5, textureManager.getTexture(img_path + "Fire_Lake1.png"), Vector2f(1370, 1010), { 0.15f, 0.15f });


			GameObject lake2;
			initializeGameObject(lake2, textureManager.getTexture(img_path + "glake1.png"), Vector2f(150, 700), { 0.90f, 0.90f });


			GameObject lake6;
			initializeGameObject(lake6, textureManager.getTexture(img_path + "glake1.png"), Vector2f(1650, 685), { 0.90f, 0.90f });

			GameObject lake4;
			initializeGameObject(lake4, textureManager.getTexture(img_path + "Fire_Lake1.png"), Vector2f(1357, 545), { 0.15f, 0.15f });

			GameObject redExitDoor;
			initializeGameObject(redExitDoor, textureManager.getTexture(img_path + "Fire_Exit_Doooor.png"), Vector2f(1180, 425));

			GameObject blueExitDoor;
			initializeGameObject(blueExitDoor, textureManager.getTexture(img_path + "Water_Door2.png"), Vector2f(695, 425));




			GameObject pauseButton;
			initializeGameObject(pauseButton, textureManager.getTexture("Pause_Button.jpeg"), { 1785, 25 }, { 0.2f, 0.2f });


			// Pause overlay
			RectangleShape pauseOverlay(Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT));
			pauseOverlay.setFillColor(Color(0, 0, 0, 150)); // Semi-transparent black

			bool isPaused = false;
			Font font5;



			font5.loadFromFile(font_path + "TrajanPro-Bold.otf"); // Load a font



			Text homeButtonText;
			homeButtonText.setFont(font5);
			homeButtonText.setString("Home");
			homeButtonText.setCharacterSize(60);
			homeButtonText.setFillColor(Color::Red);
			homeButtonText.setPosition(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 100);

			// Resume button as text
			Text resumeButtonText;
			resumeButtonText.setFont(font5);
			resumeButtonText.setString("Resume");
			resumeButtonText.setCharacterSize(60);
			resumeButtonText.setFillColor(Color::Green);
			resumeButtonText.setPosition(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 50);

			Font font1;
			if (!font1.loadFromFile(font_path + "TrajanPro-Bold.otf")) {
				cerr << "Failed to load font!" << endl;
				return -1;
			}

			initializeGameObject(TimerBox, textureManager.getTexture(img_path + "The_Timer_Box.png"), Vector2f(800, 0), { 1.2f, 1.2f });

			Text timerText;
			timerText.setFont(font1);
			timerText.setCharacterSize(50);
			timerText.setFillColor(Color::Yellow);
			timerText.setPosition(TimerBox.position.x + (TimerBox.sprite.getGlobalBounds().width / 2) - 55, TimerBox.position.y); // Centered in TimerBox


			const int startingTime = 120; // Set the countdown duration here
			Clock clocks;
			int remainingTime = startingTime;





			while (window.isOpen()) {


				// Create score board text
				Font font;
				if (!font.loadFromFile(font_path + "TrajanPro-Bold.otf")) {
					cerr << "Failed to load font!" << endl;
					return -1;
				}

				Text fireboyScoreText("Fireboy: 0", font, 30);
				Text watergirlScoreText("Watergirl: 0", font, 30);
				fireboyScoreText.setFillColor(Color::Red);
				watergirlScoreText.setFillColor(Color::Cyan);
				watergirlScoreText.setPosition(0, 5);
				fireboyScoreText.setPosition(0, 60);

				font.loadFromFile(font_path + "TrajanPro-Bold.otf"); // Load a font
				Text ScboardB;
				ScboardB.setFont(font);
				ScboardB.setCharacterSize(24);
				ScboardB.setFillColor(Color::Blue);
				ScboardB.setString("Score: " + to_string(ScB));
				ScboardB.setPosition(0, 0);

				Text ScboardR;
				ScboardR.setFont(font);
				ScboardR.setCharacterSize(24);
				ScboardR.setFillColor(Color::Red);
				ScboardR.setString("Score: " + to_string(ScR));
				ScboardR.setPosition(0, 20);

				Event events;
				while (window.pollEvent(events)) {
					if (events.type == Event::Closed) {
						window.close();
					}
					if (events.type == Event::MouseButtonPressed && events.mouseButton.button == Mouse::Left)
					{
						Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

						// If clicked on pause button
						if (pauseButton.sprite.getGlobalBounds().contains(mousePos))
						{
							isPaused = !isPaused;
						}

						// If clicked on "Resume" button
						if (isPaused && resumeButtonText.getGlobalBounds().contains(mousePos))
						{
							isPaused = false; // Resume the game
						}
						// If clicked on "home" button
						if (isPaused && homeButtonText.getGlobalBounds().contains(mousePos))
						{
							window.close();
							state = -1;// Resume the game
						}
					}
				}

				score(Red, fireboy.shape, Blue, watergirl.shape, ScR, ScB, ScboardR, ScboardB);

				if (!isPaused) {
					// Main loop
					Vector2f fireboyMovement(0.f, 0.f);
					if (Keyboard::isKeyPressed(Keyboard::Key::A)) {
						fireboyMovement.x -= 5.f; // Move left
						fireboy.animState = MOVING_LEFT;
					}
					else if (Keyboard::isKeyPressed(Keyboard::Key::D)) {
						fireboyMovement.x += 5.f; // Move right
						fireboy.animState = MOVING_RIGHT;
					}

					// Jump logic
					if (Keyboard::isKeyPressed(Keyboard::Key::W) && fireboy.isGrounded) {
						fireboy.jump();
						fireboy.animState = JUMPING;
					}


					// Reset to IDLE only if on the ground and not moving
					if (!Keyboard::isKeyPressed(Keyboard::Key::A) &&
						!Keyboard::isKeyPressed(Keyboard::Key::D) &&
						fireboy.isGrounded) {
						fireboy.animState = IDLE;
					}

					// Move Fireboy horizontally
					fireboy.move(fireboyMovement.x, 0);

					// Resolve horizontal collisions for Fireboy
					for (const auto& col : cols) {
						if (fireboy.shape.getGlobalBounds().intersects(col.sprite.getGlobalBounds())) {
							resolveCollision(fireboy, col.sprite.getGlobalBounds(), fireboyMovement);
						}
					}
					for (const auto& small_col : small_cols) {
						if (fireboy.shape.getGlobalBounds().intersects(small_col.sprite.getGlobalBounds())) {
							resolveCollision(fireboy, small_col.sprite.getGlobalBounds(), fireboyMovement);
						}
					}
					for (const auto& Top_bottom_background : Top_bottom_backgrounds) {
						if (fireboy.shape.getGlobalBounds().intersects(Top_bottom_background.sprite.getGlobalBounds())) {
							resolveCollision(fireboy, Top_bottom_background.sprite.getGlobalBounds(), fireboyMovement);
						}
					}




					// Apply gravity
					fireboy.velocity.y += 0.1f; // Apply gravity
					fireboyMovement.y = fireboy.velocity.y;

					// Move Fireboy vertically
					fireboy.move(0, fireboyMovement.y);

					// Resolve vertical collisions for Fireboy
					bool isOnGround = false;
					for (const auto& col : cols) {
						if (fireboy.shape.getGlobalBounds().intersects(col.sprite.getGlobalBounds())) {
							FloatRect playerBounds = fireboy.shape.getGlobalBounds();
							FloatRect colBounds = col.sprite.getGlobalBounds();

							// Check if Fireboy is directly on top of the object
							if (playerBounds.top + playerBounds.height <= colBounds.top + 5.0f && fireboyMovement.y > 0) {
								isOnGround = true;
								fireboy.velocity.y = 0; // Stop downward movement
							}
							resolveCollision(fireboy, colBounds, fireboyMovement);
						}
					}
					for (const auto& small_col : small_cols) {
						if (fireboy.shape.getGlobalBounds().intersects(small_col.sprite.getGlobalBounds())) {

							if (fireboyMovement.y > 0) {
								isOnGround = true;

							}
							resolveCollision(fireboy, small_col.sprite.getGlobalBounds(), fireboyMovement);
						}
					}
					for (const auto& Top_bottom_background : Top_bottom_backgrounds) {
						if (fireboy.shape.getGlobalBounds().intersects(Top_bottom_background.sprite.getGlobalBounds())) {
							FloatRect playerBounds = fireboy.shape.getGlobalBounds();
							FloatRect colBounds = Top_bottom_background.sprite.getGlobalBounds();

							// Check if Fireboy is directly on top of the object
							if (playerBounds.top + playerBounds.height <= colBounds.top + 20 && fireboyMovement.y > 0) {
								isOnGround = true;
								fireboy.velocity.y = 0; // Stop downward movement
							}
							resolveCollision(fireboy, colBounds, fireboyMovement);
						}
					}
					if (fireboy.shape.getGlobalBounds().intersects(hill.sprite.getGlobalBounds())) {
						FloatRect playerBounds = fireboy.shape.getGlobalBounds();
						FloatRect colBounds = hill.sprite.getGlobalBounds();

						// Check if Fireboy is directly on top of the object
						if (playerBounds.top + playerBounds.height <= colBounds.top + 5.0f && fireboyMovement.y > 0) {
							isOnGround = true;
							fireboy.velocity.y = 0; // Stop downward movement
						}
						resolveCollision(fireboy, colBounds, fireboyMovement);
					}


					// Update grounded state
					fireboy.isGrounded = isOnGround;

					// Ensure Fireboy can only jump when directly on top of an object
					if (!fireboy.isGrounded) {
						fireboy.velocity.y += 0.1f; // Apply gravity
					}

					Vector2f watergirlMovement(0.f, 0.f);
					if (Keyboard::isKeyPressed(Keyboard::Key::Left)) {
						watergirlMovement.x -= 5.f; // Move left
						watergirl.animState = WATERGIRL_MOVING_LEFT;
					}
					else if (Keyboard::isKeyPressed(Keyboard::Key::Right)) {
						watergirlMovement.x += 5.f; // Move right
						watergirl.animState = WATERGIRL_MOVING_RIGHT;
					}
					if (Keyboard::isKeyPressed(Keyboard::Key::Up)) {
						watergirl.jump();
						if (watergirl.velocity.y < 0) {
							watergirl.animState = WATERGIRL_JUMPING;
						}
					}
					// Reset to IDLE only if on the ground and not moving
					if (!Keyboard::isKeyPressed(Keyboard::Key::Left) &&
						!Keyboard::isKeyPressed(Keyboard::Key::Right) &&
						watergirl.isGrounded) {
						watergirl.animState = WATERGIRL_IDLE; // Reset to IDLE state when not moving
					}

					watergirl.move(watergirlMovement.x, 0);

					// Resolve horizontal collisions for Fireboy
					for (const auto& col : cols) {
						if (watergirl.shape.getGlobalBounds().intersects(col.sprite.getGlobalBounds())) {
							resolveCollision(watergirl, col.sprite.getGlobalBounds(), watergirlMovement);
						}
					}
					for (const auto& small_col : small_cols) {
						if (watergirl.shape.getGlobalBounds().intersects(small_col.sprite.getGlobalBounds())) {
							resolveCollision(watergirl, small_col.sprite.getGlobalBounds(), watergirlMovement);
						}
					}
					for (const auto& Top_bottom_background : Top_bottom_backgrounds) {
						if (watergirl.shape.getGlobalBounds().intersects(Top_bottom_background.sprite.getGlobalBounds())) {
							resolveCollision(watergirl, Top_bottom_background.sprite.getGlobalBounds(), watergirlMovement);
						}
					}

					// Move Watergirl
					watergirl.velocity.y += 0.1f; // Apply gravity
					watergirlMovement.y = watergirl.velocity.y;

					// Move Fireboy vertically
					watergirl.move(0, watergirlMovement.y);


					// Update characters
					float deltaTime = clock.restart().asSeconds();
					fireboy.updateAnimations();
					watergirl.updateAnimation();
					fireboy.update(deltaTime);
					watergirl.update(deltaTime);

					for (const auto& col : cols) {
						isOnGround = false;
						if (watergirl.shape.getGlobalBounds().intersects(col.sprite.getGlobalBounds())) {
							FloatRect playerBounds = watergirl.shape.getGlobalBounds();
							FloatRect colBounds = col.sprite.getGlobalBounds();

							// Check if Fireboy is directly on top of the object
							if (playerBounds.top + playerBounds.height <= colBounds.top + 5.0f && watergirlMovement.y > 0) {
								isOnGround = true;
								watergirl.velocity.y = 0; // Stop downward movement
							}
							resolveCollision(watergirl, colBounds, watergirlMovement);
						}
					}
					for (const auto& small_col : small_cols) {
						if (watergirl.shape.getGlobalBounds().intersects(small_col.sprite.getGlobalBounds())) {

							if (watergirlMovement.y > 0) {
								isOnGround = true;

							}
							resolveCollision(watergirl, small_col.sprite.getGlobalBounds(), watergirlMovement);
						}
					}
					for (const auto& Top_bottom_background : Top_bottom_backgrounds) {
						if (watergirl.shape.getGlobalBounds().intersects(Top_bottom_background.sprite.getGlobalBounds())) {
							FloatRect playerBounds = watergirl.shape.getGlobalBounds();
							FloatRect colBounds = Top_bottom_background.sprite.getGlobalBounds();

							// Check if Fireboy is directly on top of the object
							if (playerBounds.top + playerBounds.height <= colBounds.top + 20 && watergirlMovement.y > 0) {
								isOnGround = true;
								watergirl.velocity.y = 0; // Stop downward movement
							}
							resolveCollision(watergirl, colBounds, watergirlMovement);
						}
					}

					// Check for intersections
					// Check for Fireboy's collision with lakes from the top
					bool gameOver = false;

					// Check for Fireboy's collision with lakes
					if (fireboy.shape.getGlobalBounds().intersects(lake.sprite.getGlobalBounds()) ||
						fireboy.shape.getGlobalBounds().intersects(lake3.sprite.getGlobalBounds())) {
						gameOver = true; // Trigger game over on any intersection
					}

					// Check for Watergirl's collision with lakes
					if (watergirl.shape.getGlobalBounds().intersects(lake2.sprite.getGlobalBounds()) ||
						watergirl.shape.getGlobalBounds().intersects(lake4.sprite.getGlobalBounds())) {
						gameOver = true; // Trigger game over on any intersection
					}
					// Check for Fireboy's collision with lakes
					if (fireboy.shape.getGlobalBounds().intersects(lake.sprite.getGlobalBounds())) {
						FloatRect fireboyBounds = fireboy.shape.getGlobalBounds();
						FloatRect lakeBounds = lake.sprite.getGlobalBounds();

						// Check if Fireboy is on top of the lake
						if (fireboyBounds.top + fireboyBounds.height <= lakeBounds.top + 7.6f) {
							gameOver = true;
						}
					}
					if (fireboy.shape.getGlobalBounds().intersects(lake.sprite.getGlobalBounds()) || fireboy.shape.getGlobalBounds().intersects(lake3.sprite.getGlobalBounds())) {
						gameOver = true;
					}if (watergirl.shape.getGlobalBounds().intersects(lake5.sprite.getGlobalBounds()) || watergirl.shape.getGlobalBounds().intersects(lake4.sprite.getGlobalBounds())) {
						gameOver = true;
					}if (fireboy.shape.getGlobalBounds().intersects(lake2.sprite.getGlobalBounds()) || fireboy.shape.getGlobalBounds().intersects(lake6.sprite.getGlobalBounds())) {
						gameOver = true;
					}if (watergirl.shape.getGlobalBounds().intersects(lake2.sprite.getGlobalBounds()) || watergirl.shape.getGlobalBounds().intersects(lake6.sprite.getGlobalBounds())) {
						gameOver = true;
					}

					// Handle game over logic
					if (gameOver) {
						window.close();
						state = -1;
					}

					// Check for Watergirl's collision with lakes
					if (watergirl.shape.getGlobalBounds().intersects(lake2.sprite.getGlobalBounds())) {
						FloatRect watergirlBounds = watergirl.shape.getGlobalBounds();
						FloatRect lake2Bounds = lake2.sprite.getGlobalBounds();

						// Check if Watergirl is on top of the lake
						if (watergirlBounds.top + watergirlBounds.height <= lake2Bounds.top + 7.6f) {
							gameOver = true;
						}
					}

					if (watergirl.shape.getGlobalBounds().intersects(lake4.sprite.getGlobalBounds())) {
						FloatRect watergirlBounds = watergirl.shape.getGlobalBounds();
						FloatRect lake4Bounds = lake4.sprite.getGlobalBounds();

						// Check if Watergirl is on top of the lake
						if (watergirlBounds.top + watergirlBounds.height <= lake4Bounds.top + 17.0f) {
							gameOver = true;
						}
					}

					int capturedTime = 0; // Variable to store the remaining time
					// End the game if a collision is detected
					if (gameOver || remainingTime == 0) {
						window.close();
						state = 9;
					}

					bool gameWin = false;

					// Define the center range

					// Check if Fireboy is in the center or within range of the red exit door
					capturedTime = static_cast<int>(clocks.getElapsedTime().asSeconds());

					if (watergirl.shape.getGlobalBounds().intersects(blueExitDoor.sprite.getGlobalBounds()) && fireboy.shape.getGlobalBounds().intersects(redExitDoor.sprite.getGlobalBounds())) {
						// Check if both characters are in range of their respective exit doors			
						gameWin = true;
						ofstream offile;
						offile.open("History.txt", ios::app);
						if (offile.is_open()) {
							offile << name1 << " & " << name2 << " " << capturedTime << "*" << endl;
							offile.close();
						}
						else {
							cout << "Unable to open file!" << endl;
						}


						// Inside your game loop or when you want to capture the time:


						// Now, capturedTime holds the remaining time at this specific point.
						std::cout << "Captured Remaining Time: " << capturedTime << " seconds\n";


					}
					//{470, 550}, {1150, 550}

					// Handle game win logic
					if (gameWin) {
						window.close();
						state = 8;
					}

					if (watergirl.shape.getGlobalBounds().intersects(hill.sprite.getGlobalBounds())) {
						FloatRect playerBounds = watergirl.shape.getGlobalBounds();
						FloatRect colBounds = hill.sprite.getGlobalBounds();

						// Check if Fireboy is directly on top of the object
						if (playerBounds.top + playerBounds.height <= colBounds.top + 5.0f && watergirlMovement.y > 0) {
							isOnGround = true;
							watergirl.velocity.y = 0; // Stop downward movement
						}
						resolveCollision(watergirl, colBounds, watergirlMovement);
					}


					// Update grounded state
					watergirl.isGrounded = isOnGround;

					// Ensure Fireboy can only jump when directly on top of an object
					if (!watergirl.isGrounded) {
						watergirl.velocity.y += 0.1f; // Apply gravity
					}

					// Rendering process
					window.clear();

					Time elapsed = clocks.getElapsedTime();
					remainingTime = startingTime - static_cast<int>(elapsed.asSeconds());
					if (remainingTime < 0) {
						remainingTime = 0;
					}
					int minutes = remainingTime / 60; // Calculate minutes
					int seconds = remainingTime % 60;  // Calculate remaining seconds

					// Format the timer string as MM:SS
					timerText.setString(
						(minutes < 10 ? "0" : "") + to_string(minutes) + ":" +
						(seconds < 10 ? "0" : "") + to_string(seconds)
					);


					for (const auto& background : backgrounds) {
						window.draw(background.sprite);
					}
					for (const auto& col : cols) {
						window.draw(col.sprite);
					}
					for (const auto& small_col : small_cols) {
						window.draw(small_col.sprite);
					}

					for (const auto& tp_bt : Top_bottom_backgrounds)
					{
						window.draw(tp_bt.sprite);
					}


					for (int i = 0; i < 4; ++i)
					{
						window.draw(Red[i]);
						window.draw(Blue[i]);
					}

					window.draw(ScboardB);
					window.draw(ScboardR);

					window.draw(TimerBox.sprite);

					window.draw(timerText);
					window.draw(pauseButton.sprite);
					window.draw(hill.sprite);
					window.draw(lake.sprite);
					window.draw(lake2.sprite);
					window.draw(lake3.sprite);
					window.draw(lake4.sprite);
					window.draw(lake6.sprite);

					window.draw(lake5.sprite);
					window.draw(redExitDoor.sprite);
					window.draw(blueExitDoor.sprite);

					window.draw(blueExitDoor.sprite);

					window.draw(fireboy.shape);
					window.draw(watergirl.shape);

				}
				if (isPaused)
				{
					window.draw(pauseOverlay);
					window.draw(homeButtonText);
					window.draw(resumeButtonText);
				}
				// Display the contents of the window
				window.display();
			}

		}
		else if (state == 6)
		{
			int ScR = 0;
			int ScB = 0;
			// Create red and blue diamond sprites
			Sprite Red[5]; // Assume these are initialized properly
			Sprite Blue[5]; // Assume these are initialized properly

			RenderWindow window(VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Fireboy and Watergirl", Style::Titlebar | Style::Close);
			window.setFramerateLimit(70);
			Fireboy fireboy("fireboy.png", Vector2f(100, 100), Vector2f(150, 1010));
			Watergirl watergirl("watergirl_sprite.png", Vector2f(100, 100), Vector2f(1680, 1010));
			Clock clock; // For delta time

			TextureManager textureManager;

			// Background initialization
			vector<GameObject> backgrounds;
			vector<Vector2f> positions = {
				{0, 0}, {250, 0}, {500, 0}, {750, 0}, {1000, 0}, {1250, 0}, {1500, 0}, {1650, 0},
				{0, 193.5}, {250, 193.5}, {500, 193.5}, {750, 193.5}, {1000, 193.5}, {1250, 193.5}, {1500, 193.5},
				{1650, 193.5}, {0, 386.5}, {250, 386.5}, {500, 386.5}, {750, 386.5}, {1000, 386.5}, {1250, 386.5}, {1500, 386.5},
				{1650, 386.5}, {0, 579.5}, {250, 579.5}, {500, 579.5}, {750, 579.5}, {1000, 579.5}, {1250, 579.5}, {1500, 579.5},
				{1650, 579.5}, {0, 719}, {250, 719}, {500, 719}, {750, 719}, {1000, 719}, {1250, 719}, {1500, 719}, {1650, 719},
				{0, 912.5}, {250, 912.5}, {500, 912.5}, {750, 912.5}, {1000, 912.5}, {1250, 912.5}, {1500, 912.5}, {1650, 912.5}
			};

			const Texture& backgroundTexture = textureManager.getTexture(img_path + "background2.png");
			for (const auto& position : positions) {
				GameObject background;
				initializeGameObject(background, backgroundTexture, position);
				backgrounds.push_back(background);
			}
			// Top & Bottom Background Blocks
			vector<GameObject> Top_bottom_backgrounds;
			vector<Vector2f> tp_bt_positions =
			{
				{0, 0}, {400, 0}, {800, 0}, {1200, 0}, {1600, 0}, {0, 1020}, {400, 1020}, {800, 1020}, {1200, 1020}, {1600, 1020},
				{0, 700}, {1540, 700}, {470, 550}, {1150, 550}, {250, 820}, {1340, 820}
			};
			const Texture& Top_bottom_backgroundsTexture = textureManager.getTexture(img_path + "Tb8.png");
			for (const auto& pos : tp_bt_positions)
			{
				GameObject Top_bottom_background;
				initializeGameObject(Top_bottom_background, Top_bottom_backgroundsTexture, pos);
				Top_bottom_backgrounds.push_back(Top_bottom_background);
			}

			// Small columns
			vector<GameObject> small_cols;
			vector<Vector2f> scols_positions =
			{
				{300, 300}, {1530, 300}, {680, 800}, {870, 675}, {1210, 800}, {1020, 675}, {525, 200}, {1355, 200}, {945, 300}, {850, 300}, {1040, 300},
				{680, 150}, {1210, 150}, {20, 940}, {1770, 940}, {940, 210}, {686, 990}, {1200, 990}
			};
			const Texture& small_colsTexture = textureManager.getTexture(img_path + "Tb9.png");
			for (const auto& j : scols_positions)
			{
				GameObject small_col;
				initializeGameObject(small_col, small_colsTexture, j);
				small_cols.push_back(small_col);
			}

			// Columns
			vector<GameObject> cols;
			vector<Vector2f> col_positions =
			{

				{-3, 0}, {-3, 400}, {-3, 700}, {1884, 0}, {1884, 400}, {1884, 700}, {650, 620}, {650, 250}, {650, 150}, {1330, 620}, {1330, 250},
				{1330, 150}, {990, 620}, {990, 300}, {270, 200}, {1650, 200}

			};
			const Texture& colsTexture = textureManager.getTexture(img_path + "col2.png");
			for (const auto& p : col_positions)
			{
				GameObject col_background;
				initializeGameObject(col_background, colsTexture, p);
				cols.push_back(col_background);
			}

			// red diamonds
			vector<GameObject> rdiamonds;
			vector<Vector2f> rdiamonds_positions =
			{
				{505, 940}, {750, 900}, {1430, 460},{1780, 400}, {1600,750}

			};
			const Texture& rdiamondsTexture = textureManager.getTexture(img_path + "red_diamond.png");
			for (const auto& p : rdiamonds_positions)
			{
				GameObject rdiamond;
				initializeGameObject(rdiamond, rdiamondsTexture, p);
				rdiamonds.push_back(rdiamond);
			}
			for (int i = 0; i < rdiamonds_positions.size(); ++i) {
				Red[i].setTexture(rdiamondsTexture);
				Red[i].setPosition(rdiamonds_positions[i]);
			}

			// blue diamonds
			vector<GameObject> bdiamonds;
			vector<Vector2f> bdiamonds_positions =
			{

				{ 1210, 900 },  {1455, 930}, {540, 460}, {85, 400}

			};
			const Texture& bdiamondsTexture = textureManager.getTexture(img_path + "blue_diamond.png");
			for (const auto& p : bdiamonds_positions)
			{
				GameObject bdiamond;
				initializeGameObject(bdiamond, bdiamondsTexture, p);
				bdiamonds.push_back(bdiamond);
			}
			for (int i = 0; i < bdiamonds_positions.size(); ++i) {
				Blue[i].setTexture(bdiamondsTexture);
				Blue[i].setPosition(bdiamonds_positions[i]);
			}

			// other game objects

			GameObject hill;
			initializeGameObject(hill, textureManager.getTexture(img_path + "background1.png"), Vector2f(880, 880), { 1.0f, 1.0f });

			GameObject lake;
			initializeGameObject(lake, textureManager.getTexture(img_path + "w.png"), Vector2f(475, 535), { 0.5f, 0.5f });

			GameObject lake3;
			initializeGameObject(lake3, textureManager.getTexture(img_path + "w.png"), Vector2f(1370, 1005), { 0.6f, 0.6f });

			GameObject lake2;
			initializeGameObject(lake2, textureManager.getTexture(img_path + "Fire_Lake1.png"), Vector2f(440, 1015), { 0.155f, 0.155f });

			GameObject lake4;
			initializeGameObject(lake4, textureManager.getTexture(img_path + "Fire_Lake1.png"), Vector2f(1360, 540), { 0.155f, 0.155f });

			GameObject redExitDoor;
			initializeGameObject(redExitDoor, textureManager.getTexture(img_path + "Fire_Exit_Doooor.png"), Vector2f(695, 425));

			GameObject blueExitDoor;
			initializeGameObject(blueExitDoor, textureManager.getTexture(img_path + "Water_Door2.png"), Vector2f(1180, 425));

			GameObject fan1;
			initializeGameObject(fan1, textureManager.getTexture(img_path + "fan.png"), Vector2f(50, 660));

			GameObject fan2;
			initializeGameObject(fan2, textureManager.getTexture(img_path + "fan.png"), Vector2f(1750, 660));

			GameObject air;
			initializeGameObject(air, textureManager.getTexture("Air.png"), Vector2f(1770, 280));
			GameObject Air;
			initializeGameObject(Air, textureManager.getTexture("Air.png"), Vector2f(70, 280));


			GameObject pauseButton;
			initializeGameObject(pauseButton, textureManager.getTexture("Pause_Button.jpeg"), { 1785, 25 }, { 0.2f, 0.2f });


			// Pause overlay
			RectangleShape pauseOverlay(Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT));
			pauseOverlay.setFillColor(Color(0, 0, 0, 150)); // Semi-transparent black

			bool isPaused = false;
			Font font5;



			font5.loadFromFile(font_path + "TrajanPro-Bold.otf"); // Load a font



			Text homeButtonText;
			homeButtonText.setFont(font5);
			homeButtonText.setString("Home");
			homeButtonText.setCharacterSize(60);
			homeButtonText.setFillColor(Color::Red);
			homeButtonText.setPosition(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 100);

			// Resume button as text
			Text resumeButtonText;
			resumeButtonText.setFont(font5);
			resumeButtonText.setString("Resume");
			resumeButtonText.setCharacterSize(60);
			resumeButtonText.setFillColor(Color::Green);
			resumeButtonText.setPosition(SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 + 50);

			Font font1;
			if (!font1.loadFromFile(font_path + "TrajanPro-Bold.otf")) {
				cerr << "Failed to load font!" << endl;
				return -1;
			}

			initializeGameObject(TimerBox, textureManager.getTexture(img_path + "The_Timer_Box.png"), Vector2f(800, 0), { 1.2f, 1.2f });

			Text timerText;
			timerText.setFont(font1);
			timerText.setCharacterSize(50);
			timerText.setFillColor(Color::Yellow);
			timerText.setPosition(TimerBox.position.x + (TimerBox.sprite.getGlobalBounds().width / 2) - 55, TimerBox.position.y); // Centered in TimerBox


			const int startingTime = 120; // Set the countdown duration here
			Clock clocks;
			int remainingTime = startingTime;





			while (window.isOpen()) {


				// Create score board text
				Font font;
				if (!font.loadFromFile(font_path + "TrajanPro-Bold.otf")) {
					cerr << "Failed to load font!" << endl;
					return -1;
				}

				Text fireboyScoreText("Fireboy: 0", font, 30);
				Text watergirlScoreText("Watergirl: 0", font, 30);
				fireboyScoreText.setFillColor(Color::Red);
				watergirlScoreText.setFillColor(Color::Cyan);
				watergirlScoreText.setPosition(0, 5);
				fireboyScoreText.setPosition(0, 60);

				font.loadFromFile(font_path + "TrajanPro-Bold.otf"); // Load a font
				Text ScboardB;
				ScboardB.setFont(font);
				ScboardB.setCharacterSize(24);
				ScboardB.setFillColor(Color::Blue);
				ScboardB.setString("Score: " + to_string(ScB));
				ScboardB.setPosition(0, 0);

				Text ScboardR;
				ScboardR.setFont(font);
				ScboardR.setCharacterSize(24);
				ScboardR.setFillColor(Color::Red);
				ScboardR.setString("Score: " + to_string(ScR));
				ScboardR.setPosition(0, 20);

				Event events;
				while (window.pollEvent(events)) {
					if (events.type == Event::Closed) {
						window.close();
					}
					if (events.type == Event::MouseButtonPressed && events.mouseButton.button == Mouse::Left)
					{
						Vector2f mousePos = window.mapPixelToCoords(Mouse::getPosition(window));

						// If clicked on pause button
						if (pauseButton.sprite.getGlobalBounds().contains(mousePos))
						{
							isPaused = !isPaused;
						}

						// If clicked on "Resume" button
						if (isPaused && resumeButtonText.getGlobalBounds().contains(mousePos))
						{
							isPaused = false; // Resume the game
						}
						if (isPaused && homeButtonText.getGlobalBounds().contains(mousePos))
						{
							state = -1; // Resume the game
							window.close();
						}
					}
				}

				score(Red, fireboy.shape, Blue, watergirl.shape, ScR, ScB, ScboardR, ScboardB);

				if (!isPaused) {
					// Main loop
					Vector2f fireboyMovement(0.f, 0.f);
					if (Keyboard::isKeyPressed(Keyboard::Key::A)) {
						fireboyMovement.x -= 5.f; // Move left
						fireboy.animState = MOVING_LEFT;
					}
					else if (Keyboard::isKeyPressed(Keyboard::Key::D)) {
						fireboyMovement.x += 5.f; // Move right
						fireboy.animState = MOVING_RIGHT;
					}

					// Jump logic
					if (Keyboard::isKeyPressed(Keyboard::Key::W) && fireboy.isGrounded) {
						fireboy.jump();
						fireboy.animState = JUMPING;
					}


					// Reset to IDLE only if on the ground and not moving
					if (!Keyboard::isKeyPressed(Keyboard::Key::A) &&
						!Keyboard::isKeyPressed(Keyboard::Key::D) &&
						fireboy.isGrounded) {
						fireboy.animState = IDLE;
					}

					// Move Fireboy horizontally
					fireboy.move(fireboyMovement.x, 0);

					// Resolve horizontal collisions for Fireboy
					for (const auto& col : cols) {
						if (fireboy.shape.getGlobalBounds().intersects(col.sprite.getGlobalBounds())) {
							resolveCollision(fireboy, col.sprite.getGlobalBounds(), fireboyMovement);
						}
					}
					for (const auto& small_col : small_cols) {
						if (fireboy.shape.getGlobalBounds().intersects(small_col.sprite.getGlobalBounds())) {
							resolveCollision(fireboy, small_col.sprite.getGlobalBounds(), fireboyMovement);
						}
					}
					for (const auto& Top_bottom_background : Top_bottom_backgrounds) {
						if (fireboy.shape.getGlobalBounds().intersects(Top_bottom_background.sprite.getGlobalBounds())) {
							resolveCollision(fireboy, Top_bottom_background.sprite.getGlobalBounds(), fireboyMovement);
						}
					}




					// Apply gravity
					fireboy.velocity.y += 0.1f; // Apply gravity
					fireboyMovement.y = fireboy.velocity.y;

					// Move Fireboy vertically
					fireboy.move(0, fireboyMovement.y);

					// Resolve vertical collisions for Fireboy
					bool isOnGround = false;
					for (const auto& col : cols) {
						if (fireboy.shape.getGlobalBounds().intersects(col.sprite.getGlobalBounds())) {
							FloatRect playerBounds = fireboy.shape.getGlobalBounds();
							FloatRect colBounds = col.sprite.getGlobalBounds();

							// Check if Fireboy is directly on top of the object
							if (playerBounds.top + playerBounds.height <= colBounds.top + 5.0f && fireboyMovement.y > 0) {
								isOnGround = true;
								fireboy.velocity.y = 0; // Stop downward movement
							}
							resolveCollision(fireboy, colBounds, fireboyMovement);
						}
					}
					for (const auto& small_col : small_cols) {
						if (fireboy.shape.getGlobalBounds().intersects(small_col.sprite.getGlobalBounds())) {

							if (fireboyMovement.y > 0) {
								isOnGround = true;

							}
							resolveCollision(fireboy, small_col.sprite.getGlobalBounds(), fireboyMovement);
						}
					}
					for (const auto& Top_bottom_background : Top_bottom_backgrounds) {
						if (fireboy.shape.getGlobalBounds().intersects(Top_bottom_background.sprite.getGlobalBounds())) {
							FloatRect playerBounds = fireboy.shape.getGlobalBounds();
							FloatRect colBounds = Top_bottom_background.sprite.getGlobalBounds();

							// Check if Fireboy is directly on top of the object
							if (playerBounds.top + playerBounds.height <= colBounds.top + 20 && fireboyMovement.y > 0) {
								isOnGround = true;
								fireboy.velocity.y = 0; // Stop downward movement
							}
							resolveCollision(fireboy, colBounds, fireboyMovement);
						}
					}
					if (fireboy.shape.getGlobalBounds().intersects(hill.sprite.getGlobalBounds())) {
						FloatRect playerBounds = fireboy.shape.getGlobalBounds();
						FloatRect colBounds = hill.sprite.getGlobalBounds();

						// Check if Fireboy is directly on top of the object
						if (playerBounds.top + playerBounds.height <= colBounds.top + 5.0f && fireboyMovement.y > 0) {
							isOnGround = true;
							fireboy.velocity.y = 0; // Stop downward movement
						}
						resolveCollision(fireboy, colBounds, fireboyMovement);
					}


					// Update grounded state
					fireboy.isGrounded = isOnGround;

					// Ensure Fireboy can only jump when directly on top of an object
					if (!fireboy.isGrounded) {
						fireboy.velocity.y += 0.1f; // Apply gravity
					}

					Vector2f watergirlMovement(0.f, 0.f);
					if (Keyboard::isKeyPressed(Keyboard::Key::Left)) {
						watergirlMovement.x -= 5.f; // Move left
						watergirl.animState = WATERGIRL_MOVING_LEFT;
					}
					else if (Keyboard::isKeyPressed(Keyboard::Key::Right)) {
						watergirlMovement.x += 5.f; // Move right
						watergirl.animState = WATERGIRL_MOVING_RIGHT;
					}
					if (Keyboard::isKeyPressed(Keyboard::Key::Up)) {
						watergirl.jump();
						if (watergirl.velocity.y < 0) {
							watergirl.animState = WATERGIRL_JUMPING;
						}
					}
					// Reset to IDLE only if on the ground and not moving
					if (!Keyboard::isKeyPressed(Keyboard::Key::Left) &&
						!Keyboard::isKeyPressed(Keyboard::Key::Right) &&
						watergirl.isGrounded) {
						watergirl.animState = WATERGIRL_IDLE; // Reset to IDLE state when not moving
					}

					watergirl.move(watergirlMovement.x, 0);

					// Resolve horizontal collisions for Fireboy
					for (const auto& col : cols) {
						if (watergirl.shape.getGlobalBounds().intersects(col.sprite.getGlobalBounds())) {
							resolveCollision(watergirl, col.sprite.getGlobalBounds(), watergirlMovement);
						}
					}
					for (const auto& small_col : small_cols) {
						if (watergirl.shape.getGlobalBounds().intersects(small_col.sprite.getGlobalBounds())) {
							resolveCollision(watergirl, small_col.sprite.getGlobalBounds(), watergirlMovement);
						}
					}
					for (const auto& Top_bottom_background : Top_bottom_backgrounds) {
						if (watergirl.shape.getGlobalBounds().intersects(Top_bottom_background.sprite.getGlobalBounds())) {
							resolveCollision(watergirl, Top_bottom_background.sprite.getGlobalBounds(), watergirlMovement);
						}
					}

					// Move Watergirl
					watergirl.velocity.y += 0.1f; // Apply gravity
					watergirlMovement.y = watergirl.velocity.y;

					// Move Fireboy vertically
					watergirl.move(0, watergirlMovement.y);


					// Update characters
					float deltaTime = clock.restart().asSeconds();
					fireboy.updateAnimations();
					watergirl.updateAnimation();
					fireboy.update(deltaTime);
					watergirl.update(deltaTime);

					for (const auto& col : cols) {
						isOnGround = false;
						if (watergirl.shape.getGlobalBounds().intersects(col.sprite.getGlobalBounds())) {
							FloatRect playerBounds = watergirl.shape.getGlobalBounds();
							FloatRect colBounds = col.sprite.getGlobalBounds();

							// Check if Fireboy is directly on top of the object
							if (playerBounds.top + playerBounds.height <= colBounds.top + 5.0f && watergirlMovement.y > 0) {
								isOnGround = true;
								watergirl.velocity.y = 0; // Stop downward movement
							}
							resolveCollision(watergirl, colBounds, watergirlMovement);
						}
					}
					for (const auto& small_col : small_cols) {
						if (watergirl.shape.getGlobalBounds().intersects(small_col.sprite.getGlobalBounds())) {

							if (watergirlMovement.y > 0) {
								isOnGround = true;

							}
							resolveCollision(watergirl, small_col.sprite.getGlobalBounds(), watergirlMovement);
						}
					}
					for (const auto& Top_bottom_background : Top_bottom_backgrounds) {
						if (watergirl.shape.getGlobalBounds().intersects(Top_bottom_background.sprite.getGlobalBounds())) {
							FloatRect playerBounds = watergirl.shape.getGlobalBounds();
							FloatRect colBounds = Top_bottom_background.sprite.getGlobalBounds();

							// Check if Fireboy is directly on top of the object
							if (playerBounds.top + playerBounds.height <= colBounds.top + 20 && watergirlMovement.y > 0) {
								isOnGround = true;
								watergirl.velocity.y = 0; // Stop downward movement
							}
							resolveCollision(watergirl, colBounds, watergirlMovement);
						}
					}
					// Check for intersection with fan objects
					if (watergirl.shape.getGlobalBounds().intersects(fan1.sprite.getGlobalBounds()) ||
						fireboy.shape.getGlobalBounds().intersects(fan1.sprite.getGlobalBounds()) ||
						watergirl.shape.getGlobalBounds().intersects(fan2.sprite.getGlobalBounds()) ||
						fireboy.shape.getGlobalBounds().intersects(fan2.sprite.getGlobalBounds())) {

						// Increase jump height for Watergirl
						if (watergirl.shape.getGlobalBounds().intersects(fan1.sprite.getGlobalBounds()) ||
							watergirl.shape.getGlobalBounds().intersects(fan2.sprite.getGlobalBounds())) {
							watergirl.velocity.y = -20.f; // Increase jump strength
						}

						// Increase jump height for Fireboy
						if (fireboy.shape.getGlobalBounds().intersects(fan1.sprite.getGlobalBounds()) ||
							fireboy.shape.getGlobalBounds().intersects(fan2.sprite.getGlobalBounds())) {
							fireboy.velocity.y = -20.f; // Increase jump strength
						}
					}

					// Check for intersections
					// Check for Fireboy's collision with lakes from the top
					bool gameOver = false;

					if (fireboy.shape.getGlobalBounds().intersects(lake.sprite.getGlobalBounds()) || fireboy.shape.getGlobalBounds().intersects(lake3.sprite.getGlobalBounds())) {
						gameOver = true;
					}if (watergirl.shape.getGlobalBounds().intersects(lake2.sprite.getGlobalBounds()) || watergirl.shape.getGlobalBounds().intersects(lake4.sprite.getGlobalBounds())) {
						gameOver = true;
					}

					// Handle game over logic
					if (gameOver || remainingTime == 0) {
						window.close();
						state = 9;
					}
					int capturedTime = 0; // Variable to store the remaining time
					// End the game if a collision is detected


					bool gameWin = false;

					// Define the center range

					// Check if Fireboy is in the center or within range of the red exit door
					capturedTime = static_cast<int>(clocks.getElapsedTime().asSeconds());

					if (watergirl.shape.getGlobalBounds().intersects(Top_bottom_backgrounds[13].sprite.getGlobalBounds()) && fireboy.shape.getGlobalBounds().intersects(Top_bottom_backgrounds[12].sprite.getGlobalBounds())) {
						// Check if both characters are in range of their respective exit doors			
						gameWin = true;


						ofstream offile;
						offile.open("History.txt", ios::app);
						if (offile.is_open()) {
							offile << name1 << " & " << name2 << " " << capturedTime << "*" << endl;
							offile.close();
						}
						else {
							cout << "Unable to open file!" << endl;
						}

					}
					//{470, 550}, {1150, 550}

					// Handle game win logic
					if (gameWin) {
						window.close();
						state = 8;
					}

					if (watergirl.shape.getGlobalBounds().intersects(hill.sprite.getGlobalBounds())) {
						FloatRect playerBounds = watergirl.shape.getGlobalBounds();
						FloatRect colBounds = hill.sprite.getGlobalBounds();

						// Check if Fireboy is directly on top of the object
						if (playerBounds.top + playerBounds.height <= colBounds.top + 5.0f && watergirlMovement.y > 0) {
							isOnGround = true;
							watergirl.velocity.y = 0; // Stop downward movement
						}
						resolveCollision(watergirl, colBounds, watergirlMovement);
					}


					// Update grounded state
					watergirl.isGrounded = isOnGround;

					// Ensure Fireboy can only jump when directly on top of an object
					if (!watergirl.isGrounded) {
						watergirl.velocity.y += 0.1f; // Apply gravity
					}

					// Rendering process
					window.clear();

					Time elapsed = clocks.getElapsedTime();
					remainingTime = startingTime - static_cast<int>(elapsed.asSeconds());
					if (remainingTime < 0) {
						remainingTime = 0;
					}
					int minutes = remainingTime / 60; // Calculate minutes
					int seconds = remainingTime % 60;  // Calculate remaining seconds

					// Format the timer string as MM:SS
					timerText.setString(
						(minutes < 10 ? "0" : "") + to_string(minutes) + ":" +
						(seconds < 10 ? "0" : "") + to_string(seconds)
					);


					for (const auto& background : backgrounds) {
						window.draw(background.sprite);
					}
					for (const auto& col : cols) {
						window.draw(col.sprite);
					}
					for (const auto& small_col : small_cols) {
						window.draw(small_col.sprite);
					}

					for (const auto& tp_bt : Top_bottom_backgrounds)
					{
						window.draw(tp_bt.sprite);
					}

					/*for (const auto& rdiamond : rdiamonds)
					{
						window.draw(rdiamond.sprite);
					}*/
					for (int i = 0; i < 4; ++i)
					{
						window.draw(Red[i]);
						window.draw(Blue[i]);
					}
					/*for (const auto& bdiamond : bdiamonds)
					{
						window.draw(bdiamond.sprite);
					}*/
					window.draw(ScboardB);
					window.draw(ScboardR);

					window.draw(TimerBox.sprite);

					window.draw(timerText);
					window.draw(pauseButton.sprite);

					window.draw(hill.sprite);
					window.draw(lake.sprite);
					window.draw(lake2.sprite);
					window.draw(lake3.sprite);
					window.draw(lake4.sprite);
					window.draw(redExitDoor.sprite);
					window.draw(blueExitDoor.sprite);
					window.draw(fan1.sprite);
					window.draw(fan2.sprite);
					window.draw(fireboy.shape);
					window.draw(watergirl.shape);
					window.draw(air.sprite);
					window.draw(Air.sprite);
				}
				if (isPaused)
				{
					window.draw(pauseOverlay);
					window.draw(homeButtonText);
					window.draw(resumeButtonText);
				}
				// Display the contents of the window
				window.display();
			}
		}



		else if (state == 8) {
			RenderWindow window(VideoMode(1921, 1080), "Win State");

			// Load background texture
			Texture how;
			if (!how.loadFromFile(img_path + "bgr.jpg")) {
				// Handle error
				std::cerr << "Error loading background image!" << std::endl;
				return -1;
			}

			Sprite history;
			history.setTexture(how);

			// Load font

			Font font;
			if (!font.loadFromFile("C:/Windows/Fonts/Arial.ttf")) {
				// Handle error
				std::cerr << "Error loading font!" << std::endl;
				return -1;
			}

			// Set up "You Won!!!" text
			Text zz;
			zz.setFont(font);
			zz.setFillColor(Color::Red);
			zz.setString("You Won!!!");
			zz.setCharacterSize(80);
			zz.setStyle(Text::Bold);
			zz.setPosition(1921 / 2 - zz.getGlobalBounds().width / 2, 1080 / 3);

			// Set up "Press Enter to return" text
			Text zzz;
			zzz.setFont(font);
			zzz.setFillColor(Color::Red);
			zzz.setString("Press Enter to return to the main menu");
			zzz.setCharacterSize(50);
			zzz.setStyle(Text::Bold);
			zzz.setPosition(1921 / 2 - zzz.getGlobalBounds().width / 2, 1080 / 2);

			// Main rendering loop
			while (window.isOpen()) {
				Event event;
				while (window.pollEvent(event)) {
					if (event.type == Event::Closed) {
						window.close();
						ongoing = false;
					}
					if (Keyboard::isKeyPressed(Keyboard::Enter)) {
						window.close();
						state = -1;
					}
				}

				// Clear the window
				window.clear();

				// Draw the background and text
				window.draw(history);
				window.draw(zz);
				window.draw(zzz);

				// Display the updated frame
				window.display();
			}

		}
		else if (state == 9) {
			RenderWindow window(VideoMode(1921, 1080), "lose State");

			// Load background texture
			Texture h;
			if (!h.loadFromFile(img_path + "bgr.jpg")) {
				// Handle error
				std::cerr << "Error loading background image!" << std::endl;
				return -1;
			}

			Sprite history;
			history.setTexture(h);

			// Load font

			Font font;
			if (!font.loadFromFile("C:/Windows/Fonts/Arial.ttf")) {
				// Handle error
				std::cerr << "Error loading font!" << std::endl;
				return -1;
			}

			// Set up "You Won!!!" text
			Text zzzz;
			zzzz.setFont(font);
			zzzz.setFillColor(Color::Red);
			zzzz.setString("You lost :(");
			zzzz.setCharacterSize(80);
			zzzz.setStyle(Text::Bold);
			zzzz.setPosition(1921 / 2 - zzzz.getGlobalBounds().width / 2, 1080 / 3);

			// Set up "Press Enter to return" text
			Text z;
			z.setFont(font);
			z.setFillColor(Color::Red);
			z.setString("Press Enter to return to the main menu");
			z.setCharacterSize(50);
			z.setStyle(Text::Bold);
			z.setPosition(1921 / 2 - z.getGlobalBounds().width / 2, 1080 / 2);

			// Main rendering loop
			while (window.isOpen()) {
				Event event;
				while (window.pollEvent(event)) {
					if (event.type == Event::Closed) {
						window.close();
						ongoing = false;
					}
					if (Keyboard::isKeyPressed(Keyboard::Enter)) {
						window.close();
						state = -1;
					}
				}

				// Clear the window
				window.clear();

				// Draw the background and text
				window.draw(history);
				window.draw(zzzz);
				window.draw(z);

				// Display the updated frame
				window.display();
			}
		}
	}
	return 0;

}


void enterthename(RenderWindow& window, string& name1, string& name2) {
	Texture bg;
	bg.loadFromFile("bgr.jpg");
	Sprite b;
	b.setTexture(bg);

	Font font;
	font.loadFromFile("C:/Windows/Fonts/Arial.ttf");

	Text t1, t2, t3, t4;
	t1.setFont(font);
	t2.setFont(font);
	t3.setFont(font);
	t4.setFont(font);
	t1.setString("Enter first name:");
	t1.setCharacterSize(70);
	t2.setCharacterSize(60);
	t3.setCharacterSize(70);
	t4.setCharacterSize(60);
	t1.setPosition(10, 300);
	t2.setPosition(10, 390);
	t3.setPosition(10, 490);
	t4.setPosition(10, 580);
	t1.setFillColor(Color::Red);
	t2.setFillColor(Color::White);
	t3.setFillColor(Color::Red);
	t4.setFillColor(Color::White);

	bool enteringFirstName = true;

	while (window.isOpen()) {
		Event event;
		while (window.pollEvent(event)) {
			if (event.type == Event::Closed) {
				window.close();
				return;
			}
			if (event.type == Event::Closed) {
				window.close();
				return;
			}
			if (event.type == Event::TextEntered) {
				if (enteringFirstName) {
					if (event.text.unicode == '\b' && !name1.empty()) {
						name1.pop_back();
					}
					else if (event.text.unicode < 128 && event.text.unicode != '\b') {
						name1 += static_cast<char>(event.text.unicode);
					}
					t2.setString(name1);
				}
				else {
					if (event.text.unicode == '\b' && !name2.empty()) {
						name2.pop_back();
					}
					else if (event.text.unicode < 128 && event.text.unicode != '\b') {
						name2 += static_cast<char>(event.text.unicode);
					}
					t4.setString(name2);
				}
			}
			if (event.type == Event::KeyPressed && event.key.code == Keyboard::Enter) {
				if (enteringFirstName && !name1.empty()) {
					cout << "First name entered: " << name1 << endl;
					t3.setString("Enter second name:");
					enteringFirstName = false;
				}
				else if (!enteringFirstName && !name2.empty()) {
					cout << "Second name entered: " << name2 << endl;
					return;
				}
			}
			if (Keyboard::isKeyPressed(Keyboard::Escape)) {
				return;
			}

			window.clear();
			window.draw(b);
			window.draw(t1);
			window.draw(t2);
			window.draw(t3);
			window.draw(t4);
			window.display();
		}
	}
}

