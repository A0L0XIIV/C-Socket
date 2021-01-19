#pragma once

#include <vector>

class Map {
private:
	int row{ 0 };
	int column{ 0 };
	std::vector<std::vector<char>> map; //[row][column]

public:
	// Constructors
	Map() = default;
	Map(int r, int c) : row(r), column(c) {};

	// Get Functions
	int getRow() { return this->row; };
	int getColumn() { return this->column; };
	std::vector<std::vector<char>> getMap() { return map; };

	// Set Functions
	void setRow(int r) { this->row = r; };
	void setColumn(int c) { this->column = c; };
	std::vector<std::vector<char>> drawMap(char wall);
};