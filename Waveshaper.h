#pragma once
#include <JuceHeader.h>

class Waveshaper
{
public:
    Waveshaper() = default;

    void setIntensity(float newIntensity)
    {
        intensity = newIntensity;
        updateCompensatingGain();
    }

    void setDrive(float newDrive) { drive = newDrive; }
    void setSymmetry(float newSymmetry) { symmetry = newSymmetry; }
    void setAmount(float newAmount) { amount = newAmount; }

    float processSample(float inputSample)
    {
        // Applica il parametro di Drive per amplificare l’ingresso
        float shapedSample = drive * inputSample;

        // Modifica della simmetria: applica simmetria asimmetrica in base al valore impostato
        shapedSample = (1.0f - symmetry) * shapedSample + symmetry * std::abs(shapedSample);

        // Applica la curva di waveshaping con l'intensità e l’amount
        float threshold = amount; // Controlla la quantità di distorsione
        if (shapedSample > threshold)
            shapedSample = threshold + (shapedSample - threshold) / (1 + (shapedSample - threshold) * (shapedSample - threshold));
        else if (shapedSample < -threshold)
            shapedSample = -threshold + (shapedSample + threshold) / (1 + (shapedSample + threshold) * (shapedSample + threshold));

        // Applica l'intensità come fattore di scaling
        shapedSample *= intensity;

        // Compensa il gain in uscita
        return shapedSample * compensatingGain;
    }
    bool waveshapingState=true;
private:
    float intensity = 1.0f;
    float drive = 1.0f;
    float symmetry = 0.0f;
    float amount = 1.0f;
    float compensatingGain;

    void updateCompensatingGain()
    {
        compensatingGain = 1.0f / (1.0f + intensity * 0.5f); // Compensa il volume in uscita
    }
};
