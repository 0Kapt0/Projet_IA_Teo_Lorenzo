#include"Cookie.hpp"

Cookie::Cookie(float x, float y) : Entity(x, y, Color::Yellow){
	shape.setPosition(x, y);
	shape.setScale(0.5, 0.5);
}
void Cookie::update(float deltaTime, Grid& grid){}
