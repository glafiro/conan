#pragma once

#include <JuceHeader.h>

#include <vector>
#include <array>
#include <unordered_map>
using std::vector;
using std::array;
using std::unordered_map;

#include "DSPParameters.h"
#include "MultibandDistortion.h"
#include "Utils.h"
#include "APVTSParameter.h"

#define MIN_DB  -60.0f
#define MAX_DB  12.0f

enum ParameterNames{
    INPUT_GAIN_1, OUTPUT_GAIN_1, 
    DRIVE_1, KNEE_1,
    BIT_1,
    BYPASS_1,

    INPUT_GAIN_2, OUTPUT_GAIN_2, 
    DRIVE_2, KNEE_2,
    BIT_2,
    BYPASS_2,

    INPUT_GAIN_3, OUTPUT_GAIN_3, 
    DRIVE_3, KNEE_3,
    BIT_3,
    BYPASS_3,

    MIX,
    INPUT_GLOBAL, OUTPUT_GLOBAL,
    BYPASS,

    LOW_MID_CUT, MID_HIGH_CUT,
    PARAMETER_COUNT
};

static std::array<std::unique_ptr<IAPVTSParameter>, ParameterNames::PARAMETER_COUNT> apvtsParameters{
    std::make_unique<APVTSParameterFloat> ("inputGain1",      "Low Input Gain",        0.0f),
    std::make_unique<APVTSParameterFloat> ("outputGain1",     "Low Output Gain",       0.0f),
    std::make_unique<APVTSParameterFloat> ("drive1",          "Low Drive",             0.0f),
    std::make_unique<APVTSParameterFloat> ("knee1",           "Low Knee",              1.0f),
    std::make_unique<APVTSParameterInt>   ("bit1",            "Low Bit",               16),
    std::make_unique<APVTSParameterBool>  ("bypass1",         "Low Bypass",            false),
    std::make_unique<APVTSParameterFloat> ("inputGain2",      "Mid Input Gain",        0.0f),
    std::make_unique<APVTSParameterFloat> ("outputGain2",     "Mid Output Gain",       0.0f),
    std::make_unique<APVTSParameterFloat> ("drive2",          "Mid Drive",             0.0f),
    std::make_unique<APVTSParameterFloat> ("knee2",           "Mid Knee",              1.0f),
    std::make_unique<APVTSParameterInt>   ("bit2",            "Mid Bit",               16),
    std::make_unique<APVTSParameterBool>  ("bypass2",         "Mid Bypass",            false),
    std::make_unique<APVTSParameterFloat> ("inputGain3",      "High Input Gain",       0.0f),
    std::make_unique<APVTSParameterFloat> ("outputGain3",     "High Output Gain",      0.0f),
    std::make_unique<APVTSParameterFloat> ("drive3",          "High Drive",            0.0f),
    std::make_unique<APVTSParameterFloat> ("knee3",           "High Knee",             1.0f),
    std::make_unique<APVTSParameterInt>   ("bit3",            "High Bit",              16),
    std::make_unique<APVTSParameterBool>  ("bypass3",         "High Bypass",           false),
    std::make_unique<APVTSParameterFloat> ("mix",             "Mix",                   100.0f),
    std::make_unique<APVTSParameterFloat> ("inputGain",       "Input",                 0.0f),
    std::make_unique<APVTSParameterFloat> ("outputGain",      "Output",                0.0f),
    std::make_unique<APVTSParameterBool>  ("bypass",          "Bypass",                false),
    std::make_unique<APVTSParameterFloat> ("lowMidCut",       "Low/Mid Cut",           700.0f),
    std::make_unique<APVTSParameterFloat> ("midHighCut",      "Mid/high Cut",          5000.0f)
};

class AttilaAudioProcessor  : 
    public juce::AudioProcessor,
    public ValueTree::Listener
{
public:
    //==============================================================================
    AttilaAudioProcessor();
    ~AttilaAudioProcessor() override;

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

    AudioProcessorValueTreeState apvts;

private:
    //==============================================================================
    AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    std::atomic<bool> parametersChanged{ false };

    void valueTreePropertyChanged(ValueTree&, const Identifier&) override {
        parametersChanged.store(true);
    }

    void updateDSP();
    DSPParameters<float> distortionParameters;

    MultibandDistortion distortion;

    size_t oversampleFactor = 2;
    dsp::Oversampling<float> oversampling{ 2, oversampleFactor, juce::dsp::Oversampling<float>::filterHalfBandPolyphaseIIR, false};

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AttilaAudioProcessor)
};
