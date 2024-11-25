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

//GUI- Sostituzione juce::AudioProcessor -> foleys::MagicProcessor
class AdditiveSynthWithWaveshapingAudioProcessor : public foleys::MagicProcessor
{
public:
    ProcessingMode mode = ProcessingMode::InputOnly;
    AdditiveSynthWithWaveshapingAudioProcessor();
    ~AdditiveSynthWithWaveshapingAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //GUI-Rimozione non utilizzati
    //juce::AudioProcessorEditor* createEditor() override;
    //bool hasEditor() const override { return true; }

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

    //GUI- Rimozione non utilizzati
    //void getStateInformation(juce::MemoryBlock& destData) override {}
    //void setStateInformation(const void* data, int sizeInBytes) override {}

    // Parametri di controllo dell'oscillatore e del waveshaping
    float frequency = 220.0f;
    int numHarmonics = 5;
    // Parametri di intensità del waveshaping e gain di uscita
    float waveshapingIntensity = 1.0f;
    
    float drive = 1.0f;       // Controlla l’intensità dell’ingresso nel waveshaper
    float symmetry = 0.0f;    // Controlla la simmetria della distorsione
    float amount = 1.0f;      // Controlla l’intensità complessiva del waveshaping

    void setDrive(float newDrive) { drive = newDrive; }
    void setSymmetry(float newSymmetry) { symmetry = newSymmetry; }
    void setAmount(float newAmount) { amount = newAmount; }

    void onAmplitudeSliderChange(int harmonicIndex, float newAmplitude)
    {
        oscillator.setHarmonicAmplitude(harmonicIndex, newAmplitude);
    }

    void onPhaseSliderChange(int harmonicIndex, float newPhase)
    {
        oscillator.setHarmonicPhase(harmonicIndex, newPhase);
    }
    
    //float outputGain = 0.5f;  // Gain per normalizzare l'output
    AdditiveOscillator oscillator;
    Waveshaper waveshaper;
    Equalizer equalizer;
    SpectrumAnalyzer spectrumAnalyzer;
    std::atomic<float> outputGain { 1.0f };  // Parametro per il gain di output
    
    
    bool dontCallProcess=true;
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AdditiveSynthWithWaveshapingAudioProcessor);
};


