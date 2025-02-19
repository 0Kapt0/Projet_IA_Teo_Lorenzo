#ifndef GRID_HPP
#define GRID_HPP

#include <vector>
#include <SFML/Graphics.hpp>
#include <string>

using namespace sf;
using namespace std;

const int GRID_WIDTH = 40;
const int GRID_HEIGHT = 15;
const int CELL_SIZE = 40;

struct Cell {
    bool walkable;
    Vector2f position;
    RectangleShape shape;
};

class Grid {
public:
    Grid();
    void loadFromFile(const string& filename);
    void draw(RenderWindow& window);
    Cell& getCell(int x, int y);
    bool isWalkable(int x, int y) const;
    vector<Vector2i> getNeighbors(int x, int y) const;
    vector<Vector2i> debugPath;
private:
    vector<vector<Cell>> cells;
};

#endif // GRID_HPP
