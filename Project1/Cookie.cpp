#include"Cookie.hpp"

Cookie::Cookie(float x, float y, Texture cookietexture) : Entity(x, y, Color::Yellow){

	shape.setTexture(&cookietexture);
	shape.setPosition(x, y);
	shape.setScale(0.5, 0.5);
}
void Cookie::update(float deltaTime, Grid& grid){}
