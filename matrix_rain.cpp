#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <thread>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
void enable_virtual_terminal() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
#else
void enable_virtual_terminal() {}
#endif

const int WIDTH = 80;
const int HEIGHT = 30;
const char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789@#$%^&*";

char random_char() {
    return charset[rand() % (sizeof(charset) - 1)];
}

void clear_screen() {
    std::cout << "\033[2J\033[H";
}

void matrix_rain() {
    std::vector<int> positions(WIDTH, 0);

    while (true) {
        for (int i = 0; i < WIDTH; ++i) {
            if (positions[i] < HEIGHT && rand() % 10 < 2)
                positions[i]++;

            for (int j = 0; j < positions[i]; ++j) {
                std::cout << "\033[" << j+1 << ";" << i+1 << "H";
                if (j == positions[i] - 1)
                    std::cout << "\033[1;92m" << random_char();
                else
                    std::cout << "\033[32m" << random_char();
            }
        }

        std::cout << std::flush;
        std::this_thread::sleep_for(std::chrono::milliseconds(75));
    }
}

int main() {
    srand(static_cast<unsigned int>(time(0)));
    enable_virtual_terminal();
    clear_screen();
    matrix_rain();
    return 0;
}
