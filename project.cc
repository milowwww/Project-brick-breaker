#include <iostream>
#include <string>
#include "game.h"

int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cerr << "Usage: ./project test.txt" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    Game game;
    // Cette fonction doit être définie dans game.cc / déclarée dans game.h
    // Elle lit le fichier, construit l'état du jeu, affiche le message approprié,
    // puis renvoie true si la lecture est valide, false sinon.
    bool ok = load_game(filename, game);

    if (ok) {
        return 0;
    }

    return 1;
}