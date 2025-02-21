#include"Cookie.hpp"

Cookie::Cookie(float x, float y) : Entity(x, y, Color::Yellow){
	if (!cookietexture.loadFromFile("assets/texture/Cookie.png")) {
		cerr << "Erreur chargement cookie !" << endl;
	}
	shape.setTexture(&cookietexture);
	shape.setPosition(x, y);
	shape.setScale(0.5, 0.5);
}
void Cookie::update(float deltaTime, Grid& grid){}
