#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

using namespace std;

const int WIDTH = 120;
const int HEIGHT = 40;

int count_neighbors(const vector<vector<int>>& grid, int x, int y) {
    int count = 0;

    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {

            if (dx == 0 && dy == 0)
                continue;

            int nx = x + dx;
            int ny = y + dy;

            if (nx >= 0 && nx < WIDTH && ny >= 0 && ny < HEIGHT)
                count += grid[ny][nx];
        }
    }

    return count;
}

int main() {

    srand(time(nullptr));

    vector<vector<int>> grid(HEIGHT, vector<int>(WIDTH));
    vector<vector<int>> next(HEIGHT, vector<int>(WIDTH));

    for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++)
            grid[y][x] = rand() % 2;

    cout << "\x1b[2J";

    while (true) {

        cout << "\x1b[H";

        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {
                cout << (grid[y][x] ? '#' : ' ');
            }
            cout << "\n";
        }

        for (int y = 0; y < HEIGHT; y++) {
            for (int x = 0; x < WIDTH; x++) {

                int neighbors = count_neighbors(grid, x, y);

                if (grid[y][x] == 1) {
                    next[y][x] = (neighbors == 2 || neighbors == 3);
                } else {
                    next[y][x] = (neighbors == 3);
                }
            }
        }

        grid.swap(next);

        this_thread::sleep_for(chrono::milliseconds(80));
    }

    return 0;
}