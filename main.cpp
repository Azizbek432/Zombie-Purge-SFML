#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>

int main() {
    sf::RenderWindow window(sf::VideoMode({1200, 800}), "Zombies vs Azizbek - Final Edition", sf::Style::Titlebar | sf::Style::Close);
    window.setFramerateLimit(60);

    sf::Texture heroTex, enemyTex, bgTex;
    sf::Image heroImg, enemyImg;

    // --- QAHRAMON YUKLASH ---
    if (heroImg.loadFromFile("assets/hero.png")) {
        heroImg.createMaskFromColor(sf::Color::White);
        // Warningni yo'qotish uchun 'if' ichida tekshiramiz
        if (!heroTex.loadFromImage(heroImg)) {
            std::cerr << "Hero tekstura xatosi!" << std::endl;
        }
    }

    sf::Sprite hero(heroTex);
    hero.setScale({0.8f, 0.8f});
    hero.setPosition({100.f, 400.f});

    // --- ZOMBI YUKLASH ---
    if (enemyImg.loadFromFile("assets/enemy.png")) {
        enemyImg.createMaskFromColor(sf::Color::White);
        if (!enemyTex.loadFromImage(enemyImg)) {
            std::cerr << "Zombi tekstura xatosi!" << std::endl;
        }
    }

    // --- FON ---
    if (!bgTex.loadFromFile("assets/background.png")) {
        std::cerr << "Background topilmadi!" << std::endl;
    }
    bgTex.setRepeated(true);
    sf::Sprite bg(bgTex);
    bg.setTextureRect(sf::IntRect({0, 0}, {1200, 800}));

    std::vector<sf::Sprite> enemies;
    float enemyTimer = 0.f;
    struct Bullet { sf::RectangleShape shape; };
    std::vector<Bullet> bullets;

    int score = 0;
    float bgOffset = 0.f;
    bool gameOver = false;

    sf::Font font;
    sf::Text uiText(font);
    if (font.openFromFile("C:/Windows/Fonts/arial.ttf")) { 
        uiText.setCharacterSize(35);
        uiText.setFillColor(sf::Color::Red);
        uiText.setOutlineColor(sf::Color::Black);
        uiText.setOutlineThickness(2);
        uiText.setPosition({30.f, 30.f});
    }

    sf::Clock clock;

    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();

            if (!gameOver && event->is<sf::Event::KeyPressed>()) {
                if (event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Space) {
                    Bullet b;
                    b.shape.setSize({25.f, 5.f});
                    b.shape.setFillColor(sf::Color::Yellow);
                    b.shape.setPosition({hero.getPosition().x + 90.f, hero.getPosition().y + 45.f});
                    bullets.push_back(b);
                }
            }
        }

        if (!gameOver) {
            bgOffset += 200.f * dt;
            bg.setTextureRect(sf::IntRect({(int)bgOffset, 0}, {1200, 800}));

            float speed = 600.f;
            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) 
                && hero.getPosition().y > 150) hero.move({0.f, -speed * dt});
            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down)) 
                && hero.getPosition().y < 650) hero.move({0.f, speed * dt});

            for (size_t i = 0; i < bullets.size(); ) {
                bullets[i].shape.move({1100.f * dt, 0.f});
                if (bullets[i].shape.getPosition().x > 1200) bullets.erase(bullets.begin() + i);
                else i++;
            }

            enemyTimer += dt;
            if (enemyTimer > std::max(0.4f, 1.2f - (score / 300.f))) {
                sf::Sprite e(enemyTex);
                e.setScale({0.7f, 0.7f});
                e.setPosition({1250.f, (float)(150 + rand() % 500)});
                enemies.push_back(e);
                enemyTimer = 0.f;
            }

            for (size_t i = 0; i < enemies.size(); ) {
                float wobble = std::sin(enemies[i].getPosition().x * 0.03f) * 1.8f;
                enemies[i].move({-(450.f + score / 5.f) * dt, wobble});

                if (hero.getGlobalBounds().findIntersection(enemies[i].getGlobalBounds())) {
                    gameOver = true;
                    break;
                }

                bool zombiHit = false;
                for (size_t j = 0; j < bullets.size(); j++) {
                    if (bullets[j].shape.getGlobalBounds().findIntersection(enemies[i].getGlobalBounds())) {
                        bullets.erase(bullets.begin() + j);
                        zombiHit = true; score += 10; break;
                    }
                }

                if (zombiHit || enemies[i].getPosition().x < -100.f) enemies.erase(enemies.begin() + i);
                else i++;
            }
            uiText.setString("ZOMBIES PURGED: " + std::to_string(score));
        } else {
            uiText.setString("GAME OVER!\nScore: " + std::to_string(score) + "\nPress 'R' to Respawn");
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R)) {
                gameOver = false; score = 0; enemies.clear(); bullets.clear();
                hero.setPosition({100.f, 400.f});
            }
        }

        window.clear();
        window.draw(bg);
        window.draw(hero);
        for (auto& b : bullets) window.draw(b.shape);
        for (auto& e : enemies) window.draw(e);
        window.draw(uiText);
        window.display();
    }
    return 0;
}