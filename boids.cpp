#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <random>
#include <thread>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;

    Vec2 operator+(const Vec2& other) const {
        return {x + other.x, y + other.y};
    }

    Vec2 operator-(const Vec2& other) const {
        return {x - other.x, y - other.y};
    }

    Vec2 operator*(float scalar) const {
        return {x * scalar, y * scalar};
    }

    Vec2 operator/(float scalar) const {
        return {x / scalar, y / scalar};
    }

    Vec2& operator+=(const Vec2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
};

struct Boid {
    Vec2 position;
    Vec2 velocity;
};

float magnitude(const Vec2& v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}

Vec2 normalize(const Vec2& v) {
    float mag = magnitude(v);
    if (mag == 0.0f) return {0.0f, 0.0f};
    return v / mag;
}

Vec2 limit(const Vec2& v, float maxVal) {
    float mag = magnitude(v);
    if (mag > maxVal) {
        return normalize(v) * maxVal;
    }
    return v;
}

int main() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif

    const int WIDTH = 100;
    const int HEIGHT = 30;
    const int NUM_BOIDS = 100;

    const float MAX_SPEED = 1.2f;
    const float MAX_FORCE = 0.03f;

    const float NEIGHBOR_RADIUS = 8.0f;
    const float SEPARATION_RADIUS = 3.0f;

    const float ALIGNMENT_WEIGHT = 1.0f;
    const float COHESION_WEIGHT = 0.8f;
    const float SEPARATION_WEIGHT = 1.5f;

    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<float> posX(0, WIDTH - 1);
    std::uniform_real_distribution<float> posY(0, HEIGHT - 1);
    std::uniform_real_distribution<float> vel(-1.0f, 1.0f);

    std::vector<Boid> boids;

    for (int i = 0; i < NUM_BOIDS; i++) {
        boids.push_back({
            {posX(gen), posY(gen)},
            {vel(gen), vel(gen)}
        });
    }

    while (true) {
        std::vector<std::string> screen(
            HEIGHT,
            std::string(WIDTH, ' ')
        );

        std::vector<Vec2> accelerations(NUM_BOIDS);

        for (int i = 0; i < NUM_BOIDS; i++) {
            Vec2 alignment{0, 0};
            Vec2 cohesion{0, 0};
            Vec2 separation{0, 0};

            int totalNeighbors = 0;
            int separationNeighbors = 0;

            for (int j = 0; j < NUM_BOIDS; j++) {
                if (i == j) continue;

                Vec2 diff =
                    boids[j].position - boids[i].position;

                float distance = magnitude(diff);

                if (distance < NEIGHBOR_RADIUS) {
                    alignment += boids[j].velocity;
                    cohesion += boids[j].position;
                    totalNeighbors++;
                }

                if (distance < SEPARATION_RADIUS &&
                    distance > 0.0f) {
                    separation +=
                        (boids[i].position -
                         boids[j].position) /
                        distance;
                    separationNeighbors++;
                }
            }

            Vec2 steering{0, 0};

            // Alignment
            if (totalNeighbors > 0) {
                alignment =
                    alignment / totalNeighbors;
                alignment =
                    normalize(alignment) *
                    MAX_SPEED;

                alignment =
                    alignment -
                    boids[i].velocity;

                alignment =
                    limit(alignment,
                          MAX_FORCE);

                steering +=
                    alignment *
                    ALIGNMENT_WEIGHT;
            }

            // Cohesion
            if (totalNeighbors > 0) {
                cohesion =
                    cohesion / totalNeighbors;

                cohesion =
                    cohesion -
                    boids[i].position;

                cohesion =
                    normalize(cohesion) *
                    MAX_SPEED;

                cohesion =
                    cohesion -
                    boids[i].velocity;

                cohesion =
                    limit(cohesion,
                          MAX_FORCE);

                steering +=
                    cohesion *
                    COHESION_WEIGHT;
            }

            // Separation
            if (separationNeighbors > 0) {
                separation =
                    separation /
                    separationNeighbors;

                separation =
                    normalize(separation) *
                    MAX_SPEED;

                separation =
                    separation -
                    boids[i].velocity;

                separation =
                    limit(separation,
                          MAX_FORCE);

                steering +=
                    separation *
                    SEPARATION_WEIGHT;
            }

            accelerations[i] = steering;
        }

        for (int i = 0; i < NUM_BOIDS; i++) {
            boids[i].velocity +=
                accelerations[i];

            boids[i].velocity =
                limit(
                    boids[i].velocity,
                    MAX_SPEED
                );

            boids[i].position +=
                boids[i].velocity;

            // Wraparound
            if (boids[i].position.x < 0)
                boids[i].position.x += WIDTH;
            if (boids[i].position.x >= WIDTH)
                boids[i].position.x -= WIDTH;

            if (boids[i].position.y < 0)
                boids[i].position.y += HEIGHT;
            if (boids[i].position.y >= HEIGHT)
                boids[i].position.y -= HEIGHT;

            int x =
                static_cast<int>(
                    boids[i].position.x
                );

            int y =
                static_cast<int>(
                    boids[i].position.y
                );

            if (x >= 0 &&
                x < WIDTH &&
                y >= 0 &&
                y < HEIGHT) {
                screen[y][x] = '*';
            }
        }

#ifdef _WIN32
        HANDLE hOut =
            GetStdHandle(STD_OUTPUT_HANDLE);
        COORD coord = {0, 0};
        SetConsoleCursorPosition(
            hOut,
            coord
        );
#else
        std::cout << "\033[H";
#endif

        for (const auto& row : screen) {
            std::cout << row << '\n';
        }

        std::cout
            << "Boids Simulation "
            << "(Ctrl+C to quit)\n";

        std::this_thread::sleep_for(
            std::chrono::milliseconds(33)
        );
    }

    return 0;
}

