#include "Piece.h"

void Piece::getValidMoves(int row, int col, std::vector<Move> &moves)
{
	for (int dest_row = 0; dest_row < 8; dest_row++) {
		for (int dest_col = 0; dest_col < 8; dest_col++) {
			if (isValidMove(row, col, dest_row, dest_col)) {
				Move move{ row, col, dest_row, dest_col };
				moves.push_back(move);
			}
		}
	}
}

int Piece::getValue() const
{
    switch (m_type)
    {
    case PieceType::Pawn:
        return 1;
    case PieceType::Knight:
        return 3;
    case PieceType::Bishop:
        return 3;
    case PieceType::Rook:
        return 5;
    case PieceType::Queen:
        return 9;
    case PieceType::King:
        return 1000; // Use a high value for King; this value is just an example
    default:
        return 0; // For PieceType::Empty or unknown types
    }
}