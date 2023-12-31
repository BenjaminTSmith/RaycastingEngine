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
    handTexture.loadFromFile("textures/hand1.png");
    sf::Sprite hand(handTexture);
    hand.scale(2, 2);
    hand.rotate(10);
    hand.setPosition(SCREEN_WIDTH - 250, SCREEN_HEIGHT - 100);

    sf::Texture wallTexture;
    wallTexture.loadFromFile("textures/bricks.jpg");
    sf::Sprite wall(wallTexture);

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
        int FOV = 90;
        std::array<float, SCREEN_WIDTH> vectorArray = {};
        std::vector<int> textureVector = {};
        float screenToFOV = (float) FOV / SCREEN_WIDTH;
        for (int i = 0; i < SCREEN_WIDTH; i++) {
            float rayX = player.getPosition().x + 100 * cos(
                    (-player.getRotation() - (i - SCREEN_WIDTH / 2) * screenToFOV) / 57.295);
            float rayY = player.getPosition().y + -100 * sin(
                    (-player.getRotation() - (i - SCREEN_WIDTH / 2) * screenToFOV) / 57.295);
            sf::Vector2f rayPos(rayX, rayY);
            vectorArray[i] = castRay(player.getPosition(), rayPos, mapArray, window, wall, textureVector);
        }
        render3D(window, vectorArray, wall, textureVector);
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

float castRay(sf::Vector2f startPos, sf::Vector2f endPos, std::array<std::string, 10> &mapArray,
              sf::RenderWindow &window, sf::Sprite &wall, std::vector<int> &textureVector) {

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

                float roundedX = std::round(ray.x / 100);
                float roundedY = std::round(ray.y / 100);

                if (std::abs(ray.x - roundedX * 100) < std::abs(ray.y - roundedY * 100)) {
                    textureVector.push_back((int) ray.y % 100);
                } else {
                    textureVector.push_back((int) ray.x % 100);
                }
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
    }

    // code for displaying circles at end of each ray in 2D mode
    /*sf::CircleShape circle(10.f);
    circle.setOrigin(5.f, 5.f);
    circle.setOutlineColor(sf::Color::Green);
    circle.setOutlineThickness(1.f);
    circle.setFillColor(sf::Color::Transparent);
    circle.setPosition(ray.x, ray.y);
    window.draw(circle);*/

    /*switch (mapArray[(int) (ray.y / 100)][(int) (ray.x / 100)]) {
        case '#':
    }*/

    return sqrtf((ray.x - startPos.x) * (ray.x - startPos.x) + (ray.y - startPos.y) * (ray.y - startPos.y));
}

void render3D(sf::RenderWindow &window, std::array<float, SCREEN_WIDTH> vectorArray, sf::Sprite &wall,
              std::vector<int> &textureVector) {

    int FOV = 90;
    float screenToFOV = (float) FOV / SCREEN_WIDTH;
    sf::RectangleShape sky(sf::Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT));
    sky.setFillColor(sf::Color(113, 188, 225));
    window.draw(sky);

    sf::RectangleShape ground(sf::Vector2f(SCREEN_WIDTH, SCREEN_HEIGHT / 2));
    ground.setFillColor(sf::Color(127, 132, 133));
    ground.setPosition(0, SCREEN_HEIGHT / 2);
    window.draw(ground);
    // number from 0-255 that determines how dark everything is
    float gamma = 10;
    float projectionHeight = SCREEN_HEIGHT / 2 / tan(1.309);

    for (int i = 0; i < vectorArray.size(); i++) {
        float rayProjectionPosition = 0.5f * tan(((i - SCREEN_WIDTH / 2) * screenToFOV) / 57.296) / tan(45 / 57.296);
        float currentColumn = SCREEN_WIDTH - (SCREEN_WIDTH * (0.5f - rayProjectionPosition));
        float nextRayProjectionPosition = 0.5f * tan((((i + 1) - SCREEN_WIDTH / 2) * screenToFOV) / 57.296) / tan(45 / 57.296);
        float nextColumn = SCREEN_WIDTH - (SCREEN_WIDTH * (0.5f - nextRayProjectionPosition));
        /*sf::RectangleShape column(sf::Vector2f(nextColumn - currentColumn + 1, SCREEN_HEIGHT * projectionHeight / (vectorArray[i] * cos((i - 45) / 57.295))));

        column.setOrigin(sf::Vector2f(0.f, SCREEN_HEIGHT * projectionHeight / (cos((i - 45) / 57.295) * vectorArray[i]) * 0.5f));
        column.setFillColor(sf::Color(0, 255 / (gamma * ((vectorArray[i] - 1) / (3999)) + 1), 0));
        column.setPosition(currentColumn, SCREEN_HEIGHT / 2);
        window.draw(column);*/

        wall.setTextureRect(sf::IntRect(textureVector.at(i) * (SCREEN_WIDTH / 100), 0,
                                        nextColumn - currentColumn + 1, SCREEN_WIDTH));
        /*wall.setColor(sf::Color(255 / (gamma * ((vectorArray[i] - 1) / 3999) + 1),
                                255 / (gamma * ((vectorArray[i] - 1) / 3999) + 1),
                                255 / (gamma * ((vectorArray[i] - 1) / 3999 + 1))));*/
        wall.setOrigin(sf::Vector2f(0.f, 320));
        wall.setScale(1, ((SCREEN_HEIGHT * projectionHeight / (vectorArray[i] * cos((
                (i - SCREEN_WIDTH / 2) * screenToFOV) / 57.295)))) / SCREEN_WIDTH);
        wall.setPosition(currentColumn, SCREEN_HEIGHT / 2);
        window.draw(wall);
    }

}
