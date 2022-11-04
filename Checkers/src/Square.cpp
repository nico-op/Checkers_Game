#include "Square.hpp"


//Crear un cuadrado con los parametros de columna y fila
Square :: Square(const sf::Vector2f& area, int row, int col, int idNumber) 
	: sf::RectangleShape(area),
	row(row),
	col(col),
	idNumber(idNumber)
{

}

Square :: ~Square()
{
	
}

// getters
bool Square :: getOccupied()
{
	return occupied;
}

const int Square :: getRow()
{
	return row;
}

const int Square :: getCol()
{
	return col;
}

const int Square :: getIdNumber()
{
	return idNumber;
}

// setters
void Square :: setOccupied(bool occupied)
{
	this->occupied = occupied;
}