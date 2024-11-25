#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

class AdditiveSynthWithWaveshapingAudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Slider::Listener
{
public:
    AdditiveSynthWithWaveshapingAudioProcessorEditor(AdditiveSynthWithWaveshapingAudioProcessor&);
    ~AdditiveSynthWithWaveshapingAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    
private:
    void sliderValueChanged(juce::Slider* slider) override;
    void updateEqualizerSliders();
    void updateFrequencyText();
    AdditiveSynthWithWaveshapingAudioProcessor& audioProcessor;

    juce::Slider frequencySlider, harmonicsSlider, intensitySlider;
    juce::Slider AmplitudeHarmonicSlider,PhaseArmonicSlider;
    juce::Label frequencyLabel, harmonicsLabel, intensityLabel;
    juce::Label AmplitudeHarmonicLabel,PhaseArmonicLabel;
    // Slider per il controllo del waveshaping
    juce::Slider driveSlider;
    juce::Slider symmetrySlider;
    juce::Slider amountSlider;
    juce::Label driveLabel;
    juce::Label symmetryLabel;
    juce::Label amountLabel;
    
    
    juce::ComboBox modeSelector;
    juce::Slider gainSlider; // Nuovo slider per il gain
    juce::Label gainLabel; // Label per il controllo del gain
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(AdditiveSynthWithWaveshapingAudioProcessorEditor)
    //parte grafica per l'equalizzatore a 3 bande
    
    juce::Slider lowGainSlider;
    juce::Slider midGainSlider;
    juce::Slider highGainSlider;
    
    //label low,mid,high frequency
    
    juce::Label lowFreqLabel, midFreqLabel, highFreqLabel;
    
    juce::Label lowGainLabel;
    juce::Label midGainLabel;
    juce::Label highGainLabel;
    juce::ComboBox presetSelector;
    juce::Label WaveshapingActivatorLabel;
    juce::ToggleButton waveshaperOnOff;
    
    juce::Slider lowCutFrequencyUser,midCutFrequencyUser,highCutFrequencyUser;
    void checkValueGain();
    void saveSettingsToFile();
    void loadSettingsFromFile();
};
