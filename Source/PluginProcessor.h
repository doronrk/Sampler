/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#ifndef PLUGINPROCESSOR_H_INCLUDED
#define PLUGINPROCESSOR_H_INCLUDED

#include "../JuceLibraryCode/JuceHeader.h"
#include "SyncSynthesiser.h"
#include "SyncSampler.h"

//==============================================================================
/**
*/
class SampleDropArea;

class SamplerAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    SamplerAudioProcessor();
    ~SamplerAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    int getNumParameters() override;
    float getParameter (int index) override;
    void setParameter (int index, float newValue) override;

    const String getParameterName (int index) override;
    const String getParameterText (int index) override;

    const String getInputChannelName (int channelIndex) const override;
    const String getOutputChannelName (int channelIndex) const override;
    bool isInputChannelStereoPair (int index) const override;
    bool isOutputChannelStereoPair (int index) const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool silenceInProducesSilenceOut() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    //==============================================================================
    void setNewSample(AudioFormatReader& audioReader);
    void setRootMidiNote(int note);
    void setNumVoices(int numVoices);
    void setSustainMode(SyncSamplerSound::SustainMode mode);
    void setSyncState(bool isOn);
    
    int getRootMidiNote();
    int getNumVoices();
    SyncSamplerSound::SustainMode getSustainMode();
    bool getSyncState();
    
    void beginPreviewSound();
    void endPreviewSound();
    
    
    Array<double>* getSamplePositions();
    
private:
    //==============================================================================
    SyncSynthesiser sampler;
    ReferenceCountedObjectPtr<SampleDropArea> sampleDropArea;
    ReferenceCountedObjectPtr<SyncSamplerSound> currentSound;
    
    // this keeps a copy of the last set of time info that was acquired during an audio
    // callback
    AudioPlayHead::CurrentPositionInfo lastPosInfo;
    void updatePositionInfo();
    
    const double maxSampleLengthSeconds;
    
    
    //  parameters
    int rootMidiNote;
    int numVoices;
    SyncSamplerSound::SustainMode sustainMode;
    bool syncOn;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SamplerAudioProcessor)
};


#endif  // PLUGINPROCESSOR_H_INCLUDED
