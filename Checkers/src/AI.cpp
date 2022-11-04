#include "AI.hpp"

using std::vector;
using std::cout;
using std::endl;
//HI
// Constructor predeterminado
AI::AI()
{

}

AI::~AI()
{
    delete coords;
}

/* Encuentra el índice de la ficha en movimiento actual para el jugador activo usando la clase Moveable */
int AI::getCurrentMoveIndex(Player* active, Player* enemy, Checkerboard* checkerboard)
{
    vector<Checkerpiece*> activeCheckers = active->getCheckersVector();
    vector<Checkerpiece*> enemyCheckers = enemy->getCheckersVector();
    // iterar buscando la primera ficha con capacidad de salto
    for(unsigned int i = 0; i != activeCheckers.size(); ++i)
        if(Moveable::hasJump(activeCheckers.at(i), enemyCheckers, checkerboard))
        {
            cout << "P2 has found a jump @ index " << i << endl;
            currentIndex = i;
            return i;
        }
    // iterar de nuevo pero solo buscar un movimiento
    for(unsigned int i = 0; i != activeCheckers.size(); ++i)
        if(Moveable::hasMove(activeCheckers.at(i), enemyCheckers, checkerboard))
        {
            cout << "P2 has found a move @ index " << i << endl;
            currentIndex = i;
            return i;
        }
    return -1; // esto implica el fin del juego
}

/*
* AI_Move se usa cuando la computadora determina un movimiento.
* Maneja decisiones de salto y movimiento, así como actualizaciones y eliminaciones de variables.
* Estos son los pasos del algoritmo.
* 1) Iterar a través de las fichas de este jugador.
* 2) Iterar a través de las fichas enemigas hasta que encontremos una que podamos saltar. (usando el formato jumpByChecker)
3) Saltar, y actualizar posiciones y eliminaciones, y listo.
* 4) Si no hay salto, entonces encontramos un cuadrado para movernos.
* 5) Mueva y actualice las posiciones para el corrector en movimiento, y hemos terminado.
* 6) Si no podemos mover o saltar ninguna de las fichas, entonces se acabó el juego para este jugador.
*/
vector<int>* AI::AI_Move(Player* active, Player* enemy, Checkerboard* checkerboard)
{
    vector<Checkerpiece*> activeCheckers = active->getCheckersVector();
    vector<Checkerpiece*> enemyCheckers = enemy->getCheckersVector();
    for(unsigned i = 0; i != activeCheckers.size(); ++i)
    {
        // comprueba si la i-ésima ficha puede saltar
        coords = Moveable::findJump(activeCheckers.at(i), enemyCheckers, checkerboard); // obtiene coordenadas
        if(coords != nullptr)
        {
            cout << "P2 has found a jump->coords[0,1] " << coords->at(0) << " " << coords->at(1) << endl;
            return coords; //se detiene aquí y retorna las coordenadas
        }
    }

    // Llegados a este punto, si no hemos devuelto las coordenadas, sabemos que no hay saltos disponibles



// iterar de nuevo, pero esta vez solo buscar un movimiento ya que sabemos que no hay saltos
    for(int i = 0; i != activeCheckers.size(); ++i)
    {
        coords = Moveable::findMove(activeCheckers.at(i), checkerboard);
        if(coords != nullptr)
        {
            cout << "P2 has found a move->coords[0,1] " << coords->at(0) << " " << coords->at(1) << endl;
            return coords;
        }
    }

// Si llegamos tan lejos y no hemos devuelto ninguna coord, entonces sabemos que el juego ha terminado para este jugador.
    return coords;
}
