#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <JuceHeader.h>

#include "PluginProcessor.h"

AdditiveSynthWithWaveshapingAudioProcessor::AdditiveSynthWithWaveshapingAudioProcessor()
    : MagicProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
    apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    mode = ProcessingMode::InputOnly;
    waveshapingIntensity = 1.0f;
}

juce::AudioProcessorValueTreeState::ParameterLayout AdditiveSynthWithWaveshapingAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back(std::make_unique<juce::AudioParameterFloat>("frequency", "Frequency", 10.0f, 1000.0f, 220.0f));
    params.push_back(std::make_unique<juce::AudioParameterInt>("numHarmonics", "Harmonics", 1, 20, 5));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("harmonicsAmplitude", "Harmonics Amplitude", 0.0f, 1.0f, 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("harmonicsPhase", "Harmonics Phase", 0.0f, 360.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("waveshapingIntensity", "Waveshaping Intensity", 0.1f, 10.0f, 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("drive", "Drive", 0.1f, 10.0f, 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("symmetry", "Symmetry", -1.0f, 1.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("amount", "Amount", 0.0f, 1.0f, 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("volume", "Volume", 0.0f, 2.0f, 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("lowGain", "Bass Gain", -12.0f, 12.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("midGain", "Mid Gain", -12.0f, 12.0f, 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>("highGain", "Treble Gain", -12.0f, 12.0f, 0.0f));


    return { params.begin(), params.end() };
}

void AdditiveSynthWithWaveshapingAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    oscillator.setSampleRate(getSampleRate());
    oscillator.setFrequency(*apvts.getRawParameterValue("frequency"));
    oscillator.setNumHarmonics(*apvts.getRawParameterValue("numHarmonics"));
    oscillator.resetPhase();

    
    waveshaper.setDrive(*apvts.getRawParameterValue("drive"));
    waveshaper.setSymmetry(*apvts.getRawParameterValue("symmetry"));
    waveshaper.setAmount(*apvts.getRawParameterValue("amount"));
    waveshaper.setIntensity(*apvts.getRawParameterValue("waveshapingIntensity"));

    equalizer.setSampleRate(getSampleRate());
    equalizer.prepare();
}




AdditiveSynthWithWaveshapingAudioProcessor::~AdditiveSynthWithWaveshapingAudioProcessor() {}
void AdditiveSynthWithWaveshapingAudioProcessor::releaseResources() {}

void AdditiveSynthWithWaveshapingAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    // Aggiorna i parametri dall'APVTS
    float currentFrequency = *apvts.getRawParameterValue("frequency");
    int currentNumHarmonics = *apvts.getRawParameterValue("numHarmonics");
    float currentWaveshapingIntensity = *apvts.getRawParameterValue("waveshapingIntensity");
    float currentDrive = *apvts.getRawParameterValue("drive");
    float currentSymmetry = *apvts.getRawParameterValue("symmetry");
    float currentAmount = *apvts.getRawParameterValue("amount");

    // Applica i parametri aggiornati agli oggetti
    oscillator.setFrequency(currentFrequency);
    oscillator.setNumHarmonics(currentNumHarmonics);
    waveshaper.setIntensity(currentWaveshapingIntensity);
    waveshaper.setDrive(currentDrive);
    waveshaper.setSymmetry(currentSymmetry);
    waveshaper.setAmount(currentAmount);

    // Itera sui canali di input
    auto numInputChannels = getTotalNumInputChannels();
    auto numOutputChannels = getTotalNumOutputChannels();
    auto numSamples = buffer.getNumSamples();

    for (int channel = 0; channel < numInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            float inputSample = channelData[sample];
            float generatedSample = oscillator.getNextSample();
            float finalSample = 0.0f;

            // Switch per la modalità di elaborazione
            switch (mode)
            {
            case ProcessingMode::InputOnly:
                finalSample = waveshaper.waveshapingState ? waveshaper.processSample(inputSample) : inputSample;
                break;

            case ProcessingMode::GeneratedOnly:
                finalSample = waveshaper.waveshapingState ? waveshaper.processSample(generatedSample) : generatedSample;
                break;

            case ProcessingMode::InputAndGenerated:
                finalSample = waveshaper.waveshapingState
                    ? waveshaper.processSample(inputSample + generatedSample)
                    : (inputSample + generatedSample);
                break;
            }

            // Applica il gain di uscita
            finalSample *= outputGain.load();
            channelData[sample] = finalSample;
        }
    }

    // Pulisci i canali non usati
    for (int channel = numInputChannels; channel < numOutputChannels; ++channel)
    {
        buffer.clear(channel, 0, numSamples);
    }

    // Passa il buffer all'equalizzatore
    if (!dontCallProcess)
    {
        equalizer.process(buffer);
    }

    // Aggiorna l'analizzatore di spettro
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getReadPointer(channel);
        for (int smp = 0; smp < buffer.getNumSamples(); ++smp)
        {
            spectrumAnalyzer.pushNextSampleIntoFifo(channelData[smp]);
        }
    }
}







//GUI- Rimozione non utilizzati
//juce::AudioProcessorEditor* AdditiveSynthWithWaveshapingAudioProcessor::createEditor()
//{
//    return new AdditiveSynthWithWaveshapingAudioProcessorEditor(*this);
//}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AdditiveSynthWithWaveshapingAudioProcessor();
}