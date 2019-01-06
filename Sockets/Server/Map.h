#include <iostream>
#include <vector>

using namespace std;

class Map {
private:
	int row{ 0 };
	int column{ 0 };
	vector<vector<char>> map; //[row][column]

public:
	Map();
	Map(int r, int c);
	int getRow() { return this->row; };
	int getColumn() { return this->column; };
	vector<vector<char>> getMap() { return map; };
	void setRow(int r) { this->row = r; };
	void setColumn(int c) { this->column = c; };
	vector<vector<char>> drawMap(char wall);
};

Map::Map() { 
	this->row = 0;
	this->column = 0;
};

Map::Map(int r, int c) {
	this->row = r;
	this->column = c;
};

vector<vector<char>> Map::drawMap(char wall) {
	//Zero row or column
	if (this->row == 0 || this->column == 0)
		cout << "Row or column cannot be zero" << endl;

	//Generate the basic map
	//4x4 map w/ * -> *  *
	vector<char> edges(1, wall);
	for (int i = 0; i < column - 2; i++)
		edges.push_back(' ');
	edges.push_back(wall);

	//Push horizantalLine and edges into the map
	//4x4 map w/ * -> ****
	map.push_back(vector<char> (column, wall));
	for (int i = 0; i < column - 2; i++)
		map.push_back(edges);
	map.push_back(vector<char>(column, wall));

	return map;
};