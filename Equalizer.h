#pragma once

#include <JuceHeader.h>


enum class Preset
{
    Flat=1,
    Rock,
    Disco,
    Jazz,
    Pop,
    Classical,
    HipHop,
    Metal,
    Dance,
    VocalBoost,
    BassBoost,
    TrebleBoost,
    MyEqu
};


class Equalizer
{
public:
    Equalizer(float lowFreq = 200.0f, float midFreq = 1000.0f, float highFreq = 5000.0f, float Q = 1.0f, double sampleRate = 48000.0f);
    ~Equalizer() = default;

    void setLowFrequency(float frequency);
    void setMidFrequency(float frequency);
    void setHighFrequency(float frequency);

    void setLowGain(float gain);
    void setMidGain(float gain);
    void setHighGain(float gain);
    //aggiunta
    void setGainLowSlider(float gainNotNormalized);
    void setGainMidSlider(float gainNotNormalized);
    void setGainHighSlider(float gainNotNormalized);
    //----------------
    void setQ(float qualityFactor);

    void setSampleRate(double sampleRate);

    void prepare();
    void prepareV4();
    void process(juce::AudioBuffer<float>& buffer);
    void processV2(juce::AudioBuffer<float>& buffer);
    void processV3(juce::AudioBuffer<float>& buffer);
    void processV4(juce::AudioBuffer<float>& buffer);
    float getSampleRate();
    float getLowFrequency();
    float getMidFrequency();
    float getHighFrequency();
    //aggiunta
    float getLowGain();
    float getMidGain();
    float getHighGain();
    
    float getGainLowSlider();
    float getGainMidSlider();
    float getGainHighSlider();
    //------------------------------
    float getQ();
    void applyPreset(Preset preset);
    float dBToLinear(float dB);
    
    
    float myEqLow=0.0f,myEqMid=0.0f,myEqHigh=0.0f;
    int myLowCutFrequency,myMidCutFrequency,myHighCutFrequency;
    
private:
    float lowFreq, midFreq, highFreq;
    float Q;
    float lowGain = 0.5f, midGain = 0.5f, highGain = 0.5f;
    float lowGainSlider=0.0f,midGainSlider=0.0f,highGainSlider=0.0f;//aggiunta
    double sampleRate;

    juce::IIRFilter lowPassFilter;
    juce::IIRFilter bandPassFilter;
    juce::IIRFilter highPassFilter;
    
    // Dichiarazione nei membri privati di Equalizer
    juce::AudioBuffer<float> lowBuffer, midBuffer, highBuffer;
    int bufferSize = 512; // Dimensione del buffer
    
    
};
