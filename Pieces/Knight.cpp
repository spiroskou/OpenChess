#include "Knight.h"
#include <iostream>
#include <array>
#include "Board.h"

bool Knight::isValidMove(int src_row, int src_col, int trg_row, int trg_col) const
{
    std::shared_ptr<Board> board = getBoard();

    if (src_row == trg_row && src_col == trg_col) {
        return false;
    }

    // Calculate the absolute difference in rows and columns
    int row_diff = std::abs(trg_row - src_row);
    int col_diff = std::abs(trg_col - src_col);

    // Check if the move is in an L-shape (2 squares in one direction and 1 square in the other)
    if ((row_diff == 2 && col_diff == 1) || (row_diff == 1 && col_diff == 2)) {
        // Check if the target block is empty or contains an opponent's piece
        std::shared_ptr<Piece> trg_piece = board->getPiece(trg_row, trg_col);
        if (!trg_piece || trg_piece->getColor() != this->getColor()) {
            return true;
        }
    }

    return false;
}

std::string Knight::getImagePath() const
{
    std::string str1, str2 = "-knight.png";

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

