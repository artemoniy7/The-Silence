#include <SFML/Graphics.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <string>
#include <vector>
#include <ctime>
#include <cstdint>

struct Button {
    std::string text;
    sf::FloatRect rect;
    float animOffset = 0.0f;
    float pulse = 0.0f;
};

struct Runner {
    sf::Vector2f pos;
    sf::Vector2f vel;
    float life = 0.0f;
    bool hasEnemy = false;
    float enemyDelay = 0.0f;
};

float randomRange(float minValue, float maxValue) {
    return minValue + static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX) * (maxValue - minValue);
}

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    constexpr unsigned int screenWidth = 1280;
    constexpr unsigned int screenHeight = 720;

    sf::RenderWindow window(sf::VideoMode(screenWidth, screenHeight), "The silence - Menu prototype (SFML)");
    window.setFramerateLimit(60);

    sf::Font font;
    std::array<std::string, 4> fontCandidates = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
        "/usr/share/fonts/truetype/liberation2/LiberationSans-Regular.ttf",
        "/usr/share/fonts/truetype/freefont/FreeSans.ttf",
        "C:/Windows/Fonts/arial.ttf"
    };

    bool fontLoaded = false;
    for (const auto& path : fontCandidates) {
        if (font.openFromFile(path)) {
            fontLoaded = true;
            break;
        }
    }

    const float menuLeft = 70.0f;
    const float startY = 220.0f;
    const float width = 280.0f;
    const float height = 72.0f;
    const float gap = 22.0f;

    std::vector<Button> buttons = {
        {"Levels", {menuLeft, startY, width, height}},
        {"Settings", {menuLeft, startY + (height + gap), width, height}},
        {"Exit", {menuLeft, startY + 2.0f * (height + gap), width, height}}
    };

    std::vector<Runner> runners;
    float spawnTimer = 0.0f;
    float ambientTimer = 0.0f;

    sf::Clock frameClock;

    while (window.isOpen()) {
        float dt = frameClock.restart().asSeconds();
        dt = std::min(dt, 0.033f);

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                const sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                for (size_t i = 0; i < buttons.size(); ++i) {
                    if (buttons[i].rect.contains(mouse) && i == 2) {
                        window.close();
                    }
                }
            }
        }

        spawnTimer -= dt;
        if (spawnTimer <= 0.0f) {
            spawnTimer = randomRange(0.45f, 1.1f);

            Runner r;
            r.pos = {-80.0f, randomRange(100.0f, static_cast<float>(screenHeight - 80))};
            const float speed = randomRange(240.0f, 420.0f);
            r.vel = {speed, randomRange(-30.0f, 30.0f)};
            r.life = 4.4f;
            r.hasEnemy = (randomRange(0.0f, 1.0f) < 0.55f);
            r.enemyDelay = randomRange(0.08f, 0.22f);
            runners.push_back(r);
        }

        for (auto it = runners.begin(); it != runners.end();) {
            it->life -= dt;
            it->pos += it->vel * dt;
            if (it->pos.x > static_cast<float>(screenWidth + 250) || it->life <= 0.0f) {
                it = runners.erase(it);
            } else {
                ++it;
            }
        }

        ambientTimer += dt;

        const sf::Vector2f mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
        for (auto& b : buttons) {
            const bool hovered = b.rect.contains(mouse);
            const float targetOffset = hovered ? 10.0f : 0.0f;
            b.animOffset += (targetOffset - b.animOffset) * (8.0f * dt);

            const float targetPulse = hovered ? 1.0f : 0.0f;
            b.pulse += (targetPulse - b.pulse) * (6.0f * dt);
        }

        window.clear(sf::Color(7, 7, 9));

        for (int i = 0; i < 18; ++i) {
            float x = static_cast<float>(i) / 18.0f;
            int alpha = 10 + static_cast<int>(14.0f * std::sin(ambientTimer * 0.7f + x * 8.0f));
            alpha = std::max(alpha, 0);

            sf::RectangleShape stripe(sf::Vector2f(static_cast<float>(screenWidth) / 18.0f + 1.0f, static_cast<float>(screenHeight)));
            stripe.setPosition({x * screenWidth, 0.0f});
            stripe.setFillColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(alpha)));
            window.draw(stripe);
        }

        for (const Runner& r : runners) {
            sf::VertexArray beam(sf::Lines, 2);
            beam[0].position = {r.pos.x - 120.0f, r.pos.y - r.vel.y * 0.2f};
            beam[1].position = r.pos;
            beam[0].color = sf::Color(220, 220, 220, 120);
            beam[1].color = sf::Color(255, 255, 255, 200);
            window.draw(beam);

            sf::CircleShape head(7.0f);
            head.setOrigin(7.0f, 7.0f);
            head.setPosition(r.pos);
            head.setFillColor(sf::Color(255, 255, 255, 220));
            window.draw(head);

            if (r.hasEnemy && r.life < (4.4f - r.enemyDelay)) {
                const sf::Vector2f enemyPos = {r.pos.x - 60.0f, r.pos.y + 15.0f};

                sf::VertexArray enemyBeam(sf::Lines, 2);
                enemyBeam[0].position = {enemyPos.x - 80.0f, enemyPos.y};
                enemyBeam[1].position = enemyPos;
                enemyBeam[0].color = sf::Color(255, 50, 50, 130);
                enemyBeam[1].color = sf::Color(255, 70, 70, 220);
                window.draw(enemyBeam);

                sf::CircleShape enemyHead(6.0f);
                enemyHead.setOrigin(6.0f, 6.0f);
                enemyHead.setPosition(enemyPos);
                enemyHead.setFillColor(sf::Color(255, 70, 70, 220));
                window.draw(enemyHead);
            }
        }

        if (fontLoaded) {
            sf::Text title(font, "The silence", 56);
            title.setPosition({menuLeft, 120.0f});
            title.setFillColor(sf::Color::White);
            window.draw(title);
        }

        sf::RectangleShape divider(sf::Vector2f(310.0f, 2.0f));
        divider.setPosition({menuLeft, 195.0f});
        divider.setFillColor(sf::Color(255, 255, 255, 90));
        window.draw(divider);

        for (const Button& b : buttons) {
            const float x = b.rect.position.x + b.animOffset;
            const float y = b.rect.position.y;

            const float pulseGlow = 40.0f * b.pulse;
            sf::RectangleShape glow({b.rect.size.x + pulseGlow * 0.4f, b.rect.size.y + pulseGlow * 0.2f});
            glow.setPosition({x - pulseGlow * 0.2f, y - pulseGlow * 0.1f});
            glow.setFillColor(sf::Color(255, 255, 255, static_cast<std::uint8_t>(20 + 35 * b.pulse)));
            window.draw(glow);

            sf::RectangleShape body({b.rect.size.x, b.rect.size.y});
            body.setPosition({x, y});
            body.setFillColor(sf::Color::Black);
            body.setOutlineThickness(2.0f);
            body.setOutlineColor(sf::Color(static_cast<std::uint8_t>(210 + 40 * b.pulse), static_cast<std::uint8_t>(210 + 40 * b.pulse), static_cast<std::uint8_t>(210 + 40 * b.pulse)));
            window.draw(body);

            if (fontLoaded) {
                sf::Text text(font, b.text, 30);
                text.setFillColor(sf::Color::White);
                sf::FloatRect bounds = text.getLocalBounds();
                text.setPosition({
                    x + (b.rect.size.x - bounds.size.x) * 0.5f,
                    y + (b.rect.size.y - bounds.size.y) * 0.5f - 8.0f
                });
                window.draw(text);
            }
        }

        window.display();
    }

    return 0;
}
