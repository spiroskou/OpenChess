#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include "ChessSDL.h"
#include <SDL.h> // for linking error
#include "Board.h"

int main(int argc, char* args[])
{
    if (ChessSDL_MakePreparations()) {
        return 1;
    }

#ifdef __EMSCRIPTEN__
    // Use emscripten_set_main_loop to call GameLoopIteration repeatedly
    emscripten_set_main_loop(ChessSDL_GameLoopIteration, 0, 1);
#else
    // Call GameLoopIteration in a loop for non-web environments
    while (!ChessSDL_NeedToQuit()) {
        ChessSDL_GameLoopIteration();
    }
#endif

    ChessSDL_Close();
    return 0;
}
