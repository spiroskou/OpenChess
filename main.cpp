#include <SDL.h>
#include <iostream>
#include <string>
#include "Board.h"
#include "ChessSDL.h"

constexpr auto AI_OPPONENT = 1;
constexpr auto depth = 3;

int main(int argc, char* args[]) 
{
    if (ChessSDL_MakePreparations()) {
        return 1;
    }

    bool quit = false;
    SDL_Event e;
    int print = 0;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {

            if (!print) {
                std::cout << (getTurnCounter() % 2 != 0 ? "Player1 (White) " : "Player2 (Black) ") << "turn" << std::endl;
                print = 1;
            }

            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                int row = y / TILE_SIZE;
                int col = x / TILE_SIZE;

                // Simulate reading input from mouse clicks
                static int click_counter = 0;
                static int first_row, first_col;

                if (click_counter == 0) {
                    first_row = row;
                    first_col = col;
                    click_counter++;
                } else {
                    int src_row = first_row;
                    int src_col = first_col;
                    int dest_row = row;
                    int dest_col = col;
                    click_counter = 0;
                    print = 0;

                    MoveResult res = makeTheMove(src_row, src_col, dest_row, dest_col);

                    if (res == MoveResult::Checkmate || res == MoveResult::Stalemate) {
                        ChessSDL_RenderChessBoard();
                        ChessSDL_ShowMoveMessage(res);
                        quit = true;
                        break;
                    }

                    if (res == MoveResult::ValidMove) {
                        IncrementTurnCounter();
                    }
                    ChessSDL_ShowMoveMessage(res);
                }
            }
        }

        if (AI_OPPONENT) {
            // AI's turn (Player 2)
            if (getTurnCounter() % 2 == 0 && !quit) {  // Assuming AI plays as Black
                Move aiMove = findBestMove(depth);
                MoveResult aiRes = makeTheMove(aiMove.src_row, aiMove.src_col, aiMove.dest_row, aiMove.dest_col);
 
                if (aiRes == MoveResult::Checkmate || aiRes == MoveResult::Stalemate) {
                    ChessSDL_RenderChessBoard();
                    ChessSDL_ShowMoveMessage(aiRes);
                    quit = true;
                }
 
 		       if (aiRes == MoveResult::ValidMove) {
 		            IncrementTurnCounter();
 		       }
                ChessSDL_ShowMoveMessage(aiRes);
            }
        }
        ChessSDL_RenderChessBoard();
    }

    ChessSDL_Close();
    return 0;
}
