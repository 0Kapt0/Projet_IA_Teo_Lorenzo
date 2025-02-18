#include"Cookie.hpp"

Cookie::Cookie(float x, float y) : Entity(x, y, Color::Yellow){
	shape.setPosition(x, y);
}
void Cookie::update(float deltaTime, Grid& grid){}
