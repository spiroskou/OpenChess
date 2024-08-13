#pragma once
#include "Piece.h"

class Knight : public Piece
{
public:
	Knight(PieceColor col) : Piece(col, PieceType::Knight)
	{
	};

    bool isValidMove(int src_row, int src_col, int trg_row, int trg_col) const override;
    std::string getImagePath() const override;
};

