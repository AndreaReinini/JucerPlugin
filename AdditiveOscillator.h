#pragma once
#include <JuceHeader.h>
#include <vector>
#include <cmath>

class AdditiveOscillator
{
public:
    AdditiveOscillator()
    {
        resetPhase();
        harmonicsAmplitude.resize(maxHarmonics, 1.0f); // Ampiezza predefinita per ogni armonico
        harmonicsPhase.resize(maxHarmonics, 0.0f); // Fase predefinita per ogni armonico
    }

    void setFrequency(float frequency)
    {
        baseFrequency = frequency;
    }

    void setNumHarmonics(int numHarmonics)
    {
        harmonics = std::min(numHarmonics, maxHarmonics);
    }

    void setSampleRate(float newSampleRate)
    {
        sampleRate = newSampleRate;
    }

    void setWaveshapingIntensity(float intensity)
    {
        waveshapingIntensity = intensity;
    }

    void resetPhase()
    {
        for (auto& phase : phases)
            phase = 0.0f;
    }

    // Setta l'ampiezza di uno specifico armonico
    void setHarmonicAmplitude(int harmonicIndex, float amplitude)
    {
        if (harmonicIndex >= 0 && harmonicIndex < harmonicsAmplitude.size())
            harmonicsAmplitude[harmonicIndex] = amplitude;
    }

    // Setta la fase di uno specifico armonico
    void setHarmonicPhase(int harmonicIndex, float phase)
    {
        if (harmonicIndex >= 0 && harmonicIndex < harmonicsPhase.size())
            harmonicsPhase[harmonicIndex] = phase;
    }

    float getNextSample()
    {
        float sample = 0.0f;

        for (int harmonic = 1; harmonic <= harmonics; ++harmonic)
        {
            int index = harmonic - 1;
            float harmonicFrequency = baseFrequency * harmonic;
            float amplitude = harmonicsAmplitude[index];
            float phaseOffset = harmonicsPhase[index];

            // Calcola il campione per l'armonico corrente
            sample += amplitude * std::sin(phases[index] + phaseOffset);

            // Aggiorna la fase
            phases[index] += 2.0f * juce::MathConstants<float>::pi * harmonicFrequency / sampleRate;

            // Rende la fase ciclica
            if (phases[index] > 2.0f * juce::MathConstants<float>::pi)
                phases[index] -= 2.0f * juce::MathConstants<float>::pi;
        }

        // Applica il waveshaping al campione finale
        return std::tanh(waveshapingIntensity * sample);
    }

private:
    const int maxHarmonics = 20; // Numero massimo di armoniche
    float baseFrequency = 440.0f;
    float sampleRate = 48000.0f;
    int harmonics = 5;
    float waveshapingIntensity = 1.0f; // Intensità del waveshaping

    std::vector<float> phases = std::vector<float>(maxHarmonics, 0.0f); // Phase per ogni armonico
    std::vector<float> harmonicsAmplitude; // Ampiezza per ogni armonico
    std::vector<float> harmonicsPhase;     // Fase per ogni armonico
};
