#pragma once

#include <vector>
#include <cstdint>
#include <cmath>

enum class Pitch {
    C, Cs, D, Ds, E, F,
    Fs, G, Gs, A, As, B
};

class Note {
private: 
    inline static int refereceFrequency = 440;
    double frequency_;

    const int semitoneFromA[12] = {
        -9,  // C
        -8,  // C#
        -7,  // D
        -6,  // D#
        -5,  // E
        -4,  // F
        -3,  // F#
        -2,  // G
        -1,  // G#
        0,  // A
        1,  // A#
        2   // B
    };
public:
    Note(Pitch pitch, int octave) {
        int semitoneOffsetFromA = semitoneFromA[static_cast<int>(pitch)];
        int totalSemitoneOffset = semitoneOffsetFromA + (octave - 4) * 12;

        frequency_ = refereceFrequency * std::pow(2.0, totalSemitoneOffset / 12.0);
    }
    double getFrequency() {
        return frequency_;
    }
};

class Oscillator {
protected:
    double sampleRate;
    double phase = 0.0;

    virtual double generateSample() = 0;

public:
    Oscillator(double sr) : sampleRate(sr) {}
    virtual ~Oscillator() = default;

    std::vector<int16_t> fillFreq(double frequency, int timesteps) {
        std::vector<int16_t> buffer(timesteps);
        double angularVelocity = 2.0 * M_PI * frequency;
        double phaseIncrement = angularVelocity / sampleRate;

        for (int i = 0; i < timesteps; ++i) {
            buffer[i] = static_cast<int16_t>(generateSample() * 32767);
            phase += phaseIncrement;

            if (phase >= 2.0 * M_PI)
                phase -= 2.0 * M_PI;
        }

        return buffer;
    }
};

class SineOscillator : public Oscillator {
public:
    SineOscillator(double sr) : Oscillator(sr) {}

protected:
    double generateSample() override {
        return std::sin(phase);
    }
};
