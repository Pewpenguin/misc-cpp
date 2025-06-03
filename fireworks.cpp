#include <iostream>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

#ifdef _WIN32
#include <windows.h>
void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}
#else
void setColor(int color) {
    std::cout << "\033[" << (30 + color) << "m";
}
#endif

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    std::cout << "\033[2J\033[1;1H";
#endif
}

int main() {
    srand((unsigned)time(nullptr));
    const int width = 40;
    const int height = 20;

    for (int frame = 0; frame < 30; frame++) {
        clearScreen();

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                int chance = rand() % 100;
                if (chance < frame * 3) {
                    int color = 1 + rand() % 6;
                    setColor(color);
                    std::cout << "*";
                } else {
                    std::cout << " ";
                }
            }
            std::cout << "\n";
        }

        setColor(7);
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }

    std::cout << "Boom! Fireworks done.\n";
    return 0;
}
