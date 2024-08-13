#pragma once

#include <array>
#include <memory>
#include <vector>
#include "Knight.h"
#include "Rook.h"
#include "Pawn.h"
#include "Bishop.h"
#include "Queen.h"
#include "King.h"

enum class MoveResult {
    InvalidPiece = 0,
    OpponentPiece,
    InvalidMove,
    KingInCheck,
    Checkmate,
    Stalemate,
    ValidMove
};

struct Move {
    int src_row, src_col;
    int dest_row, dest_col;
};

class Board
{
private:
	std::array<std::array<std::shared_ptr<Piece>, 8>, 8> m_layout;
    std::vector<Move> moveHistory;

    void initializePieceRow(int row, PieceColor color);
    void initializePawnRow(int row, PieceColor color);
    void initializeEmptyRows();
public:
    Board() {
        initializePieceRow(0, PieceColor::White);
        initializePawnRow(1, PieceColor::White);
        initializeEmptyRows();
        initializePawnRow(6, PieceColor::Black);
        initializePieceRow(7, PieceColor::Black);
        moveHistory.clear();
    }

    MoveResult move(int src_row, int src_col, int trg_row, int trg_col);
    std::shared_ptr<Piece> replace(int src_row, int src_col, int trg_row, int trg_col);
    void restore(int src_row, int src_col, int trg_row, int trg_col, std::shared_ptr<Piece> tmp_piece);
    std::shared_ptr<Piece> getPiece(int row, int col) const { return m_layout[row][col]; };
    void setPiece(int row, int col, std::shared_ptr<Piece> piece) { m_layout[row][col] = piece; };
    bool isCheckmate();
    std::shared_ptr<King> getKing(PieceColor color, int& king_row, int& king_col) const;
    bool isKingInCheck(PieceColor color) const;
    bool isSquareAttacked(int row, int col, PieceColor color) const;
    void performCastling(int src_row, int src_col, int trg_row, int trg_col);
    int checkForPromotion(int dest_row, int dest_col);
    void performEnPassant(int src_row, int src_col, int trg_row, int trg_col);
    void removePiece(int row, int col);
    Move getLastMove() const;
    bool isEnPassant(int src_row, int src_col, int trg_row, int trg_col) const;
    bool isStalemate();
};

std::shared_ptr<Board> getBoard();
void IncrementTurnCounter();
int getTurnCounter();
MoveResult makeTheMove(int src_row, int src_col, int trg_row, int trg_col);
