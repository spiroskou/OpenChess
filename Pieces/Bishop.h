#pragma once
#include "Piece.h"
class Bishop :
    public Piece
{
public:
    Bishop(PieceColor col) : Piece(col, PieceType::Bishop) {};
    bool isValidMove(int src_row, int src_col, int trg_row, int trg_col) const override;
    bool isPathClear(int src_row, int src_col, int trg_row, int trg_col) const;
    std::string getImagePath() const override;
};

