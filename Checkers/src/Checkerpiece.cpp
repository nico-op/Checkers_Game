#include "Checkerpiece.hpp"

using std::vector;

Checkerpiece::Checkerpiece()
	: startingRow(0),
	kingRow(0),
	player(0)
{
	// This is a dummy constructor. It is only called once in the CheckerGame class in order to use the cpPointer which handles drawing
}

Checkerpiece::Checkerpiece(sf::Color color, int radius, int startingX, int startingY, const int kingRow, const int startingRow, const int player) 
	: kingRow(kingRow),
	startingRow(startingRow), 
	player(player),
	sf::CircleShape(static_cast<float>(radius))
{
	setPosition(static_cast<float>(startingX), static_cast<float>(startingY));
	isKing = false;
	setFillColor(color);
}

// getters
bool Checkerpiece::getKing()
{
	return isKing;
}

const int Checkerpiece::getKingRow()
{
	return kingRow;
}

const int Checkerpiece::getStartingRow()
{
	return startingRow;
}

const int Checkerpiece::getPlayer()
{
	return player;
}

int Checkerpiece::getRank()
{
	return rank;
}

// setters
void Checkerpiece::setKing(bool isKing)
{
	this->isKing = isKing;
}

void Checkerpiece::setRank(int rank)
{
	this->rank = rank;
}

// draw the checkerpieces to the SFML window
void Checkerpiece::drawCheckers(sf::RenderWindow& window, const vector<Checkerpiece*>& checkers)
{
	sf::CircleShape circle;
	/*
	sf::Texture crownTexture;
	// for drawing a crown to represent a kinged checker
	if(!crownTexture.loadFromFile("resources/Crown.png"))
		std::cout << EXIT_FAILURE << std::endl;
	*/

	// only draw checkers if the container is not empty
	if(!checkers.empty())
	{
		if(XOFFSET == YOFFSET) // check if the width and height of the window are the same
			circle.setRadius(WINDOW_WIDTH / (2 * SQUARES_HORIZONTAL)); // all the checkerpieces have the same radius 
		else if(XOFFSET > YOFFSET)
			circle.setRadius(WINDOW_HEIGHT / (2 * SQUARES_HORIZONTAL));
		else
			circle.setRadius(WINDOW_WIDTH / (2 * SQUARES_HORIZONTAL));
		
		vector<Checkerpiece*>::const_iterator it;
		for(it = checkers.begin(); it != checkers.end(); ++it)
		{
			circle.setPosition((*it)->getPosition().x, (*it)->getPosition().y);
			circle.setFillColor((*it)->getFillColor());
			if((*it)->getKing()) 
			{
				//circle.setTexture(&crownTexture);
				circle.setOutlineThickness(-10);
				circle.setOutlineColor(sf::Color::Yellow);
			}
			else
			{
				//circle.setTexture(nullptr);
				circle.setOutlineThickness(0);
				circle.setOutlineColor(sf::Color(0, 0, 0, 255));
			}
			
			window.draw(circle);
		}
	}
}