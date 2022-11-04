#include "Moveable.hpp"

using std::vector;
using std::abs;

/*
*	The moveable function determines whether or not the checker
*	is making a valid movement. It does not check whether or not the future
*	square it is moving to is occupied, that is done externally in CheckerGame class.
*	We first check if the checker is moving onto a square containing a friendly checker.
*	Then we check both cases of wether or not the current checker is a king.
*	If it is a pawn, we check to make sure it is moving in the right direction,
*	otherwise it is not necessary because a king can move in any direciton.
*	Lastly, we check to see if the distance is moveable (one square at a time).
*	All default, or unchecked cases will return false.
*/
bool Moveable::moveable(std::vector<Checkerpiece*> checkers, Square* current, Square* future, const int& currentIndex)
{
    if(friendly(checkers, future))
        return false;

    if(static_cast<unsigned>(currentIndex) < checkers.size())
    {
        // if a king is being moved
        if(checkers.at(currentIndex)->getKing())
        {
            // movement on the checkerboard from current to future square is always the same color (black to black)
            if(current->getFillColor() == future->getFillColor())
            {
                // make sure that the distance is within the approriate distance range
                if(moveableDistance(static_cast<int>(current->getPosition().x), static_cast<int>(current->getPosition().y),
                                    static_cast<int>(future->getPosition().x), static_cast<int>(future->getPosition().y)))
                    return true;
                else
                    return false;
            }
            else
                return false;
        }
            // otherwise a pawn is being moved (can only move diagonal, "forwards", which is determined by their king row)
        else
        {
            // movement on the checkerboard from current to future square is always the same color (black to black)
            if(current->getFillColor() == future->getFillColor())
            {
                // case: KING_ROW_0 (northern part of the checkerboard relative to the monitor)
                if(checkers.at(currentIndex)->getKingRow() == KING_ROW_0)
                {
                    // pawn is headed towards ROW 0, so y must decrease in the computer's coordinate system
                    if(current->getPosition().y > future->getPosition().y)
                    {
                        if(moveableDistance((int) current->getPosition().x, (int) current->getPosition().y, (int) future->getPosition().x, (int) future->getPosition().y))
                            return true;
                        else
                            return false;
                    }
                    else
                        return false;
                }
                    // case: KING_ROW_7 (southern part of the checkerboard relative to the monitor)
                else if(checkers[currentIndex]->getKingRow() == KING_ROW_7)
                {
                    // pawn is headed towards ROW 7, so y must increase in the computer's coordinate system
                    if(current->getPosition().y < future->getPosition().y)
                    {
                        if(moveableDistance((int) current->getPosition().x, (int) current->getPosition().y, (int) future->getPosition().x, (int) future->getPosition().y))
                            return true;
                        else
                            return false;
                    }
                    else
                        return false;
                }
                else
                    return false;
            }
            else
                return false;
        }
    }
    else
        return false; // default return of false
}

// overloaded moveable which will directly check to make sure the future square isn't occupied
bool Moveable::moveable(Checkerpiece* checker, Square* current, Square* future)
{
    // check to make sure future position isn't already occupied.
    if(future->getOccupied())
        return false;
    else if(checker->getKing()) // a king is being moved
    {
        // movement on the checkerboard from current to future square is always the same color (black to black)
        if(current->getFillColor() == future->getFillColor())
        {
            // no need to check forward or backward movement for kings
            if(moveableDistance((int) current->getPosition().x, (int) current->getPosition().y, (int) future->getPosition().x, (int) future->getPosition().y))
                return true;
            else
                return false;
        }
        else
            return false;
    }
    else // a pawn is being moved (can only move diagonal and in one direction)
    {
        // movement on the checkerboard from current to future square is always the same color (black to black)
        if(current->getFillColor() == future->getFillColor())
        {
            // pawns can only move forward, where forward is defined by what team the pawn is on
            if(checker->getKingRow() == KING_ROW_0)
            {
                // pawn is headed towards ROW 0, so y decreases in the computer's coordinate system
                if(current->getPosition().y > future->getPosition().y)
                {
                    if(moveableDistance((int) current->getPosition().x, (int) current->getPosition().y, (int) future->getPosition().x, (int) future->getPosition().y))
                        return true;
                    else
                        return false;
                }
                else
                    return false;
            }
                // this pawn can only move "down" the board, in terms of the computer's coordinate system, y must increase
            else if(checker->getKingRow() == KING_ROW_7)
            {
                // pawn is headed towards ROW 7, so y must increase in the computer's coordinate system
                if(current->getPosition().y < future->getPosition().y)
                {
                    if(moveableDistance((int) current->getPosition().x, (int) current->getPosition().y, (int) future->getPosition().x, (int) future->getPosition().y))
                        return true;
                    else
                        return false;
                }
                else
                    return false;
            }
            else
                return false;
        }
        else
            return false;
    }
}

// helper function that checks the distance between current and future relative to the offset.
bool Moveable::moveableDistance(const int& currentX, const int& currentY, const int& futureX, const int& futureY)
{
    if((abs(currentX - futureX) <= XOFFSET) && (futureX <= (XOFFSET * SQUARES_HORIZONTAL)) && (futureX >= 0))
        if((abs(currentY - futureY) <= YOFFSET) && (futureY <= (YOFFSET * SQUARES_VERTICAL)) && (futureY >= 0))
            return true; // distance is moveable
    return false; // distance is not moveable
}

// find the general direction from current relative to future (in terms of a 2-dimensional unit circle)
int Moveable::findGeneralDirection(Square* current, Square* future)
{
    if((current->getPosition().x < future->getPosition().x) && (current->getPosition().y == future->getPosition().y))
        return 0; // general direction is 0 degrees towards the right
    else if((current->getPosition().x > future->getPosition().x) && (current->getPosition().y == future->getPosition().y))
        return 180; // general direction is 180 degrees towards the left
    else if((current->getPosition().x == future->getPosition().x) && (current->getPosition().y < future->getPosition().y))
        return 270; // general direction is 270 degrees straight downwards
    else if((current->getPosition().x == future->getPosition().x) && (current->getPosition().y > future->getPosition().y))
        return 90; // general direction is 90 degrees straight upwards
    else if((current->getPosition().x < future->getPosition().x) && (current->getPosition().y > future->getPosition().y))
        return 45; // general direction is 45 degrees upwards and to the right
    else if((current->getPosition().x < future->getPosition().x) && (current->getPosition().y < future->getPosition().y))
        return 315; // general direction is 315 degrees downwards and to the right
    else if((current->getPosition().x > future->getPosition().x) && (current->getPosition().y > future->getPosition().y))
        return 135; // general direction is 135 degrees upwards and to the left
    else if((current->getPosition().x > future->getPosition().x) && (current->getPosition().y < future->getPosition().y))
        return 225; // general direction is 225 degrees downwards and to the left
    else return -1;
}

/*
*	jumpBySquare checks to see if the player can perform a jump by choosing the square to land on following a jump. 
*	Future square is the square to land on followin the jump (should be unoccupied).
*	Temp square is the middle square (should be occupied).
*	First we check to make sure that the moving checker isn't jumping over a friendly checker.
*	Then we check the distance between current->temp and temp->future by calling moveable().
*/
bool Moveable::jumpBySquare(std::vector<Checkerpiece*> checkers, Square* current, Square* future, Square* temp, const int& currentIndex)
{
    if(friendly(checkers, temp, future))
        return false;
    if(temp->getOccupied())
        if(moveable(checkers, current, temp, currentIndex)) // checks if current->temp is moveable
            if(moveable(checkers, temp, future, currentIndex)) // checks if temp->future is moveable
                return true;
    return false;
}

/*
*	jumpByChecker checks if the user can perform a jump by selecting the checker they want to jump over.
*	Future square is the square to jump over (should be occupied).
*	Temp square is the square to land on following the jump (should be unoccipied).
*	First we check to make sure that the moving checker isn't jumping over a friendly checker.
*	Then we check the distance from future->temp and then current->future by calling moveable(). 
*/
bool Moveable::jumpByChecker(std::vector<Checkerpiece*> checkers, Square* current, Square* future, Square* temp, const int& currentIndex)
{
    if(friendly(checkers, temp, future))
        return false;
    if(!temp->getOccupied())
        if(moveable(checkers, future, temp, currentIndex)) // checks if current->temp is moveable
            if(moveable(checkers, current, future, currentIndex)) // checks if temp->future is moveable
                return true;
    return false;
}

/* 
*	We check if the current checker is trying to move over a friendly checker by scanning the temp/future square's
*	alongside every other checker that the player has on the board to see if their positions match. 
*	If there's a match we return true. Return false otherwise.
*/
bool Moveable::friendly(const vector<Checkerpiece*>& checkers, Square* temp, Square* future)
{
    vector<Checkerpiece*>::const_iterator it;
    for(it = checkers.begin(); it != checkers.end(); ++it)
        if((*it)->getPosition() == temp->getPosition() || (*it)->getPosition() == future->getPosition())
            return true;
    return false;
}

bool Moveable::friendly(const vector<Checkerpiece*>& checkers, Square* future)
{
    vector<Checkerpiece*>::const_iterator it;
    for(it = checkers.begin(); it != checkers.end(); ++it)
        if((*it)->getPosition() == future->getPosition())
            return true;
    return false;
}

/*
*	The hasJump function figures out if the active checkerpiece in play can jump.
*	We can the enemy checkers to see if one is jumpeable by the active checker.
*	First checking if the active checker is king,
*	if it is not a king then we check which king row it has,
*	and then we iterate through several cases to see if the active checker can safely 
*	land a jump over the enemy (by making sure that landing space is unoccupied).
*	We assume that the active checker is not within the enemy checker list.
*	There's also the kitty corner jump bug (documentation in Checkerboard.hpp) for cases 135, 225.
*/
bool Moveable::hasJump(Checkerpiece* checker, const vector<Checkerpiece*>& enemy, Checkerboard*& tempBoard)
{
    Square* future;
    int futureX, futureY;
    vector<Checkerpiece*>::const_iterator it;
    for(it = enemy.begin(); it != enemy.end(); ++it)
    {
        if(!checker->getKing()) // test only 2 possible jumps
        {
            if(checker->getKingRow() == KING_ROW_7) // the current check is moving southward
            {
                if((*it)->getPosition().x != 0 && abs(checker->getPosition().x - XOFFSET) == (*it)->getPosition().x
                   && (checker->getPosition().y  + YOFFSET) == (*it)->getPosition().y) // first check 225 degrees ( down and to the left )
                {
                    // enemy checker is within jumpeable range, so check if there is a friendly or enemy withing landing range
                    futureX = (int) abs(checker->getPosition().x - (2*XOFFSET));
                    futureY = (int) checker->getPosition().y + (2*YOFFSET);
                    // now see if the landing square is occupied
                    future = tempBoard->findSquare(futureX, futureY);
                    if(future && !future->getOccupied())
                        return true; // can jump
                }
                else if((checker->getPosition().x + XOFFSET) == (*it)->getPosition().x
                        && (checker->getPosition().y + YOFFSET) == (*it)->getPosition().y) // now check 315 degrees ( down and to the right )
                {
                    // enemy checker is within jumpeable range, so check if there is a friendly or enemy withing landing range
                    futureX = (int) checker->getPosition().x + (2*XOFFSET);
                    futureY = (int) checker->getPosition().y + (2*YOFFSET);
                    future = tempBoard->findSquare(futureX, futureY);
                    if(future && !future->getOccupied())
                        return true; // can jump
                }
            }
            else // the current checker is moving northward up the board
            {
                if((*it)->getPosition().y != 0 && (checker->getPosition().x + XOFFSET) == (*it)->getPosition().x
                   && abs(checker->getPosition().y - YOFFSET) == (*it)->getPosition().y) // first check 45 degrees ( up and to the right )
                {
                    // enemy checker is within jumpeable range, so check if there is a friendly or enemy withing landing range
                    futureX = (int) checker->getPosition().x + (2*XOFFSET);
                    futureY = (int) abs(checker->getPosition().y - (2*YOFFSET));
                    // now see if the landing square is occupied
                    future = tempBoard->findSquare(futureX, futureY);
                    if(future && !future->getOccupied())
                        return true; // can jump
                }
                else if((*it)->getPosition().y != 0 && (*it)->getPosition().x != 0 && abs(checker->getPosition().x - XOFFSET) == (*it)->getPosition().x
                        && std::abs(checker->getPosition().y - YOFFSET) == (*it)->getPosition().y) // now check 135 degrees ( up and to the left )
                {
                    // enemy checker is within jumpeable range, so check if there is a friendly or enemy withing landing range
                    futureX = (int) abs(checker->getPosition().x - (2*XOFFSET));
                    futureY = (int) abs(checker->getPosition().y - (2*YOFFSET));
                    future = tempBoard->findSquare(futureX, futureY);
                    if(future && !future->getOccupied())
                        return true; // can jump
                }
            }
        }
        else // test all 4 possible jumps because we're dealing with a king checker
        {
            if((*it)->getPosition().x != 0 && checker->getPosition().x != 0 && std::abs(checker->getPosition().x - XOFFSET) == (*it)->getPosition().x
               && (checker->getPosition().y  + YOFFSET) == (*it)->getPosition().y) // first check 225 degrees ( down and to the left )
            {
                // enemy checker is within jumpeable range, so check if there is a friendly or enemy withing landing range
                futureX = (int) std::abs(checker->getPosition().x - (2*XOFFSET));
                futureY = (int) checker->getPosition().y + (2*YOFFSET);
                // now see if the landing square is occupied
                future = tempBoard->findSquare(futureX, futureY);
                if(future && !future->getOccupied())
                    return true; // can jump
            }
            else if((checker->getPosition().x + XOFFSET) == (*it)->getPosition().x
                    && (checker->getPosition().y + YOFFSET) == (*it)->getPosition().y) // now check 315 degrees ( down and to the right )
            {
                // enemy checker is within jumpeable range, so check if there is a friendly or enemy withing landing range
                futureX = (int) checker->getPosition().x + (2*XOFFSET);
                futureY = (int) checker->getPosition().y + (2*YOFFSET);
                future = tempBoard->findSquare(futureX, futureY);
                if(future && !future->getOccupied())
                    return true; // can jump
            }
            else if((*it)->getPosition().y != 0 && (checker->getPosition().x + XOFFSET) == (*it)->getPosition().x
                    && std::abs(checker->getPosition().y - YOFFSET) == (*it)->getPosition().y) // first check 45 degrees ( up and to the right )
            {
                // enemy checker is within jumpeable range, so check if there is a friendly or enemy withing landing range
                futureX = (int) checker->getPosition().x + (2*XOFFSET);
                futureY = (int) std::abs(checker->getPosition().y - (2*YOFFSET));
                // now see if the landing square is occupied
                future = tempBoard->findSquare(futureX, futureY);
                if(future && !future->getOccupied())
                    return true; // can jump
            }
            else if((*it)->getPosition().y != 0 && (*it)->getPosition().x != 0 && checker->getPosition().x != 0
                    && std::abs(checker->getPosition().x - XOFFSET) == (*it)->getPosition().x
                    && std::abs(checker->getPosition().y - YOFFSET) == (*it)->getPosition().y) // now check 135 degrees ( up and to the left )
            {
                // enemy checker is within jumpeable range, so check if there is a friendly or enemy withing landing range
                futureX = (int) abs(checker->getPosition().x - (2*XOFFSET));
                futureY = (int) abs(checker->getPosition().y - (2*YOFFSET));
                future = tempBoard->findSquare(futureX, futureY);
                if(future && !future->getOccupied())
                    return true; // can jump
            }
        }
    }
    return false;
}

/*
*	Similar to hasJump, we look to see if the active checker can jump,
*	but instead of returning a boolean, we return the coords of
*	the current checker, the enemy checker that we're 
*	jumping over, and where we'll land (futureX/Y), 
*	(in that order, so coords[0],coords[1] = currentX, currentY).
*/
vector<int>* Moveable::findJump(Checkerpiece* checker, const vector<Checkerpiece*>& enemy, Checkerboard*& tempBoard)
{
    // the coords where the active checker would land after jumping, and the coords of the checker to jump over
    const int TOTAL_POSITIONS = 6;
    vector<int>* coords = new vector<int> (TOTAL_POSITIONS);
    coords->reserve(TOTAL_POSITIONS);
    Square* future;
    int futureX, futureY;
    for(unsigned int i = 0; i != enemy.size(); ++i)
    {
        if(!checker->getKing()) // test only 2 possible jumps depending on KING_ROW
        {
            if(checker->getKingRow() == KING_ROW_7) // the current check is moving southward
            {
                if(enemy[i]->getPosition().x != 0 && std::abs(checker->getPosition().x - XOFFSET) == enemy[i]->getPosition().x
                   && (checker->getPosition().y  + YOFFSET) == enemy[i]->getPosition().y) // first check 225 degrees ( down and to the left )
                {
                    // enemy checker is within jumpeable range, so check if there is a friendly or enemy withing landing range
                    futureX = (int) std::abs(checker->getPosition().x - (2*XOFFSET));
                    futureY = (int) checker->getPosition().y + (2*YOFFSET);
                    // now see if the landing square is occupied
                    future = tempBoard->findSquare(futureX, futureY);
                    if(future && !future->getOccupied())
                    {
                        coords->at(0) = (int) checker->getPosition().x;
                        coords->at(1) = (int) checker->getPosition().y;
                        coords->at(2) = (int) enemy[i]->getPosition().x;
                        coords->at(3) = (int) enemy[i]->getPosition().y;
                        coords->at(4) = futureX;
                        coords->at(5) = futureY;
                        return coords; // return coords of jump
                    }
                }
                else if((checker->getPosition().x + XOFFSET) == enemy[i]->getPosition().x
                        && (checker->getPosition().y + YOFFSET) == enemy[i]->getPosition().y) // now check 315 degrees ( down and to the right )
                {
                    // enemy checker is within jumpeable range, so check if there is a friendly or enemy withing landing range
                    futureX = (int) checker->getPosition().x + (2*XOFFSET);
                    futureY = (int) checker->getPosition().y + (2*YOFFSET);
                    future = tempBoard->findSquare(futureX, futureY);
                    if(future && !future->getOccupied())
                    {
                        coords->at(0) = (int) checker->getPosition().x;
                        coords->at(1) = (int) checker->getPosition().y;
                        coords->at(2) = (int) enemy[i]->getPosition().x;
                        coords->at(3) = (int) enemy[i]->getPosition().y;
                        coords->at(4) = futureX;
                        coords->at(5) = futureY;
                        return coords; // return coords of jump
                    }
                }
            }
            else // the current checker is moving northward up the board
            {
                if(enemy[i]->getPosition().y != 0 && (checker->getPosition().x + XOFFSET) == enemy[i]->getPosition().x
                   && std::abs(checker->getPosition().y - YOFFSET) == enemy[i]->getPosition().y) // first check 45 degrees ( up and to the right )
                {
                    // enemy checker is within jumpeable range, so check if there is a friendly or enemy withing landing range
                    futureX = (int) checker->getPosition().x + (2*XOFFSET);
                    futureY = (int) std::abs(checker->getPosition().y - (2*YOFFSET));
                    // now see if the landing square is occupied
                    future = tempBoard->findSquare(futureX, futureY);
                    if(future && !future->getOccupied())
                    {
                        coords->at(0) = (int) checker->getPosition().x;
                        coords->at(1) = (int) checker->getPosition().y;
                        coords->at(2) = (int) enemy[i]->getPosition().x;
                        coords->at(3) = (int) enemy[i]->getPosition().y;
                        coords->at(4) = futureX;
                        coords->at(5) = futureY;
                        return coords; // return coords of jump
                    }
                }
                else if(enemy[i]->getPosition().y != 0 && enemy[i]->getPosition().x != 0 && std::abs(checker->getPosition().x - XOFFSET) == enemy[i]->getPosition().x
                        && std::abs(checker->getPosition().y - YOFFSET) == enemy[i]->getPosition().y) // now check 135 degrees ( up and to the left )
                {
                    // enemy checker is within jumpeable range, so check if there is a friendly or enemy withing landing range
                    futureX = (int) std::abs(checker->getPosition().x - (2*XOFFSET));
                    futureY = (int) std::abs(checker->getPosition().y - (2*YOFFSET));
                    future = tempBoard->findSquare(futureX, futureY);
                    if(future && !future->getOccupied())
                    {
                        coords->at(0) = (int) checker->getPosition().x;
                        coords->at(1) = (int) checker->getPosition().y;
                        coords->at(2) = (int) enemy[i]->getPosition().x;
                        coords->at(3) = (int) enemy[i]->getPosition().y;
                        coords->at(4) = futureX;
                        coords->at(5) = futureY;
                        return coords; // return coords of jump
                    }
                }
            }
        }
        else // test all 4 possible jump directions because we're dealing with a king checker
        {
            if(enemy[i]->getPosition().x != 0 && checker->getPosition().x != 0 && std::abs(checker->getPosition().x - XOFFSET) == enemy[i]->getPosition().x
               && (checker->getPosition().y  + YOFFSET) == enemy[i]->getPosition().y) // first check 225 degrees ( down and to the left )
            {
                // enemy checker is within jumpeable range, so check if there is a friendly or enemy withing landing range
                futureX = (int) std::abs(checker->getPosition().x - (2*XOFFSET));
                futureY = (int) checker->getPosition().y + (2*YOFFSET);
                // now see if the landing square is occupied
                future = tempBoard->findSquare(futureX, futureY);
                if(future && !future->getOccupied())
                {
                    coords->at(0) = (int) checker->getPosition().x;
                    coords->at(1) = (int) checker->getPosition().y;
                    coords->at(2) = (int) enemy[i]->getPosition().x;
                    coords->at(3) = (int) enemy[i]->getPosition().y;
                    coords->at(4) = futureX;
                    coords->at(5) = futureY;
                    return coords; // return coords of jump
                }
            }
            else if((checker->getPosition().x + XOFFSET) == enemy[i]->getPosition().x
                    && (checker->getPosition().y + YOFFSET) == enemy[i]->getPosition().y) // now check 315 degrees ( down and to the right )
            {
                // enemy checker is within jumpeable range, so check if there is a friendly or enemy withing landing range
                futureX = (int) checker->getPosition().x + (2*XOFFSET);
                futureY = (int) checker->getPosition().y + (2*YOFFSET);
                future = tempBoard->findSquare(futureX, futureY);
                if(future && !future->getOccupied())
                {
                    coords->at(0) = (int) checker->getPosition().x;
                    coords->at(1) = (int) checker->getPosition().y;
                    coords->at(2) = (int) enemy[i]->getPosition().x;
                    coords->at(3) = (int) enemy[i]->getPosition().y;
                    coords->at(4) = futureX;
                    coords->at(5) = futureY;
                    return coords; // return coords of jump
                }
            }
            else if(enemy[i]->getPosition().y != 0 && (checker->getPosition().x + XOFFSET) == enemy[i]->getPosition().x
                    && std::abs(checker->getPosition().y - YOFFSET) == enemy[i]->getPosition().y) // first check 45 degrees ( up and to the right )
            {
                // enemy checker is within jumpeable range, so check if there is a friendly or enemy withing landing range
                futureX = (int) checker->getPosition().x + (2*XOFFSET);
                futureY = (int) std::abs(checker->getPosition().y - (2*YOFFSET));
                // now see if the landing square is occupied
                future = tempBoard->findSquare(futureX, futureY);
                if(future && !future->getOccupied())
                {
                    coords->at(0) = (int) checker->getPosition().x;
                    coords->at(1) = (int) checker->getPosition().y;
                    coords->at(2) = (int) enemy[i]->getPosition().x;
                    coords->at(3) = (int) enemy[i]->getPosition().y;
                    coords->at(4) = futureX;
                    coords->at(5) = futureY;
                    return coords; // return coords of jump
                }
            }
            else if(enemy[i]->getPosition().y != 0 && enemy[i]->getPosition().x != 0 && checker->getPosition().x != 0
                    && std::abs(checker->getPosition().x - XOFFSET) == enemy[i]->getPosition().x
                    && std::abs(checker->getPosition().y - YOFFSET) == enemy[i]->getPosition().y) // now check 135 degrees ( up and to the left )
            {
                // enemy checker is within jumpeable range, so check if there is a friendly or enemy withing landing range
                futureX = (int) std::abs(checker->getPosition().x - (2*XOFFSET));
                futureY = (int) std::abs(checker->getPosition().y - (2*YOFFSET));
                future = tempBoard->findSquare(futureX, futureY);
                if(future && !future->getOccupied())
                {
                    coords->at(0) = (int) checker->getPosition().x;
                    coords->at(1) = (int) checker->getPosition().y;
                    coords->at(2) = (int) enemy[i]->getPosition().x;
                    coords->at(3) = (int) enemy[i]->getPosition().y;
                    coords->at(4) = futureX;
                    coords->at(5) = futureY;
                    return coords; // return coords of jump
                }
            }
        }
    }

    return nullptr; // return empty coords, size of zero
}

/*
*	Similar functionality to hasJump, but only looks for a move by using the moveable function.
*	It is important to note that the moveable function does not verify if the future square is occupied,
*	so we must verify it for ourselves here.
*/
bool Moveable::hasMove(Checkerpiece* active, const vector<Checkerpiece*>& enemy, Checkerboard*& checkerboard)
{
    const int TOTAL_POSITIONS = 4;
    std::vector<int> coords;
    coords.reserve(TOTAL_POSITIONS);
    int currentX = (int) active->getPosition().x;
    int currentY = (int) active->getPosition().y;
    int futureX, futureY;
    Square* current, *future; // current and future squares to move onto
    // iterate through the different cases
    futureX = currentX + XOFFSET;
    futureY = currentY + YOFFSET;
    current = checkerboard->findSquare(currentX, currentY);
    future = checkerboard->findSquare(futureX, futureY);
    if(current != nullptr && future != nullptr && moveable(active, current, future))
        if(!future->getOccupied())
            return true;

    futureX = currentX - XOFFSET;
    futureY = currentY - YOFFSET;
    current = checkerboard->findSquare(currentX, currentY);
    future = checkerboard->findSquare(futureX, futureY);
    if(current != nullptr && future != nullptr && moveable(active, current, future))
        if(!future->getOccupied())
            return true;

    futureX = currentX - XOFFSET;
    futureY = currentY + YOFFSET;
    current = checkerboard->findSquare(currentX, currentY);
    future = checkerboard->findSquare(futureX, futureY);
    if(current != nullptr && future != nullptr && moveable(active, current, future))
        if(!future->getOccupied())
            return true;

    futureX = currentX + XOFFSET;
    futureY = currentY - YOFFSET;
    current = checkerboard->findSquare(currentX, currentY);
    future = checkerboard->findSquare(futureX, futureY);
    if(current != nullptr && future != nullptr && moveable(active, current, future))
        if(!future->getOccupied())
            return true;

    return false; // no move available
}

/*
*	Find a simple move (no jump), based on the first unoccupied
*	square that is within moveable distance to the active checkerpiece.
*	We simply try different combinations of adding, subtracting
*	the X/Y Offsets until we find one that's moveable.
*	the coords return corresponds to (coords[0,1] = currentX,Y, and coords[2,3] = futureX,Y).
*/
vector<int>* Moveable::findMove(Checkerpiece* active, Checkerboard*& checkerboard)
{
    const int TOTAL_POSITIONS = 4;
    vector<int>* coords = new vector<int> (TOTAL_POSITIONS);
    coords->reserve(TOTAL_POSITIONS);
    int currentX = static_cast<int>(active->getPosition().x);
    int currentY = static_cast<int>(active->getPosition().y);
    int futureX, futureY;
    Square* current, *future; // current and future squares to move from and onto

    // iterate through the different cases of future X,Y manipulations
    futureX = currentX + XOFFSET;
    futureY = currentY + YOFFSET;
    current = checkerboard->findSquare(currentX, currentY);
    future = checkerboard->findSquare(futureX, futureY);
    if(current != nullptr && future != nullptr && moveable(active, current, future)
       && !future->getOccupied())
    {
        coords->at(0) = currentX;
        coords->at(1) = currentY;
        coords->at(2) = futureX;
        coords->at(3) = futureY;
        return coords;
    }
    futureX = currentX - XOFFSET;
    futureY = currentY - YOFFSET;
    current = checkerboard->findSquare(currentX, currentY);
    future = checkerboard->findSquare(futureX, futureY);
    if(current != nullptr && future != nullptr && moveable(active, current, future)
       && !future->getOccupied())
    {
        coords->at(0) = currentX;
        coords->at(1) = currentY;
        coords->at(2) = futureX;
        coords->at(3) = futureY;
        return coords;
    }
    futureX = currentX - XOFFSET;
    futureY = currentY + YOFFSET;
    current = checkerboard->findSquare(currentX, currentY);
    future = checkerboard->findSquare(futureX, futureY);
    if(current != nullptr && future != nullptr && moveable(active, current, future)
       && !future->getOccupied())
    {
        coords->at(0) = currentX;
        coords->at(1) = currentY;
        coords->at(2) = futureX;
        coords->at(3) = futureY;
        return coords;
    }
    futureX = currentX + XOFFSET;
    futureY = currentY - YOFFSET;
    current = checkerboard->findSquare(currentX, currentY);
    future = checkerboard->findSquare(futureX, futureY);
    if(current != nullptr && future != nullptr && moveable(active, current, future)
       && !future->getOccupied())
    {
        coords->at(0) = currentX;
        coords->at(1) = currentY;
        coords->at(2) = futureX;
        coords->at(3) = futureY;
        return coords;
    }

    return nullptr; // return empty coords of size 0

}