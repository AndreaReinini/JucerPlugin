#include "SpectrumAnalyzer.h"

SpectrumAnalyzer::SpectrumAnalyzer() : forwardFFT(fftOrder), window(fftSize, juce::dsp::WindowingFunction<float>::hann)
{
    setSize(600, 200);
    startTimerHz(60);
}

void SpectrumAnalyzer::pushNextSampleIntoFifo(float sample) noexcept
{
    if (fifoIndex == fftSize)
    {
        if (!nextFFTBlockReady)
        {
            std::copy(std::begin(fifo), std::end(fifo), fftData);
            nextFFTBlockReady = true;
        }

        fifoIndex = 0;
    }

    fifo[fifoIndex++] = sample;
}

void SpectrumAnalyzer::timerCallback()
{
    if (nextFFTBlockReady)
    {
        window.multiplyWithWindowingTable(fftData, fftSize);
        forwardFFT.performFrequencyOnlyForwardTransform(fftData);
        juce::FloatVectorOperations::copy(magnitude, fftData, fftSize / 2);
        nextFFTBlockReady = false;
        repaint();
    }
}

void SpectrumAnalyzer::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::lime);
    auto width = getLocalBounds().getWidth();
    auto height = getLocalBounds().getHeight();
    
    //originale
    for (int i = 1; i < fftSize / 2; ++i)
    {
        /*auto x1 = juce::jmap(float(i - 1), 0.0f, float(fftSize / 2), 0.0f, float(width));
        auto y1 = juce::jmap(juce::Decibels::gainToDecibels(magnitude[i - 1]), -100.0f, 0.0f, float(height), 0.0f);
        auto x2 = juce::jmap(float(i), 0.0f, float(fftSize / 2), 0.0f, float(width));
        auto y2 = juce::jmap(juce::Decibels::gainToDecibels(magnitude[i]), -100.0f, 0.0f, float(height), 0.0f);

        g.drawLine(x1, y1, x2, y2);*/
        auto y1 = juce::jmap(juce::Decibels::gainToDecibels(magnitude[i - 1]), -120.0f, 120.0f, float(height), 0.0f);
        auto y2 = juce::jmap(juce::Decibels::gainToDecibels(magnitude[i]), -120.0f, 120.0f, float(height), 0.0f);
        auto x1 = juce::jmap(float(i - 1), 0.0f, float(fftSize / 2), 0.0f, float(width));
        auto x2 = juce::jmap(float(i), 0.0f, float(fftSize / 2), 0.0f, float(width));
        g.drawLine(x1, y1, x2, y2);
    }
    
    
}

