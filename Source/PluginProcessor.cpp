/*
  ==============================================================================

    This file was auto-generated by the Introjucer!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "Gui.h"
#include "SyncSampler.h"


//==============================================================================
SamplerAudioProcessor::SamplerAudioProcessor():
    sampleDropArea(new SampleDropArea(*this)),
    maxSampleLengthSeconds(500.0),
    rootMidiNote(60), // default C3
    numVoices(4),
    sustainMode(SyncSamplerSound::SustainMode::SINGLE),
    syncOn(true)
{
    setNumVoices(numVoices);
}

SamplerAudioProcessor::~SamplerAudioProcessor()
{
}

//==============================================================================
const String SamplerAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

int SamplerAudioProcessor::getNumParameters()
{
    return 0;
}

float SamplerAudioProcessor::getParameter (int index)
{
    return 0.0f;
}

void SamplerAudioProcessor::setParameter (int index, float newValue)
{
}

const String SamplerAudioProcessor::getParameterName (int index)
{
    return String();
}

const String SamplerAudioProcessor::getParameterText (int index)
{
    return String();
}

const String SamplerAudioProcessor::getInputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

const String SamplerAudioProcessor::getOutputChannelName (int channelIndex) const
{
    return String (channelIndex + 1);
}

bool SamplerAudioProcessor::isInputChannelStereoPair (int index) const
{
    return true;
}

bool SamplerAudioProcessor::isOutputChannelStereoPair (int index) const
{
    return true;
}

bool SamplerAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SamplerAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SamplerAudioProcessor::silenceInProducesSilenceOut() const
{
    return false;
}

double SamplerAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SamplerAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SamplerAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SamplerAudioProcessor::setCurrentProgram (int index)
{
}

const String SamplerAudioProcessor::getProgramName (int index)
{
    return String();
}

void SamplerAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void SamplerAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
    DBG("prepare to play called");
    sampler.setCurrentPlaybackSampleRate(sampleRate);
}

void SamplerAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
    DBG("release resources called");
}

void SamplerAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    // clear extra channels
    for (int i = getNumInputChannels(); i < getNumOutputChannels(); ++i)
    {
        buffer.clear (i, 0, buffer.getNumSamples());
    }
    
    updatePositionInfo();
    
    sampler.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples(), lastPosInfo);
}

void SamplerAudioProcessor::updatePositionInfo()
{
    // ask the host for the current time
    AudioPlayHead::CurrentPositionInfo newTime;
    
    if (getPlayHead() != nullptr && getPlayHead()->getCurrentPosition (newTime))
    {
        // Successfully got the current time from the host..
        lastPosInfo = newTime;
    }
    else
    {
        // If the host fails to fill-in the current time, we'll just clear it to a default
        lastPosInfo.resetToDefault();
    }
}


//==============================================================================
bool SamplerAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* SamplerAudioProcessor::createEditor()
{
    return new Gui (*this, *sampleDropArea);
}

//==============================================================================
void SamplerAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    DBG("get state information called");
}

void SamplerAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    DBG("set state information called");
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SamplerAudioProcessor();
}

//

void SamplerAudioProcessor::setNewSample(AudioFormatReader& audioReader)
{
    sampler.clearSounds();
    BigInteger allNotes;
    allNotes.setRange (0, 128, true);
    
    double defaultDurationRelQuarterNote = 1.0;
    SyncSynthesiserSound *sound = sampler.addSound (new SyncSamplerSound ("some name", audioReader, allNotes, rootMidiNote, 0.0, 0.0, maxSampleLengthSeconds, defaultDurationRelQuarterNote, sustainMode));
    currentSound = dynamic_cast <SyncSamplerSound*> (sound);
}

void SamplerAudioProcessor::setRootMidiNote(int note)
{
    rootMidiNote = note;
    if (currentSound != nullptr)
    {
        currentSound->setRootMidiNote(note);
    }
}

void SamplerAudioProcessor::setNumVoices(int nVoices)
{
    sampler.clearVoices();
    sampleDropArea->clearPositionMarkers();
    for (int i = 0; i < nVoices; i++)
    {
        sampler.addVoice(new SyncSamplerVoice());
        sampleDropArea->addPositionMarker();
    }
    this->numVoices = nVoices;
}

void SamplerAudioProcessor::setSustainMode(SyncSamplerSound::SustainMode mode)
{
    sustainMode = mode;
    if (currentSound != nullptr)
    {
        currentSound->setSustainMode(mode);
    }
}

int SamplerAudioProcessor::getRootMidiNote()
{
    return rootMidiNote;
}

int SamplerAudioProcessor::getNumVoices()
{
    return numVoices;
}

SyncSamplerSound::SustainMode SamplerAudioProcessor::getSustainMode()
{
    return sustainMode;
}

void SamplerAudioProcessor::beginPreviewSound()
{
    sampler.noteOn(0, rootMidiNote, .80, lastPosInfo);
}

void SamplerAudioProcessor::endPreviewSound()
{
    sampler.noteOff(0, rootMidiNote, .80, true);
}

Array<double> *SamplerAudioProcessor::getSamplePositions()
{
    Array<double> *positions = new Array<double>();
    int numVoices = sampler.getNumVoices();
    for (int i = 0; i < numVoices; i++)
    {
        SyncSynthesiserVoice *voice= sampler.getVoice(i);
        if (const SyncSamplerVoice* const syncVoice = dynamic_cast <const SyncSamplerVoice*> (voice))
        {
            if (syncVoice->isPlayingChannel(0))
            {
                double samplePos = syncVoice->getSourceSamplePosition();
                double sampleLen = syncVoice->getCurrentSampleLength();
                positions->add(samplePos/sampleLen);
            }
        }
    }
    return positions;
}
