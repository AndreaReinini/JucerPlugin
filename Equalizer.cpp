#include "Equalizer.h"
//preset generi musicali
#include "PluginProcessor.h"


Equalizer::Equalizer(float lowFreq, float midFreq, float highFreq, float Q, double sampleRate) : lowFreq(lowFreq), midFreq(midFreq), highFreq(highFreq), Q(Q), sampleRate(sampleRate)
{
    //myLowCutFrequency=getLowFrequency();
    //myMidCutFrequency=getMidFrequency();
    //myHighCutFrequency=getHighFrequency();
}

void Equalizer::setLowFrequency(float frequency)
{
    lowFreq = frequency;
}

float Equalizer::getSampleRate()
{
    return sampleRate;
}

void Equalizer::setMidFrequency(float frequency)
{
    midFreq = frequency;
}

void Equalizer::setHighFrequency(float frequency)
{
    highFreq = frequency;
}

void Equalizer::setLowGain(float gain)
{
    lowGain = gain;
    
}

void Equalizer::setMidGain(float gain)
{
    midGain = gain;
}

void Equalizer::setHighGain(float gain)
{
    highGain = gain;
}
//aggiunta
void Equalizer::setGainLowSlider(float gainNotNormalized)
{
    lowGainSlider = gainNotNormalized;
    
}

void Equalizer::setGainMidSlider(float gainNotNormalized)
{
    midGainSlider = gainNotNormalized;
}

void Equalizer::setGainHighSlider(float gainNotNormalized)
{
    highGainSlider = gainNotNormalized;
}
float Equalizer::getLowGain()
{
    return lowGain;
    
}

float Equalizer::getMidGain()
{
    return midGain;
}

float Equalizer::getHighGain()
{
    return highGain;
}
float Equalizer::getGainLowSlider()
{
    return lowGainSlider;
    
}

float Equalizer::getGainMidSlider()
{
    return midGainSlider;
}

float Equalizer::getGainHighSlider()
{
    return highGainSlider;
}
void Equalizer::setQ(float qualityFactor)
{
    Q = qualityFactor;
}

void Equalizer::setSampleRate(double sampleRate)
{
    sampleRate = sampleRate;
}

float Equalizer::getLowFrequency()
{
    return lowFreq;
}

float Equalizer::getMidFrequency()
{
    return midFreq;
}

float Equalizer::getHighFrequency()
{
    return highFreq;
}

float Equalizer::getQ()
{
    return Q;
}

void Equalizer::prepare()
{
    lowPassFilter.reset();
    bandPassFilter.reset();
    highPassFilter.reset();
    
    
    lowPassFilter.setCoefficients(juce::IIRCoefficients::makeLowPass(getSampleRate(), getLowFrequency(),0.5f));
    bandPassFilter.setCoefficients(juce::IIRCoefficients::makeBandPass(getSampleRate(), getMidFrequency(), 0.7f));
    highPassFilter.setCoefficients(juce::IIRCoefficients::makeHighPass(getSampleRate(), getHighFrequency(),1.0f));
    
    
    /*if (sampleRate != getSampleRate() || lowFreq != getLowFrequency() || midFreq != getMidFrequency() || highFreq != getHighFrequency() || Q != getQ())
       {
           lowPassFilter.setCoefficients(juce::IIRCoefficients::makeLowPass(getSampleRate(), getLowFrequency()));
           bandPassFilter.setCoefficients(juce::IIRCoefficients::makeBandPass(getSampleRate(), getMidFrequency(), getQ()));
           highPassFilter.setCoefficients(juce::IIRCoefficients::makeHighPass(getSampleRate(), getHighFrequency()));
       }*/
}
void Equalizer::prepareV4()
{
    // Nel metodo prepare()
    lowBuffer.setSize(2,  bufferSize);  // Supponendo un massimo di 2 canali
    midBuffer.setSize(2, bufferSize);
    highBuffer.setSize(2, bufferSize);
    
    lowPassFilter.setCoefficients(juce::IIRCoefficients::makeLowPass(sampleRate, lowFreq, 0.5f));
    bandPassFilter.setCoefficients(juce::IIRCoefficients::makeBandPass(sampleRate, midFreq, 0.7f));
    highPassFilter.setCoefficients(juce::IIRCoefficients::makeHighPass(sampleRate, highFreq, 1.0f));
}
 //Versione originale
 void Equalizer::process(juce::AudioBuffer<float>& buffer)
{
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        juce::AudioBuffer<float> lowBuffer(buffer.getNumChannels(), buffer.getNumSamples());
        lowBuffer.copyFrom(channel, 0, buffer, channel, 0, buffer.getNumSamples());
        lowPassFilter.processSamples(lowBuffer.getWritePointer(channel), buffer.getNumSamples());

        juce::AudioBuffer<float> midBuffer(buffer.getNumChannels(), buffer.getNumSamples());
        midBuffer.copyFrom(channel, 0, buffer, channel, 0, buffer.getNumSamples());
        bandPassFilter.processSamples(midBuffer.getWritePointer(channel), buffer.getNumSamples());

        juce::AudioBuffer<float> highBuffer(buffer.getNumChannels(), buffer.getNumSamples());
        highBuffer.copyFrom(channel, 0, buffer, channel, 0, buffer.getNumSamples());
        highPassFilter.processSamples(highBuffer.getWritePointer(channel), buffer.getNumSamples());

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            channelData[sample] = lowBuffer.getSample(channel, sample) * lowGain
                + midBuffer.getSample(channel, sample) * midGain
                + highBuffer.getSample(channel, sample) * highGain;
        }
    }
    
    
    
    
    //in più
}
 //V2
  void Equalizer::processV2(juce::AudioBuffer<float>& buffer)
 {
     // Buffers temporanei per ogni banda di frequenza
     juce::AudioBuffer<float> lowBuffer(buffer.getNumChannels(), buffer.getNumSamples());
     juce::AudioBuffer<float> midBuffer(buffer.getNumChannels(), buffer.getNumSamples());
     juce::AudioBuffer<float> highBuffer(buffer.getNumChannels(), buffer.getNumSamples());

     // Copia l'intero buffer di input nei buffer temporanei una sola volta
     lowBuffer.makeCopyOf(buffer);
     midBuffer.makeCopyOf(buffer);
     highBuffer.makeCopyOf(buffer);

     // Applica ciascun filtro ai rispettivi buffer temporanei
     for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
     {
         lowPassFilter.processSamples(lowBuffer.getWritePointer(channel), buffer.getNumSamples());
         bandPassFilter.processSamples(midBuffer.getWritePointer(channel), buffer.getNumSamples());
         highPassFilter.processSamples(highBuffer.getWritePointer(channel), buffer.getNumSamples());
         auto* channelData = buffer.getWritePointer(channel);
         //auto* lowData = lowBuffer.getReadPointer(channel);
         //auto* midData = midBuffer.getReadPointer(channel);
         //auto* highData = highBuffer.getReadPointer(channel);
         
         for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
         {
             channelData[sample] = juce::jlimit(-1.0f, 1.0f,(lowBuffer.getSample(channel, sample) * lowGain)+ (midBuffer.getSample(channel, sample) * midGain)+ (highBuffer.getSample(channel, sample) * highGain));
         }
     }

     // Somma i risultati filtrati nel buffer di output, applicando i guadagni
     //for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
     //{
        
         //for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
         //{
           //  channelData[sample] = juce::jlimit(-1.0f, 1.0f,
             //    (lowData[sample] * lowGain) +
               //  (midData[sample] * midGain) +
                 //(highData[sample] * highGain)
             //);
         //}
     //}
 }
//V3

void Equalizer::processV3(juce::AudioBuffer<float>& buffer)
{
    juce::AudioBuffer<float> lowBuffer(buffer.getNumChannels(), buffer.getNumSamples());
    juce::AudioBuffer<float> midBuffer(buffer.getNumChannels(), buffer.getNumSamples());
    juce::AudioBuffer<float> highBuffer(buffer.getNumChannels(), buffer.getNumSamples());

    // Copia il buffer nei buffer temporanei
    lowBuffer.makeCopyOf(buffer);
    midBuffer.makeCopyOf(buffer);
    highBuffer.makeCopyOf(buffer);

    // Applica i filtri
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        lowPassFilter.processSamples(lowBuffer.getWritePointer(channel), buffer.getNumSamples());
        bandPassFilter.processSamples(midBuffer.getWritePointer(channel), buffer.getNumSamples());
        highPassFilter.processSamples(highBuffer.getWritePointer(channel), buffer.getNumSamples());
    }

    // Combina i risultati con guadagni bilanciati
    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float lowSample = lowBuffer.getSample(channel, sample) * lowGain;
            float midSample = midBuffer.getSample(channel, sample) * midGain;
            float highSample = highBuffer.getSample(channel, sample) * highGain;
            float outputSample = lowSample + midSample + highSample;
            if (outputSample > 1.0)
                {
                    lowGain /= outputSample;
                    midGain /= outputSample;
                    highGain /= outputSample;
                }
            outputSample = lowSample * lowGain + midSample * midGain + highSample * highGain;
            // Limita il segnale tra -1.0f e 1.0f per evitare il clipping
            outputSample = std::clamp(outputSample, -1.0f, 1.0f);
            // Limita il risultato combinato
            channelData[sample] = juce::jlimit(-1.0f, 1.0f, outputSample);
        }
    }
}
void Equalizer::processV4(juce::AudioBuffer<float>& buffer)
{
    float preComputedLowGain = lowGain;
    float preComputedMidGain = midGain;
    float preComputedHighGain = highGain;

    for (int channel = 0; channel < buffer.getNumChannels(); ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        auto* lowData = lowBuffer.getReadPointer(channel);
        auto* midData = midBuffer.getReadPointer(channel);
        auto* highData = highBuffer.getReadPointer(channel);

        for (int sample = 0; sample < buffer.getNumSamples(); ++sample)
        {
            float outputSample = (lowData[sample] * preComputedLowGain) +
                                 (midData[sample] * preComputedMidGain) +
                                 (highData[sample] * preComputedHighGain);
            channelData[sample] = std::clamp(outputSample, -1.0f, 1.0f);
        }
    }
}

void Equalizer::applyPreset(Preset preset)
{
    switch (preset)
    {
        case Preset::Flat:
            setGainLowSlider(0.0f);
            setGainMidSlider(0.0f);
            setGainHighSlider(0.0f);
            setLowGain(dBToLinear(0.0f));
            setMidGain(dBToLinear(0.0f));
            setHighGain(dBToLinear(0.0f));
            setLowFrequency(200.0f);
            setMidFrequency(1000.0f);
            setHighFrequency(5000.0f);
            break;

        case Preset::Rock:
            setGainLowSlider(6.0f);
            setGainMidSlider(-3.0f);
            setGainHighSlider(6.0f);
            setLowGain(dBToLinear(6.0f));   // Enfatizza i bassi
            setMidGain(dBToLinear(-3.0f));   // Un po' di enfasi sui medi
            setHighGain(dBToLinear(6.0f));  // Alti bilanciati
            setLowFrequency(80.0f);
            setMidFrequency(1500.0f);
            setHighFrequency(6000.0f);
            break;

        case Preset::Disco:
            setGainLowSlider(4.0f);
            setGainMidSlider(-2.0f);
            setGainHighSlider(5.0f);
            setLowGain(dBToLinear(4.0f));
            setMidGain(dBToLinear(-2.0f));
            setHighGain(dBToLinear(5.0f));  // Enfatizza gli alti per i piatti
            setLowFrequency(100.0f);
            setMidFrequency(1200.0f);
            setHighFrequency(8000.0f);
            break;

        case Preset::Jazz:
            setGainLowSlider(3.0f);
            setGainMidSlider(0.0f);
            setGainHighSlider(3.0f);
            setLowGain(dBToLinear(3.0f));
            setMidGain(dBToLinear(0.0f));   // Medi enfatizzati per strumenti acustici
            setHighGain(dBToLinear(3.0f));
            setLowFrequency(120.0f);
            setMidFrequency(1000.0f);
            setHighFrequency(5000.0f);
            break;

        case Preset::Pop:
            setGainLowSlider(2.0f);
            setGainMidSlider(5.0f);
            setGainHighSlider(2.0f);
            setLowGain(dBToLinear(2.0f));
            setMidGain(dBToLinear(5.0f));
            setHighGain(dBToLinear(2.0f));  // Enfatizza gli alti per le voci
            setLowFrequency(100.0f);
            setMidFrequency(1200.0f);
            setHighFrequency(7000.0f);
            break;

        case Preset::Classical:
            setGainLowSlider(-2.0f);
            setGainMidSlider(0.0f);
            setGainHighSlider(-2.0f);
            setLowGain(dBToLinear(-2.0f));
            setMidGain(dBToLinear(0.0f));
            setHighGain(dBToLinear(-2.0f));  // Alti bilanciati
            setLowFrequency(90.0f);
            setMidFrequency(1000.0f);
            setHighFrequency(5000.0f);
            break;
            
        case Preset::HipHop:
            setGainLowSlider(7.0f);
            setGainMidSlider(-3.0f);
            setGainHighSlider(4.0f);
            setLowGain(dBToLinear(7.0f));
            setMidGain(dBToLinear(-3.0f));
            setHighGain(dBToLinear(4.0f));
            setLowFrequency(70.0f);
            setMidFrequency(1200.0f);
            setHighFrequency(5000.0f);
            break;
            
        case Preset::Metal:
            setGainLowSlider(5.0f);
            setGainMidSlider(0.0f);
            setGainHighSlider(6.0f);
            setLowGain(dBToLinear(5.0f));
            setMidGain(dBToLinear(0.0f));
            setHighGain(dBToLinear(6.0f));
            setLowFrequency(100.0f);
            setMidFrequency(1500.0f);
            setHighFrequency(8000.0f);
            break;
        case Preset::Dance:
            setGainLowSlider(6.0f);
            setGainMidSlider(-2.0f);
            setGainHighSlider(5.0f);
            setLowGain(dBToLinear(6.0f));
            setMidGain(dBToLinear(-2.0f));
            setHighGain(dBToLinear(5.0f));
            setLowFrequency(80.0f);
            setMidFrequency(1200.0f);
            setHighFrequency(10000.0f);
            break;
        case Preset::VocalBoost:
            setGainLowSlider(0.0f);
            setGainMidSlider(6.0f);
            setGainHighSlider(2.0f);
            setLowGain(dBToLinear(0.0f));
            setMidGain(dBToLinear(6.0f));
            setHighGain(dBToLinear(2.0f));
            setLowFrequency(150.0f);
            setMidFrequency(1000.0f);
            setHighFrequency(5000.0f);
            break;
        case Preset::BassBoost:
            setGainLowSlider(8.0f);
            setGainMidSlider(-2.0f);
            setGainHighSlider(0.0f);
            setLowGain(dBToLinear(8.0f));
            setMidGain(dBToLinear(-2.0f));
            setHighGain(dBToLinear(0.0f));
            setLowFrequency(60.0f);
            setMidFrequency(1000.0f);
            setHighFrequency(5000.0f);
            break;
            
        case Preset::TrebleBoost:
            setGainLowSlider(0.0f);
            setGainMidSlider(0.0f);
            setGainHighSlider(8.0f);
            setLowGain(dBToLinear(0.0f));
            setMidGain(dBToLinear(0.0f));
            setHighGain(dBToLinear(8.0f));
            setLowFrequency(100.0f);
            setMidFrequency(1000.0f);
            setHighFrequency(8000.0f);
            break;
        case Preset::MyEqu:
            setGainLowSlider(myEqLow);
            setGainMidSlider(myEqMid);
            setGainHighSlider(myEqHigh);
            setLowGain(dBToLinear(myEqLow));
            setMidGain(dBToLinear(myEqMid));
            setHighGain(dBToLinear(myEqHigh));
            setLowFrequency(myLowCutFrequency);
            setMidFrequency(myMidCutFrequency);
            setHighFrequency(myHighCutFrequency);

    }
    
    prepare(); // Ricalcola i filtri con i nuovi valori
    
}
float Equalizer::dBToLinear(float dB)
{
    return std::pow(10.0f, dB / 20.0f); // Conversione da dB a lineare
}







