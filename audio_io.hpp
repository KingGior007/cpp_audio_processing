#ifndef AUDIO_IO_HPP
#define AUDIO_IO_HPP

#include <fstream>
#include <vector>
#include <cstdint>
#include <string>

class FMTChunk {
private:
    int sampleRate_;

    int32_t subchunk1Size_;
    int16_t audioFormat_;
    int16_t numChannels_;
    int32_t byteRate_;
    int16_t blockAlign_;
    int16_t bitsPerSample_;

    template <typename T>
    void write_bytes(std::ofstream& file, const T& value) const {
        file.write(reinterpret_cast<const char*>(&value), sizeof(T));
    }
public:
    FMTChunk(int sampleRate): sampleRate_(sampleRate) {
        subchunk1Size_ = 16;
        audioFormat_ = 1; // PCM
        numChannels_ = 1;
        byteRate_ = sampleRate * numChannels_ * sizeof(int16_t);
        blockAlign_ = numChannels_ * sizeof(int16_t);
        bitsPerSample_ = 8 * sizeof(int16_t);
    }

    void writeToFile(std::ofstream& file) const {
        file.write("fmt ", 4);

        write_bytes(file, subchunk1Size_);
        write_bytes(file, audioFormat_);
        write_bytes(file, numChannels_);
        write_bytes(file, sampleRate_);
        write_bytes(file, byteRate_);
        write_bytes(file, blockAlign_);
        write_bytes(file, bitsPerSample_);
    }
};

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
    FMTChunk fmt_;
    template <typename T>
    void write_bytes(std::ofstream& file, const T& value) const {
        file.write(reinterpret_cast<const char*>(&value), sizeof(T));
    }

public:
    WavFile(int sampleRate=defaultSampleRate):
        data_(), fmt_(sampleRate), sampleRate_(sampleRate) {}

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
        fmt_.writeToFile(file);

        // data chunk
        data_.writeToFile(file);

        file.close();
        return true;
    }
};

#endif // AUDIO_IO_HPP
