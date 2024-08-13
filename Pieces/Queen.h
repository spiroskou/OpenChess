#pragma once
#include "Piece.h"
class Queen :
    public Piece
{
public:
    Queen(PieceColor col) : Piece(col, PieceType::Queen) {};
    bool isValidMove(int src_row, int src_col, int trg_row, int trg_col) const override;
    bool isPathClear(int src_row, int src_col, int trg_row, int trg_col) const;
    std::string getImagePath() const override;
};

