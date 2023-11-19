#include <iostream>
#include <SFML/Graphics.hpp>
#include "functions.h"
#include <array>
#include <cmath>
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 360

int main() {
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "RayCasting");

    std::array<std::string, 10> mapArray = {
            "#################################",
            "#...............................#",
            "#...######################......#",
            "#...#....................#......#",
            "#........................#......#",
            "#...#....................#......#",
            "#...######################......#",
            "#...............................#",
            "#...............................#",
            "#################################",
    };

    sf::Texture playerTexture;
    playerTexture.loadFromFile("textures/player.png");

    sf::Sprite player(playerTexture);
    player.setPosition(151, 151);
    player.setOrigin(25.f, 25.f);

    sf::Texture handTexture;
    handTexture.loadFromFile("textures/hand2.png");
    sf::Sprite hand(handTexture);
    hand.scale(2, 2);
    hand.setPosition(SCREEN_WIDTH - 250, SCREEN_HEIGHT - 85);

    while (window.isOpen()) {

        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
                case (sf::Event::Closed):
                    window.close();
                    break;
            }
        }
        //-------------input handling----------------------
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            player.rotate(-0.12f);

        } if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            player.rotate(0.12f);

        } if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            float x = 0.3 * cos(-player.getRotation() / 57.295);
            float y = -0.3 * sin(-player.getRotation() / 57.295);

            sf::Vector2f newPos = player.getPosition() + sf::Vector2f(x, y);

            if (x > 0) {
                if (mapArray[floor(player.getPosition().y / 100)][floor((newPos.x + 25) / 100)] == '.') {
                    player.move(x, 0);
                }
            } else {
                if (mapArray[floor(player.getPosition().y / 100)][floor((newPos.x - 25) / 100)] == '.') {
                    player.move(x, 0);
                }
            }

            if (y > 0){
                if (mapArray[floor((newPos.y + 25) / 100)][floor(player.getPosition().x / 100)] == '.') {
                    player.move(0, y);
                }
            } else {
                if (mapArray[floor((newPos.y - 25) / 100)][floor(player.getPosition().x / 100)] == '.') {
                    player.move(0, y);
                }
            }
        } if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            float x = 0.25f * cosf(-player.getRotation() / 57.295);
            float y = -0.25f * sinf(-player.getRotation() / 57.295);

            sf::Vector2f newPos = player.getPosition() + sf::Vector2f(-x, -y);

            if (-x > 0) {
                if (mapArray[floor(player.getPosition().y / 100)][floor((newPos.x + 25) / 100)] == '.') {
                    player.move(-x, 0);
                }
            } else {
                if (mapArray[floor(player.getPosition().y / 100)][floor((newPos.x - 25) / 100)] == '.') {
                    player.move(-x, 0);
                }
            }

            if (-y > 0){
                if (mapArray[floor((newPos.y + 25) / 100)][floor(player.getPosition().x / 100)] == '.') {
                    player.move(0, -y);
                }
            } else {
                if (mapArray[floor((newPos.y - 25) / 100)][floor(player.getPosition().x / 100)] == '.') {
                    player.move(0, -y);
                }
            }

        }

        window.clear(sf::Color::Black);
        // drawMap(window, mapArray);
        // window.draw(player);
        const int fov = 90;
        std::array<float, fov> vectorArray = {};
        for (int i = -fov / 2; i < fov / 2; i++) {
            float rayX = player.getPosition().x + 100 * cos((-player.getRotation() - i) / 57.295);
            float rayY = player.getPosition().y + -100 * sin((-player.getRotation() - i) / 57.295);
            sf::Vector2f rayPos(rayX, rayY);
            // fish-eye effect
            vectorArray[i + 45] = castRay(player.getPosition(), rayPos, mapArray, window);
            // non fish-eye effect

        }
        render3D(window, vectorArray);
        window.draw(hand);
        // animateHand(hand);
        window.display();
    }


    return 0;
}


void drawMap(sf::RenderWindow &window, std::array<std::string, 10> &mapArray) {
    // creates the two-dimensional rendering of the map
    sf::Texture mapTexture;
    mapTexture.loadFromFile("textures/brick_tiles.png");

    sf::Sprite map(mapTexture);
    map.scale(3.125, 3.125);

    for (int i = 0; i < mapArray.size(); i++) {
        for (int j = 0; j < mapArray[i].size(); j++) {
            if (mapArray[i][j] == '#') {
                map.setTextureRect(sf::IntRect(1, 1, 32, 32));
                map.setPosition(j * 100, i * 100);
                window.draw(map);
            }
        }
    }

}

float castRay(sf::Vector2f startPos, sf::Vector2f endPos, std::array<std::string, 10> &mapArray, sf::RenderWindow &window) {

    float dx = endPos.x - startPos.x;
    float dy = endPos.y - startPos.y;

    float xDist = sqrtf(1 + (dy / dx) * (dy / dx));
    float yDist = sqrtf(1 + (dx / dy) * (dx / dy));
    int xStep;
    int yStep;

    sf::Vector2f ray(startPos.x, startPos.y);

    if (dx < 0) {
        xStep = -1;
    } else {
        xStep = 1;
    }
    if (dy < 0) {
        yStep = -1;
    } else {
        yStep = 1;
    }

    bool found = false;
    while (!found) {
        if ((int) (ray.y / 100) <= mapArray.size() && (int) (ray.x / 100) <= mapArray[(int) ray.y / 100].size()) {
            if (mapArray[(int) (ray.y / 100)][(int) (ray.x / 100)] != '.') {
                found = true;
            }
        }

        if (xDist < yDist) {
            ray.x += xStep;
            ray.y += xStep * (dy / dx);
        } else {
            ray.x += yStep * (dx / dy);
            ray.y += yStep;
        }
        if (sqrtf((ray.x - startPos.x) * (ray.x - startPos.x) + (ray.y - startPos.y) * (ray.y - startPos.y)) > 4000) {
            found = true;
        }
    }

    /*sf::CircleShape circle(10.f);
    circle.setOrigin(5.f, 5.f);
    circle.setOutlineColor(sf::Color::Green);
    circle.setOutlineThickness(1.f);
    circle.setFillColor(sf::Color::Transparent);
    circle.setPosition(ray.x, ray.y);
    window.draw(circle);*/

    return sqrtf((ray.x - startPos.x) * (ray.x - startPos.x) + (ray.y - startPos.y) * (ray.y - startPos.y));
}

void render3D(sf::RenderWindow &window, std::array<float, 90> vectorArray) {

    sf::RectangleShape ground(sf::Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT / 2));
    ground.setFillColor(sf::Color(10, 0, 40));
    ground.setPosition(0, SCREEN_HEIGHT / 2);
    window.draw(ground);

    float projectionHeight = 20000;

    for (int i = 0; i < vectorArray.size(); i++) {
        sf::RectangleShape column(sf::Vector2f(7.111f, projectionHeight / vectorArray[i]));
        column.setOrigin(sf::Vector2f(0.5f * 7.111f, projectionHeight / vectorArray[i] * 0.5f));
        column.setFillColor(sf::Color(0, 255 * (1 / (vectorArray[i] * 0.05f)), 0));
        column.setPosition((float) i * 7.111f, SCREEN_HEIGHT / 2);
        window.draw(column);
    }

}

void animateHand(sf::Sprite &hand) {

}
