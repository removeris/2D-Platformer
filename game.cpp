#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>

#include "Objects.h"

using namespace std;

short int coinsCollected = 0;
short int coinCount = totalCoins;

bool gameOver = false;

enum gameStates {MENU, GAME, GAME_OVER};
gameStates state;

void Setup(Player &player) {
	player.pos = player.startPos;
	player.texture.loadFromFile("Resources/Player.png", sf::IntRect(0, 0, 24, 26));
	player.sprite.setTexture(player.texture);
	player.sprite.setOrigin(12, 13);
	player.sprite.setPosition(player.pos);
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

void coinCollected(Player player, vector<sf::Sprite> coin) {
	for (int i = 0; i < coinCount; i++) {
		if (player.sprite.getGlobalBounds().intersects(coin[i].getGlobalBounds())) {
			coin.erase(coin.begin() + i);
			coinCount--;
			coinsCollected++;
		}
	}
}

void movePlatform(sf::RectangleShape platform[], vector<float> &travelDist, vector<int> dir) {

	for (int i = staticPlatformCount; i < platformCount; i++) {
		platform[i].move(platformSpeed * dir[i - staticPlatformCount], 0);
		travelDist[i - staticPlatformCount] += platformSpeed;
	}

}

void Update(Player &player, Enemy enemy[], vector<sf::Sprite>& coin, sf::RectangleShape platform[], vector<float>& travelDist, vector<int> dir, sf::Text& score, vector<sf::Sprite>& hearts) {
	player.sprite.move(player.velocity.x, player.velocity.y);
	movePlatform(platform, travelDist, dir);
	for (int i = 0; i < enemyCount; i++) {
		enemy[i].sprite.move(enemy[i].speed, 0);

		if (enemy[i].speed > 0) {
			enemy[i].sprite.setScale(-1.0f, 1.0f);
		}
		else enemy[i].sprite.setScale(1.0f, 1.0f);
	}


	coinCollected(player, coin);
	score.setString("SCORE " + to_string(coinsCollected));
	score.move(player.velocity.x, 0);

	for (int i = 0; i < totalLives; i++) {
		hearts[i].move(player.velocity.x, 0);
	}



	
}

bool enemyOutOfBounds(sf::RectangleShape platform, Enemy enemy) {
	float enemyLeftX = enemy.sprite.getGlobalBounds().left;
	float enemyRightX = enemy.sprite.getGlobalBounds().left + enemy.sprite.getGlobalBounds().width;
	float enemyBottomY = enemy.sprite.getGlobalBounds().top + enemy.sprite.getGlobalBounds().height;

	sf::Vector2f enemyLeft(enemyLeftX, enemyBottomY);
	sf::Vector2f enemyRight(enemyRightX, enemyBottomY);

	
	if (!platform.getGlobalBounds().contains(enemyRight) && enemy.speed > 0)
		return true;
	if (!platform.getGlobalBounds().contains(enemyLeft) && enemy.speed < 0)
		return true;
	return false;
}

void changeEnemyDirection(Enemy &enemy) {
	enemy.speed *= -1;
}

bool playerOnGround(Player player, sf::RectangleShape ground) {
	if (player.sprite.getGlobalBounds().intersects(ground.getGlobalBounds())) {
		if (player.sprite.getGlobalBounds().top + player.sprite.getGlobalBounds().height >= ground.getGlobalBounds().top) {
			return true;
		}
	}
	return false;
}

bool playerOnPlatform(Player player, sf::RectangleShape platform[], int &index) {
	for (int i = 0; i < platformCount; i++) {
		if (player.sprite.getGlobalBounds().intersects(platform[i].getGlobalBounds())) {
			if (player.sprite.getGlobalBounds().top + player.sprite.getGlobalBounds().height >= platform[i].getGlobalBounds().top && player.velocity.y >= 0) {
				index = i;
				return true;
			}
		}
	}
	return false;
}

void heartLoss(vector<sf::Sprite> &hearts, Player &player, sf::Texture &ehTexture, sf::Clock &clock) {
	
	// Change Texture

	hearts[player.lives].setTexture(ehTexture);

	// Start I frames;

	clock.restart();
	player.inIFrame = true;
	player.sprite.setColor(sf::Color(255, 255, 255, 75));
}

void Logic(Player &player, sf::RectangleShape ground, sf::RectangleShape platform[], Enemy enemy[], vector<float>& travelDist, vector<int>& dir, vector<sf::Sprite>& hearts, sf::Texture& ehTexture, sf::Clock& iFrameClock, int enemyPlatforms[]) {

	// Ground Collision

	player.onGround = playerOnGround(player, ground);

	// Check if player is on platform
	int index;
	player.onPlatform = playerOnPlatform(player, platform, index);

	// Player Updates according to collision

	double playerLegsY = player.sprite.getGlobalBounds().top + player.sprite.getGlobalBounds().height;


	if (!player.onGround && !player.onPlatform) {
		player.velocity.y += gravity;
	}
	else {
		
		if (player.onGround) {
			player.sprite.move(0, ground.getGlobalBounds().top - playerLegsY + .001f);
		}
		else if (player.onPlatform) {
			player.sprite.move(0, platform[index].getGlobalBounds().top - playerLegsY + .0001f);
		}
		
		player.velocity.y = 0;
	}

	// Player and Enemy collision

	for (int i = 0; i < enemyCount; i++) {

		if (player.sprite.getGlobalBounds().intersects(enemy[i].sprite.getGlobalBounds()) && player.lives > 0 && !player.inIFrame) {
			player.lives--;
			heartLoss(hearts, player, ehTexture, iFrameClock);
		}
	}


	sf::Time iFrameTime;

	iFrameTime = iFrameClock.getElapsedTime();

	if (player.inIFrame && iFrameTime.asSeconds() > 1.5) {
		player.inIFrame = false;
		player.sprite.setColor(sf::Color::White);
	}
		
	if (player.lives == 0)
		gameOver = true;

	// Enemy boundaries

	for (int i = 0; i < enemyCount; i++) {
		if (enemyOutOfBounds(platform[enemyPlatforms[i]], enemy[i]))
			changeEnemyDirection(enemy[i]);
	}

	if (travelDist[0] > 150) {
		travelDist[0] = 0;
		dir[0] *= -1;
	}
	if (travelDist[1] > 300) {
		travelDist[1] = 0;
		dir[1] *= -1;
	}
	if (travelDist[2] > 300) {
		travelDist[2] = 0;
		dir[2] *= -1;
	}
	if (travelDist[3] > 300) {
		travelDist[3] = 0;
		dir[3] *= -1;
	}
	
}

void drawGame(sf::RenderWindow& window, Player player, Enemy enemy[], sf::RectangleShape ground, sf::RectangleShape platform[], vector<sf::Sprite> coin, sf::Text score, vector<sf::Sprite> hearts) {
	window.draw(ground);
	
	for (int i = 0; i < platformCount; i++) {
		window.draw(platform[i]);
	}

	for (int i = 0; i < coinCount; i++) {
		window.draw(coin[i]);
	}

	for (int i = 0; i < enemyCount; i++) {
		window.draw(enemy[i].sprite);
	}

	for (int i = 0; i < totalLives; i++) {
		window.draw(hearts[i]);
	}

	window.draw(player.sprite);
	window.draw(score);
}

void centreText(sf::Text& text) {
	sf::FloatRect textRect = text.getLocalBounds();
	text.setOrigin(textRect.left + textRect.width / 2., textRect.top + textRect.height / 2.);
}
void initMenu(sf::Text &menuTitle, sf::Text buttonText[], sf::Font& font, sf::Color textColor, sf::Sprite button[], sf::Texture& bTexture, sf::Color backg) {
	
	// Button Borders

	bTexture.loadFromFile("Resources/Button.png");
	for (int i = 0; i < 2; i++) {
		button[i].setTextureRect(sf::IntRect(0, 0, 128, 32));
		button[i].setTexture(bTexture);
		button[i].setOrigin(64, 16);
	}
	button[0].setPosition(SCREEN_WIDTH / 2., 200);
	button[1].setPosition(SCREEN_WIDTH / 2., 250);
	
	// Text

	menuTitle.setFont(font);
	menuTitle.setCharacterSize(48);
	menuTitle.setFillColor(textColor);
	menuTitle.setString("2D PLATFORMER");
	centreText(menuTitle);
	menuTitle.setPosition(SCREEN_WIDTH / 2., 100);

	buttonText[0].setString("START");
	buttonText[1].setString("EXIT");
	for (int i = 0; i < 2; i++) {
		buttonText[i].setFont(font);
		buttonText[i].setCharacterSize(24);
		buttonText[i].setFillColor(backg);
		centreText(buttonText[i]);
	}
	
	buttonText[0].setPosition(button[0].getPosition());
	buttonText[1].setPosition(button[1].getPosition());


}
void menuLogic(sf::Sprite button[], sf::RenderWindow& window) {

	sf::Vector2f mousePos(sf::Mouse::getPosition(window));

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		if (button[0].getGlobalBounds().contains(mousePos))
			state = GAME;
		else if (button[1].getGlobalBounds().contains(mousePos))
			window.close();	
	}
}
void drawMenu(sf::RenderWindow& window, sf::Text menuTitle, sf::Sprite button[], sf::Text buttonText[]) {
	window.draw(menuTitle);

	window.draw(button[0]);
	window.draw(buttonText[0]);

	window.draw(button[1]);
	window.draw(buttonText[1]);
}

void initGameOver(sf::Text& title, sf::Sprite button[], sf::Text buttonText[], sf::Font &font, sf::Color textColor, sf::Color backg, sf::RenderWindow &window) {
	
	title.setString("GAME OVER");
	title.setFillColor(textColor);
	title.setPosition(SCREEN_WIDTH / 2., 100);
	title.setFont(font);
	title.setCharacterSize(48);
	centreText(title);

	buttonText[0].setString("RETRY");
	buttonText[1].setString("EXIT");
	for (int i = 0; i < 2; i++) {
		buttonText[i].setFont(font);
		buttonText[i].setCharacterSize(24);
		buttonText[i].setFillColor(backg);
		centreText(buttonText[i]);
	}

	button[0].setPosition(SCREEN_WIDTH / 2., 200);
	button[1].setPosition(SCREEN_WIDTH / 2., 250);

	buttonText[0].setPosition(button[0].getPosition());
	buttonText[1].setPosition(button[1].getPosition());

	window.setView(window.getDefaultView());
}
void gameOverLogic(sf::RenderWindow &window, sf::Sprite button[], Player &player, vector<sf::Sprite> &hearts, sf::Texture &hTexture, sf::View &view, sf::Text &score) {

	sf::Vector2f mousePos(sf::Mouse::getPosition(window));

	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		if (button[0].getGlobalBounds().contains(mousePos)) {
			player.lives = 3;
			player.sprite.setPosition(player.startPos);
			sf::Vector2f initial(-190, 5);
			for (int i = 0; i < 3; i++) {
				hearts[i].setTexture(hTexture);
				hearts[i].setPosition(initial);
				initial.x += 20;
			}
			score.setPosition(SCREEN_WIDTH - 300, 0);
			coinsCollected = 0;
			coinCount = totalCoins;
			state = GAME;
			gameOver = false;
			view.setSize(SCREEN_WIDTH, SCREEN_HEIGHT);
			view.setCenter(player.startPos.x, SCREEN_HEIGHT / 2.);

		}
		else if (button[1].getGlobalBounds().contains(mousePos))
			window.close();
	}

}

int main() {

	sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "2D Platformer", sf::Style::Default);
	window.setFramerateLimit(60);
	window.clear();

	// Setup Menu Scene

	state = MENU;

	sf::Sprite button[2];
	sf::Texture bTexture;

	sf::Color backg(sf::Color(80, 42, 69));
	sf::Color textColor(sf::Color(135, 186, 197));
	sf::Font font;
	font.loadFromFile("Resources/ARCADECLASSIC.TTF");
	
	sf::Text menuTitle;

	sf::Text menuButtonText[2];

	initMenu(menuTitle, menuButtonText, font, textColor, button, bTexture, backg);
	
	// GameOver

	sf::Text gameOverTitle;

	sf::Text gameOverButtonText[2];

	// Create Player

	Player player;
	
	// Set up camera
	
	sf::View view;

	view.setSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	view.setCenter(player.startPos.x, SCREEN_HEIGHT / 2.);
	
	// Create Ground

	sf::RectangleShape ground;
	sf::Texture gTexture;
	gTexture.loadFromFile("Resources/Ground.png", sf::IntRect(0, 0, 32, 32));
	gTexture.setRepeated(true);
	ground.setSize(sf::Vector2f(32 * 80, 32));
	ground.setTexture(&gTexture);
	ground.setTextureRect({ 0, 0, 32 * 80, 32 });
	ground.setOrigin(16, 16);
	ground.setPosition(-200, SCREEN_HEIGHT - 16);

	// Create Platforms

	sf::Texture pTexture;
	pTexture.loadFromFile("Resources/Platform.png", sf::IntRect(0, 0, 32, 8));
	sf::RectangleShape platform[platformCount];

	for (int i = 0; i < platformCount; i++) {
		platform[i].setSize(sf::Vector2f(32, 8));
		platform[i].setTexture(&pTexture);
		platform[i].setOrigin(16, 4);
	}

	// Set Static Platform Positions

	platform[0].setPosition(100, SCREEN_HEIGHT - 80);
	platform[1].setPosition(190, SCREEN_HEIGHT - 120);
	platform[2].setPosition(300, SCREEN_HEIGHT - 160);
	platform[3].setPosition(495, SCREEN_HEIGHT - 260);
	platform[4].setScale(3.0f, 1.0f);
	platform[4].setPosition(600, SCREEN_HEIGHT - 200);
	platform[5].setScale(3.0f, 1.0f);
	platform[5].setPosition(680, SCREEN_HEIGHT - 260);
	
	platform[6].setScale(3.0f, 1.0f);
	platform[6].setPosition(1400, SCREEN_HEIGHT - 160);
	platform[7].setPosition(1460, SCREEN_HEIGHT - 90);
	platform[8].setPosition(1800, SCREEN_HEIGHT - 260);
	platform[9].setPosition(1880, SCREEN_HEIGHT - 260);
	platform[10].setPosition(1960, SCREEN_HEIGHT - 260);
	platform[11].setPosition(2000, SCREEN_HEIGHT - 300);
	platform[12].setPosition(2070, SCREEN_HEIGHT - 160);
	platform[13].setPosition(2150, SCREEN_HEIGHT - 80);
	platform[14].setPosition(1430, SCREEN_HEIGHT - 200);
	// Set Moving Platform positions

	platform[staticPlatformCount].setPosition(370, SCREEN_HEIGHT - 160); // end x pos 495

	platform[staticPlatformCount + 1].setPosition(740, SCREEN_HEIGHT - 260); // 
	
	platform[staticPlatformCount + 2].setPosition(1400, SCREEN_HEIGHT - 260); // 
	
	platform[staticPlatformCount + 3].setPosition(1450, SCREEN_HEIGHT - 260); // d


	vector<float> travelDist(4, 0);
	vector<int> dir(4, 1);
	dir[2] = -1;

	// Set Static Invisible Platform positions for enemy boundaries



	// Create Enemies

	sf::Texture eTexture;
	eTexture.loadFromFile("Resources/Enemy1.png", sf::IntRect(0, 0, 26, 16));
	Enemy enemy[enemyCount];

	for (int i = 0; i < enemyCount; i++) {
		enemy[i].sprite.setTexture(eTexture);
		enemy[i].sprite.setOrigin(13, 8);
	}

	//enemy[0].sprite.setPosition(46, platform[3].getGlobalBounds().top - 16 / 2. + 0.01);

	int enemyPlatforms[enemyCount] = {4, 5, 6};

	enemy[0].sprite.setPosition(platform[4].getPosition().x, platform[4].getGlobalBounds().top - 16 / 2. + 0.01);
	enemy[1].sprite.setPosition(platform[5].getPosition().x, platform[5].getGlobalBounds().top - 16 / 2. + 0.01);
	enemy[2].sprite.setPosition(platform[6].getPosition().x, platform[6].getGlobalBounds().top - 16 / 2. + 0.01);
	

	// Create Coins

	sf::Texture cTexture;
	cTexture.loadFromFile("Resources/Coin.png", sf::IntRect(0, 0, 8, 16));

	sf::Sprite coinSprite;
	//coinSprite.setTexture(cTexture);
	vector<sf::Sprite> coin(totalCoins);

	for (int i = 0; i < totalCoins; i++) {
		coin[i].setTexture(cTexture);
	}

	coin[0].setPosition(sf::Vector2f(platform[3].getPosition().x, platform[3].getPosition().y - 30));
	coin[1].setPosition(sf::Vector2f(platform[7].getPosition().x, platform[7].getPosition().y - 30));
	coin[2].setPosition(sf::Vector2f(platform[8].getPosition().x, platform[8].getPosition().y - 30));
	coin[3].setPosition(sf::Vector2f(platform[13].getPosition().x, platform[13].getPosition().y - 30));

	// GUI

	// Lives

	sf::Texture hTexture;
	sf::Texture ehTexture;
	hTexture.loadFromFile("Resources/FilledHeart.png", sf::IntRect(0, 0, 15, 13));
	ehTexture.loadFromFile("Resources/EmptyHeart.png", sf::IntRect(0, 0, 15, 13));

	vector<sf::Sprite> hearts(player.lives);

	sf::Vector2f initial(-190, 5);

	for (int i = 0; i < player.lives; i++) {
		hearts[i].setTexture(hTexture);
		hearts[i].setPosition(initial);
		initial.x += 20;
	}

	// Text

	sf::Text score;
	score.setFont(font);
	score.setCharacterSize(24);
	score.setFillColor(textColor);
	score.setString("SCORE " + to_string(coinsCollected));
	score.setPosition(SCREEN_WIDTH - 300, 0);

	sf::Clock iFrameClock;

	sf::Vector2f finishFlag(1000, 10);

	Setup(player);

	while (window.isOpen()) {

		sf::Event event;

		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}

		if (gameOver) {
			state = GAME_OVER;
			initGameOver(gameOverTitle, button, gameOverButtonText, font, textColor, backg, window);
		}

		window.clear(sf::Color(backg));
		

		if (state == GAME) {
			Logic(player, ground, platform, enemy, travelDist, dir, hearts, ehTexture, iFrameClock, enemyPlatforms);
			Input(player);
			Update(player, enemy, coin, platform, travelDist, dir, score, hearts);
			
			view.move(player.velocity.x, 0);
			window.setView(view);
			
			drawGame(window, player, enemy, ground, platform, coin, score, hearts);
		}
		else if (state == MENU) {
			
			menuLogic(button, window);
			drawMenu(window, menuTitle, button, menuButtonText);
		}
		else {
			gameOverLogic(window, button, player, hearts, hTexture, view, score);
			drawMenu(window, gameOverTitle, button, gameOverButtonText);
		}
		
		for (int i = 0; i < coinCount; i++) {
			cout << coin[i].getPosition().x << " ";
		}
		
		cout << player.sprite.getPosition().x;

		cout << endl;

		window.display();	
	}

	return EXIT_SUCCESS;
}
