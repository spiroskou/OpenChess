#include <iostream>
#include <map>
#include <string>
#include <SDL.h>
#include "ChessSDL.h"
#include <vector>
#include <memory>
#include "Board.h"
#include <SDL_image.h>

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
        nullptr;
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

    return 0;
}

void ChessSDL_HighlightLastMove()
{
    std::shared_ptr<Board> board = getBoard();
    Move move = board->getLastMove();

    if (move.src_col == -1) return;

    SDL_Renderer* renderer = getRenderer();

	SDL_SetRenderDrawColor(renderer, 255, 255, 130, 255);
	SDL_Rect tile = { move.src_col * TILE_SIZE, move.src_row * TILE_SIZE, TILE_SIZE, TILE_SIZE };
	SDL_RenderFillRect(renderer, &tile);
	tile = { move.dest_col * TILE_SIZE, move.dest_row * TILE_SIZE, TILE_SIZE, TILE_SIZE };
	SDL_RenderFillRect(renderer, &tile);
    
    std::shared_ptr<Piece> piece = board->getPiece(move.dest_row, move.dest_col);
	if (piece) {
		std::string imagePath;

		imagePath = piece->getImagePath();
		SDL_Texture* texture = getTexture(imagePath);
		if (texture) {
			SDL_Rect pieceRect = { move.dest_col * TILE_SIZE, move.dest_row * TILE_SIZE, TILE_SIZE, TILE_SIZE };
			SDL_RenderCopy(renderer, texture, nullptr, &pieceRect);
		}
	}

	SDL_RenderPresent(renderer);
}

void ChessSDL_RenderChessBoard(bool lastMove)
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
        white = !white;
    }

	SDL_RenderPresent(renderer);

    if (lastMove) ChessSDL_HighlightLastMove();
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
