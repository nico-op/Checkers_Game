#include "CheckerGame.hpp"

using std::cerr;
using std::cout;
using std::endl;
using std::string;
using std::ostringstream;

sf::Time CheckerGame::timeElapsed;
int CheckerGame::winner = 0; // no hay ganador en la inicialización

CheckerGame::CheckerGame(sf::RenderWindow& window, const bool& isHuman1, const bool& isHuman2)
{
    isPlaying = false; // bucle de juego booleano
    checkerboard = new Checkerboard(window); // crea un nuevo tablero de ajedrez
    SoundManager::getSoundManager()->addSound("../resources/bites-ta-da-winner.ogg");
    SoundManager::getSoundManager()->addSound("../resources/ganar-ganador.ogg");
    SoundManager::getSoundManager()->addSound("../resources/mario-bros-woo-hoo.ogg");
    SoundManager::getSoundManager()->addSound("../resources/bonus.ogg");
    p1 = new Player (1, isHuman1); // create player 1
    p2 = new Player (2, isHuman2); // create player 2
}

CheckerGame::~CheckerGame()
{
    // punteros de recolección de basura
    delete checkerboard;
    checkerboard = nullptr;
    delete p1;
    delete p2;
    p1 = nullptr;
    p2 = nullptr;
}

void CheckerGame::startCheckers(sf::RenderWindow& window, sf::Event& event)
{
    p1->setTurn(true); // p1 comienza el juego
    p2->setTurn(false);
    createTeams();
    isPlaying = true;
    // inicia el bucle del juego de damas
    gameLoop(window, event);
}

// bucle de juego de damas (estado del juego: empatado, p1 arriba, p2 arriba, p1 gana, p2 gana)
void CheckerGame::gameLoop(sf::RenderWindow& window, sf::Event& event)
{
    sf::Clock clock; // poner en marcha el reloj
    int currentX = 0, currentY = 0; // las coordenadas actuales de la selección de un jugador
    int futureX = 0, futureY = 0; // se usa después de que un jugador selecciona una ficha y quiere moverse/saltar
    int mouseOverX = 0, mouseOverY = 0; // utilizado para crear el resaltado de cuadros verdes y morados
    bool selecting = true; // determina si un jugador está seleccionando o moviendo / saltando una ficha.
    bool checkDoubleJump = false; // esto se usa para determinar múltiples secuencias de salto
    int currentIndex = 0; // índice del cheque actual seleccionado en relación con el vector de un jugador
    int deleteIndex = 0; // índice del cheque actual que se elimina en relación con el vector de un jugador
    int generalDirection = 0; // la dirección general, determinada restando el futuro de las distancias actuales en la clase Movible
    Square* currentSquare = nullptr; // el cuadrado actual seleccionado
    Square* futureSquare = nullptr; // el futuro cuadrado seleccionado
    Square* tempSquare = nullptr; // el cuadrado temporal puede cumplir múltiples roles, consulte los métodos jumpBySquare / jumpByChecker en la clase Moveable.
    bool activePlayerIsHuman = isActivePlayerHuman(); // se usa para asegurarse de que solo se procesen los eventos de ventana si el jugador humano está activo

    while(isPlaying) // el juego continúa hasta que hay un ganador, un empate o se termina la ventana.
    {
/************************************************************* AI MOVEMENTS ******************************************/
        if(p2->getTurn() && !p2->getIsHuman()) // el jugador 2 es AI y este es su turno
        {
            // IA para la jugadora 2
            AI ai;
            currentIndex = ai.getCurrentMoveIndex(p2, p1, checkerboard);
            if(currentIndex == -1) // tenemos que hacer esto antes de establecer las coordenadas (no se pueden establecer las cuerdas en nullptr)
            {
                // juego terminado para p2, no más movimientos acordes
                p2->deleteAllCheckers();
            }
            else // las coordenadas serán de tamaño 4 o 6
            {
                std::vector<int> coords;
                coords = *ai.AI_Move(p2, p1, checkerboard);
                std::cout << "(AI) P2 turn : currentIndex " << currentIndex << std::endl;
                std::cout << "(AI) P2 turn : coord size " << coords.size() << std::endl;
                if(coords.size() == 4) // only a move
                {
                    currentX = coords[0];
                    currentY = coords[1];
                    mouseOverX = coords[2];
                    mouseOverY = coords[3];
                    // actualizar variables
                    currentSquare = checkerboard->findSquare(coords[0], coords[1]);
                    futureSquare = checkerboard->findSquare(coords[2], coords[3]); // el cuadrado al que moverse
                    if(currentSquare != nullptr && futureSquare != nullptr)
                    {
                        currentSquare->setOccupied(false); // el cuadrado en el que estaba sentado el corrector activo
                        futureSquare->setOccupied(true); // la plaza para moverse
                        p2->getChecker(currentIndex)->setPosition((float) coords[2], (float) coords[3]); // actualiza la posición actual de la ficha
                        // comprobar si nos coronaron
                        ifCheckerKinged(p2->getChecker(currentIndex), futureSquare);
                        printChecker(p2->getChecker(currentIndex), "MOVIENDO");
                        changeTurn(); // change to player 1 turn
                        selecting = true;
                        SoundManager::getSoundManager()->playSound(SOUND_MOVE);
                    }
                }
                else if(coords.size() == 6) // jumpByChecker
                {
                    currentX = coords[0];
                    currentY = coords[1];
                    mouseOverX = coords[2];
                    mouseOverY = coords[3];
                    // actualizar variables, usamos el formato jumpByChecker
                    currentSquare = checkerboard->findSquare(coords[0], coords[1]);
                    futureSquare = checkerboard->findSquare(coords[2], coords[3]); // el cuadrado en el medio
                    tempSquare = checkerboard->findSquare(coords[4], coords[5]); // la plaza para aterrizar
                    if(currentSquare != nullptr && futureSquare != nullptr && tempSquare != nullptr)
                    {
                        currentSquare->setOccupied(false); // el cuadrado en el que estaba sentado el corrector activo
                        futureSquare->setOccupied(false); // the square to land on
                        tempSquare->setOccupied(true); // la plaza que fue saltada
                        p2->getChecker(currentIndex)->setPosition((float) coords[4], (float) coords[5]); // actualiza la posición actual de la ficha
                        // comprobar si nos coronaron
                        ifCheckerKinged(p2->getChecker(currentIndex), tempSquare);
                        printChecker(p2->getChecker(currentIndex), "SALTANDO");
                        // elimina la ficha saltada (el turno de p2, así que elimina de p1)
                        deleteCheckerFromGame(p1, p1->findCheckerIndex(futureSquare));
                        changeTurn(); // change to player 1 turn
                        selecting = true;
                        SoundManager::getSoundManager()->playSound(SOUND_JUMP_CHECKER);
                    }
                }
            }
        }
        else if(!p2->getIsHuman() && !p1->getIsHuman()) // Ambas jugadoras son computadora
        {
            // both players use AI class
            // variable updates and checker deletions handled in player class
            changeTurn(); // change to player 2 turn
            selecting = true;
        }
/*********************************************************** END OF AI MOVEMENTS ************************************/
        // para todos los eventos de ventana, asumimos que el jugador activo está controlado por humanos.
        while(activePlayerIsHuman && window.pollEvent(event))
        {
            // terminates the application
            if(event.type == sf::Event::Closed)
            {
                std::cout << std::endl << "Se ha cerrado el juego" << std::endl;
                isPlaying = false;
                window.close();
            }
                // go back to the main menu (login screen)
            else if((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::B))
            {
                std::cout << std::endl << "Ha terminado el juego, vuelva al menu principal" << std::endl;
                isPlaying = false;
            }
            else if (event.type == sf::Event::MouseButtonPressed) // handles game movements
            {
                if(event.mouseButton.button == sf::Mouse::Left) // select a checker to move
                {
                    if(selecting) // player is selecting a checker to move
                    {
                        // get the coordinates from where the user clicked
                        currentX = event.mouseButton.x;
                        currentY = event.mouseButton.y;
                        currentSquare = checkerboard->findSquare(currentX, currentY);
                        /* The code for both players selection validation is symmetric, even though only player 1 has comments on the code. */
/********************************************************** PLAYER SELECTING ********************************************************************************/
                        if(currentSquare != nullptr && currentSquare->getOccupied()) // validar selección
                        {
                            if(p1->getTurn()) // p1's turn
                            {
                                currentIndex = p1->findCheckerIndex(currentSquare);
                                if(currentIndex != -1 && currentIndex < p1->getCounter())
                                {
                                    printChecker(p1->getChecker(currentIndex), "SELECCIONADO");
                                    // primero verifica si el verificador actual está saltando
                                    if(Moveable::hasJump(p1->getChecker(currentIndex), p2->getCheckersVector(), checkerboard))
                                    {
                                        // el verificador actual tiene un salto para que sepamos que es una selección válida
                                        selecting = false;
                                    }
                                    else
                                    {
                                        if(playerHasToJump(p1, p2))
                                            selecting = true; // el jugador tiene un salto, obliga al jugador a seleccionar otra ficha
                                        else
                                            selecting = false; // no hay saltos disponibles, el jugador está listo para mover su ficha seleccionada
                                    }
                                }
                                else
                                    selecting = true; // sigue seleccionando (no se encontró el verificador, probablemente un error)
                            }
                            else // turno de p2
                            {
                                currentIndex = p2->findCheckerIndex(currentSquare);
                                if(currentIndex != -1 && currentIndex < p2->getCounter())
                                {
                                    printChecker(p2->getChecker(currentIndex), "SELECCIONADO");
                                    if(Moveable::hasJump(p2->getChecker(currentIndex), p1->getCheckersVector(), checkerboard))
                                    {
                                        selecting = false;
                                    }
                                    else
                                    {
                                        if(playerHasToJump(p2, p1))
                                            selecting = true; // el jugador tiene que saltar, fuerza al jugador a seleccionar otra ficha
                                        else
                                            selecting = false; // El jugador está listo para mover su corrector seleccionado
                                    }
                                }
                                else
                                    selecting = true; // sigue seleccionando (no se encontró el corrector)
                            }
                        }
                        else // sigue seleccionando, el usuario seleccionó un cuadrado vacío
                            selecting = true;
/**************************************************** END OF PLAYERS SELECTING. *****************************************************************************/
                    }
                    else if(!selecting) // una pieza de ajedrez se mueve o salta
                    {
                        checkDoubleJump = false;
                        /* Se analizarán los movimientos de ambos jugadores para determinar si se trata de un salto, movimiento, etc.
                               El código real para cada jugador es simétrico, aunque solo el jugador 1 tiene comentarios. */
                        futureX = event.mouseButton.x;
                        futureY = event.mouseButton.y;
                        futureSquare = checkerboard->findSquare(futureX, futureY); // El cuadrado al que el jugador quiere moverse
                        generalDirection = Moveable::findGeneralDirection(currentSquare, futureSquare); // encuentra la dirección general (en grados) con respecto al futuro cuadrado seleccionado
                        std::cout << "Moving in the general Direction of: " << generalDirection << std::endl;
                        if(generalDirection == -1)
                        {
                            std::cout << "Moving nowhere (no turn change). Picking another checker to move." << std::endl;
                            tempSquare = nullptr;
                        }
                        else if(futureSquare != nullptr)
                        {
                            if(futureSquare->getOccupied()) // el cuadrado futuro está ocupado, por lo que podríamos estar saltando sobre una ficha
                                tempSquare = checkerboard->findJumpOntoSquare(futureSquare, generalDirection);
                            else // FutureSquare desocupado, Caso 1) podría ser seleccionar el cuadrado para aterrizar después de saltar, o Caso 2) simplemente moverse
                                tempSquare = checkerboard->findIntermSquare(futureSquare, generalDirection);
                        }
                        else
                            std::cout << "The Future Square was not found." << std::endl;

/************************************************************* CODE FOR BOTH PLAYERS MOVEMENTS. *******************************************************/
                        if(tempSquare != nullptr) // la única forma en que tempSquare es nulo es cuando no se encuentra generalDirection == -1 o futureSquare.
                        {
                            if(futureSquare->getFillColor() == sf::Color(92,37,0)) //Color cafe // make sure the future square is moveable
                            {
                                if(p1->getTurn())
                                {
                                    if(p1->getIsHuman())
                                    {
                                        // primero verifica si el jugador está seleccionando la ficha para saltar (debe llamar a jumpByChecker() y jumpBySquare() antes de moveable())
                                        if(Moveable::jumpByChecker(p1->getCheckersVector(), currentSquare, futureSquare, tempSquare, currentIndex))
                                        {
                                            // actualización de variables
                                            currentSquare->setOccupied(false);
                                            futureSquare->setOccupied(false);
                                            tempSquare->setOccupied(true);
                                            // actualizar la posición de la ficha saltadora
                                            p1->getChecker(currentIndex)->setPosition(tempSquare->getPosition().x, tempSquare->getPosition().y);
                                            // comprueba si la ficha saltadora se convirtió en rey
                                            ifCheckerKinged(p1->getChecker(currentIndex), tempSquare);
                                            printChecker(p1->getChecker(currentIndex), "SALTANDO");
                                            // elimina la ficha saltada (el turno de p1, así que elimina de p2)
                                            deleteCheckerFromGame(p2, p2->findCheckerIndex(futureSquare));
                                            checkDoubleJump = true;// acabamos de completar un salto, veamos si hay un doble salto disponible
                                            changeTurn();
                                            SoundManager::getSoundManager()->playSound(SOUND_JUMP_CHECKER);
                                        }
                                            // Comprueba si el jugador está saltando seleccionando el cuadrado para aterrizar después de un salto (futuro cuadrado en este caso), el cuadrado temporal está en el medio
                                        else if(Moveable::jumpBySquare(p1->getCheckersVector(), currentSquare, futureSquare, tempSquare, currentIndex))
                                        {
                                            // actualiza las variables después de saltar
                                            futureSquare->setOccupied(true);
                                            currentSquare->setOccupied(false);
                                            tempSquare->setOccupied(false); // la ficha saltada
                                            // actualiza la posición de la ficha saltadora
                                            p1->getChecker(currentIndex)->setPosition(futureSquare->getPosition().x, futureSquare->getPosition().y);
                                            // comprueba si la ficha saltadora se convirtió en rey
                                            ifCheckerKinged(p1->getChecker(currentIndex), futureSquare);
                                            printChecker(p1->getChecker(currentIndex), "SALTANDO");
                                            // elimina la ficha saltada (el turno de p1, así que elimina de p2)
                                            deleteCheckerFromGame(p2, p2->findCheckerIndex(tempSquare));
                                            checkDoubleJump = true; // acabamos de completar un salto, veamos si hay un doble salto disponible
                                            changeTurn();
                                            SoundManager::getSoundManager()->playSound(SOUND_JUMP_SQUARE);
                                        }
                                            // finalmente verifique si se mueve sin saltar
                                        else if(Moveable::moveable(p1->getCheckersVector(), currentSquare, futureSquare, currentIndex) && !futureSquare->getOccupied())
                                        {
                                            /* Esta llamada hasJump es necesaria para evitar que una ficha salte dos veces
                                             de saltar un salto. Esto se debe a que la variable booleana 'seleccionar' no
                                             volver a verdadero durante un doble salto, que es donde se realizaría una validación
                                             para asegurarse de que no hay saltos pendientes para el verificador activo. */
                                            if(!Moveable::hasJump(p1->getChecker(currentIndex), p2->getCheckersVector(), checkerboard))
                                            {
                                                // actualiza la posición de todos los cuadrados y fichas en juego
                                                futureSquare->setOccupied(true);
                                                currentSquare->setOccupied(false);
                                                p1->getChecker(currentIndex)->setPosition(futureSquare->getPosition().x, futureSquare->getPosition().y);
                                                // verificar si la ficha se movió a una fila de rey
                                                ifCheckerKinged(p1->getChecker(currentIndex), futureSquare);
                                                printChecker(p1->getChecker(currentIndex), "MOVIENDO");
                                                checkDoubleJump = false;
                                                changeTurn();
                                                SoundManager::getSoundManager()->playSound(SOUND_MOVE);
                                            }
                                        }

                                        // Ahora tenemos que averiguar si la verificación actual puede hacer un doble salto (esto solo se verifica después de un salto exitoso)
                                        if(checkDoubleJump && Moveable::hasJump(p1->getChecker(currentIndex), p2->getCheckersVector(), checkerboard))
                                        {
                                            // doble salto disponible, actualizamos el cuadro actual para la próxima iteración del ciclo del juego
                                            currentX = (int) p1->getChecker(currentIndex)->getPosition().x;
                                            currentY = (int) p1->getChecker(currentIndex)->getPosition().y;
                                            currentSquare = checkerboard->findSquare(currentX, currentY);
                                            selecting = false; // Técnicamente, la selección nunca cambió, pero queremos asegurarnos de que siga siendo falsa.
                                            /* necesitamos volver a llamar a changeTurn() porque ya se llamó en el primer salto exitoso
                                            (así que cancela la llamada por completo y, por lo tanto, sigue siendo el turno del jugador 1). */
                                            changeTurn();
                                            checkDoubleJump = false; // acabamos de hacer un doble salto, así que reinicia esto a falso.
                                            tempSquare = nullptr; // restablecer esto a nulo para la siguiente iteración del ciclo del juego
                                        }
                                        else
                                            selecting = true; // Sin doble salto, el giro ha cambiado, vuelve a seleccionar
                                    }
                                }
                                else // p2->getTurn()
                                {
                                    if(p2->getIsHuman())
                                    {
                                        if(Moveable::jumpByChecker(p2->getCheckersVector(), currentSquare, futureSquare, tempSquare, currentIndex))
                                        {
                                            currentSquare->setOccupied(false);
                                            futureSquare->setOccupied(false);
                                            tempSquare->setOccupied(true);
                                            p2->getChecker(currentIndex)->setPosition(tempSquare->getPosition().x, tempSquare->getPosition().y);
                                            ifCheckerKinged(p2->getChecker(currentIndex), tempSquare);
                                            printChecker(p2->getChecker(currentIndex), "SALTANDO");
                                            deleteCheckerFromGame(p1, p1->findCheckerIndex(futureSquare));
                                            checkDoubleJump = true;
                                            changeTurn();
                                            SoundManager::getSoundManager()->playSound(SOUND_JUMP_CHECKER);
                                        }
                                        else if(Moveable::jumpBySquare(p2->getCheckersVector(), currentSquare, futureSquare, tempSquare, currentIndex))
                                        {
                                            futureSquare->setOccupied(true);
                                            currentSquare->setOccupied(false);
                                            tempSquare->setOccupied(false);
                                            p2->getChecker(currentIndex)->setPosition(futureSquare->getPosition().x, futureSquare->getPosition().y);
                                            ifCheckerKinged(p2->getChecker(currentIndex), futureSquare);
                                            printChecker(p2->getChecker(currentIndex), "SALTANDO");
                                            deleteCheckerFromGame(p1, p1->findCheckerIndex(tempSquare));
                                            checkDoubleJump = true;
                                            changeTurn();
                                            SoundManager::getSoundManager()->playSound(SOUND_JUMP_SQUARE);
                                        }
                                        else if(Moveable::moveable(p2->getCheckersVector(), currentSquare, futureSquare, currentIndex) && !futureSquare->getOccupied())
                                        {
                                            if(!Moveable::hasJump(p2->getChecker(currentIndex), p1->getCheckersVector(), checkerboard))
                                            {
                                                futureSquare->setOccupied(true);
                                                currentSquare->setOccupied(false);
                                                p2->getChecker(currentIndex)->setPosition(futureSquare->getPosition().x, futureSquare->getPosition().y);
                                                ifCheckerKinged(p2->getChecker(currentIndex), futureSquare);
                                                printChecker(p2->getChecker(currentIndex), "MOVIENDO");
                                                checkDoubleJump = false;
                                                changeTurn();
                                                SoundManager::getSoundManager()->playSound(SOUND_MOVE);
                                            }
                                        }

                                        if(checkDoubleJump && Moveable::hasJump(p2->getChecker(currentIndex), p1->getCheckersVector(), checkerboard))
                                        {
                                            currentX = (int) p2->getChecker(currentIndex)->getPosition().x;
                                            currentY = (int) p2->getChecker(currentIndex)->getPosition().y;
                                            currentSquare = checkerboard->findSquare(currentX, currentY);
                                            selecting = false;
                                            changeTurn();
                                            checkDoubleJump = false;
                                            tempSquare = nullptr;
                                        }
                                        else
                                            selecting = true;
                                    }
                                } // end of p2's turn
/************************************************* END OF MOVEMENT VALIDATION FOR EACH PLAYER *******************************************************/
                            }
                            std::cout << "Changing current checker. Code executed @ line " << __LINE__ << std::endl;
                            selecting = true;
                        }
                    } // end of if(!selecting)
                } // end of if(event.mouseButton.button == sf::Mouse::Left)
            }/* Guarda las coordenadas actuales del mouse para el gráfico resaltado */
            else if (event.type == sf::Event::MouseMoved)
            {
                mouseOverX = event.mouseMove.x;
                mouseOverY = event.mouseMove.y;
            }
            else if(event.type == sf::Event::Resized)
            {
                std::cout << std::endl << "new window x: " << window.getSize().x << std::endl;
                std::cout << "new window y: " << window.getSize().y << std::endl << std::endl;

            }
        }// eventos de fin de ventana loopcords

        handleGameState(clock); // comprobar el estado del juego
        activePlayerIsHuman = isActivePlayerHuman(); //Determinar si el jugador activo es humano

// Secuencia de dibujo de la ventana SFML
        window.clear();
        checkerboard->drawGrid(window, mouseOverX, mouseOverY, currentX, currentY, selecting);
        cpDrawer.drawCheckers(window, p1->getCheckersVector());
        cpDrawer.drawCheckers(window, p2->getCheckersVector());
        window.display();

    } // end of isPlaying
} // end of gameLoop

// cambios
void CheckerGame::changeTurn()
{
    p1->setTurn(!p1->getTurn());
    p2->setTurn(!p2->getTurn());

}

// create the teams
void CheckerGame::createTeams()
{
    int startingX = 0;
    int startingY = 0;
    int radius = 0;
    int idNumber = 0;

    for(int i = 0; i < SQUARES_VERTICAL; ++i)
    {
        for(int j = 0; j < SQUARES_HORIZONTAL; ++j)
        {
            // check if the current square holds a checker
            if(checkerboard->findSquare(idNumber++)->getOccupied())
            {
                radius = SQUARES_VERTICAL / (2 * SQUARES_HORIZONTAL);
                if(i <= 2)
                    p1->addChecker(new Checkerpiece(sf::Color::White, radius, startingX, startingY, KING_ROW_7, i, p1->getNumber()));
                else
                    p2->addChecker(new Checkerpiece(sf::Color::Black, radius, startingX, startingY, KING_ROW_0, i, p2->getNumber()));
            }
            startingX += XOFFSET;
        }
        startingY += YOFFSET;
        startingX = 0;

    }
}

// comprueba si el jugador tiene otros saltos
bool CheckerGame::playerHasToJump(Player*& active, Player*& notActive)
{
    /* Iterar a través de cada ficha en el vector del jugador actual,
     debemos asegurarnos de que el jugador no tenga otros saltos disponibles
     antes de que puedan mover su ficha seleccionada. */
    for(int i = 0; i < active->getCounter(); ++i)
        if(Moveable::hasJump(active->getChecker(i), notActive->getCheckersVector(), checkerboard))
            return true; // player has to jump
    return false;
}

/*
* Determina si el jugador activo puede mover cualquiera de sus fichas.
* Utiliza una IA para encontrar un movimiento actual para el jugador (-1 significa que no hay movimiento actual).
*/
bool CheckerGame::playerCannotMove(Player*& p1, Player*& p2, Checkerboard*& checkerboard)
{
    if(p1->getTurn() == true)
    {
        AI ai;
        if(ai.getCurrentMoveIndex(p1, p2, checkerboard) == -1)
        {
            p1->deleteAllCheckers();
            return true;
        }
        else
            return false;
    }
    else
    {
        AI ai;
        if(ai.getCurrentMoveIndex(p2, p1, checkerboard) == -1)
        {
            p2->deleteAllCheckers();
            return true;
        }
        else
            return false;
    }
}

// handles the game state, modifies the class variable 'isPlaying'
void CheckerGame::handleGameState(sf::Clock& clock)
{
    // tied
    if(p1->getCounter() == p2->getCounter())
    {
    }
        // player 2 wins
    else if(p1->getCounter() == 0)
    {
        isPlaying = false;
        std::cout << std::endl << "Jugador dos gana " << std::endl;
        winner = p2->getNumber();
        CheckerGame::timeElapsed = clock.restart(); // obtener la duración del juego
        std::cout << "Time Elapsed (in minutes): " << (CheckerGame::timeElapsed.asSeconds() / 60) << std::endl;
        saveTime((CheckerGame::timeElapsed.asSeconds() / 60)); // guarda el tiempo en el almacenamiento externo
    }
        // player 1 wins
    else if(p2->getCounter() == 0)
    {
        isPlaying = false;
        std::cout << std::endl << "Jugador uno gana" << std::endl;
        winner = p1->getNumber();
        CheckerGame::timeElapsed = clock.restart(); // obtener la duración del juego
        std::cout << "Time Elapsed (in minutes): " << (CheckerGame::timeElapsed.asSeconds() / 60) << std::endl;
        saveTime((CheckerGame::timeElapsed.asSeconds() / 60)); // guarda el tiempo en el almacenamiento externo
    }
        // p2 leads
    else if(p1->getCounter() < p2->getCounter())
    {

    }
        // p1 leads
    else if(p1->getCounter() > p2->getCounter())
    {

    }
}

// determine if the active player is a human
bool CheckerGame::isActivePlayerHuman()
{
    if(p1->getTurn() && p1->getIsHuman())
        return true;
    else if(p1->getTurn() && !p1->getIsHuman())
        return false;
    else if(p2->getTurn() && p2->getIsHuman())
        return true;
    else if(p2->getTurn() && !p2->getIsHuman())
        return false;
    else
        return false; // default false
}

// imprimir las credenciales del verificador seleccionado
void CheckerGame::printChecker(Checkerpiece* selected, const std::string& action)
{
    std::cout << action << " Checker (x, y): (" << selected->getPosition().x << ", "
              << selected->getPosition().y << ")" << " King: "
              << selected->getKing() << std::endl;


}

// delete from the inactive player
void CheckerGame::deleteCheckerFromGame(Player* deleteFrom, const int& deleteIndex)
{
    // elimina la ficha saltada (delete_From es el jugador inactivo cuya ficha saltó)
    if(deleteIndex != -1 && deleteIndex < deleteFrom->getCounter())
    {
        std::cout << "Eliminando Checker (x, y): (" << deleteFrom->getChecker(deleteIndex)->getPosition().x << ", "
                  << deleteFrom->getChecker(deleteIndex)->getPosition().y
                  << ")" << " King: " << deleteFrom->getChecker(deleteIndex)->getKing() << std::endl;
        deleteFrom->deleteChecker(deleteIndex);
    }
}

// comprobar si la pieza de ajedrez activa se convirtió en un rey
void CheckerGame::ifCheckerKinged(Checkerpiece* checker, Square* square)
{
    if((square->getRow() == KING_ROW_0 && checker->getKingRow() == KING_ROW_0)
       || (square->getRow() == KING_ROW_7 && checker->getKingRow() == KING_ROW_7))
    {
        checker->setKing(true); // actualizar el corrector para ser un rey
        SoundManager::getSoundManager()->playSound(SOUND_KING);
    }
}

// save the time
void CheckerGame::saveTime(const double& time)
{
    std::cout << "Saving the time to game_times.sav" << std::endl;
    std::ofstream file("game_times.sav", std::ios::app); //actualizar el corrector para ser un rey
    if(!file)
        std::cerr << "File won't open." << std::endl;
    else
    {
        file << time; // save the tkkkime to the file
        file << "\n"; // add a newline
        file.close(); // close the file
    }
}

/* Mostrar el ganador en la ventana de SFML después de que termine el juego */
void CheckerGame::showWinner(sf::RenderWindow& window, sf::Event& event)
{
    ostringstream ostr; // create ostream
    ostr << "The\nwinner\nis\nPlayer " << winner << "\n"; // ostr takes in the text
    const string winnerText = ostr.str(); // convert ostr to string and save it
    // load the text font
    sf::Font font;
    if(!font.loadFromFile("resources/ENGR.TTF"))
        cerr << "ERROR - cannot load resources" << endl;
    sf::Text text;
    text.setFont(font);
    text.setCharacterSize(WINDOW_WIDTH / 8);
    text.setPosition(0, 0); // draw slightly below the title
    text.setColor(sf::Color::Black);
    text.setString(winnerText);

    //Rectangulo horizontal
    sf::RectangleShape tile2;
    tile2.setSize(sf::Vector2f(70.f,100.f));
    tile2.setPosition(sf::Vector2f(50,454));
    tile2.setFillColor(sf::Color(139,69,19)); //Color cafe
    window.draw(tile2);

    // this loop keeps the top 10 best scores on the window
    bool view = true;
    while(view)
    {
        while(window.pollEvent(event))
        {
            // go back to the main menu
            if((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape))
            {
                cout << endl << "Closing Winner View. Returning to Main Menu." << endl;
                view = false;
            }

            // SFML drawing sequence
            window.clear(sf::Color::White);
            window.draw(text); // draw the times and header
            window.display();


        }
    }


}