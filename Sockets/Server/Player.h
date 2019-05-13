#include <iostream>
#include <vector>
#include <windows.h> // for sleep()

using namespace std;

class Player {
private:
	string name{ "Player" };
	char symbol{ 'o' };
public:
	Player() = default;
	Player(string Name, char sym) { name = Name; symbol = sym; };
	string getName() { return name; };
	char getSymbol() { return symbol; };
	void setName(string Name) { name = Name; };
	void setSymbol(char sym) { symbol = sym; };
}; 
