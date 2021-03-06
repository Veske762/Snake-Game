//Creator: Stefan Veselinovic
//Contact: stefanveselinovic379@yahoo.com
#include "stdafx.h"
#include <SFML\Graphics.hpp>

const int levelWidth = 20; //size in tiles
const int levelHeight = 12;
int tileSize = 30; // Game scales with tile size
int moveCap = 12;
int score = 0;
 
enum movement
{
	LEFT = 0,
	RIGHT,
	UP,
	DOWN
};

enum snake_head
{
	HEAD = 0
};

struct Egg
{
	sf::CircleShape eggShape;
	bool isEaten = false;
};

struct Player 
{
	std::vector<sf::RectangleShape> snake;
	int currentDirection; //0 left,1 right,2 up, 3down
	int newDirection;
};

struct WallBlock
{
	sf::RectangleShape wallBlockShape;
	bool isCollider = false;
};

sf::Vector2f GetAvaliableSpawnLocation(std::vector<WallBlock>& map, Player& player) //egg cannot spawn on the snake or on a tile that is a collider
{
	std::vector<WallBlock> nonOccupiedBlocks;
	for (int i = 0; i < map.size(); i++)
	{
		if (map[i].isCollider == false)
		{
			bool occupied = false;
			for (int j = 0; j < player.snake.size(); j++)
			{
				if (player.snake[j].getGlobalBounds().intersects(map[i].wallBlockShape.getGlobalBounds()))
				{
					occupied = true;
				}
			}
			if (occupied == false)
			{
				nonOccupiedBlocks.push_back(map[i]);
			}
		}
	}
	int newEggPositionIndex = rand() % nonOccupiedBlocks.size();
	return nonOccupiedBlocks[newEggPositionIndex].wallBlockShape.getPosition();
}

Egg SpawnEgg(std::vector<WallBlock> &map, Player &player)
{
	Egg egg;
	egg.eggShape.setRadius(tileSize/2);
	egg.eggShape.setOrigin(sf::Vector2f(tileSize/2, tileSize/2));
	egg.eggShape.setFillColor(sf::Color::Blue);
	egg.isEaten = false;
	sf::Vector2f eggPos = GetAvaliableSpawnLocation(map, player);
	egg.eggShape.setPosition(sf::Vector2f(eggPos.x + (tileSize / 2), eggPos.y + (tileSize / 2)));
	return egg;
}

void InitMap(std::vector<WallBlock> &map, char mapChar[levelHeight][levelWidth+1])
{
	map.clear();
	for (int i = 0; i < levelHeight; i++)
	{
		for (int j = 0; j < levelWidth; j++)
		{
			WallBlock block;
			if (mapChar[i][j] == '1')
			{
				block.wallBlockShape.setFillColor(sf::Color::Magenta);
				block.isCollider = true;
			}
			else
			{
				block.wallBlockShape.setFillColor(sf::Color::Green);
			}
			block.wallBlockShape.setPosition(j * tileSize, i * tileSize);
			block.wallBlockShape.setSize(sf::Vector2f(tileSize, tileSize));
			map.push_back(block);
		}
	}
}
void InitSnake(Player& player);
void GameOver(Player& player, std::vector<Egg> &eggs,std::vector<WallBlock> &map)
{
	score = 0;
	eggs.clear();
	player.snake.clear();
	InitSnake(player);
	eggs.push_back(SpawnEgg(map, player));
}

bool SelfCollision(Player& player)
{
	for (int i = 3; i < player.snake.size(); i++)// i is 3 because its impossible for the head to hit 3 blocks behind it
	{
		if (player.snake[HEAD].getGlobalBounds().intersects(player.snake[i].getGlobalBounds()))
		{
			return true;
		}
	}
	return false;
}

bool WallCollision(std::vector<WallBlock> map, Player& player)
{
	for (int i = 0; i < map.size(); i++)
	{
		if (player.snake[HEAD].getGlobalBounds().intersects(map[i].wallBlockShape.getGlobalBounds())
			&&
			map[i].isCollider == true)
		{
			return true;
		}
	}
	return false;
}

void CheckOutOfBounds(Player &player)
{
	if (player.snake[HEAD].getPosition().x > (levelWidth-1)* tileSize)
	{
		player.snake[HEAD].setPosition(sf::Vector2f(0, player.snake[HEAD].getPosition().y));
	}
	else if (player.snake[HEAD].getPosition().y > (levelHeight-1) * tileSize)
	{
		player.snake[HEAD].setPosition(sf::Vector2f(player.snake[HEAD].getPosition().x, 0));
	}
	else if (player.snake[HEAD].getPosition().x < 0)
	{
		player.snake[HEAD].setPosition(sf::Vector2f(levelWidth * tileSize - player.snake[HEAD].getSize().x, player.snake[HEAD].getPosition().y));
	}
	else if (player.snake[HEAD].getPosition().y < 0)
	{
		player.snake[HEAD].setPosition(sf::Vector2f(player.snake[HEAD].getPosition().x, levelHeight *tileSize - player.snake[HEAD].getSize().y));
	}
}

void ExtendSnake(Player &player)
{
	sf::RectangleShape extendedPart;
	extendedPart.setPosition(player.snake[player.snake.size() - 1].getPosition());
	extendedPart.setSize(sf::Vector2f(tileSize, tileSize));
	extendedPart.setFillColor(sf::Color::Red);
	player.snake.push_back(extendedPart);
}

void MoveSnake(Player &player)
{
	static int movementTimer = 0;
	movementTimer += 1;
	if (movementTimer > moveCap)
	{ 
		player.currentDirection = player.newDirection;
		for (int i = player.snake.size()-1; i >0; i--)
		{
			player.snake[i].setPosition(player.snake[i - 1].getPosition());
		}
		switch (player.currentDirection)
		{
		case LEFT:
		{
			player.snake[HEAD].setPosition(player.snake[HEAD].getPosition().x - tileSize, player.snake[HEAD].getPosition().y);
		}break;
		case RIGHT:
		{
			player.snake[HEAD].setPosition(player.snake[HEAD].getPosition().x + tileSize, player.snake[HEAD].getPosition().y);
		}break;
		case UP:
		{
			player.snake[HEAD].setPosition(player.snake[HEAD].getPosition().x, player.snake[HEAD].getPosition().y - tileSize);
		}break;
		case DOWN:
		{
			player.snake[HEAD].setPosition(player.snake[HEAD].getPosition().x, player.snake[HEAD].getPosition().y + tileSize);
		}break;
		default:
			break;
		}
		movementTimer = 0;
	}
}
 
void InitSnake(Player &player)
{
	player.snake.push_back(sf::RectangleShape());
	player.snake[HEAD].setSize(sf::Vector2f(tileSize, tileSize));
	player.snake[HEAD].setFillColor(sf::Color::Yellow);
	player.snake[HEAD].setPosition(sf::Vector2f(tileSize *5, tileSize));
	player.currentDirection = RIGHT;
	player.newDirection = RIGHT;
	for (int i = 0; i < 3; i++)
	{	
		sf::RectangleShape shape;
		shape.setPosition(player.snake[HEAD].getPosition().x - (tileSize * (i + 1)), player.snake[HEAD].getPosition().y);
		shape.setSize(sf::Vector2f(tileSize, tileSize));
		shape.setFillColor(sf::Color::Red);
		player.snake.push_back(shape);
	}
}

void ControlSnake(Player &player)
{
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && player.currentDirection != LEFT)//if the snake is facing left we cant go right
	{
		player.newDirection = RIGHT;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && player.currentDirection != RIGHT)
	{
		player.newDirection = LEFT;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && player.currentDirection != UP)
	{
		player.newDirection = DOWN;
	}
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && player.currentDirection != DOWN) 
	{
		player.newDirection = UP;
	}
}

void EggCollision(Player &player, std::vector<Egg> &eggs, std::vector<WallBlock> &map)
{
	for (int i = 0; i < eggs.size(); i++)
	{
		if (player.snake[HEAD].getGlobalBounds().intersects(eggs[i].eggShape.getGlobalBounds()) && eggs[i].isEaten == false)
		{
			score++;
			eggs[i].isEaten = true;
			eggs[i].eggShape.setFillColor(sf::Color::Red);
			eggs[i].eggShape.setRadius(tileSize*0.80);
			eggs[i].eggShape.setOrigin(sf::Vector2f(eggs[i].eggShape.getRadius(), eggs[i].eggShape.getRadius()));
			eggs.push_back(SpawnEgg(map, player));
		}

		if (player.snake[player.snake.size() - 1].getGlobalBounds().contains(eggs[i].eggShape.getPosition()))
		{
			eggs.erase(eggs.begin() + i);
			ExtendSnake(player);
		}
	}
}

char level1[levelHeight][levelWidth + 1]
{
	"00000000000000000000",
	"00000000000000000000",
	"00000000000000000000",
	"00000000000000000000",
	"00000000000000000000",
	"00000000000000000000",
	"00000000000000000000",
	"00000000000000000000",
	"00000000000000000000",
	"00000000000000000000" ,
	"00000000000000000000",
	"00000000000000000000"
};
 
char level2[levelHeight][levelWidth+1]
{
	"0000000000000000000",
	"0000000000000000000",
	"0000000000000000000",
	"0000111111111111000",
	"0000000000000000000",
	"0000000000000000000",
	"0000000000000000000",
	"0000000000000000000",
	"0000111111111111000",
	"0000000000000000000",
	"0000000000000000000",
	"0000000000000000000"
	
};

char level3[levelHeight][levelWidth+1]
{
	"11111111111111111111",
	"10000000000000000001",
	"10000000000000000001",
	"10000000000000000001",
	"10000000000000000001",
	"10000000000000000001",
	"10000000000000000001",
	"10000000000000000001",
	"10000000000000000001",
	"10000000000000000001" ,
	"10000000000000000001",
	"11111111111111111111"
};

void ChangeLevel(Player &player, std::vector<Egg> &eggs, std::vector<WallBlock> &map)
{
	static bool isPressed = false;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1) && isPressed == false)
	{
		moveCap = 12;
		isPressed = true;
		InitMap(map, level1);
		GameOver(player, eggs, map);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2) && isPressed == false)
	{
		moveCap = 7;
		isPressed = true;
		InitMap(map, level2);
		GameOver(player, eggs, map);
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num3) && isPressed == false)
	{
		moveCap = 4;
		isPressed = true;
		InitMap(map, level3);
		GameOver(player, eggs, map);
	}

	if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Num1) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Num2) && !sf::Keyboard::isKeyPressed(sf::Keyboard::Num3))
	{
		isPressed = false;
	}
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(levelWidth*tileSize , levelHeight*tileSize + tileSize*3), "Snake");
	window.setFramerateLimit(60);
	srand(time(NULL));
	Player player;
	std::vector<Egg> eggs;
	  
	sf::Font font;
	font.loadFromFile("prstartk.ttf");

	sf::Text text;
	text.setFont(font);
	text.setCharacterSize(tileSize*0.45);

	std::vector<WallBlock> map;
	
	InitMap(map, level2);

	InitSnake(player);

	eggs.push_back(SpawnEgg(map, player));

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}
		ChangeLevel(player, eggs, map);
	 
		ControlSnake(player);

		MoveSnake(player);
 
		EggCollision(player, eggs, map);
		 
		if (SelfCollision(player) || WallCollision(map, player))
		{
			GameOver(player, eggs,map);
		}

		CheckOutOfBounds(player);
		  
		window.clear();
		for (int i = 0; i < map.size(); i++)
		{
			window.draw(map[i].wallBlockShape);
		}

		for (int i = 0; i < player.snake.size(); i++)
		{
			window.draw(player.snake[i]);
		}

		for (int i = 0; i < eggs.size(); i++)
		{
			window.draw(eggs[i].eggShape);
		}

		text.setString("Change level with numbers 1,2,3");
		text.setPosition(20,levelHeight*tileSize);
		window.draw(text);
		text.setString("Use arrow keys for movement");
		text.setPosition(20, levelHeight*tileSize +tileSize);
		window.draw(text);
		text.setString("SCORE "+ std::to_string(score));
		text.setPosition((levelWidth - 4)* tileSize, levelHeight*tileSize + tileSize),
		window.draw(text);

		window.display();
	}
	return 0;
}