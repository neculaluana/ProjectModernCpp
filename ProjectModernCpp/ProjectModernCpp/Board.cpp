﻿#include "Board.h"

Board::Board(size_t boardSize) :
	m_boardSize{ boardSize }, m_board{ boardSize, std::vector<Status>{boardSize, Status::Empty} }
{
	setBases(boardSize);
}

Board::Board(const Board& other)
	: m_boardSize{ other.m_boardSize }, m_board{ other.m_board }
{

}

Board& Board::operator=(const Board& other)
{
	if (this == &other)
		return *this;

	m_boardSize = other.m_boardSize;
	m_board = other.m_board;

	return *this;
}

Board::Board(Board&& other)noexcept
	: m_boardSize{ std::exchange(other.m_boardSize, 0) }, m_board{ std::move(other.m_board) }
{

}

Board& Board::operator=(Board&& other)noexcept
{
	if (this == &other)
	{
		return *this;
	}

	m_boardSize = std::exchange(other.m_boardSize, 0);
	m_board = std::move(other.m_board);

	return *this;
}

void Board::setBases(size_t boardSize) {
	boardSize = std::min(boardSize, m_boardSize);

	for (size_t i = 0; i < boardSize; ++i) {
		m_board[i][0] = Status::BaseRed;
		m_board[i][boardSize - 1] = Status::BaseRed;
		m_board[0][i] = Status::BaseRed;
		m_board[boardSize - 1][i] = Status::BaseRed;
	}
}

void Board::boardResize(size_t boardSize){
	m_boardSize = boardSize;
	m_board.resize(boardSize, std::vector<Status>(boardSize, Status::Empty));
	setBases(boardSize);
}

void Board::setStatus(const Position& coordinate, Board::Status status)
{
	m_board[coordinate.first][coordinate.second] = status;
}


Board::Status Board::getStatus(const Position& coordinate)const
{
	if (coordinate.first < m_boardSize && coordinate.second < m_boardSize)
	{
		return m_board[coordinate.first][coordinate.second];
	}
	else
		return Status::Invalid;
}

size_t Board::getBoardSize()const noexcept
{
	return m_boardSize;
}

void Board::printBoard()const
{
	std::cout << *this;

}
void Board::addPoint(const Point& p)
{
	if (p.getColor() == Point::Color::Red)
		m_board[p.getCoordinates().first][p.getCoordinates().second] = Board::Status::PlayerRed;
	else 
		m_board[p.getCoordinates().first][p.getCoordinates().second] = Board::Status::PlayerBlack;

}

bool Board::isBridgePossible(const Point& p1, const Point& p2)const
{
	if (((std::abs(p1.getCoordinates().first - p2.getCoordinates().first) == 1 && std::abs(p1.getCoordinates().second - p2.getCoordinates().second) == 2) ||
		(std::abs(p1.getCoordinates().first - p2.getCoordinates().first) == 2 && std::abs(p1.getCoordinates().second - p2.getCoordinates().second) == 1) ||
		(std::abs(p1.getCoordinates().first - p2.getCoordinates().first) == 0 && std::abs(p1.getCoordinates().second - p2.getCoordinates().second) == 2) ||
		(std::abs(p1.getCoordinates().first - p2.getCoordinates().first) == 2 && std::abs(p1.getCoordinates().second - p2.getCoordinates().second) == 1) ||
		(std::abs(p1.getCoordinates().first - p2.getCoordinates().first) == 1 && std::abs(p1.getCoordinates().second - p2.getCoordinates().second) == 2)) &&
		(p1.getColor() == p2.getColor()) && 
		(p1.getCoordinates().first != p2.getCoordinates().first && p1.getCoordinates().second != p2.getCoordinates().second))
	{
		return true;
	}
	return false;

}


void Board::makeBridges(const Point& point, Player& player)
{
	std::vector<Point> points = player.getPoints();
	for (const auto& pnt : points)
	{
		if (Board::isBridgePossible(point,pnt))
		{
			Bridge bridge(point,pnt);
			bool bridgePossible = true;

			for (const auto& b : m_bridges) {
				if (isBridgesIntersection(b, bridge))
				{
					bridgePossible = false;
					break;
				}
			}

			if(bridgePossible)
			{
				player.addBridge(bridge);
				m_bridges.push_back(bridge);
			}
		}
	}
}


bool Board::isBridgesIntersection(const Bridge& existingBridge, const Bridge& newBridge) {
	auto onSegment = [](const Point& p, const Point& q, const Point& r) -> bool {
		// If q is the same as p or r, it is considered on segment but not necessarily crossing.
		if (q.getCoordinates() == p.getCoordinates() || q.getCoordinates() == r.getCoordinates()) return false;

		// Check if q lies on segment pr.
		return q.getCoordinates().first <= std::max(p.getCoordinates().first, r.getCoordinates().first) &&
			q.getCoordinates().first >= std::min(p.getCoordinates().first, r.getCoordinates().first) &&
			q.getCoordinates().second <= std::max(p.getCoordinates().second, r.getCoordinates().second) &&
			q.getCoordinates().second >= std::min(p.getCoordinates().second, r.getCoordinates().second);
		};

	auto orientation = [](const Point& p, const Point& q, const Point& r) -> int {
		// ... orientation logic as previously defined// Determine the orientation of the triplet (p, q, r)
        // The function returns the following values:
        // 0 --> p, q, and r are collinear
        // 1 --> Clockwise
        // 2 --> Counterclockwise
		int val = (q.getCoordinates().second - p.getCoordinates().second) * (r.getCoordinates().first - q.getCoordinates().first) -
			(q.getCoordinates().first - p.getCoordinates().first) * (r.getCoordinates().second - q.getCoordinates().second);
		if (val == 0) return 0;  // Collinear
		return (val > 0) ? 1 : 2; // Clockwise or Counterclockwise
		};


	// Start and end points of the existing and new bridges
	Point p1 = existingBridge.getStartPoint();
	Point q1 = existingBridge.getEndPoint();
	Point p2 = newBridge.getStartPoint();
	Point q2 = newBridge.getEndPoint();

	if (p1 == p2 || p1 == q2 || q1 == p2 || q1 == q2) return false;

	if ((p1 == p2 && q1 == q2) || (p1 == q2 && q1 == p2)) {
		return false;
	}

	// General case: Check if there is an intersection between non-collinear segments.
	int o1 = orientation(p1, q1, p2);
	int o2 = orientation(p1, q1, q2);
	int o3 = orientation(p2, q2, p1);
	int o4 = orientation(p2, q2, q1);

	if ((o1 != o2 && o3 != o4) && !(p1 == q1 || p1 == q2 || p2 == q1 || p2 == q2))
		return true; // The new bridge crosses the existing bridge.
	

	// Collinear cases
	if (o1 == 0 && onSegment(p1, p2, q1)) return true;
	if (o2 == 0 && onSegment(p1, q2, q1)) return true;
	if (o3 == 0 && onSegment(p2, p1, q2)) return true;
	if (o4 == 0 && onSegment(p2, q1, q2)) return true;

	return false;
}

bool Board::isPointPossible(const Position& coordinate) const
{
	if (m_board[coordinate.first][coordinate.second] == Board::Status::Empty)
		return true;
	else
		return false;
}

std::ostream& operator<<(std::ostream& os, const Board& board) {
	for (size_t y= 0; y < board.getBoardSize(); ++y) {
		for (size_t x = 0; x < board.getBoardSize(); ++x) {
			Board::Status status = board.getStatus({ x, y });
			os << static_cast<char>(status) << ' ';
		}
		os << '\n';
	}
	return os;
}

std::istream& operator>>(std::istream& is, Board& board) {
	char ch;
	for (size_t y = 0; y < board.getBoardSize(); ++y) {
		for (size_t x = 0; x < board.getBoardSize(); ++x) {
			is >> ch;
			Board::Position pos = { x, y };
			switch (ch) {
			case '.': board.m_board[x][y] = Board::Status::Empty; break;
			case 'r': board.m_board[x][y] = Board::Status::PlayerRed; break;
			case 'b': board.m_board[x][y] = Board::Status::PlayerBlack; break;
			case '-': board.m_board[x][y] = Board::Status::BaseRed; break;
			case '|': board.m_board[x][y] = Board::Status::BaseBlack; break;
			default: board.m_board[x][y] = Board::Status::Empty; break;
			}
		}
	}
	return is;
}