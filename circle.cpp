#include <iostream>
#include <cmath>
#include <unistd.h> 
#include <ctime>    

using namespace std;

const int WIDTH = 40;
const int HEIGHT = 20;
const float PI = 3.14159265;

void clearScreen() {
    cout << "\x1B[2J\x1B[H";
}

void drawCircle(int radius, float phase) {
    clearScreen();
    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            int dx = x - WIDTH/2;
            int dy = y - HEIGHT/2;
            float dist = sqrt(dx*dx + dy*dy);
            float pulse = sin(phase) * 0.5 + 0.5; // 0..1
            int r = 128 + 127 * sin(phase + dist * 0.2);
            int g = 128 + 127 * sin(phase * 1.3 + dist * 0.15);
            int b = 128 + 127 * sin(phase * 0.7 + dist * 0.25);
            if (dist < radius * pulse) {
                cout << "\x1B[48;2;" << r << ";" << g << ";" << b << "m ";
            } else {
                cout << "\x1B[0m ";
            }
        }
        cout << "\x1B[0m" << endl;
    }
}

int main() {
    float phase = 0.0;
    while (true) {
        drawCircle(10, phase);
        phase += 0.1;
        usleep(50000);
    }
    return 0;
}
