#pragma once

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
public:
    FMTChunk(int sampleRate);
    void writeToFile(std::ofstream& file) const;
};

class DataChunk {
private:
    std::vector<int16_t> samples_;
public:
    void addSample(const int16_t& sample);
    void addSamples(const std::vector<int16_t>& new_samples);
    void writeToFile(std::ofstream& file) const;
    size_t size() const;
};

class WavFile {
private:
    int sampleRate_;
    DataChunk data_;
    FMTChunk fmt_;
public:
    static const int defaultSampleRate = 44100;
    WavFile(int sampleRate = defaultSampleRate);

    void addSample(const int16_t& sample);
    void addSamples(const std::vector<int16_t>& samples);

    double getDuration() const;
    bool saveFile(std::string fileName) const;
};
