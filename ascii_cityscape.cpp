#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

using namespace std;

const int WIDTH = 120;
const int HEIGHT = 40;

struct Building {
    int x;
    int width;
    int height;
};

void clear_screen() {
    cout << "\x1b[2J";
}

void move_cursor_home() {
    cout << "\x1b[H";
}

int rand_range(int a, int b) {
    return a + rand() % (b - a + 1);
}

int main() {
    srand(time(nullptr));

    vector<Building> buildings;

    int x = 0;

    while (x < WIDTH * 2) {
        Building b;
        b.x = x;
        b.width = rand_range(5, 12);
        b.height = rand_range(10, HEIGHT - 5);

        buildings.push_back(b);

        x += b.width + rand_range(2, 6);
    }

    int offset = 0;

    clear_screen();

    while (true) {

        move_cursor_home();

        vector<string> screen(HEIGHT, string(WIDTH, ' '));

        for (auto &b : buildings) {

            int bx = b.x - offset;

            for (int x = 0; x < b.width; x++) {

                int screen_x = bx + x;

                if (screen_x < 0 || screen_x >= WIDTH)
                    continue;

                for (int y = 0; y < b.height; y++) {

                    int screen_y = HEIGHT - 1 - y;

                    if (screen_y < 0)
                        continue;

                    char c = '#';

                    if (y % 3 == 1 && x % 3 == 1) {
                        c = (rand() % 4 == 0) ? '*' : ' ';
                    }

                    screen[screen_y][screen_x] = c;
                }
            }
        }

        for (auto &row : screen)
            cout << row << "\n";

        offset++;

        if (offset > WIDTH)
            offset = 0;

        this_thread::sleep_for(chrono::milliseconds(120));
    }

    return 0;
}