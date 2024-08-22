#include "ChessSDL.h"
#include <SDL.h> // for linking error

int main(int argc, char* args[]) 
{
    if (ChessSDL_MakePreparations()) {
        return 1;
    }

    ChessSDL_HandleGameLoop();

    ChessSDL_Close();
    return 0;
}
