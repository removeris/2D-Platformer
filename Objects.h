#pragma once

#include "SFML/Graphics.hpp"
#include "Defines.h"

struct Player {
	sf::Vector2f speed = sf::Vector2f(0.3f, 9.f);
	sf::Vector2f max_velocity = sf::Vector2f(4, 7);

	sf::Vector2f pos;
	sf::Vector2f startPos = sf::Vector2f(100, 355.01);
	sf::Vector2f velocity = sf::Vector2f(0, 0);

	sf::Sprite sprite;
	sf::Texture texture;

	int height = 13;
	int width = 12;

	bool onGround = false;
	bool onPlatform = false;

	bool underPlatform = false;
	bool isColliding = false;
	bool inIFrame = false;

	short int lives = totalLives;
};

struct Enemy {
	sf::Sprite sprite;

	float speed = .6f;
};
