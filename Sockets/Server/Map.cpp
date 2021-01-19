#include "Map.h"

#include <iostream>
#include <vector>

using namespace std;

// Functions
vector<vector<char>> Map::drawMap(char wall) {
	//Zero row or column
	if (this->row == 0 || this->column == 0)
		cout << "Row or column cannot be zero" << endl;

	//Generate the basic map
	//4x4 map w/ '*' -> *  *
	vector<char> edges(1, wall);
	for (int i = 0; i < column - 2; i++)
		edges.push_back(' ');
	edges.push_back(wall);

	//Push horizantalLine and edges into the map
	//4x4 map w/ '*' -> ****
	map.push_back(vector<char>(column, wall));
	for (int i = 0; i < column - 2; i++)
		map.push_back(edges);
	map.push_back(vector<char>(column, wall));

	return map;
};