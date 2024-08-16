#pragma once

#include <string>
#include "Board.h"

const int ROWS = 8;
const int COLS = 8;
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 640;
const int TILE_SIZE = SCREEN_WIDTH / COLS;

int ChessSDL_MakePreparations();
void ChessSDL_Close();
void ChessSDL_RenderChessBoard(bool lastMove);
void ChessSDL_ShowMoveMessage(MoveResult res);
void ChessSDL_HighlightLastMove();
