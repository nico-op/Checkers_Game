#include "Player.hpp"

using std::vector;
using std::cout;
using std::endl;
using std::string;

Player::Player(const int number, const bool isHuman)
	: isHuman(isHuman)
	, number(number)
{
	vector<Checkerpiece*> checkers; 
	// reserva espacio de memoria para 12 piezas
	checkers.reserve(MAX_CHECKERS_PER_TEAM);
}

Player::~Player()
{
	// borrar por completo el vector perteneciente a checkers
	checkers.erase(checkers.begin(), checkers.end());
}

/* Muestra en pantalla las piezas de checkers */
void Player::displayCheckers() 
{
	cout << endl <<"Display Player# " << this->getNumber() << "'s active checkers" << endl;
	vector<Checkerpiece*>::const_iterator it;
	for(it = this->checkers.begin(); it != this->checkers.end(); ++it)
		cout << "Position(x,y): (" << (*it)->getPosition().x << ", " << (*it)->getPosition().y << ") " << 
			"King Row: " << (*it)->getKingRow() << " King: " << (*it)->getKing() << endl;
}

// añade una pieza checker a la lista del jugador
void Player::addChecker(Checkerpiece* checkerpiece)
{
	checkers.push_back(checkerpiece);//añade la pieza a la lista checkers
}


// retorna el index del número de un pieza checker en el vector general de checkers tomando la posición(x,y)
int Player::findCheckerIndex(const int& x, const int& y)
{
	for(unsigned int i = 0; i != this->checkers.size(); ++i)
		if(checkers[i]->getPosition().x == x && checkers[i]->getPosition().y == y)
			return i;
	return -1; // pieza checker no encontrada
}


// encuentra un pieza de checkers en la lista general de checkers comparando la posición del cuadro y la posición de la pieza de checkers
int Player::findCheckerIndex(const Square* tempSquare)
{
	for(unsigned int i = 0; i < checkers.size(); ++i)
		if((checkers[i]->getPosition().x == tempSquare->getPosition().x) && (checkers[i]->getPosition().y == tempSquare->getPosition().y))
			return i;
	return -1; // pieza de checker no encontrada
}


// elimina una pieza de checker
void Player::deleteChecker(const int& deleteIndex)
{
    this->checkers.erase(this->checkers.begin() + deleteIndex);
}


// elimina a todos los checkers
void Player::deleteAllCheckers()
{
	this->checkers.erase(this->checkers.begin(), this->checkers.end());
}

// actuliza el vector donde se guarda las piezas de checkers
vector<Checkerpiece*>& Player::getCheckersVector()
{
	return checkers;
}

// return the Checkerpiece* at the index
// retorna la pieza de checker en el indice
Checkerpiece* Player::getChecker(const int& index)
{
	return this->checkers.at(index);
}

bool Player::getTurn()
{
	return turn;
}

//Verifica que si el jugador está jugando, además de llegar un conteo de las jugadas
int Player::getCounter()
{
	return checkers.size();
}

const bool Player::getIsHuman()
{
	return isHuman;
}

const int Player::getNumber()
{
	return number;
}

// Indica a quien de las partes le toca hacer un movimiento
void Player::setTurn(const bool& turn)
{
	this->turn = turn;
}