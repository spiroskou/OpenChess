#include <iostream>
#include "Board.h"
#include "Piece.h"
#include <windows.h>
#include <memory>
#include <limits>
#include "SDL.h"

#undef min
#undef max

static std::shared_ptr<Board> board = std::make_shared<Board>();
static int turn_counter = 1;

void Board::initializePieceRow(int row, PieceColor color) {
	m_layout[row][0] = std::make_shared<Rook>(color);
	m_layout[row][1] = std::make_shared<Knight>(color);
	m_layout[row][2] = std::make_shared<Bishop>(color);
	m_layout[row][3] = std::make_shared<Queen>(color);
	m_layout[row][4] = std::make_shared<King>(color);
	m_layout[row][5] = std::make_shared<Bishop>(color);
	m_layout[row][6] = std::make_shared<Knight>(color);
	m_layout[row][7] = std::make_shared<Rook>(color);
}

void Board::initializePawnRow(int row, PieceColor color) {
	for (int col = 0; col < 8; ++col) {
		m_layout[row][col] = std::make_shared<Pawn>(color);
	}
}

void Board::initializeEmptyRows() {
	for (int row = 2; row <= 5; ++row) {
		for (int col = 0; col < 8; ++col) {
			m_layout[row][col] = nullptr;
		}
	}
}

static PieceColor getOpponentColor()
{
	if (turn_counter % 2 != 0) {
		return PieceColor::Black;
	}

	return PieceColor::White;
}

static PieceColor getColor()
{
	if (turn_counter % 2 != 0) {
		return PieceColor::White;
	}

	return PieceColor::Black;
}

int getTurnCounter()
{
	return turn_counter;
}

void IncrementTurnCounter()
{
	turn_counter++;
}

std::shared_ptr<Board> getBoard()
{
	return board;
}

std::shared_ptr<Piece> Board::replace(int src_row, int src_col, int trg_row, int trg_col)
{
	std::shared_ptr<Piece> src_piece = getPiece(src_row, src_col);
	std::shared_ptr<Piece> dest_piece = getPiece(trg_row, trg_col);
	setPiece(trg_row, trg_col, src_piece);
	setPiece(src_row, src_col, nullptr);

	return dest_piece;
}

void Board::restore(int src_row, int src_col, int trg_row, int trg_col, std::shared_ptr<Piece> tmp_piece)
{
	std::shared_ptr<Piece> dest_piece = getPiece(trg_row, trg_col);
	setPiece(trg_row, trg_col, tmp_piece);
	setPiece(src_row, src_col, dest_piece);
}

Move Board::getLastMove() const 
{
	if (moveHistory.empty()) {
		return Move{ -1, -1, -1, -1 }; // Return an invalid move if no moves have been made
	}
	return moveHistory.back();
}

static bool isDoubleStep(Move& move)
{
	int rowDiff = abs(move.dest_row - move.src_row);
	if (rowDiff == 2) {
		return true;
	}
	return false;
}

bool Board::isEnPassant(int src_row, int src_col, int trg_row, int trg_col) const 
{
	auto pawn = getPiece(src_row, src_col);
	if (pawn->getType() != PieceType::Pawn) {
		return false;
	}

	// Check if the move is a valid en passant capture
	int direction = (pawn->getColor() == PieceColor::White) ? 1 : -1;
	if (abs(trg_col - src_col) == 1 && trg_row == src_row + direction) {
		auto target_pawn = getPiece(src_row, trg_col);
		if (target_pawn && target_pawn->getType() == PieceType::Pawn && target_pawn->getColor() != pawn->getColor()) {
			// Ensure the target pawn just moved two squares in the last turn
			Move move = getLastMove();
			if (isDoubleStep(move)) {
				return true;
			}
		}
	}

	return false;
}

void Board::removePiece(int row, int col) 
{
	m_layout[row][col] = nullptr;
}

void Board::performEnPassant(int src_row, int src_col, int trg_row, int trg_col) 
{
	auto pawn = getPiece(src_row, src_col);
	int direction = (pawn->getColor() == PieceColor::White) ? 1 : -1;

	// Capture the target pawn
	auto target_pawn = getPiece(src_row, trg_col);
	removePiece(src_row, trg_col);

	// Move the capturing pawn to the target square
	replace(src_row, src_col, trg_row, trg_col);
}

bool Board::isSquareAttacked(int row, int col, PieceColor color) const 
{
	PieceColor opponentColor = getOpponentColor();

	// Iterate over all squares on the board
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			auto piece = getPiece(i, j);

			// If the piece is null or not an opponent's piece, continue
			if (!piece || piece->getColor() != opponentColor) {
				continue;
			}

			// Handle pawn's attack moves separately
			if (piece->getType() == PieceType::Pawn) {
				int direction = (piece->getColor() == PieceColor::White) ? 1 : -1;
				if (i + direction == row && (j - 1 == col || j + 1 == col)) {
					return true;
				}
			} else {
				// Check if the piece can move to (row, col)
				if (piece->isValidMove(i, j, row, col)) {
					return true;
				}
			}
		}
	}

	return false;
}

std::shared_ptr<King> Board::getKing(PieceColor color, int &king_row, int& king_col) const
{
	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			std::shared_ptr<Piece> piece = m_layout[row][col];
			if (!piece) continue;
			
			if (piece->getType() == PieceType::King && piece->getColor() == color) {
				std::shared_ptr<King> king = std::static_pointer_cast<King> (piece);
				king_row = row;
				king_col = col;
				return king;
			}
		}
	}

	return nullptr;
}

bool Board::isKingInCheck(PieceColor color) const
{
	// If the king is not found, it's not a valid state, so return false
	int king_row = -1, king_col = -1;
	std::shared_ptr<King> king = getKing(color, king_row, king_col);
	if (!king) {
		return false;
	}

	// Check if any opponent's pieces threaten the king

	for (int row = 0; row < 8; row++) {
		for (int col = 0; col < 8; col++) {
			std::shared_ptr<Piece> piece = getPiece(row,col);
			if (!piece) continue;
			
			if (piece->getColor() == king->getColor()) {
				continue;
			}

			if (piece->isValidMove(row, col, king_row, king_col)) {
				return true;
			}
		}
	}

	return false; // King is not in check
}

int Board::checkForPromotion(int dest_row, int dest_col)
{
	std::shared_ptr<Piece> tmp_piece = getPiece(dest_row, dest_col);
	if (!tmp_piece) return 0;

	if (tmp_piece->getType() == PieceType::Pawn && (dest_row == 0 || dest_row == 7)) {
		setPiece(dest_row, dest_col, std::make_shared<Queen>(getColor()));
		return 1;
	}

	return 0;
}

bool Board::isCheckmate()
{
	PieceColor opp_color = getOpponentColor();

	int king_row = -1, king_col = -1;
	std::shared_ptr<King> king = getKing(opp_color, king_row, king_col);
	// If the king is not found, it's not a valid state, so return false
	if (!king) {
		return false;
	}

	// Check if the king is in check
	if (!isKingInCheck(opp_color)) {
		return false; // King is not in check, so it's not checkmate
	}

	// Check if the king has any legal moves to escape check
	for (int row = king_row - 1; row <= king_row + 1; ++row) {
		for (int col = king_col - 1; col <= king_col + 1; ++col) {
			if (row >= 0 && row < 8 && col >= 0 && col < 8 && !(row == king_row && col == king_col)) {
				std::shared_ptr<Piece> piece = getPiece(row, col);
				if (piece && piece->getColor() == opp_color) continue;
				int dum_king_row = -1, dum_king_col = -1;

				// Try to move the king to the target position and check if it's still in check
				king = getKing(opp_color, dum_king_row, dum_king_col);
				std::shared_ptr<Piece> tmp_piece = replace(dum_king_row, dum_king_col, row, col);
				bool still_in_check = isKingInCheck(opp_color);

				// Undo the move
				restore(dum_king_row, dum_king_col, row, col, tmp_piece);

				if (!still_in_check) {
					return false; // King has at least one legal move to escape check
				}
			}
		}
	}

	// Check if any piece can block the check or capture the attacking piece
	for (int row = 0; row < 8; ++row) {
		for (int col = 0; col < 8; ++col) {
			if (row == king_row && col == king_col) continue;
			std::shared_ptr<Piece> piece = getPiece(row, col);
			if (!piece) continue;

			if (piece->getColor() == opp_color) {
				// Check all possible moves for this piece
				for (int target_row = 0; target_row < 8; ++target_row) {
					for (int target_col = 0; target_col < 8; ++target_col) {
						if (piece->isValidMove(row, col, target_row, target_col)) {
							std::shared_ptr<Piece> tmp_piece = replace(row, col, target_row, target_col);
							bool still_in_check = isKingInCheck(opp_color);

							// Undo the move
							restore(row, col, target_row, target_col, tmp_piece);

							if (!still_in_check) {
								return false; // There is a piece that can block the check or capture the attacker
							}
						}
					}
				}
			}
		}
	}

	// If the king has no legal moves to escape check, it's checkmate
	return true;

}
bool Board::isStalemate() 
{
	// Get the position of the current player's king
	int kingRow = -1, kingCol = -1;
	std::shared_ptr<King> king = getKing(getColor(), kingRow, kingCol);

	// If the king is in check, it's not a stalemate
	if (isKingInCheck(getColor())) {
		return false;
	}

	// Iterate through all pieces of the current player
	for (int row = 0; row < 8; ++row) {
		for (int col = 0; col < 8; ++col) {
			std::shared_ptr<Piece> piece = getPiece(row, col);
			if (piece && piece->getColor() == getColor()) {
				// Check all possible moves for this piece
				for (int destRow = 0; destRow < 8; ++destRow) {
					for (int destCol = 0; destCol < 8; ++destCol) {
						if (piece->isValidMove(row, col, destRow, destCol)) {
							// Simulate the move
							std::shared_ptr<Piece> tmpPiece = replace(row, col, destRow, destCol);
							bool stillInCheck = isKingInCheck(getColor());
							restore(row, col, destRow, destCol, tmpPiece);

							// If the move does not leave the king in check, it's not a stalemate
							if (!stillInCheck) {
								return false;
							}
						}
					}
				}
			}
		}
	}

	// If no legal moves are found, it's a stalemate
	return true;
}

void Board::performCastling(int src_row, int src_col, int trg_row, int trg_col) 
{
	// Perform the castling move
	int rook_col = (trg_col == 6) ? 7 : 0;
	int new_rook_col = (trg_col == 6) ? 5 : 3;

	auto king = getPiece(src_row, src_col);
	auto rook = getPiece(src_row, rook_col);

	replace(src_row, src_col, trg_row, trg_col); // Move the king
	replace(src_row, rook_col, trg_row, new_rook_col); // Move the rook
}

MoveResult Board::move(int src_row, int src_col, int trg_row, int trg_col)
{
	auto src_piece = getPiece(src_row, src_col);

	// Check if the player chose a piece
	if (!src_piece) {
		return MoveResult::InvalidPiece;
	}

	// Check if the player chose opponent's piece
	PieceColor opp_color = getOpponentColor();
	if (src_piece->getColor() == opp_color) {
		return MoveResult::OpponentPiece;
	}

	// Handle castling move
	if (src_piece->getType() == PieceType::King) {
		std::shared_ptr<King> king = std::static_pointer_cast<King> (src_piece);
		if (king->canCastle(src_row, src_col, trg_row, trg_col)) {
			performCastling(src_row, src_col, trg_row, trg_col);
			king->setMoved(true);
			Move move{ src_row, src_col, trg_row, trg_col };
			moveHistory.push_back(move);
			return MoveResult::ValidMove;
		}
	}

	// Handle EnPassant move
	if (src_piece->getType() == PieceType::Pawn) {
		if (isEnPassant(src_row, src_col, trg_row, trg_col)) {
			performEnPassant(src_row, src_col, trg_row, trg_col);
			src_piece->setMoved(true);
			Move move{ src_row, src_col, trg_row, trg_col };
			moveHistory.push_back(move);
			return MoveResult::ValidMove;
		}
	}

	// Check if the player chose a valid move for the corresponding Piece
	if (!getPiece(src_row, src_col)->isValidMove(src_row, src_col, trg_row, trg_col)) {
		return MoveResult::InvalidMove;
	}

	std::shared_ptr<Piece> tmp_piece = replace(src_row, src_col, trg_row, trg_col);
	
	if (isKingInCheck(getColor())) {
		restore(src_row, src_col, trg_row, trg_col, tmp_piece);
		return MoveResult::KingInCheck;
	}

	if (isCheckmate()) {
		return MoveResult::Checkmate;
	}

	if (isStalemate()) {
		return MoveResult::Stalemate;
	}

	if (checkForPromotion(trg_row, trg_col)) {
		if (isCheckmate()) {
			return MoveResult::Checkmate;
		}
	}

	src_piece->setMoved(true);
	Move move{ src_row, src_col, trg_row, trg_col };
	moveHistory.push_back(move);

	return MoveResult::ValidMove;
}

MoveResult makeTheMove(int src_row, int src_col, int trg_row, int trg_col)
{
	return board->move(src_row, src_col, trg_row, trg_col);
}

void Board::makeMove(const Move& move) 
{
	moveHistory.push_back(move);
	replace(move.src_row, move.src_col, move.dest_row, move.dest_col);
}

void Board::undoMove(const Move& move, std::shared_ptr<Piece> capturedPiece) 
{
	// Revert the move
	restore(move.src_row, move.src_col, move.dest_row, move.dest_col, capturedPiece);
	moveHistory.pop_back();
}

std::vector<Move> Board::getPossibleMoves(PieceColor color) const 
{
	std::vector<Move> moves;
	for (int row = 0; row < 8; ++row) {
		for (int col = 0; col < 8; ++col) {
			auto piece = getPiece(row, col);

			if (piece && piece->getColor() == color) {
				piece->getValidMoves(row, col, moves);
			}
		}
	}
	return moves;
}

int Board::evaluate() const 
{
	// A simple evaluation function: positive for White, negative for Black
	int score = 0;
	for (int row = 0; row < 8; ++row) {
		for (int col = 0; col < 8; ++col) {
			auto piece = getPiece(row, col);
			if (piece) {
				score -= piece->getValue();
			}
		}
	}
	return score;
}

int minimax(int depth, int alpha, int beta, bool isMaximizingPlayer) 
{
	if (depth == 0 || board->isCheckmate() || board->isStalemate()) {
		return board->evaluate();
	}

	PieceColor currentTurn = isMaximizingPlayer ? PieceColor::White : PieceColor::Black;

	if (isMaximizingPlayer) {
		int maxEval = std::numeric_limits<int>::min();
		for (const Move& move : board->getPossibleMoves(currentTurn)) {
			std::shared_ptr<Piece> capturedPiece = board->getPiece(move.dest_row, move.dest_col);
			board->makeMove(move);
			int eval = minimax(depth - 1, alpha, beta, false);
			board->undoMove(move, capturedPiece);
			maxEval = std::max(maxEval, eval);
			alpha = std::max(alpha, eval);
			if (beta <= alpha) {
				break;
			}
		}
		return maxEval;
	} else {
		int minEval = std::numeric_limits<int>::max();
		for (const Move& move : board->getPossibleMoves(currentTurn)) {
			std::shared_ptr<Piece> capturedPiece = board->getPiece(move.dest_row, move.dest_col);
			board->makeMove(move);
			int eval = minimax(depth - 1, alpha, beta, true);
			board->undoMove(move, capturedPiece);
			minEval = std::min(minEval, eval);
			beta = std::min(beta, eval);
			if (beta <= alpha) {
				break;
			}
		}
		return minEval;
	}
}

Move findBestMove(int depth) 
{
	int bestValue = std::numeric_limits<int>::max();
	Move bestMove {-1, -1, -1, -1};
	PieceColor currentTurn = PieceColor::Black;  // Assuming AI plays as Black
	std::vector<Move> moves = board->getPossibleMoves(currentTurn);

	for (const Move& move : moves) {
		std::shared_ptr<Piece> capturedPiece = board->getPiece(move.dest_row, move.dest_col);

		board->makeMove(move);
		int boardValue = minimax(depth - 1, std::numeric_limits<int>::min(), std::numeric_limits<int>::max(), true);
		board->undoMove(move, capturedPiece);

		if (boardValue < bestValue) {
			bestValue = boardValue;
			bestMove = move;
		}
	}
	return bestMove;
}


