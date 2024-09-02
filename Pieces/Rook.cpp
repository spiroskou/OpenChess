#include "Rook.h"
#include <iostream>
#include <array>
#include "Board.h"

bool Rook::isValidMove(int src_row, int src_col, int trg_row, int trg_col) const
{
    std::shared_ptr<Board> board = getBoard();

    if (src_row == trg_row && src_col == trg_col) {
        return false;
    }

    // Check if the rook is moving horizontally or vertically
    if (src_row == trg_row || src_col == trg_col) {
        // Check if the path is clear
        if (isPathClear(src_row, src_col, trg_row, trg_col)) {
            // Check if the target block is empty or contains an opponent's piece
            std::shared_ptr<Piece> trg_piece = board->getPiece(trg_row, trg_col);
            if (!trg_piece || trg_piece->getColor() != this->getColor()) {
                return true;
            }
        }
    }

    return false;
}

bool Rook::isPathClear(int src_row, int src_col, int trg_row, int trg_col) const
{
    std::shared_ptr<Board> board = getBoard();

    // Check if the rook is moving horizontally
    if (src_row == trg_row) {
        int start_col = std::min(src_col, trg_col);
        int end_col = std::max(src_col, trg_col);
        for (int col = start_col + 1; col < end_col; ++col) {
            if (board->getPiece(src_row, col)) {
                return false; // Path is pieceed
            }
        }
    }
    // Check if the rook is moving vertically
    else if (src_col == trg_col) {
        int start_row = std::min(src_row, trg_row);
        int end_row = std::max(src_row, trg_row);
        for (int row = start_row + 1; row < end_row; ++row) {
            if (board->getPiece(row, src_col)) {
                return false; // Path is pieceed
            }
        }
    }

    return true; // Path is clear
}
std::string Rook::getImagePath() const
{
    std::string str1, str2 = "-rook.png";

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
