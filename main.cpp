#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

using namespace std;

const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 400;

const float gravity = 0.8f;
const float friction = 1.2f;

const short int platformCount = 4;
const short int enemyCount = 2;

struct Player {
	sf::Vector2f speed = sf::Vector2f(0.3f, 9.f);
	sf::Vector2f max_velocity = sf::Vector2f(4, 7);

	sf::Vector2f pos;
	sf::Vector2f velocity = sf::Vector2f(0, 0);
	
	sf::Sprite sprite;
	sf::Texture texture;

	int height = 12;
	int width = 12;

	bool onGround = false;
	bool onPlatform = false;

	bool underPlatform = false;

	bool isColliding = false;
	
	short int jumps = 0;

	short int lives = 3;
};

struct Enemy {
	sf::Sprite sprite;

	float speed = .4f;
};


void Setup(Player &player);
void Input(Player &player);
void Update(Player &player);
void Logic(Player& player, sf::RectangleShape ground, sf::RectangleShape platform[]);

int main() {

	sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "2D Platformer", sf::Style::Default);
	window.setFramerateLimit(60);
	window.clear();
	
	// Create Player

	Player player;
	
	// Create Ground

	sf::RectangleShape ground;
	sf::Texture gTexture;
	gTexture.loadFromFile("Dirt.png", sf::IntRect(0, 0, 32, 32));
	gTexture.setRepeated(true);
	ground.setSize(sf::Vector2f(32 * 50, 32));
	ground.setTexture(&gTexture);
	ground.setTextureRect({ 0, 0, 32 * 50, 32 });
	ground.setOrigin(16 * 50, 16);
	ground.setPosition(SCREEN_WIDTH / 2, SCREEN_HEIGHT - 16);

	// Create Platforms

	sf::Texture pTexture;
	pTexture.loadFromFile("Platform.png", sf::IntRect(4, 0, 56, 16));
	sf::RectangleShape platform[platformCount];

	for (int i = 0; i < platformCount; i++) {
		platform[i].setSize(sf::Vector2f(56, 16));
		platform[i].setTexture(&pTexture);
		platform[i].setOrigin(32, 8);
	}

	// Set Platform Positions

	platform[0].setPosition(50, SCREEN_HEIGHT - 60);
	platform[1].setPosition(160, SCREEN_HEIGHT - 90);
	platform[2].setPosition(230, SCREEN_HEIGHT - 150);
	platform[3].setPosition(300, SCREEN_HEIGHT - 150);

	// Create Enemies

	sf::Texture eTexture;
	eTexture.loadFromFile("Enemy.png", sf::IntRect(0, 0, 32, 20));
	Enemy enemy[enemyCount];

	for (int i = 0; i < enemyCount; i++) {
		enemy[i].sprite.setTexture(eTexture);
		enemy[i].sprite.setOrigin(16, 10);
	}

	enemy[0].sprite.setPosition(200, SCREEN_HEIGHT - 32 - 5);



	Setup(player);


	while (window.isOpen()) {

		sf::Event event;

		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}

		window.clear();
		
		Logic(player, ground, platform);
		Input(player);
		Update(player);

		
		window.draw(player.sprite);
		window.draw(ground);
		
		for (int i = 0; i < platformCount; i++) {
			window.draw(platform[i]);
		}
		for (int i = 0; i < enemyCount; i++) {
			window.draw(enemy[i].sprite);
		}

		window.display();
	}

	return EXIT_SUCCESS;
}

void Setup(Player &player) {

	// Create Player
	player.pos = sf::Vector2f(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	player.texture.loadFromFile("Player1.png", sf::IntRect(0, 0, 24, 24));
	player.sprite.setTexture(player.texture);
	player.sprite.setOrigin(12, 12);
	player.sprite.setPosition(player.pos);

	// Create Enemies


	
}

void Input(Player& player) {
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
		if (player.velocity.x > -player.max_velocity.x) {
			player.velocity.x -= player.speed.x;
			player.sprite.setScale(-1.0f, 1.0f);
		}
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
		if (player.velocity.x < player.max_velocity.x) {
			player.velocity.x += player.speed.x;
			player.sprite.setScale(1.0f, 1.0f);
		}
	}
	else {
		player.velocity.x /= friction;
		if (fabs(player.velocity.x) < 0.2f)
			player.velocity.x = 0;
	}
	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
		if (player.velocity.y > -player.max_velocity.y && (player.onGround || player.onPlatform)) {
			player.velocity.y -= player.speed.y;
		}
	}
}

void Update(Player &player) {
	player.sprite.move(player.velocity.x, player.velocity.y);
}

void Logic(Player &player, sf::RectangleShape ground, sf::RectangleShape platform[]) {

	// Ground Collision


	double legsY = player.sprite.getGlobalBounds().top + player.sprite.getGlobalBounds().height;
	double headY = player.sprite.getGlobalBounds().top;

	if (player.sprite.getGlobalBounds().intersects(ground.getGlobalBounds())) {
		if (legsY >= ground.getGlobalBounds().top) {
			player.onGround = true;
		}
	}
	else
		player.onGround = false;

	// Check if player is on platform
	int index = -1;
	for (int i = 0; i < platformCount; i++) {
		if (player.sprite.getGlobalBounds().intersects(platform[i].getGlobalBounds())) {
			if (legsY >= platform[i].getGlobalBounds().top && player.velocity.y >= 0) {
				player.onPlatform = true;
				index = i;
				break;
			}
		}
	}
	if (index == -1) {
		player.onPlatform = false;
	}

	// Updates according to collision

	if (!player.onGround && !player.onPlatform) {
		player.velocity.y += gravity;
	}
	else {
		
		if (player.onGround) {
			player.sprite.move(0, ground.getGlobalBounds().top - legsY + .001f);
		}
		else if (player.onPlatform) {
			player.sprite.move(0, platform[index].getGlobalBounds().top - legsY + .001f);
		}
		
		player.velocity.y = 0;
	}
}
