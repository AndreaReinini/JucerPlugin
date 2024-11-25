#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <JuceHeader.h>

AdditiveSynthWithWaveshapingAudioProcessor::AdditiveSynthWithWaveshapingAudioProcessor()
    : MagicProcessor(BusesProperties()
                      .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      .withOutput ("Output", juce::AudioChannelSet::stereo(), true))

{
    mode = ProcessingMode::InputOnly; // Imposta la modalità di default
    waveshapingIntensity = 1.0f;      // Imposta l'intensità di default
    //equalizer.setSampleRate(getSampleRate());
    
}



AdditiveSynthWithWaveshapingAudioProcessor::~AdditiveSynthWithWaveshapingAudioProcessor() {}

void AdditiveSynthWithWaveshapingAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    oscillator.setSampleRate(getSampleRate());
    oscillator.setFrequency(frequency);
    oscillator.setNumHarmonics(numHarmonics);
    oscillator.resetPhase();

    waveshaper.setIntensity(waveshapingIntensity);
    waveshaper.setDrive(drive);
    waveshaper.setSymmetry(symmetry);
    waveshaper.setAmount(amount);

    equalizer.setSampleRate(getSampleRate());
    equalizer.prepare();
}



void AdditiveSynthWithWaveshapingAudioProcessor::releaseResources() {}

void AdditiveSynthWithWaveshapingAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
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

            // Switch per la gestione della modalità
            switch (mode)
            {
                case ProcessingMode::InputOnly:
                    finalSample = waveshaper.waveshapingState ? waveshaper.processSample(inputSample) : inputSample;
                    break;

                case ProcessingMode::GeneratedOnly:
                    finalSample = waveshaper.waveshapingState ? waveshaper.processSample(generatedSample) : generatedSample;
                    break;

                case ProcessingMode::InputAndGenerated:
                    finalSample = waveshaper.waveshapingState ? waveshaper.processSample(inputSample + generatedSample)
                                                              : (inputSample + generatedSample);
                    break;
            }

            finalSample *= outputGain; // Applica il guadagno
            channelData[sample] = finalSample; // Scrivi il campione nel buffer
        }
    }

    // Pulisci i canali extra
    for (int channel = numInputChannels; channel < numOutputChannels; ++channel)
    {
        buffer.clear(channel, 0, numSamples);
    }

    // Passa il buffer all'equalizzatore
    if(dontCallProcess==false)
    {
        equalizer.process(buffer);
    }


    // Analizzatore di spettro
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



