#pragma once

#include <JuceHeader.h>
#include "AdditiveOscillator.h"
#include "Waveshaper.h"
#include "Equalizer.h"
#include "SpectrumAnalyzer.h"
#include <foleys_gui_magic/foleys_gui_magic.h>

enum class ProcessingMode
{
    InputOnly,
    GeneratedOnly,
    InputAndGenerated
};

class AdditiveSynthWithWaveshapingAudioProcessor : public foleys::MagicProcessor
{
public:
    ProcessingMode mode = ProcessingMode::InputOnly;
    AdditiveSynthWithWaveshapingAudioProcessor();
    ~AdditiveSynthWithWaveshapingAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    const juce::String getName() const override { return "AdditiveSynthWithWaveshaping"; }

    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int index) override {}
    const juce::String getProgramName(int index) override { return {}; }
    void changeProgramName(int index, const juce::String& newName) override {}

    float frequency = 220.0f;
    int numHarmonics = 5;
    float waveshapingIntensity = 1.0f;

    float drive = 1.0f;
    float symmetry = 0.0f;
    float amount = 1.0f;

    AdditiveOscillator oscillator;
    Waveshaper waveshaper;
    Equalizer equalizer;
    SpectrumAnalyzer spectrumAnalyzer;

    std::atomic<float> outputGain{ 1.0f };
    bool dontCallProcess = true;

private:
    juce::AudioProcessorValueTreeState apvts;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AdditiveSynthWithWaveshapingAudioProcessor);
};
