#include "Pawn.h"
#include <iostream>
#include <array>
#include "Board.h"

bool Pawn::isValidMove(int src_row, int src_col, int trg_row, int trg_col) const
{
    std::shared_ptr<Board> board = getBoard();
    int direction = (this->getColor() == PieceColor::White) ? 1 : -1;
    std::shared_ptr<Piece> trg_piece = board->getPiece(trg_row, trg_col);

    if (src_row == trg_row && src_col == trg_col) {
        return false;
    }

    if (!trg_piece) {
        // Pawn can move forward one square
        if (trg_col == src_col && trg_row == src_row + direction) {
            return true;
        } else if (trg_col == src_col && trg_row == src_row + 2 * direction && !hasMoved()) { 
            // Pawn can move forward two squares from its starting position

            // Check if the path is clear (no pieces in the way)
            if (!board->getPiece(src_row + direction, src_col)) {
                return true;
            }
        }
    } else {
        // Check if the destination block contains an opponent's piece
        std::shared_ptr<Piece> piece = board->getPiece(trg_row, trg_col);
        if (piece && (piece->getColor() != this->getColor())) {
            // Pawn can capture diagonally
            if (std::abs(trg_col - src_col) == 1 && trg_row == src_row + direction) {
                return true;
            }
        }
    }
	
    return false;
}

static bool isDoubleStep(const Move& move)
{
	int rowDiff = abs(move.dest_row - move.src_row);
	if (rowDiff == 2) {
		return true;
	}
	return false;
}

bool Pawn::isEnPassant(int src_row, int src_col, int trg_row, int trg_col) const 
{
	if (getType() != PieceType::Pawn) {
		return false;
	}

	// Check if the move is a valid en passant capture
    std::shared_ptr<Board> board = getBoard();
	int direction = (getColor() == PieceColor::White) ? 1 : -1;
	if (abs(trg_col - src_col) == 1 && trg_row == src_row + direction) {
		auto target_pawn = board->getPiece(src_row, trg_col);
		if (target_pawn && target_pawn->getType() == PieceType::Pawn && target_pawn->getColor() != getColor()) {
			// Ensure the target pawn just moved two squares in the last turn
			Move move = board->getLastMove();
			if (isDoubleStep(move)) {
				return true;
			}
		}
	}

	return false;
}

std::string Pawn::getImagePath() const
{
    std::string str1, str2 = "-pawn.png";

    switch (getColor()) {
        case (PieceColor::White):
            str1 = "../../../images/white";
            break;
        case (PieceColor::Black):
            str1 = "../../../images/black";
            break;
        case (PieceColor::Blank):
        default:
            str1 = "";
            break;
    }

    return (str1 + str2);
}

