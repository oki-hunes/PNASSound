/**
 * 40Hz Auditory Stimulation Generator
 * 
 * Based on PNAS paper: "Long-term effects of forty-hertz auditory stimulation 
 * as a treatment of Alzheimer's disease"
 * https://www.pnas.org/doi/10.1073/pnas.2529565123
 * 
 * Specifications from the paper:
 * - 1kHz pure tone
 * - 1ms duration per tone
 * - 40Hz frequency (one sound every 25ms)
 * - 60dB intensity
 */

#include <SDL2/SDL.h>
#include <cmath>
#include <iostream>
#include <atomic>
#include <chrono>
#include <sstream>
#include <iomanip>

// Audio parameters
constexpr int SAMPLE_RATE = 44100;           // Standard audio sample rate
constexpr int TONE_FREQUENCY = 1000;         // 1kHz pure tone
constexpr double TONE_DURATION_MS = 1.0;     // 1ms tone duration
constexpr double STIMULUS_INTERVAL_MS = 25.0; // 25ms interval (40Hz)
constexpr double AMPLITUDE = 0.5;            // Volume (0.0 - 1.0)

// Window parameters
constexpr int WINDOW_WIDTH = 400;
constexpr int WINDOW_HEIGHT = 200;

// Derived constants
constexpr int SAMPLES_PER_TONE = static_cast<int>(SAMPLE_RATE * TONE_DURATION_MS / 1000.0);
constexpr int SAMPLES_PER_INTERVAL = static_cast<int>(SAMPLE_RATE * STIMULUS_INTERVAL_MS / 1000.0);

// Global state
std::atomic<bool> g_isPlaying{true};
std::atomic<int> g_samplePosition{0};
std::atomic<bool> g_continuousTone{false};  // For testing: continuous 1kHz tone

/**
 * Generate a single sample of the 40Hz stimulus pattern
 */
float generateSample(int position) {
    double t = static_cast<double>(position) / SAMPLE_RATE;
    
    // Continuous tone mode for testing
    if (g_continuousTone.load()) {
        return static_cast<float>(AMPLITUDE * std::sin(2.0 * M_PI * TONE_FREQUENCY * t));
    }
    
    // Position within current 25ms interval
    int posInInterval = position % SAMPLES_PER_INTERVAL;
    
    // Only generate tone for first 1ms of each 25ms interval
    if (posInInterval < SAMPLES_PER_TONE) {
        // Generate 1kHz sine wave
        double tLocal = static_cast<double>(posInInterval) / SAMPLE_RATE;
        double sample = AMPLITUDE * std::sin(2.0 * M_PI * TONE_FREQUENCY * tLocal);
        
        // Apply envelope to avoid clicks (short fade in/out)
        int fadeLength = SAMPLES_PER_TONE / 4;
        if (posInInterval < fadeLength) {
            sample *= static_cast<double>(posInInterval) / fadeLength;
        } else if (posInInterval > SAMPLES_PER_TONE - fadeLength) {
            sample *= static_cast<double>(SAMPLES_PER_TONE - posInInterval) / fadeLength;
        }
        
        return static_cast<float>(sample);
    }
    
    return 0.0f; // Silence between tones
}

/**
 * SDL audio callback function
 */
void audioCallback(void* /*userdata*/, Uint8* stream, int len) {
    float* buffer = reinterpret_cast<float*>(stream);
    int samples = len / sizeof(float);
    
    int pos = g_samplePosition.load();
    
    for (int i = 0; i < samples; ++i) {
        if (g_isPlaying.load()) {
            buffer[i] = generateSample(pos + i);
        } else {
            buffer[i] = 0.0f;
        }
    }
    
    g_samplePosition.store(pos + samples);
}

/**
 * Draw a filled rectangle
 */
void drawRect(SDL_Renderer* renderer, int x, int y, int w, int h, Uint8 r, Uint8 g, Uint8 b) {
    SDL_SetRenderDrawColor(renderer, r, g, b, 255);
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(renderer, &rect);
}

/**
 * Draw visual feedback for audio pulses
 */
void drawPulseIndicator(SDL_Renderer* renderer, int samplePos) {
    int posInInterval = samplePos % SAMPLES_PER_INTERVAL;
    bool isPulsing = posInInterval < SAMPLES_PER_TONE;
    
    // Pulse indicator circle (simulated with rectangles)
    int centerX = WINDOW_WIDTH / 2;
    int centerY = 80;
    int baseSize = 30;
    int size = isPulsing ? baseSize + 15 : baseSize;
    
    if (g_isPlaying.load()) {
        if (g_continuousTone.load()) {
            // Blue for continuous tone
            drawRect(renderer, centerX - size, centerY - size, size * 2, size * 2, 50, 150, 255);
        } else if (isPulsing) {
            // Bright green when pulsing
            drawRect(renderer, centerX - size, centerY - size, size * 2, size * 2, 0, 255, 100);
        } else {
            // Dim green between pulses
            drawRect(renderer, centerX - size, centerY - size, size * 2, size * 2, 0, 100, 50);
        }
    } else {
        // Gray when paused
        drawRect(renderer, centerX - size, centerY - size, size * 2, size * 2, 80, 80, 80);
    }
}

/**
 * Draw status indicators
 */
void drawStatus(SDL_Renderer* renderer, int elapsedSeconds) {
    // Status bar background
    drawRect(renderer, 0, WINDOW_HEIGHT - 50, WINDOW_WIDTH, 50, 40, 40, 40);
    
    // Play/Pause indicator
    if (g_isPlaying.load()) {
        // Green play indicator
        drawRect(renderer, 20, WINDOW_HEIGHT - 35, 20, 20, 0, 200, 0);
    } else {
        // Red pause indicator (two bars)
        drawRect(renderer, 20, WINDOW_HEIGHT - 35, 8, 20, 200, 50, 50);
        drawRect(renderer, 32, WINDOW_HEIGHT - 35, 8, 20, 200, 50, 50);
    }
    
    // Mode indicator
    if (g_continuousTone.load()) {
        // Blue for test mode
        drawRect(renderer, 60, WINDOW_HEIGHT - 35, 60, 20, 50, 100, 200);
    } else {
        // Green for normal 40Hz mode
        drawRect(renderer, 60, WINDOW_HEIGHT - 35, 60, 20, 0, 150, 100);
    }
    
    // Time bar (progress visualization)
    int minutes = elapsedSeconds / 60;
    int barWidth = (minutes % 60) * (WINDOW_WIDTH - 160) / 60;
    drawRect(renderer, 140, WINDOW_HEIGHT - 35, WINDOW_WIDTH - 160, 20, 60, 60, 60);
    drawRect(renderer, 140, WINDOW_HEIGHT - 35, barWidth, 20, 100, 180, 100);
}

/**
 * Draw key hints
 */
void drawKeyHints(SDL_Renderer* renderer) {
    // Key hint boxes at the top
    // SPACE key
    drawRect(renderer, 20, 150, 80, 25, 60, 60, 60);
    // T key
    drawRect(renderer, 110, 150, 80, 25, 60, 60, 60);
    // Q key
    drawRect(renderer, 200, 150, 80, 25, 60, 60, 60);
}

void printInfo() {
    std::cout << "========================================\n";
    std::cout << "  40Hz Auditory Stimulation Generator\n";
    std::cout << "========================================\n";
    std::cout << "\n";
    std::cout << "Based on PNAS paper specifications:\n";
    std::cout << "  - Tone frequency: " << TONE_FREQUENCY << " Hz (1kHz pure tone)\n";
    std::cout << "  - Tone duration:  " << TONE_DURATION_MS << " ms\n";
    std::cout << "  - Stimulus rate:  40 Hz (every " << STIMULUS_INTERVAL_MS << " ms)\n";
    std::cout << "  - Sample rate:    " << SAMPLE_RATE << " Hz\n";
    std::cout << "\n";
    std::cout << "Controls:\n";
    std::cout << "  [SPACE] - Pause/Resume\n";
    std::cout << "  [T]     - Toggle continuous 1kHz tone (for testing)\n";
    std::cout << "  [Q/ESC] - Quit\n";
    std::cout << "\n";
    std::cout << "WARNING: This is for research/educational purposes only.\n";
    std::cout << "         Consult a medical professional before use.\n";
    std::cout << "========================================\n";
}

int main(int /*argc*/, char* /*argv*/[]) {
    printInfo();
    
    // Initialize SDL
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }
    
    // Create window
    SDL_Window* window = SDL_CreateWindow(
        "40Hz Stimulation | SPACE:Pause  T:Test  Q:Quit",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN
    );
    
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }
    
    // Create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    // Set up audio specification
    SDL_AudioSpec desiredSpec, obtainedSpec;
    SDL_zero(desiredSpec);
    
    desiredSpec.freq = SAMPLE_RATE;
    desiredSpec.format = AUDIO_F32SYS;  // 32-bit float
    desiredSpec.channels = 1;            // Mono
    desiredSpec.samples = 1024;          // Buffer size
    desiredSpec.callback = audioCallback;
    desiredSpec.userdata = nullptr;
    
    // Open audio device
    SDL_AudioDeviceID audioDevice = SDL_OpenAudioDevice(
        nullptr, 0, &desiredSpec, &obtainedSpec, 0
    );
    
    if (audioDevice == 0) {
        std::cerr << "Failed to open audio device: " << SDL_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    std::cout << "\nAudio device opened successfully.\n";
    std::cout << "Starting 40Hz stimulation...\n\n";
    
    // Start audio playback
    SDL_PauseAudioDevice(audioDevice, 0);
    
    // Main loop
    bool running = true;
    SDL_Event event;
    auto startTime = std::chrono::steady_clock::now();
    
    while (running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                    
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_q:
                        case SDLK_ESCAPE:
                            running = false;
                            break;
                            
                        case SDLK_SPACE:
                            g_isPlaying.store(!g_isPlaying.load());
                            if (g_isPlaying.load()) {
                                std::cout << "▶ Resumed\n";
                            } else {
                                std::cout << "⏸ Paused\n";
                            }
                            break;
                            
                        case SDLK_t:
                            g_continuousTone.store(!g_continuousTone.load());
                            if (g_continuousTone.load()) {
                                std::cout << "🔊 Continuous 1kHz tone (test mode)\n";
                            } else {
                                std::cout << "🔊 40Hz pulsed mode (normal)\n";
                            }
                            break;
                    }
                    break;
            }
        }
        
        // Calculate elapsed time
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
        
        // Clear screen
        SDL_SetRenderDrawColor(renderer, 30, 30, 35, 255);
        SDL_RenderClear(renderer);
        
        // Draw UI elements
        drawPulseIndicator(renderer, g_samplePosition.load());
        drawStatus(renderer, static_cast<int>(elapsed));
        drawKeyHints(renderer);
        
        // Update window title with time
        std::ostringstream title;
        title << "40Hz Stimulation | " 
              << std::setfill('0') << std::setw(2) << elapsed / 60 << ":"
              << std::setfill('0') << std::setw(2) << elapsed % 60
              << " | SPACE:Pause  T:Test  Q:Quit";
        SDL_SetWindowTitle(window, title.str().c_str());
        
        // Present
        SDL_RenderPresent(renderer);
        
        // Small delay to reduce CPU usage
        SDL_Delay(16); // ~60 FPS
    }
    
    std::cout << "\n\nStopping...\n";
    
    // Cleanup
    SDL_PauseAudioDevice(audioDevice, 1);
    SDL_CloseAudioDevice(audioDevice);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    std::cout << "Done.\n";
    return 0;
}
