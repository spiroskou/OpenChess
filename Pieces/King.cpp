#include "King.h"
#include <iostream>
#include <array>
#include "Board.h"

bool King::isValidMove(int src_row, int src_col, int trg_row, int trg_col) const
{
    std::shared_ptr<Board> board = getBoard();

    if (src_row == trg_row && src_col == trg_col) {
        return false;
    }

    // Check if the target block is within one square of the king's position
    if (std::abs(trg_row - src_row) <= 1 && std::abs(trg_col - src_col) <= 1) {
        // Check if the target block is empty or contains an opponent's piece
        std::shared_ptr<Piece> trg_piece = board->getPiece(trg_row, trg_col);
        if (!trg_piece || trg_piece->getColor() != this->getColor()) {
            return true;
        }
    }

    return false;
}

bool King::canCastle(int src_row, int src_col, int trg_row, int trg_col) const
{
    if (abs(trg_col - src_col) != 2 || trg_row != src_row) {
        return false;
    }

    // Determine the direction of castling
    int rook_col = (trg_col == 6) ? 7 : 0;
    std::shared_ptr<Board> board = getBoard();
    auto rook = board->getPiece(src_row, rook_col);
    if (!rook || rook->getType() != PieceType::Rook || rook->hasMoved()) {
        return false;
    }

    // Check that there are no pieces between the king and the rook
    int direction = (trg_col - src_col) / 2;
    for (int col = src_col + direction; col != trg_col; col += direction) {
        if (board->getPiece(src_row, col)) {
            return false;
        }
    }

    // Ensure the king is not in check, and does not pass through or land in a square that is under attack
    for (int col = src_col; col != trg_col + direction; col += direction) {
        if (board->isSquareAttacked(src_row, col, getColor())) {
            return false;
        }
    }

    return true;
}

std::string King::getImagePath() const
{
    std::string str1, str2 = "-king.png";

    switch (getColor()) {
        case (PieceColor::White):
            str1 = "images/white";
            break;
        case (PieceColor::Black):
            str1 = "images/black";
            break;
        case (PieceColor::Blank):
        default:
            str1 = "";
            break;
    }

    return (str1 + str2);
}
