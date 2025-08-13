#include <SDL.h>
#include <algorithm>
#include <cmath>
#include <complex>
#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>

using f32 = float;
using cplx = std::complex<f32>;

struct EpicycleTerm {
    int k;
    cplx coef;
    f32 amp;
};

static const int  WIN_W = 1000;
static const int  WIN_H = 700;
static const f32  PI    = 3.14159265358979323846f;

static inline f32 clampf(f32 x, f32 a, f32 b) { return std::max(a, std::min(b, x)); }
static inline SDL_Color lerpColor(SDL_Color a, SDL_Color b, f32 t) {
    SDL_Color c;
    c.r = (Uint8)(a.r + (b.r - a.r) * t);
    c.g = (Uint8)(a.g + (b.g - a.g) * t);
    c.b = (Uint8)(a.b + (b.b - a.b) * t);
    c.a = (Uint8)(a.a + (b.a - a.a) * t);
    return c;
}

std::vector<cplx> make_circle(int N, f32 radius=200.f) {
    std::vector<cplx> pts; pts.reserve(N);
    for (int i = 0; i < N; ++i) {
        f32 t = 2*PI * (f32)i / (f32)N;
        pts.emplace_back(radius * std::cos(t), radius * std::sin(t));
    }
    return pts;
}

std::vector<cplx> make_square(int N, f32 size=350.f) {
    std::vector<cplx> pts; pts.reserve(N);
    f32 half = size * 0.5f;
    for (int i = 0; i < N; ++i) {
        f32 u = (f32)i / (f32)N;
        f32 s = u * 4.f;
        f32 x=0, y=0;
        if      (s < 1.f) { x = -half + s*size; y = -half; }
        else if (s < 2.f) { x =  half; y = -half + (s-1.f)*size; }
        else if (s < 3.f) { x =  half - (s-2.f)*size; y =  half; }
        else              { x = -half; y =  half - (s-3.f)*size; }
        pts.emplace_back(x, y);
    }
    return pts;
}

std::vector<cplx> make_heart(int N, f32 scale=12.f) {
    std::vector<cplx> pts; pts.reserve(N);
    for (int i = 0; i < N; ++i) {
        f32 t = 2*PI * (f32)i / (f32)N;
        f32 x = scale * 16.f * std::pow(std::sin(t), 3.f);
        f32 y = scale * (13.f*std::cos(t) - 5.f*std::cos(2.f*t) - 2.f*std::cos(3.f*t) - std::cos(4.f*t));
        pts.emplace_back(x, y);
    }
    f32 maxr = 1.f;
    for (auto& z: pts) maxr = std::max(maxr, std::abs(z));
    f32 s = 250.f / maxr;
    for (auto& z: pts) z *= s;
    return pts;
}

std::vector<EpicycleTerm> dft(const std::vector<cplx>& z) {
    const int N = (int)z.size();
    std::vector<EpicycleTerm> terms; terms.reserve(N);
    int kmin = -N/2;
    for (int k = kmin; k < kmin+N; ++k) {
        cplx c(0,0);
        for (int n = 0; n < N; ++n) {
            f32 phi = -2.f*PI * (f32)k * (f32)n / (f32)N;
            c += z[n] * std::exp(cplx(0, phi));
        }
        c /= (f32)N;
        terms.push_back({k, c, (f32)std::abs(c)});
    }
    std::sort(terms.begin(), terms.end(), [](const EpicycleTerm& a, const EpicycleTerm& b){
        return a.amp > b.amp;
    });
    return terms;
}

void drawCircle(SDL_Renderer* R, int cx, int cy, int r) {
    int x = r, y = 0, err = 0;
    while (x >= y) {
        SDL_RenderDrawPoint(R, cx + x, cy + y);
        SDL_RenderDrawPoint(R, cx + y, cy + x);
        SDL_RenderDrawPoint(R, cx - y, cy + x);
        SDL_RenderDrawPoint(R, cx - x, cy + y);
        SDL_RenderDrawPoint(R, cx - x, cy - y);
        SDL_RenderDrawPoint(R, cx - y, cy - x);
        SDL_RenderDrawPoint(R, cx + y, cy - x);
        SDL_RenderDrawPoint(R, cx + x, cy - y);
        if (err <= 0) { y++; err += 2*y + 1; }
        if (err > 0)  { x--; err -= 2*x + 1; }
    }
}

void drawLine(SDL_Renderer* R, int x1, int y1, int x2, int y2) {
    SDL_RenderDrawLine(R, x1, y1, x2, y2);
}

int main(int argc, char** argv) {
    (void)argc; (void)argv;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << "\n";
        return 1;
    }

    SDL_Window*  W = SDL_CreateWindow("Fourier Epicycles",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_W, WIN_H, SDL_WINDOW_SHOWN);
    if (!W) { std::cerr << "CreateWindow: " << SDL_GetError() << "\n"; return 1; }

    SDL_Renderer* R = SDL_CreateRenderer(W, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!R) { std::cerr << "CreateRenderer: " << SDL_GetError() << "\n"; return 1; }
    SDL_SetRenderDrawBlendMode(R, SDL_BLENDMODE_BLEND);

    SDL_Texture* trailTex = SDL_CreateTexture(R, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, WIN_W, WIN_H);
    SDL_SetTextureBlendMode(trailTex, SDL_BLENDMODE_BLEND);

    auto loadShape = [&](int which, int N)->std::vector<cplx> {
        switch (which) {
            case 1: return make_circle(N);
            case 2: return make_square(N);
            case 3: return make_heart(N);
            default: return make_circle(N);
        }
    };

    int shapeId = 3;
    int Nsamples = 1024;
    auto samples = loadShape(shapeId, Nsamples);
    auto spectrum = dft(samples);

    int maxHarmonics = 100;
    f32 timeScale    = 1.0f;
    bool paused      = false;
    bool showCircles = true;
    bool keepTrail   = true;

    f32 cx = WIN_W * 0.5f;
    f32 cy = WIN_H * 0.5f;

    SDL_SetRenderTarget(R, trailTex);
    SDL_SetRenderDrawColor(R, 0, 0, 0, 0);
    SDL_RenderClear(R);
    SDL_SetRenderTarget(R, nullptr);

    Uint64 prev = SDL_GetPerformanceCounter();
    f32 t = 0.f;

    bool running = true;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) { running = false; }
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_ESCAPE:
                    case SDLK_q: running = false; break;
                    case SDLK_SPACE: paused = !paused; break;
                    case SDLK_c: showCircles = !showCircles; break;
                    case SDLK_t: keepTrail = !keepTrail; break;
                    case SDLK_r: t = 0.f; break;
                    case SDLK_1: shapeId = 1; samples = loadShape(shapeId, Nsamples); spectrum = dft(samples); t = 0.f; break;
                    case SDLK_2: shapeId = 2; samples = loadShape(shapeId, Nsamples); spectrum = dft(samples); t = 0.f; break;
                    case SDLK_3: shapeId = 3; samples = loadShape(shapeId, Nsamples); spectrum = dft(samples); t = 0.f; break;
                    case SDLK_EQUALS:
                    case SDLK_PLUS:
                    case SDLK_KP_PLUS: maxHarmonics = std::min((int)spectrum.size(), maxHarmonics + 5); break;
                    case SDLK_MINUS:
                    case SDLK_KP_MINUS: maxHarmonics = std::max(5, maxHarmonics - 5); break;
                    case SDLK_LEFT:  timeScale = std::max(0.1f, timeScale * 0.8f); break;
                    case SDLK_RIGHT: timeScale = std::min(5.0f, timeScale * 1.25f); break;
                    default: break;
                }
            }
        }

        Uint64 now = SDL_GetPerformanceCounter();
        f32 dt = (f32)((now - prev) / (double)SDL_GetPerformanceFrequency());
        prev = now;
        if (!paused) {
            t += dt * timeScale;
            t = std::fmod(t, 8.0f);
        }
        f32 phase = std::fmod(t / 8.0f, 1.0f);
        f32 phi = 2.f * PI * phase;

        cplx pos(0,0);
        std::vector<cplx> centers; centers.reserve(maxHarmonics+1);
        centers.push_back(pos);

        int termsUsed = std::min((int)spectrum.size(), maxHarmonics);
        for (int i = 0; i < termsUsed; ++i) {
            const auto& term = spectrum[i];
            cplx rot = std::exp(cplx(0, (f32)term.k * phi));
            cplx contrib = term.coef * rot;
            pos += contrib;
            centers.push_back(pos);
        }

        if (!keepTrail) {
            SDL_SetRenderTarget(R, trailTex);
            SDL_SetRenderDrawColor(R, 0, 0, 0, 0);
            SDL_RenderClear(R);
            SDL_SetRenderTarget(R, nullptr);
        } else {
            SDL_SetRenderTarget(R, trailTex);
            SDL_SetRenderDrawColor(R, 0, 0, 0, 20);
            SDL_Rect full{0,0,WIN_W,WIN_H};
            SDL_SetRenderDrawBlendMode(R, SDL_BLENDMODE_BLEND);
            SDL_RenderFillRect(R, &full);
            SDL_SetRenderTarget(R, nullptr);
        }

        {
            SDL_SetRenderTarget(R, trailTex);
            int px = (int)(cx + pos.real());
            int py = (int)(cy - pos.imag());
            SDL_SetRenderDrawColor(R, 255, 255, 255, 220);
            SDL_Rect dot{px-1, py-1, 3, 3};
            SDL_RenderFillRect(R, &dot);
            SDL_SetRenderTarget(R, nullptr);
        }

        SDL_SetRenderTarget(R, nullptr);
        SDL_SetRenderDrawColor(R, 10, 12, 16, 255);
        SDL_RenderClear(R);
        SDL_RenderCopy(R, trailTex, nullptr, nullptr);

        if (showCircles) {
            SDL_Color c1{180, 90, 255, 255};
            SDL_Color c2{ 20,220,255, 255};

            int lastx = (int)(cx);
            int lasty = (int)(cy);
            for (int i = 0; i < termsUsed; ++i) {
                f32 tcol = (termsUsed <= 1) ? 0.f : (f32)i / (f32)(termsUsed-1);
                SDL_Color col = lerpColor(c1, c2, tcol);

                int radius = (int)std::round(std::abs(spectrum[i].coef));
                int nx = (int)(cx + centers[i+1].real());
                int ny = (int)(cy - centers[i+1].imag());

                SDL_SetRenderDrawColor(R, col.r, col.g, col.b, 140);
                if (radius > 0) drawCircle(R, lastx, lasty, radius);

                SDL_SetRenderDrawColor(R, col.r, col.g, col.b, 220);
                drawLine(R, lastx, lasty, nx, ny);

                lastx = nx; lasty = ny;
            }

            SDL_SetRenderDrawColor(R, 255, 255, 255, 255);
            drawCircle(R, lastx, lasty, 2);
        }

        SDL_RenderPresent(R);
    }

    SDL_DestroyTexture(trailTex);
    SDL_DestroyRenderer(R);
    SDL_DestroyWindow(W);
    SDL_Quit();
    return 0;
}
