#include <iostream>
#include <unistd.h> 
#include <cstdlib>  
#include <ctime>    
#include <vector>

using namespace std;

const int WIDTH = 40;
const int HEIGHT = 20;

vector<vector<int>> grid(HEIGHT, vector<int>(WIDTH, 0));

void clearScreen() {
    cout << "\x1B[2J\x1B[H";
}

void drawTrippy() {
    clearScreen();
    srand(time(0));

    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            int r = rand() % 256;
            int g = rand() % 256;
            int b = rand() % 256;
            int ch = (x + y + (rand() % 3)) % 94 + 33; 
            cout << "\x1B[38;2;" << r << ";" << g << ";" << b << "m";
            cout << static_cast<char>(ch);
        }
        cout << endl;
    }
    cout << "\x1B[0m"; 
}

int main() {
    while (true) {
        drawTrippy();
        usleep(100000); 
    }
    return 0;
}
