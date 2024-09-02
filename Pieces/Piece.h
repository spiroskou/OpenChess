#pragma once

#include <array>
#include <string>
#include <vector>
#include <memory>

struct Move;

enum class PieceType {Empty, Pawn, Knight, Bishop, Rook, Queen, King};
enum class PieceColor {Blank, White, Black};

class Piece
{
public:
	Piece(PieceColor col = PieceColor::Blank, PieceType type = PieceType::Empty)
		: m_type{ type }, m_color {col}
	{
		m_hasMoved = false;
	};

	const PieceColor& getColor() const { return m_color; };
	const PieceType& getType() const { return m_type; };
	const bool& hasMoved() const { return m_hasMoved; } ;
	void setMoved(bool val) { m_hasMoved = val; };
	void setType(PieceType type) { m_type = type; };
	virtual bool isValidMove(int src_row, int src_col, int trg_row, int trg_col) const { return false; };
	virtual std::string getImagePath() const { return ""; };
	void getValidMoves(int row, int col, std::vector<Move> &moves);
	int getValue() const;

private:
	PieceType m_type;
	PieceColor m_color;
	bool m_hasMoved;
};


