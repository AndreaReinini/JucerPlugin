#include "PluginEditor.h"

AdditiveSynthWithWaveshapingAudioProcessorEditor::AdditiveSynthWithWaveshapingAudioProcessorEditor(AdditiveSynthWithWaveshapingAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    setSize(1920, 1080);
    // Configurazione del ComboBox
    modeSelector.addItem("Input Only", 1);
    modeSelector.addItem("Generated Only", 2);
    modeSelector.addItem("Input and Generated", 3);
    modeSelector.setSelectedId(1);
    //Configurazione ComboBox Preset generi musicali
    presetSelector.addItem("Flat", static_cast<int>(Preset::Flat));
    presetSelector.addItem("Rock", static_cast<int>(Preset::Rock));
    presetSelector.addItem("Disco", static_cast<int>(Preset::Disco));
    presetSelector.addItem("Jazz", static_cast<int>(Preset::Jazz));
    presetSelector.addItem("Pop", static_cast<int>(Preset::Pop));
    presetSelector.addItem("Classical", static_cast<int>(Preset::Classical));
    presetSelector.addItem("HipHop", static_cast<int>(Preset::HipHop));
    presetSelector.addItem("Metal", static_cast<int>(Preset::Metal));
    presetSelector.addItem("Dance", static_cast<int>(Preset::Dance));
    presetSelector.addItem("VocalBoost", static_cast<int>(Preset::VocalBoost));
    presetSelector.addItem("BassBoost", static_cast<int>(Preset::BassBoost));
    presetSelector.addItem("TrebleBoost", static_cast<int>(Preset::TrebleBoost));
    presetSelector.addItem("MyEq", static_cast<int>(Preset::MyEqu));
    presetSelector.onChange = [this]()
    {
        auto selectedPreset = static_cast<Preset>(presetSelector.getSelectedId());
        audioProcessor.equalizer.applyPreset(selectedPreset);
        if(presetSelector.getSelectedId()==1)
        {
            audioProcessor.dontCallProcess=true;
        }
        else
        {
            audioProcessor.dontCallProcess=false;
        }
        if(presetSelector.getSelectedId()==static_cast<int>(Preset::MyEqu))
        {
            DBG("Selected preset ID: " << presetSelector.getSelectedId());
            addAndMakeVisible(lowCutFrequencyUser);
            addAndMakeVisible(midCutFrequencyUser);
            addAndMakeVisible(highCutFrequencyUser);
            lowCutFrequencyUser.setVisible(true);
            midCutFrequencyUser.setVisible(true);
            highCutFrequencyUser.setVisible(true);
            resized();
        }
        else
        {
            lowCutFrequencyUser.setVisible(false);
            midCutFrequencyUser.setVisible(false);
            highCutFrequencyUser.setVisible(false);
            resized();
        }
        updateFrequencyText();
        updateEqualizerSliders();
    };

    // Associa il callback per aggiornare la modalità
    modeSelector.onChange = [this]() {
    audioProcessor.mode = static_cast<ProcessingMode>(modeSelector.getSelectedId() - 1);
    };

    // Rendi visibile il ComboBox
    addAndMakeVisible(modeSelector);
    // Slider per il gain
    gainSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    gainSlider.setRange(0.0, 2.0, 0.01);  // Range da 0.0 a 2.0 per aumentare o ridurre il volume
    gainSlider.setValue(1.0);  // Valore di default
    gainSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 50, 20);
    gainSlider.onValueChange = [this] { audioProcessor.outputGain.store(gainSlider.getValue()); };
    gainLabel.setText("Volume",juce::dontSendNotification);
    addAndMakeVisible(gainLabel);
    addAndMakeVisible(&gainSlider);
    // Slider per la frequenza
    frequencySlider.setRange(10.0, 1000.0,1.0);
    frequencySlider.setValue(audioProcessor.frequency);
    frequencySlider.addListener(this);
    addAndMakeVisible(frequencySlider);

    frequencyLabel.setText("Frequency", juce::dontSendNotification);
    addAndMakeVisible(frequencyLabel);

    // Slider per il numero di armoniche
    harmonicsSlider.setRange(1, 20,1);
    harmonicsSlider.setValue(audioProcessor.numHarmonics);
    harmonicsSlider.addListener(this);
    addAndMakeVisible(harmonicsSlider);

    harmonicsLabel.setText("Harmonics", juce::dontSendNotification);
    addAndMakeVisible(harmonicsLabel);

    
    AmplitudeHarmonicSlider.setRange(0.0, 1.0,0.01);
    AmplitudeHarmonicSlider.setValue(0.0);
    AmplitudeHarmonicSlider.addListener(this);
    addAndMakeVisible(AmplitudeHarmonicSlider);
    AmplitudeHarmonicSlider.onValueChange=[this](){
        audioProcessor.onPhaseSliderChange(0, AmplitudeHarmonicSlider.getValue());
    };
    AmplitudeHarmonicLabel.setText("Harmonics Amplitude", juce::dontSendNotification);
    addAndMakeVisible(AmplitudeHarmonicLabel);
    
    PhaseArmonicSlider.setRange(0.0, 360.0, 1.0); // da 0 a 360 gradi con step di 1
    PhaseArmonicSlider.setValue(0.0); // Fase di default a 0 gradi
    PhaseArmonicSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 50, 20);
    PhaseArmonicSlider.setTooltip("Imposta la fase iniziale in gradi (0 - 360)");
    PhaseArmonicSlider.onValueChange=[this](){
        audioProcessor.onPhaseSliderChange(0, PhaseArmonicSlider.getValue());
    };
    addAndMakeVisible(PhaseArmonicSlider);
    
    PhaseArmonicLabel.setText("Harmonics Phase", juce::dontSendNotification);
    addAndMakeVisible(PhaseArmonicLabel);
    // Slider per l'intensità del waveshaping
    intensitySlider.setRange(0.1, 10.0, 0.1); // Imposta il limite massimo a 10.0
    intensitySlider.setValue(audioProcessor.waveshapingIntensity);
    intensitySlider.addListener(this);
    intensitySlider.onValueChange=[this](){
        audioProcessor.waveshapingIntensity=intensitySlider.getValue();
        audioProcessor.waveshaper.setIntensity(audioProcessor.waveshapingIntensity);
    };
    addAndMakeVisible(intensitySlider);
    
    WaveshapingActivatorLabel.setText("Waveshaper Activator",  juce::dontSendNotification);
    addAndMakeVisible(WaveshapingActivatorLabel);
    //Toggle StateWaveshaping
    waveshaperOnOff.onClick=[this](){
        bool isChecked=waveshaperOnOff.getToggleState();
        if(isChecked)
        {
            audioProcessor.waveshaper.waveshapingState=true;
            std::cout<<audioProcessor.getSampleRate();
        }
        else
        {
            audioProcessor.waveshaper.waveshapingState=false;
        }
    };
    addAndMakeVisible(waveshaperOnOff);
    intensityLabel.setText("Waveshaping Intensity", juce::dontSendNotification);
    addAndMakeVisible(intensityLabel);
    // Slider Drive
    driveSlider.setRange(0.1, 10.0, 0.1);
    driveSlider.setValue(audioProcessor.drive);
    driveSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    driveSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 50, 20);
    driveSlider.addListener(this);
    driveSlider.onValueChange = [this]() {
        audioProcessor.drive = driveSlider.getValue();
        audioProcessor.waveshaper.setDrive(audioProcessor.drive);
    };
    driveLabel.setText("Drive", juce::dontSendNotification);
    addAndMakeVisible(driveSlider);
    addAndMakeVisible(driveLabel);

    // Slider Symmetry
    symmetrySlider.setRange(-1.0, 1.0, 0.01);  // Range da -1 a 1 per la simmetria
    symmetrySlider.setValue(audioProcessor.symmetry);
    symmetrySlider.setSliderStyle(juce::Slider::LinearHorizontal);
    symmetrySlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 50, 20);
    symmetrySlider.addListener(this);
    symmetrySlider.onValueChange = [this]() {
        audioProcessor.symmetry = symmetrySlider.getValue();
        audioProcessor.waveshaper.setSymmetry(audioProcessor.symmetry);
    };
    symmetryLabel.setText("Symmetry", juce::dontSendNotification);
    addAndMakeVisible(symmetrySlider);
    addAndMakeVisible(symmetryLabel);

    // Slider Amount
    amountSlider.setRange(0.0, 1.0, 0.01);  // Da 0 a 1 per il controllo dell'intensità del waveshaping
    amountSlider.setValue(audioProcessor.amount);
    amountSlider.setSliderStyle(juce::Slider::LinearHorizontal);
    amountSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 50, 20);
    amountSlider.addListener(this);
    amountSlider.onValueChange=[this]()
    {
        audioProcessor.amount=amountSlider.getValue();
        audioProcessor.waveshaper.setAmount(audioProcessor.amount);
    };
    amountLabel.setText("Amount", juce::dontSendNotification);
    addAndMakeVisible(amountSlider);
    addAndMakeVisible(amountLabel);
    //parte grafica dell'equalizzatore a tre bande
    lowGainSlider.setNumDecimalPlacesToDisplay(1);
    midGainSlider.setNumDecimalPlacesToDisplay(1);
    highGainSlider.setNumDecimalPlacesToDisplay(1);
    // Low Gain Slider
    lowGainLabel.setText("Bassi", juce::dontSendNotification);
    addAndMakeVisible(lowGainLabel);

    lowGainSlider.setRange(-12.0, 12.0, 0.1);   // Gain in dB
    lowGainSlider.setValue(0.0);
    lowGainSlider.onValueChange = [this]()
    {
        audioProcessor.equalizer.setLowGain(audioProcessor.equalizer.dBToLinear(lowGainSlider.getValue()));
        if(presetSelector.getSelectedId()==13)
        {
            audioProcessor.equalizer.myEqLow=lowGainSlider.getValue();
        }
        checkValueGain();
    };
    addAndMakeVisible(lowGainSlider);
    
    // Mid Gain Slider
    midGainLabel.setText("Medi", juce::dontSendNotification);
    addAndMakeVisible(midGainLabel);

    midGainSlider.setRange(-12.0, 12.0, 0.1);
    midGainSlider.setValue(0.0);
    midGainSlider.onValueChange = [this]()
    {
        audioProcessor.equalizer.setMidGain(audioProcessor.equalizer.dBToLinear(midGainSlider.getValue()));
        if(presetSelector.getSelectedId()==13)
        {
            audioProcessor.equalizer.myEqMid=midGainSlider.getValue();
        }
        checkValueGain();
    };
    addAndMakeVisible(midGainSlider);

    // High Gain Slider
    highGainLabel.setText("Alti", juce::dontSendNotification);
    addAndMakeVisible(highGainLabel);

    highGainSlider.setRange(-12.0, 12.0, 0.1);
    highGainSlider.setValue(0.0);
    highGainSlider.onValueChange = [this]()
    {
        audioProcessor.equalizer.setHighGain(audioProcessor.equalizer.dBToLinear(highGainSlider.getValue()));
        if(presetSelector.getSelectedId()==13)
        {
            audioProcessor.equalizer.myEqHigh=highGainSlider.getValue();
        }
        checkValueGain();
    };
    addAndMakeVisible(highGainSlider);
    
    updateFrequencyText();
    lowCutFrequencyUser.setRange(30, 300,10);
    lowCutFrequencyUser.setValue(audioProcessor.equalizer.getLowFrequency());
    lowCutFrequencyUser.onValueChange=[this]()
    {
        audioProcessor.equalizer.setLowFrequency(lowCutFrequencyUser.getValue());
        audioProcessor.equalizer.myLowCutFrequency=lowCutFrequencyUser.getValue();
        updateFrequencyText();
        audioProcessor.equalizer.prepare();
    };
  
    
    midCutFrequencyUser.setRange(500, 2500,10);
    midCutFrequencyUser.setValue(audioProcessor.equalizer.getMidFrequency());
    midCutFrequencyUser.onValueChange=[this]()
    {
        audioProcessor.equalizer.setMidFrequency(midCutFrequencyUser.getValue());
        audioProcessor.equalizer.myMidCutFrequency=midCutFrequencyUser.getValue();
        updateFrequencyText();
        audioProcessor.equalizer.prepare();
    };
    
    highCutFrequencyUser.setRange(4000, 8000,10);
    highCutFrequencyUser.setValue(audioProcessor.equalizer.getHighFrequency());
    highCutFrequencyUser.onValueChange=[this]()
    {
        audioProcessor.equalizer.setHighFrequency(highCutFrequencyUser.getValue());
        audioProcessor.equalizer.myHighCutFrequency=highCutFrequencyUser.getValue();
        updateFrequencyText();
        audioProcessor.equalizer.prepare();
    };
    
    
    lowFreqLabel.setJustificationType(juce::Justification::centred);
    midFreqLabel.setJustificationType(juce::Justification::centred);
    highFreqLabel.setJustificationType(juce::Justification::centred);

    addAndMakeVisible(lowFreqLabel);
    addAndMakeVisible(midFreqLabel);
    addAndMakeVisible(highFreqLabel);

    //Analizzatore di Spettro
    
    addAndMakeVisible(audioProcessor.spectrumAnalyzer);

    juce::Timer::callAfterDelay(50, [this]() { loadSettingsFromFile(); });
}

AdditiveSynthWithWaveshapingAudioProcessorEditor::~AdditiveSynthWithWaveshapingAudioProcessorEditor() {
    saveSettingsToFile();
}

void AdditiveSynthWithWaveshapingAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colours::black);
    g.setColour(juce::Colours::white);
    g.setFont(20.0f);
    g.drawFittedText("Additive Synth with Waveshaping, Equalizer and Spectrum Analyzer", getLocalBounds(), juce::Justification::top, 1);
}

void AdditiveSynthWithWaveshapingAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced(40);

    //Analizzatore di Spettro
    
    audioProcessor.spectrumAnalyzer.setBounds(area.removeFromBottom(170).withTrimmedTop(20));
    
    
    frequencyLabel.setBounds(area.removeFromTop(20));
    frequencySlider.setBounds(area.removeFromTop(30));

    harmonicsLabel.setBounds(area.removeFromTop(20));
    harmonicsSlider.setBounds(area.removeFromTop(30));
    
    AmplitudeHarmonicLabel.setBounds(area.removeFromTop(20));
    AmplitudeHarmonicSlider.setBounds(area.removeFromTop(30));
    
    PhaseArmonicLabel.setBounds(area.removeFromTop(20));
    PhaseArmonicSlider.setBounds(area.removeFromTop(30));
    WaveshapingActivatorLabel.setBounds(area.removeFromTop(20));
    waveshaperOnOff.setBounds(area.removeFromTop(20));
    intensityLabel.setBounds(area.removeFromTop(20));
    intensitySlider.setBounds(area.removeFromTop(30));
   
    // Posiziona gli slider di Drive, Symmetry e Amount
    driveLabel.setBounds(area.removeFromTop(20));
    driveSlider.setBounds(area.removeFromTop(30));
        
    symmetryLabel.setBounds(area.removeFromTop(20));
    symmetrySlider.setBounds(area.removeFromTop(30));

    amountLabel.setBounds(area.removeFromTop(20));
    amountSlider.setBounds(area.removeFromTop(30));
    
    
    gainLabel.setBounds(area.removeFromTop(20)); // Label per il gain
    gainSlider.setBounds(area.removeFromTop(30)); // Posiziona lo slider del gain nello spazio disponibile
    
    //parte equalizzatore
    lowGainLabel.setBounds(area.removeFromTop(20));
    lowGainSlider.setBounds(area.removeFromTop(30));

    midGainLabel.setBounds(area.removeFromTop(20));
    midGainSlider.setBounds(area.removeFromTop(30));

    highGainLabel.setBounds(area.removeFromTop(20));
    highGainSlider.setBounds(area.removeFromTop(30));
    
    lowFreqLabel.setBounds(area.removeFromTop(20));
    midFreqLabel.setBounds(area.removeFromTop(20));
    highFreqLabel.setBounds(area.removeFromTop(20));
    
    
    lowCutFrequencyUser.setBounds(area.removeFromTop(20));
    midCutFrequencyUser.setBounds(area.removeFromTop(20));
    highCutFrequencyUser.setBounds(area.removeFromTop(20));
    //comboBox
    
    modeSelector.setBounds(area.removeFromTop(20)); // Imposta x, y, larghezza e altezza
    area.removeFromTop(20);
    //Combobox Generi
    
    presetSelector.setBounds(area.removeFromTop(20)); // Posiziona la ComboBox
    addAndMakeVisible(presetSelector);
}

void AdditiveSynthWithWaveshapingAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &frequencySlider)
        audioProcessor.frequency = frequencySlider.getValue();
    else if (slider == &harmonicsSlider)
        audioProcessor.numHarmonics = harmonicsSlider.getValue();
    else if (slider == &intensitySlider)
        audioProcessor.waveshapingIntensity = intensitySlider.getValue();

    audioProcessor.oscillator.setFrequency(audioProcessor.frequency);
    audioProcessor.oscillator.setNumHarmonics(audioProcessor.numHarmonics);
    audioProcessor.waveshaper.setIntensity(audioProcessor.waveshapingIntensity);
}
void AdditiveSynthWithWaveshapingAudioProcessorEditor::updateEqualizerSliders()
{
    lowGainSlider.setValue(audioProcessor.equalizer.getGainLowSlider(), juce::dontSendNotification);
    midGainSlider.setValue(audioProcessor.equalizer.getGainMidSlider(), juce::dontSendNotification);
    highGainSlider.setValue(audioProcessor.equalizer.getGainHighSlider(), juce::dontSendNotification);
}
void AdditiveSynthWithWaveshapingAudioProcessorEditor::updateFrequencyText()
{
    lowFreqLabel.setText("Low Cut: "+ juce::String(audioProcessor.equalizer.getLowFrequency())+" Hz", juce::dontSendNotification);
    midFreqLabel.setText("Mid Freq: "+juce::String(audioProcessor.equalizer.getMidFrequency())+" Hz", juce::dontSendNotification);
    highFreqLabel.setText("High Cut: "+juce::String(audioProcessor.equalizer.getHighFrequency())+" Hz", juce::dontSendNotification);
}

void AdditiveSynthWithWaveshapingAudioProcessorEditor::checkValueGain()
{
    if((lowGainSlider.getValue()==0.0 && midGainSlider.getValue()==0.0 && highGainSlider.getValue()==0.0) || presetSelector.getSelectedId()==1|| (audioProcessor.equalizer.myEqLow==0.0f && audioProcessor.equalizer.myEqMid==0.0f && audioProcessor.equalizer.myEqHigh==0.0f) )
    {
        audioProcessor.dontCallProcess=true;
    }
    else
    {
        audioProcessor.dontCallProcess=false;
    }
}
void AdditiveSynthWithWaveshapingAudioProcessorEditor::saveSettingsToFile()
{
    juce::File settingsFile = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
                              .getChildFile("EqualizerSettings.json");

    auto settings = new juce::DynamicObject(); // Creazione come oggetto puntato
// Associato a un var che gestisce il ciclo di vita
    settings->setProperty("lowCutFrequency",  lowCutFrequencyUser.getValue());
    settings->setProperty("midCutFrequency", midCutFrequencyUser.getValue());
    settings->setProperty("highCutFrequency", highCutFrequencyUser.getValue());
    settings->setProperty("Bassi", audioProcessor.equalizer.myEqLow);
    settings->setProperty("Medi", audioProcessor.equalizer.myEqMid);
    settings->setProperty("Alti", audioProcessor.equalizer.myEqHigh);
    //settings->setProperty("preset", presetSelector.getSelectedId());

    juce::var json(settings); // Condividi il ciclo di vita con var
    settingsFile.replaceWithText(juce::JSON::toString(json));
   
}
void AdditiveSynthWithWaveshapingAudioProcessorEditor:: loadSettingsFromFile()
{
    juce::File settingsFile = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
                              .getChildFile("EqualizerSettings.json");

    if (settingsFile.existsAsFile())
    {
        auto json = juce::JSON::parse(settingsFile);
        if (auto* settings = json.getDynamicObject())
        {
            lowCutFrequencyUser.setValue(settings->getProperty("lowCutFrequency"));
            audioProcessor.equalizer.myLowCutFrequency=settings->getProperty("lowCutFrequency");
            audioProcessor.equalizer.myEqLow=settings->getProperty("Bassi");
            midCutFrequencyUser.setValue(settings->getProperty("midCutFrequency"));
            audioProcessor.equalizer.myMidCutFrequency=settings->getProperty("midCutFrequency");
            audioProcessor.equalizer.myEqMid= settings->getProperty("Medi");
            highCutFrequencyUser.setValue(settings->getProperty("highCutFrequency"));
            audioProcessor.equalizer.myHighCutFrequency=settings->getProperty("highCutFrequency");
            audioProcessor.equalizer.myEqHigh=settings->getProperty("Alti");
            //presetSelector.setSelectedId(settings->getProperty("preset"));
            std::cout<<"Eccoci qui sto caricando il file";

        }
    }
    else
    {
        lowCutFrequencyUser.setValue(audioProcessor.equalizer.getLowFrequency());
        midCutFrequencyUser.setValue(audioProcessor.equalizer.getMidFrequency());
        highCutFrequencyUser.setValue(audioProcessor.equalizer.getHighFrequency());
    }
}
