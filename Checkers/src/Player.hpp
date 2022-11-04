/*
*	This class represents a Player in the checker game.
*	Each player has a number (like player 1 or player 2). And they have boolean to determine 
*	whether or not the player is human controlled or computer controlled.
*	The player has a finite number of checker pieces in the game. They can use the checker 
*	pieces to make a jump or a simple move. They cannot jump their own checkers.
*/

#ifndef PLAYER_HPP
#define PLAYER_HPP

#include "Checkerboard.hpp"
#include "Checkerpiece.hpp"
#include "Moveable.hpp"
#include <vector>
#include <iostream>
#include <string>

//Clase Player
class Player
{
    //Declaración de varialbes
private:
	const int number;
	bool turn;
	const bool isHuman;
	std::vector<Checkerpiece*> checkers;
protected:
public:
	Player(const int, const bool);//constructor
	~Player();

	int findCheckerIndex(const int&, const int&);
	int findCheckerIndex(const Square*);
	int* findJump (Square*, int, int, int, int);
	void deleteChecker(const int&);
	void deleteAllCheckers();
	void displayCheckers();
	void addChecker(Checkerpiece*);
	// getters
	bool getTurn();
	int getCounter();
	const bool getIsHuman();
	const int getNumber();
	Checkerpiece* getChecker(const int&);
	std::vector<Checkerpiece*>& getCheckersVector();
	// setters
	void setTurn(const bool&);
};
#endif 