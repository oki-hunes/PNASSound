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
#include <thread>
#include <chrono>

// Audio parameters
constexpr int SAMPLE_RATE = 44100;           // Standard audio sample rate
constexpr int TONE_FREQUENCY = 1000;         // 1kHz pure tone
constexpr double TONE_DURATION_MS = 1.0;     // 1ms tone duration
constexpr double STIMULUS_INTERVAL_MS = 25.0; // 25ms interval (40Hz)
constexpr double AMPLITUDE = 0.5;            // Volume (0.0 - 1.0)

// Derived constants
constexpr int SAMPLES_PER_TONE = static_cast<int>(SAMPLE_RATE * TONE_DURATION_MS / 1000.0);
constexpr int SAMPLES_PER_INTERVAL = static_cast<int>(SAMPLE_RATE * STIMULUS_INTERVAL_MS / 1000.0);

// Global state
std::atomic<bool> g_isPlaying{true};
std::atomic<int> g_samplePosition{0};

/**
 * Generate a single sample of the 40Hz stimulus pattern
 */
float generateSample(int position) {
    // Position within current 25ms interval
    int posInInterval = position % SAMPLES_PER_INTERVAL;
    
    // Only generate tone for first 1ms of each 25ms interval
    if (posInInterval < SAMPLES_PER_TONE) {
        // Generate 1kHz sine wave
        double t = static_cast<double>(posInInterval) / SAMPLE_RATE;
        double sample = AMPLITUDE * std::sin(2.0 * M_PI * TONE_FREQUENCY * t);
        
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
void audioCallback(void* userdata, Uint8* stream, int len) {
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
    std::cout << "  [Q/ESC] - Quit\n";
    std::cout << "\n";
    std::cout << "WARNING: This is for research/educational purposes only.\n";
    std::cout << "         Consult a medical professional before use.\n";
    std::cout << "========================================\n";
}

int main(int argc, char* argv[]) {
    printInfo();
    
    // Initialize SDL
    if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_EVENTS) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
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
                    }
                    break;
            }
        }
        
        // Display elapsed time
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(now - startTime).count();
        
        if (g_isPlaying.load()) {
            std::cout << "\rPlaying: " << elapsed / 60 << "m " << elapsed % 60 << "s   " << std::flush;
        }
        
        SDL_Delay(100); // 100ms update interval
    }
    
    std::cout << "\n\nStopping...\n";
    
    // Cleanup
    SDL_PauseAudioDevice(audioDevice, 1);
    SDL_CloseAudioDevice(audioDevice);
    SDL_Quit();
    
    std::cout << "Done.\n";
    return 0;
}
