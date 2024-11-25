#pragma once
#include <JuceHeader.h>

class SpectrumAnalyzer : public juce::Component, private juce::Timer
{
public:
    SpectrumAnalyzer();
    void pushNextSampleIntoFifo(float sample) noexcept;

private:
    void timerCallback() override;
    void paint(juce::Graphics& g) override;

    enum
    {
        fftOrder = 11,
        fftSize = 1 << fftOrder
    };

    juce::dsp::FFT forwardFFT;
    juce::dsp::WindowingFunction<float> window;
    float fifo[fftSize] = { 0 };
    float fftData[fftSize * 2] = { 0 };
    float magnitude[fftSize / 2] = { 0 };
    int fifoIndex = 0;
    bool nextFFTBlockReady = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SpectrumAnalyzer)
};
