#include "AudioManager.h"
#include <iostream>

AudioManager::AudioManager() 
	: canalActualGolpe(0)
	, sonidoMuerte(nullptr)
	, sonidoDisparo(nullptr)
	, sonidoVictoria(nullptr)
	, sonidoDerrota(nullptr)
	, sonidoMovimiento(nullptr)
	, sonidoSeleccion(nullptr)
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
	
	if (!musicaArena.openFromFile("assets/musica_arena.wav"))
		std::cout << "Error: no se pudo cargar musica_arena.wav" << std::endl;
	musicaArena.setLooping(true);
	musicaArena.setVolume(volumenMusica);

	if (!bufferVictoria.loadFromFile("assets/sonido_victoria.wav"))
		std::cout << "Error: no se pudo cargar sonido_victoria.wav" << std::endl;
	
	if (!bufferDerrota.loadFromFile("assets/sonido_derrota.wav"))
		std::cout << "Error: no se pudo cargar sonido_derrota.wav" << std::endl;
	
	if (!musicaPausa.openFromFile("assets/sonido_pausa.wav"))
		std::cout << "Error: no se pudo cargar sonido_pausa.wav" << std::endl;
	musicaPausa.setLooping(true);
	musicaPausa.setVolume(volumenMusica);
	
	if (!bufferMovimiento.loadFromFile("assets/sonido_movimiento.wav"))
		std::cout << "Error: no se pudo cargar sonido_movimiento.wav" << std::endl;
	
	if (!bufferSeleccion.loadFromFile("assets/sonido_seleccion.wav"))
		std::cout << "Error: no se pudo cargar sonido_seleccion.wav" << std::endl;

	sonidoMuerte = new sf::Sound(bufferMuerte);
	sonidoDisparo = new sf::Sound(bufferDisparo);
	sonidoVictoria = new sf::Sound(bufferVictoria);
	sonidoDerrota = new sf::Sound(bufferDerrota);
	sonidoMovimiento = new sf::Sound(bufferMovimiento);
	sonidoSeleccion = new sf::Sound(bufferSeleccion);

	for (int i = 0; i < CANALES_GOLPE; i++)
		sonidosGolpe[i].setVolume(volumenEfectos);
	if (sonidoMuerte) sonidoMuerte->setVolume(volumenEfectos);
	if (sonidoDisparo) sonidoDisparo->setVolume(volumenEfectos);

	sonidoMovimiento->setLooping(true);

	sonidoVictoria->setVolume(volumenEfectos);
	sonidoDerrota->setVolume(volumenEfectos);
	sonidoMovimiento->setVolume(volumenEfectos);
	sonidoSeleccion->setVolume(volumenEfectos);
}

void AudioManager::playMenuMusic() {
	musicaJuego.stop();
	musicaArena.stop();
	musicaPausa.stop();
	musicaMenu.play();
}

void AudioManager::playGameMusic() {
	musicaMenu.stop();
	musicaArena.stop();
	musicaPausa.stop();
	musicaJuego.play();
}

void AudioManager::stopMusic() {
	musicaMenu.stop();
	musicaJuego.stop();
}

void AudioManager::playGolpe() {
	sonidosGolpe[canalActualGolpe].setPlayingOffset(sf::Time::Zero);
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
	if (sonidoVictoria) sonidoVictoria->setVolume(vol);
	if (sonidoDerrota) sonidoDerrota->setVolume(vol);
	if (sonidoMovimiento) sonidoMovimiento->setVolume(vol);
	if (sonidoSeleccion) sonidoSeleccion->setVolume(vol);
}

void AudioManager::playArenaMusic() {
	musicaJuego.stop();
	musicaArena.play();
}

void AudioManager::playVictoria() {
	if (sonidoVictoria) sonidoVictoria->play();
}

void AudioManager::playDerrota() {
	if (sonidoDerrota) sonidoDerrota->play();
}

void AudioManager::playPausa() {
	musicaJuego.stop();
	musicaArena.stop();
	musicaPausa.play();
}

void AudioManager::playMovimiento() {
	if (sonidoMovimiento) sonidoMovimiento->play();
}

void AudioManager::stopMovimiento() {
	if (sonidoMovimiento) sonidoMovimiento->stop();
}

void AudioManager::playSeleccion() {
	if (sonidoSeleccion) sonidoSeleccion->play();
}

AudioManager::~AudioManager() {
	delete sonidoMuerte;
	delete sonidoDisparo;
	delete sonidoVictoria;
	delete sonidoDerrota;
	delete sonidoMovimiento;
	delete sonidoSeleccion;
}