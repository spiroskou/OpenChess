#pragma once
#include "Piece.h"

class Pawn : public Piece
{
public:
    Pawn(PieceColor col) : Piece(col, PieceType::Pawn)
    {
    };

    bool isValidMove(int src_row, int src_col, int trg_row, int trg_col) const override;
    bool isEnPassant(int src_row, int src_col, int trg_row, int trg_col) const;

    std::string getImagePath() const override;
};

