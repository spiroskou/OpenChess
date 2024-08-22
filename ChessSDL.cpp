#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <memory>

#include "ChessSDL.h"
#include "Board.h"
#include "Config.h"

static std::map<std::string, SDL_Texture*> textures;
static SDL_Renderer* renderer;
static SDL_Window* window;

static SDL_Texture* getTexture(std::string imagePath)
{
    return textures[imagePath];
}

static SDL_Renderer* getRenderer()
{
    return renderer;
}

static SDL_Window* getWindow()
{
    return window;
}

static int init_SDL()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    return 0;
}

static int init_SDL_Image()
{
    if (!IMG_Init(IMG_INIT_PNG)) {
        std::cerr << "SDL_image could not initialize! SDL_image Error: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }
    return 0;
}

static SDL_Window* create_SDL_Window(std::string name)
{
    SDL_Window* window = SDL_CreateWindow(name.c_str(),
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        IMG_Quit();
        SDL_Quit();
        return nullptr;
    }
    return window;
}

static SDL_Renderer* create_SDL_Renderer(SDL_Window *window)
{
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return nullptr;
    }
    return renderer;
}

static bool loadMedia(SDL_Renderer* renderer) {
    // Load images for all piece types
    std::vector<std::string> Files = {
        "images/white-pawn.png",
        "images/black-pawn.png",
        "images/white-bishop.png",
        "images/black-bishop.png",
        "images/white-rook.png",
        "images/black-rook.png",
        "images/white-king.png",
        "images/black-king.png",
        "images/white-knight.png",
        "images/black-knight.png",
        "images/white-queen.png",
        "images/black-queen.png"
    };
    for (const auto& file : Files) {
        SDL_Surface* loadedSurface = IMG_Load(file.c_str());
        if (!loadedSurface) {
            std::cerr << "Unable to load image " << file << "! SDL_image Error: " << IMG_GetError() << std::endl;
            return false;
        }
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
        if (!texture) {
            std::cerr << "Unable to create texture from " << file << "! SDL Error: " << SDL_GetError() << std::endl;
            SDL_FreeSurface(loadedSurface);
            return false;
        }
        textures[file] = texture;
        SDL_FreeSurface(loadedSurface);
    }
    return true;
}

void ChessSDL_Close() 
{
    for (auto& pair : textures) {
        SDL_DestroyTexture(pair.second);
    }
    textures.clear();
    IMG_Quit();
    SDL_Quit();
}

static void ChessSDL_RenderPiece(int row, int col)
{
	std::shared_ptr<Board> board = getBoard();
	std::shared_ptr<Piece> piece = board->getPiece(row, col);

    if (piece) {
		std::string imagePath;

		imagePath = piece->getImagePath();
		SDL_Texture* texture = getTexture(imagePath);
		if (texture) {
			SDL_Rect pieceRect = { col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE };
			SDL_RenderCopy(renderer, texture, nullptr, &pieceRect);
		}
    }
}

static void ChessSDL_HighlightSelectedTile(SDL_Renderer *renderer, int selectedRow, int selectedCol, bool revert)
{
    if (!revert) {
		SDL_SetRenderDrawColor(renderer, 255, 255, 130, 255);
    } else {
        bool isWhiteTile = (selectedRow + selectedCol) % 2 == 0;

        if (isWhiteTile) {
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White
        } else {
            SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255); // Gray
        }
    }
	SDL_Rect tile = { selectedCol * TILE_SIZE, selectedRow * TILE_SIZE, TILE_SIZE, TILE_SIZE };
	SDL_RenderFillRect(renderer, &tile);
    ChessSDL_RenderPiece(selectedRow, selectedCol);
}

static void ChessSDL_HighlightSelection(int selectedRow, int selectedCol, bool revert)
{
    SDL_Renderer* renderer = getRenderer();

    ChessSDL_HighlightSelectedTile(renderer, selectedRow, selectedCol, revert);
	SDL_RenderPresent(renderer);
}

static void ChessSDL_RenderChessBoard()
{
    SDL_Renderer* renderer = getRenderer();

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

    bool white = true;
    for (int row = 0; row < ROWS; ++row) {
        for (int col = 0; col < COLS; ++col) {

            if (white) {
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White
            } else {
                SDL_SetRenderDrawColor(renderer, 128, 128, 128, 255); // Gray
            }

            SDL_Rect tile = { col * TILE_SIZE, row * TILE_SIZE, TILE_SIZE, TILE_SIZE };
            SDL_RenderFillRect(renderer, &tile);
            white = !white;

            // Render the piece on the board
            ChessSDL_RenderPiece(row, col);
        }
        white = !white;
    }

	SDL_RenderPresent(renderer);
}

static void ChessSDL_HighlightLastMove()
{
    ChessSDL_RenderChessBoard();

    SDL_Renderer* renderer = getRenderer();
    std::shared_ptr<Board> board = getBoard();
    Move move = board->getLastMove();

    if (move.src_col == -1) return;

    ChessSDL_HighlightSelectedTile(renderer, move.src_row, move.src_col, false);
    ChessSDL_HighlightSelectedTile(renderer, move.dest_row, move.dest_col, false);
	SDL_RenderPresent(renderer);
}

int ChessSDL_MakePreparations()
{
    if (init_SDL()) {
        return 1;
    }

    if (init_SDL_Image()) {
        return 1;
    }

    window = create_SDL_Window("ChessGame");
    if (!window) {
        return 1;
    }

    renderer = create_SDL_Renderer(window);
    if (!renderer) {
        return 1;
    }

    if (!loadMedia(renderer)) {
        std::cerr << "Failed to load media!" << std::endl;
        ChessSDL_Close();
        return 1;
    }

    ChessSDL_RenderChessBoard();

    return 0;
}

static void showCheckmateMessage()
{
	std::string winner = (getTurnCounter() % 2 != 0) ? "Player1 (White)" : "Player2 (Black)";
	std::string message = "Checkmate! " + winner + " has won the game!";
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Game Over", message.c_str(), window);
}

static void showStalemateMessage()
{
	std::string message = "Stalemate! Game has ended in draw!";
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Game Over", message.c_str(), window);
}

static void showOpponentPieceMessage()
{
	std::string message = "You can't move an opponent's piece!";
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Warning", message.c_str(), window);
}

static void showInvalidMoveMessage()
{
	std::string message = "Invalid move!";
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Warning", message.c_str(), window);
}

static void showKingInCheckMessage()
{
	std::string message = "King is in check! Choose a valid move!";
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_INFORMATION, "Warning", message.c_str(), window);
}

void ChessSDL_ShowMoveMessage(MoveResult res)
{
	switch (res)
	{
	case MoveResult::InvalidPiece:
		break;
	case MoveResult::OpponentPiece:
		showOpponentPieceMessage();
		break;
	case MoveResult::InvalidMove:
		showInvalidMoveMessage();
		break;
	case MoveResult::KingInCheck:
		showKingInCheckMessage();
		break;
	case MoveResult::Checkmate:
		showCheckmateMessage();
		break;
	case MoveResult::Stalemate:
		showStalemateMessage();
		break;
	case MoveResult::ValidMove:
		break;
	default:
		break;
	}
}

static void ChessSDL_HandleMoveResult(const MoveResult& res, bool &quit, int row = -1, int col = -1)
{
	if (res == MoveResult::Checkmate || res == MoveResult::Stalemate) {
		ChessSDL_HighlightLastMove();
		ChessSDL_ShowMoveMessage(res);
		quit = true;
	}
	if (res == MoveResult::ValidMove) {
		ChessSDL_HighlightLastMove();
		IncrementTurnCounter();
	}
    if (res == MoveResult::InvalidMove) {
        ChessSDL_HighlightSelection(row, col, true);
    }
	ChessSDL_ShowMoveMessage(res);
}

void ChessSDL_HandleGameLoop()
{
    const Config& config = getConfigurations();
    bool quit = false;
    SDL_Event e;
    std::shared_ptr<Board> board = getBoard();
	static bool isPieceSelected = false;
	static int selectedRow = -1;
	static int selectedCol = -1;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            } else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                int row = y / TILE_SIZE;
                int col = x / TILE_SIZE;

                if (!isPieceSelected) {
                    auto selectedPiece = board->getPiece(row, col);
                    if (selectedPiece && selectedPiece->getColor() == getCurrentPlayerColor()) {
                        isPieceSelected = true;
                        selectedRow = row;
                        selectedCol = col;

                        // Highlight the selected piece
                        ChessSDL_HighlightSelection(selectedRow, selectedCol, false);
                    }
                } else {
                    int src_row = selectedRow;
                    int src_col = selectedCol;
                    int dest_row = row;
                    int dest_col = col;
                    isPieceSelected = false;

                    MoveResult res = makeTheMove(src_row, src_col, dest_row, dest_col);
                    ChessSDL_HandleMoveResult(res, quit, selectedRow, selectedCol);

                    if (quit == true) break;
                }
            } else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                if (isPieceSelected) {
                    ChessSDL_HighlightSelection(selectedRow, selectedCol, true);
                    isPieceSelected = false;
                    selectedRow = -1;
                    selectedCol = -1;
                }
            }
        }

        if (config.AI_OPPONENT) {
            // AI's turn (Player 2)
            if (getTurnCounter() % 2 == 0 && !quit) {  // Assuming AI plays as Black
                Move aiMove = findBestMove(config.depth);
                MoveResult aiRes = makeTheMove(aiMove.src_row, aiMove.src_col, aiMove.dest_row, aiMove.dest_col);
                ChessSDL_HandleMoveResult(aiRes, quit);
            }
        }
    }
}
