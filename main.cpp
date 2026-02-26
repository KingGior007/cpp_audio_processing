#include <fstream>
#include <cmath>
#include <cstdint>
#include <vector>
#include <iostream>

class DataChunk {
private:
    std::vector<int16_t> samples_;
    template <typename T>
    void write_bytes(std::ofstream& file, const T& value) const {
        file.write(reinterpret_cast<const char*>(&value), sizeof(T));
    }
public:
    void addSample(const int16_t& sample) {
        samples_.push_back(sample);
    }
    void addSamples(const std::vector<int16_t>& new_samples) {
        samples_.insert(samples_.end(), new_samples.begin(), new_samples.end());
    }

    void writeToFile(std::ofstream& file) const {
        // data subchunk
        file.write("data", 4);

        int32_t dataSize = static_cast<int32_t>(samples_.size() * sizeof(int16_t));
        write_bytes(file, dataSize);

        for (auto sample : samples_) {
            write_bytes(file, sample);
        }
    }

    size_t size() const {
        return samples_.size();
    }
};

class WavFile {
    static const int defaultSampleRate = 44100;

private:
    int sampleRate_;
    DataChunk data_;
    template <typename T>
    void write_bytes(std::ofstream& file, const T& value) const {
        file.write(reinterpret_cast<const char*>(&value), sizeof(T));
    }

public:
    WavFile(int sampleRate=defaultSampleRate):
        sampleRate_(sampleRate), data_() {}

    void addSample(const int16_t& sample) {
        data_.addSample(sample);
    }
    void addSamples(const std::vector<int16_t>& samples) {
        data_.addSamples(samples);
    }

    double getDuration() const {
        return static_cast<double>(data_.size()) / sampleRate_;
    }

    bool saveFile(std::string fileName) const {
        std::ofstream file(fileName, std::ios::binary);
        if (!file.is_open()) return false;

        // ===== WAV HEADER =====
        file.write("RIFF", 4);
        int32_t chunkSize = 36 + data_.size() * sizeof(int16_t);
        write_bytes(file, chunkSize);
        file.write("WAVE", 4);

        // fmt subchunk
        file.write("fmt ", 4);
        int32_t subchunk1Size = 16;
        int16_t audioFormat = 1; // PCM
        int16_t numChannels = 1;
        int32_t byteRate = sampleRate_ * numChannels * sizeof(int16_t);
        int16_t blockAlign = numChannels * sizeof(int16_t);
        int16_t bitsPerSample = 8 * sizeof(int16_t);

        write_bytes(file, subchunk1Size);
        write_bytes(file, audioFormat);
        write_bytes(file, numChannels);
        write_bytes(file, sampleRate_);
        write_bytes(file, byteRate);
        write_bytes(file, blockAlign);
        write_bytes(file, bitsPerSample);

        data_.writeToFile(file);
        file.close();
        return true;
    }
};

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
