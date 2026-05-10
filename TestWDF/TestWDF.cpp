#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <string>
#include <iomanip>
#include <numeric>
#include <algorithm>

// Include JUCE core and DSP modules
#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_formats/juce_audio_formats.h>

// Include our WDF triode implementation
#include "../Source/TriodeGainStage.h"

// Constants matching the Python script
const double SAMPLE_RATE = 96000.0;
const double DURATION = 0.05; // 50ms
const double TEST_FREQ = 440.0; // A4

// Drive values to test (matching Python script)
const std::vector<double> DRIVE_VALUES = {1.0, 5.0, 10.0, 100.0};

// Signal generation functions (matching Python script)
std::vector<float> generateSine(double freq, double duration, double sampleRate) {
    int numSamples = static_cast<int>(sampleRate * duration);
    std::vector<float> signal(numSamples);

    for (int n = 0; n < numSamples; ++n) {
        double t = static_cast<double>(n) / sampleRate;
        signal[n] = static_cast<float>(std::sin(2.0 * M_PI * freq * t));
    }

    return signal;
}

std::vector<float> generateSquare(double freq, double duration, double sampleRate) {
    int numSamples = static_cast<int>(sampleRate * duration);
    std::vector<float> signal(numSamples);

    for (int n = 0; n < numSamples; ++n) {
        double t = static_cast<double>(n) / sampleRate;
        double value = std::sin(2.0 * M_PI * freq * t);
        signal[n] = static_cast<float>((value >= 0.0 ? 1.0 : -1.0) * 0.8);
    }

    return signal;
}

std::vector<float> generateSaw(double freq, double duration, double sampleRate) {
    int numSamples = static_cast<int>(sampleRate * duration);
    std::vector<float> signal(numSamples);

    for (int n = 0; n < numSamples; ++n) {
        double t = static_cast<double>(n) / sampleRate;
        double value = 2.0 * (freq * t - std::floor(freq * t + 0.5));
        signal[n] = static_cast<float>(value);
    }

    return signal;
}

std::vector<float> generateSweep(double duration, double sampleRate) {
    int numSamples = static_cast<int>(sampleRate * duration);
    std::vector<float> signal(numSamples);

    // Log-frequency sweep from 20Hz to 20kHz
    double phase = 0.0;
    for (int n = 0; n < numSamples; ++n) {
        double t = static_cast<double>(n) / sampleRate;
        double freq = 20.0 * std::pow(20000.0 / 20.0, t / duration);
        double deltaPhase = 2.0 * M_PI * freq / sampleRate;
        phase += deltaPhase;
        signal[n] = static_cast<float>(std::sin(phase));
    }

    return signal;
}

// Process a signal through the triode gain stage and monitor internal variables
std::vector<float> processSignalWithMonitoring(const std::vector<float>& input, double sampleRate, double drive,
                                              std::vector<float>& gridVoltages,
                                              std::vector<float>& cathodeVoltages,
                                              std::vector<float>& plateVoltages,
                                              std::vector<float>& triodeVg,
                                              std::vector<float>& triodeVk,
                                              std::vector<float>& triodeVp) {
    TriodeGainStage triode;
    triode.prepare(sampleRate);

    std::vector<float> output(input.size());
    gridVoltages.resize(input.size());
    cathodeVoltages.resize(input.size());
    plateVoltages.resize(input.size());
    triodeVg.resize(input.size());
    triodeVk.resize(input.size());
    triodeVp.resize(input.size());

    for (size_t i = 0; i < input.size(); ++i) {
        // Scale input by drive (as done in Python script's processSignal)
        float scaledInput = input[i] * static_cast<float>(drive);
        output[i] = triode.processSample(scaledInput);

        // Monitor internal variables
        gridVoltages[i] = triode.getGridVoltage();
        cathodeVoltages[i] = triode.getCathodeVoltage();
        plateVoltages[i] = triode.getPlateVoltage();
        triodeVg[i] = triode.getTriodeVg();
        triodeVk[i] = triode.getTriodeVk();
        triodeVp[i] = triode.getTriodeVp();
    }

    return output;
}

// Save signal to WAV file for external analysis
void saveWavFile(const juce::String& filename, const std::vector<float>& data, double sampleRate) {
    juce::WavAudioFormat wavFormat;
    std::unique_ptr<juce::AudioFormatWriter> writer(
        wavFormat.createWriterFor(
            new juce::FileOutputStream(juce::File(filename)),
            sampleRate,
            1, // mono
            32, // bits per sample
            juce::StringPairArray(),
            0
        )
    );

    if (writer) {
        juce::AudioBuffer<float> buffer(1, static_cast<int>(data.size()));
        std::copy(data.begin(), data.end(), buffer.getWritePointer(0));
        writer->writeFromAudioSampleBuffer(buffer, 0, static_cast<int>(data.size()));
    }
}

// Save monitoring data to CSV file
void saveMonitoringData(const juce::String& filename,
                       const std::vector<float>& time,
                       const std::vector<float>& gridVoltages,
                       const std::vector<float>& cathodeVoltages,
                       const std::vector<float>& plateVoltages,
                       const std::vector<float>& triodeVg,
                       const std::vector<float>& triodeVk,
                       const std::vector<float>& triodeVp) {
    std::ofstream file(filename.toStdString());
    if (file.is_open()) {
        // Header
        file << "Time(s),Grid_Voltage(V),Cathode_Voltage(V),Plate_Voltage(V),Triode_Vg(V),Triode_Vk(V),Triode_Vp(V)\n";

        // Data
        for (size_t i = 0; i < time.size(); ++i) {
            file << std::fixed << std::setprecision(6)
                 << time[i] << ","
                 << gridVoltages[i] << ","
                 << cathodeVoltages[i] << ","
                 << plateVoltages[i] << ","
                 << triodeVg[i] << ","
                 << triodeVk[i] << ","
                 << triodeVp[i] << "\n";
        }
        file.close();
    }
}

// Main test function
int main() {
    std::cout << "Starting WDF Triode Gain Stage Test with Internal Monitoring..." << std::endl;
    std::cout << "Sample Rate: " << SAMPLE_RATE << " Hz" << std::endl;
    std::cout << "Duration: " << DURATION << " s" << std::endl;
    std::cout << "Test Frequency: " << TEST_FREQ << " Hz" << std::endl;

    // Create output directory
    juce::File outputDir("/Users/vuillemr/PremierPlugin/outputs");
    if (!outputDir.exists()) {
        outputDir.createDirectory();
    }

    // Generate test signals
    std::vector<std::pair<juce::String, std::vector<float>>> signals;
    signals.emplace_back("Sine_440Hz", generateSine(TEST_FREQ, DURATION, SAMPLE_RATE));
    signals.emplace_back("Square_440Hz", generateSquare(TEST_FREQ, DURATION, SAMPLE_RATE));
    signals.emplace_back("Sawtooth_440Hz", generateSaw(TEST_FREQ, DURATION, SAMPLE_RATE));
    signals.emplace_back("Sweep_20Hz-20kHz", generateSweep(DURATION, SAMPLE_RATE));

    // Process each signal with different drive values
    for (const auto& signalPair : signals) {
        const juce::String& signalName = signalPair.first;
        const std::vector<float>& inputSignal = signalPair.second;

        std::cout << "\nProcessing: " << signalName.toStdString() << "..." << std::endl;

        for (double drive : DRIVE_VALUES) {
            std::cout << "  Drive = " << drive << std::endl;

            // Vectors for monitoring internal variables
            std::vector<float> gridVoltages, cathodeVoltages, plateVoltages;
            std::vector<float> triodeVg, triodeVk, triodeVp;
            std::vector<float> timeVals;

            // Generate time values
            int numSamples = static_cast<int>(inputSignal.size());
            timeVals.resize(numSamples);
            for (int n = 0; n < numSamples; ++n) {
                timeVals[n] = static_cast<float>(n) / static_cast<float>(SAMPLE_RATE);
            }

            // Process the signal with monitoring
            std::vector<float> outputSignal = processSignalWithMonitoring(
                inputSignal, SAMPLE_RATE, drive,
                gridVoltages, cathodeVoltages, plateVoltages,
                triodeVg, triodeVk, triodeVp);

            // Save input and output signals for external analysis
            juce::String inputFilename = outputDir.getFullPathName() + "/" +
                                      signalName + "_drive_" + juce::String(drive) + "_input.wav";
            juce::String outputFilename = outputDir.getFullPathName() + "/" +
                                       signalName + "_drive_" + juce::String(drive) + "_output.wav";

            saveWavFile(inputFilename, inputSignal, SAMPLE_RATE);
            saveWavFile(outputFilename, outputSignal, SAMPLE_RATE);

            // Save monitoring data to CSV
            juce::String monitorFilename = outputDir.getFullPathName() + "/" +
                                        signalName + "_drive_" + juce::String(drive) + "_monitoring.csv";
            saveMonitoringData(monitorFilename, timeVals,
                              gridVoltages, cathodeVoltages, plateVoltages,
                              triodeVg, triodeVk, triodeVp);

            // Calculate and display some basic statistics
            float inputPeak = 0.0f;
            float outputPeak = 0.0f;
            float inputRMS = 0.0f;
            float outputRMS = 0.0f;

            for (size_t i = 0; i < inputSignal.size(); ++i) {
                float absInput = std::abs(inputSignal[i]);
                float absOutput = std::abs(outputSignal[i]);

                if (absInput > inputPeak) inputPeak = absInput;
                if (absOutput > outputPeak) outputPeak = absOutput;

                inputRMS += inputSignal[i] * inputSignal[i];
                outputRMS += outputSignal[i] * outputSignal[i];
            }

            inputRMS = std::sqrt(inputRMS / static_cast<float>(inputSignal.size()));
            outputRMS = std::sqrt(outputRMS / static_cast<float>(outputSignal.size()));

            std::cout << "    Input Peak: " << inputPeak << ", Output Peak: " << outputPeak << std::endl;
            std::cout << "    Input RMS: " << inputRMS << ", Output RMS: " << outputRMS << std::endl;
            std::cout << "    Gain (RMS): " << (outputRMS / inputRMS) << std::endl;

            // Display some internal variable statistics (first, middle, last samples)
            size_t mid = gridVoltages.size() / 2;
            std::cout << "    Internal Vars (start/mid/end):" << std::endl;
            std::cout << "      Grid V: " << gridVoltages[0] << "/" << gridVoltages[mid] << "/" << gridVoltages.back() << " V" << std::endl;
            std::cout << "      Cathode V: " << cathodeVoltages[0] << "/" << cathodeVoltages[mid] << "/" << cathodeVoltages.back() << " V" << std::endl;
            std::cout << "      Plate V: " << plateVoltages[0] << "/" << plateVoltages[mid] << "/" << plateVoltages.back() << " V" << std::endl;
            std::cout << "      Triode Vg: " << triodeVg[0] << "/" << triodeVg[mid] << "/" << triodeVg.back() << " V" << std::endl;
            std::cout << "      Triode Vk: " << triodeVk[0] << "/" << triodeVk[mid] << "/" << triodeVk.back() << " V" << std::endl;
            std::cout << "      Triode Vp: " << triodeVp[0] << "/" << triodeVp[mid] << "/" << triodeVp.back() << " V" << std::endl;
        }
    }

    std::cout << "\nTest completed. Results saved to: " << outputDir.getFullPathName() << std::endl;
    std::cout << "Monitoring data (.csv files) shows internal WDF variables over time." << std::endl;
    return 0;
}