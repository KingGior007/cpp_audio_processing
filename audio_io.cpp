#include "audio_io.hpp"

namespace {
    template <typename T>
    void write_bytes(std::ofstream& file, const T& value) {
        file.write(reinterpret_cast<const char*>(&value), sizeof(T));
    }
}

// ---- FMTChunk ----
FMTChunk::FMTChunk(int sampleRate) : sampleRate_(sampleRate) {
    subchunk1Size_ = 16;
    audioFormat_ = 1; // PCM
    numChannels_ = 1;
    byteRate_ = sampleRate_ * numChannels_ * sizeof(int16_t);
    blockAlign_ = numChannels_ * sizeof(int16_t);
    bitsPerSample_ = 8 * sizeof(int16_t);
}

void FMTChunk::writeToFile(std::ofstream& file) const {
    file.write("fmt ", 4);
    write_bytes(file, subchunk1Size_);
    write_bytes(file, audioFormat_);
    write_bytes(file, numChannels_);
    write_bytes(file, sampleRate_);
    write_bytes(file, byteRate_);
    write_bytes(file, blockAlign_);
    write_bytes(file, bitsPerSample_);
}

// ---- DataChunk ----
void DataChunk::addSample(const int16_t& sample) {
    samples_.push_back(sample);
}

void DataChunk::addSamples(const std::vector<int16_t>& new_samples) {
    samples_.insert(samples_.end(), new_samples.begin(), new_samples.end());
}

void DataChunk::writeToFile(std::ofstream& file) const {
    file.write("data", 4);
    int32_t dataSize = static_cast<int32_t>(samples_.size() * sizeof(int16_t));
    write_bytes(file, dataSize);
    for (auto sample : samples_) {
        write_bytes(file, sample);
    }
}

size_t DataChunk::size() const {
    return samples_.size();
}

// ---- WavFile ----
WavFile::WavFile(int sampleRate) : sampleRate_(sampleRate), data_(), fmt_(sampleRate) {}

void WavFile::addSample(const int16_t& sample) { data_.addSample(sample); }
void WavFile::addSamples(const std::vector<int16_t>& samples) { data_.addSamples(samples); }

double WavFile::getDuration() const {
    return static_cast<double>(data_.size()) / sampleRate_;
}

bool WavFile::saveFile(std::string fileName) const {
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
