#pragma once
#include "Piece.h"
class King : public Piece
{
public:
    King(PieceColor col) : Piece(col, PieceType::King) {};

    bool isValidMove(int src_row, int src_col, int trg_row, int trg_col) const override;
    bool canCastle(int src_row, int src_col, int trg_row, int trg_col) const;
    std::string getImagePath() const override;
};

