#include <cmath>
#include <cstdint>
#include <iostream>
#include "audio_io.hpp"

int main() {
    const int sampleRate = 44100;
    const int durationSeconds = 1;
    const double frequency = 261.625565; // C4
    const int16_t amplitude = 30000;

    // Create WavFile instance
    WavFile wav(sampleRate);

    // Generate samples for C4 note and add to WavFile
    const int numSamples = sampleRate * durationSeconds;
    const double twoPi = 2.0 * M_PI;
    for (int i = 0; i < numSamples; ++i) {
        double t = static_cast<double>(i) / sampleRate;
        int16_t sample = static_cast<int16_t>(amplitude * std::sin(twoPi * frequency * t));
        wav.addSample(sample); // forwards to internal DataChunk
    }
    std::cout << wav.getDuration() << std::endl;

    // Save WAV file
    if (!wav.saveFile("note.wav")) {
        return 1; // error writing file
    }

    return 0;
}
