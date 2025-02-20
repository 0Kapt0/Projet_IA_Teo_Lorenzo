#include "Grid.hpp"
#include <fstream>
#include <iostream>

Grid::Grid() {
    cells.resize(GRID_HEIGHT, vector<Cell>(GRID_WIDTH, { true, {0, 0}, RectangleShape(Vector2f(CELL_SIZE, CELL_SIZE)) }));

    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            cells[y][x].position = Vector2f(x * CELL_SIZE, y * CELL_SIZE);
            cells[y][x].shape.setPosition(cells[y][x].position);
            cells[y][x].shape.setFillColor(Color::Transparent);
            cells[y][x].shape.setOutlineThickness(1);
            cells[y][x].shape.setOutlineColor(Color(50, 50, 50));
        }
    }
}

void Grid::loadFromFile(const string& filename) {
    ifstream file(filename);
    if (!file) {
        cerr << "Erreur : Impossible d'ouvrir le fichier " << filename << endl;
        return;
    }

    for (int y = 0; y < GRID_HEIGHT; ++y) {
        string line;
        if (!getline(file, line)) break;
        for (int x = 0; x < GRID_WIDTH && x < line.size(); ++x) {
            cells[y][x].walkable = (line[x] == '0');
            if (!cells[y][x].walkable) {
                cells[y][x].shape.setFillColor(Color::White);
            }
            else {
                cells[y][x].shape.setFillColor(Color::Transparent);
            }
        }
    }
}

void Grid::draw(RenderWindow& window) {
    for (int y = 0; y < GRID_HEIGHT; ++y) {
        for (int x = 0; x < GRID_WIDTH; ++x) {
            if (!cells[y][x].walkable) {
                cells[y][x].shape.setFillColor(Color::White);
            }
            else {
                cells[y][x].shape.setFillColor(Color::Transparent);
            }
            window.draw(cells[y][x].shape);
        }
    }

    //Dessine le chemin suivi par le Dogo en JAUNE
    for (const auto& pos : debugPath) {
        if (pos.x >= 0 && pos.x < GRID_WIDTH && pos.y >= 0 && pos.y < GRID_HEIGHT) {
            cells[pos.y][pos.x].shape.setFillColor(Color::Yellow);
            window.draw(cells[pos.y][pos.x].shape);
        }
    }
}



Cell& Grid::getCell(int x, int y) {
    if (x >= 0 && x < GRID_WIDTH && y >= 0 && y < GRID_HEIGHT) {
        return cells[y][x];
    }
    throw out_of_range("Coordonnées en dehors de la grille");
}

bool Grid::isWalkable(int x, int y) const {
    if (x < 0 || x >= GRID_WIDTH || y < 0 || y >= GRID_HEIGHT) return false;

    // ✅ Vérifie que la cellule est traversable
    if (!cells[y][x].walkable) return false;

    // ✅ Vérifie que la cellule voisine en diagonale ne bloque pas un passage
    bool left = (x > 0) ? cells[y][x - 1].walkable : false;
    bool right = (x < GRID_WIDTH - 1) ? cells[y][x + 1].walkable : false;
    bool top = (y > 0) ? cells[y - 1][x].walkable : false;
    bool bottom = (y < GRID_HEIGHT - 1) ? cells[y + 1][x].walkable : false;

    if (!left && !top) return false;  // Bloqué en haut-gauche ?
    if (!right && !top) return false; // Bloqué en haut-droite ?
    if (!left && !bottom) return false; // Bloqué en bas-gauche ?
    if (!right && !bottom) return false; // Bloqué en bas-droite ?

    return true;
}




vector<Vector2i> Grid::getNeighbors(int x, int y) const {
    vector<Vector2i> neighbors;
    static const Vector2i offsets[] = {
        {0, -1}, {0, 1}, {-1, 0}, {1, 0}, // Haut, Bas, Gauche, Droite
        {-1, -1}, {1, -1}, {-1, 1}, {1, 1} // Diagonales
    };

    for (const auto& offset : offsets) {
        int nx = x + offset.x;
        int ny = y + offset.y;
        if (isWalkable(nx, ny)) {
            neighbors.push_back(Vector2i(nx, ny));
        }
    }
    return neighbors;
}
