#ifndef RAYCASTING_FUNCTIONS_H
#define RAYCASTING_FUNCTIONS_H

void drawMap(sf::RenderWindow &window, std::array<std::string, 10> &mapArray);

float castRay(sf::Vector2f startPos, sf::Vector2f endPos, std::array<std::string,10> &mapArray, sf::RenderWindow &window);

void render3D(sf::RenderWindow &window, std::array<float, 90> vectorArray);

void animateHand(sf::Sprite &hand);

#endif //RAYCASTING_FUNCTIONS_H
