#include "SoundManager.hpp"

using std::string;
using std::cout;
using std::endl;

SoundManager::SoundManager()
{
}

SoundManager::~SoundManager()
{
    cout << "Liberando espacio... ";
    for(unsigned int i = 0; i < soundBufferArray.size(); ++i)
    {
        soundArray[i]->stop();
        delete(soundArray[i]);
        delete(soundBufferArray[i]);
    }
    soundArray.clear();
    soundBufferArray.clear();
    cout << "OK" << endl;
}
//Permite dar acceso global a la instancia de SoundManaager
 SoundManager* SoundManager::getSoundManager()
 {
   static SoundManager singleton;
   return &singleton;
 }
 //Permite añadir el sonido para el juego
void SoundManager::addSound(string fileName)
{
    cout << "Cargando sonido: " << fileName << "...\n";
    sf::SoundBuffer* newSoundBuffer = new sf::SoundBuffer;
    newSoundBuffer->loadFromFile(fileName);
    soundBufferArray.push_back(newSoundBuffer);

    sf::Sound* newSound = new sf::Sound;
    newSound->setBuffer(*newSoundBuffer);
    soundArray.push_back(newSound);
}

//Permite reproducir el sonido
void SoundManager::playSound(int n)
{
    if (soundArray[n]->getStatus() != sf::Sound::Playing)
        soundArray[n]->play();
}

//Permite detener el sonido que se está reproduciendo
void SoundManager::stopSound(int n)
{
    if (soundArray[n]->getStatus() == sf::Sound::Playing)
        soundArray[n]->stop();
}
