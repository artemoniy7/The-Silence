#include <raylib.h>

#include <cmath>
#include <string>
#include <vector>

struct Button {
    std::string text;
    Rectangle rect;
    float animOffset = 0.0f;
    float pulse = 0.0f;
};

struct Runner {
    Vector2 pos;
    Vector2 vel;
    float life;
    bool hasEnemy;
    float enemyDelay;
};

int main() {
    const int screenWidth = 1280;
    const int screenHeight = 720;

    InitWindow(screenWidth, screenHeight, "The silence - Menu prototype");
    SetTargetFPS(60);

    std::vector<Button> buttons;
    const float menuLeft = 70.0f;
    const float startY = 220.0f;
    const float width = 280.0f;
    const float height = 72.0f;
    const float gap = 22.0f;

    buttons.push_back({"Levels", {menuLeft, startY, width, height}});
    buttons.push_back({"Settings", {menuLeft, startY + (height + gap), width, height}});
    buttons.push_back({"Exit", {menuLeft, startY + 2.0f * (height + gap), width, height}});

    std::vector<Runner> runners;

    float spawnTimer = 0.0f;
    float ambientTimer = 0.0f;
    bool quitRequested = false;

    while (!WindowShouldClose() && !quitRequested) {
        float dt = GetFrameTime();
        Vector2 mouse = GetMousePosition();

        // Spawn moving light entities in the background.
        spawnTimer -= dt;
        if (spawnTimer <= 0.0f) {
            spawnTimer = GetRandomValue(45, 110) / 100.0f;

            Runner r{};
            r.pos = {-80.0f, static_cast<float>(GetRandomValue(100, screenHeight - 80))};
            float speed = static_cast<float>(GetRandomValue(240, 420));
            r.vel = {speed, static_cast<float>(GetRandomValue(-30, 30))};
            r.life = 4.4f;
            r.hasEnemy = (GetRandomValue(0, 100) < 55);
            r.enemyDelay = GetRandomValue(8, 22) / 100.0f;
            runners.push_back(r);
        }

        for (auto it = runners.begin(); it != runners.end();) {
            it->life -= dt;
            it->pos.x += it->vel.x * dt;
            it->pos.y += it->vel.y * dt;

            if (it->pos.x > screenWidth + 250 || it->life <= 0.0f) {
                it = runners.erase(it);
            } else {
                ++it;
            }
        }

        ambientTimer += dt;

        for (size_t i = 0; i < buttons.size(); ++i) {
            bool hovered = CheckCollisionPointRec(mouse, buttons[i].rect);
            float targetOffset = hovered ? 10.0f : 0.0f;
            buttons[i].animOffset += (targetOffset - buttons[i].animOffset) * (8.0f * dt);

            float targetPulse = hovered ? 1.0f : 0.0f;
            buttons[i].pulse += (targetPulse - buttons[i].pulse) * (6.0f * dt);

            if (hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
                if (i == 2) {
                    quitRequested = true;
                }
            }
        }

        BeginDrawing();

        ClearBackground(Color{7, 7, 9, 255});

        // Soft vignette style stripes to build atmosphere.
        for (int i = 0; i < 18; ++i) {
            float x = static_cast<float>(i) / 18.0f;
            int alpha = 10 + static_cast<int>(14.0f * std::sin(ambientTimer * 0.7f + x * 8.0f));
            DrawRectangle(
                static_cast<int>(x * screenWidth),
                0,
                screenWidth / 18 + 1,
                screenHeight,
                Color{255, 255, 255, static_cast<unsigned char>(alpha > 0 ? alpha : 0)}
            );
        }

        // Background runners: white light and occasional red enemies behind.
        for (const Runner& r : runners) {
            Vector2 tail = {r.pos.x - 120.0f, r.pos.y - r.vel.y * 0.2f};
            DrawLineEx(tail, r.pos, 3.0f, Color{220, 220, 220, 120});
            DrawCircleV(r.pos, 7.0f, Color{255, 255, 255, 220});

            if (r.hasEnemy && r.life < (4.4f - r.enemyDelay)) {
                Vector2 enemyPos = {r.pos.x - 60.0f, r.pos.y + 15.0f};
                Vector2 enemyTail = {enemyPos.x - 80.0f, enemyPos.y};
                DrawLineEx(enemyTail, enemyPos, 2.5f, Color{255, 50, 50, 130});
                DrawCircleV(enemyPos, 6.0f, Color{255, 70, 70, 220});
            }
        }

        // Menu title.
        DrawText("The silence", static_cast<int>(menuLeft), 140, 56, WHITE);
        DrawLine(
            static_cast<int>(menuLeft),
            195,
            static_cast<int>(menuLeft + 310),
            195,
            Color{255, 255, 255, 90}
        );

        for (const Button& b : buttons) {
            Rectangle visual = b.rect;
            visual.x += b.animOffset;

            float pulseGlow = 40.0f * b.pulse;
            DrawRectangleRounded(
                {visual.x - pulseGlow * 0.2f, visual.y - pulseGlow * 0.1f, visual.width + pulseGlow * 0.4f, visual.height + pulseGlow * 0.2f},
                0.18f,
                6,
                Color{255, 255, 255, static_cast<unsigned char>(20 + 35 * b.pulse)}
            );

            DrawRectangleRounded(visual, 0.12f, 6, BLACK);
            DrawRectangleRoundedLinesEx(
                visual,
                0.12f,
                6,
                2.0f,
                Color{255, 255, 255, static_cast<unsigned char>(210 + 40 * b.pulse)}
            );

            int textSize = 30;
            int tw = MeasureText(b.text.c_str(), textSize);
            DrawText(
                b.text.c_str(),
                static_cast<int>(visual.x + visual.width / 2 - tw / 2),
                static_cast<int>(visual.y + visual.height / 2 - textSize / 2),
                textSize,
                WHITE
            );
        }

        DrawText("Prototype menu", 30, screenHeight - 30, 18, Color{255, 255, 255, 80});

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
