#pragma once

const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 400;

const float gravity = 0.8f;
const float friction = 1.2f;

const float platformSpeed = 0.7f;

const short int staticPlatformCount = 15;
const short int movingPlatformCount = 4;
const short int platformCount = movingPlatformCount + staticPlatformCount;

const short int enemyCount = 3;
const short int totalLives = 3;
const short int totalCoins = 4;
