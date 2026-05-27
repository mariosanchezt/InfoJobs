#include "AudioManager.h"
#include <iostream>

AudioManager::AudioManager() 
	: canalActualGolpe(0)
	, sonidoMuerte(nullptr)
	, sonidoDisparo(nullptr)
{
	volumenMusica = 25.f;
	volumenEfectos = 50.f;
}

void AudioManager::cargar() {
	if (!musicaMenu.openFromFile("assets/musica_menu.wav"))
		std::cout << "Error: no se pudo cargar musica_menu.wav" << std::endl;
	musicaMenu.setLooping(true);
	musicaMenu.setVolume(volumenMusica);

	if (!musicaJuego.openFromFile("assets/musica_juego.wav"))
		std::cout << "Error: no se pudo cargar musica_juego.wav" << std::endl;
	musicaJuego.setLooping(true);
	musicaJuego.setVolume(volumenMusica);

	if (!bufferGolpe.loadFromFile("assets/sonido_golpe.wav"))
		std::cout << "Error: no se pudo cargar sonido_golpe.wav" << std::endl;

	for (int i = 0; i < CANALES_GOLPE; i++)
		sonidosGolpe.emplace_back(bufferGolpe);

	if (!bufferMuerte.loadFromFile("assets/sonido_muerte.wav"))
		std::cout << "Error: no se pudo cargar sonido_muerte.wav" << std::endl;

	if (!bufferDisparo.loadFromFile("assets/sonido_disparo.wav"))
		std::cout << "Error: no se ha podido cargar sonido_disparo.wav" << std::endl;
	
	sonidoMuerte = new sf::Sound(bufferMuerte);
	sonidoDisparo = new sf::Sound(bufferDisparo);

	for (int i = 0; i < CANALES_GOLPE; i++)
		sonidosGolpe[i].setVolume(volumenEfectos);
	if (sonidoMuerte) sonidoMuerte->setVolume(volumenEfectos);
	if (sonidoDisparo) sonidoDisparo->setVolume(volumenEfectos);

	//// Precargar sonidos para eliminar lag
	//for (int i = 0; i < CANALES_GOLPE; i++){
	//	sonidosGolpe[i].play();
	//	sonidosGolpe[i].stop();
	//}
	//sonidoMuerte.play();
	//sonidoMuerte.stop();
	//sonidoDisparo.play();
	//sonidoDisparo.stop();
}

void AudioManager::playMenuMusic() {
	musicaJuego.stop();
	musicaMenu.play();
}

void AudioManager::playGameMusic() {
	musicaMenu.stop();
	musicaJuego.play();
}

void AudioManager::stopMusic() {
	musicaMenu.stop();
	musicaJuego.stop();
}

void AudioManager::playGolpe() {
	sonidosGolpe[canalActualGolpe].play();
	canalActualGolpe = (canalActualGolpe + 1) % CANALES_GOLPE;
}

void AudioManager::playMuerte() {
	if (sonidoMuerte)sonidoMuerte->play();
}

void AudioManager::playDisparo() {
	if (sonidoDisparo)sonidoDisparo->play();
}

void AudioManager::setVolumenMusica(float vol) {
	volumenMusica = vol;
	musicaMenu.setVolume(vol);
	musicaJuego.setVolume(vol);
}

void AudioManager::setVolumenEfectos(float vol) {
	volumenEfectos = vol;
	for (int i = 0; i < CANALES_GOLPE; i++)
		sonidosGolpe[i].setVolume(vol);
	if (sonidoMuerte) sonidoMuerte->setVolume(vol);
	if (sonidoDisparo) sonidoDisparo->setVolume(vol);
}

AudioManager::~AudioManager() {
	delete sonidoMuerte;
	delete sonidoDisparo;
}