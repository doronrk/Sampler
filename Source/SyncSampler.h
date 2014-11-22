/*
  ==============================================================================

    SyncSampler.h
    Created: 12 Nov 2014 10:20:34pm
    Author:  Doron Roberts-Kedes

  ==============================================================================
*/

#ifndef SYNCSAMPLER_H_INCLUDED
#define SYNCSAMPLER_H_INCLUDED




/*
 ==============================================================================
 
 This file is part of the JUCE library.
 Copyright (c) 2013 - Raw Material Software Ltd.
 
 Permission is granted to use this software under the terms of either:
 a) the GPL v2 (or any later version)
 b) the Affero GPL v3
 
 Details of these licenses can be found at: www.gnu.org/licenses
 
 JUCE is distributed in the hope that it will be useful, but WITHOUT ANY
 WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
 
 ------------------------------------------------------------------------------
 
 To release a closed-source product which uses JUCE, commercial licenses are
 available: visit www.juce.com for more information.
 
 ==============================================================================
 */


#include "JuceHeader.h"
#include "SyncSynthesiser.h"

//==============================================================================
/**
 A subclass of SynthesiserSound that represents a sampled audio clip.
 
 This is a pretty basic sampler, and just attempts to load the whole audio stream
 into memory.
 
 To use it, create a Synthesiser, add some SamplerVoice objects to it, then
 give it some SampledSound objects to play.
 
 @see SamplerVoice, Synthesiser, SynthesiserSound
 */
class SyncSamplerSound    : public SyncSynthesiserSound
{
    
    
public:
    
    enum SustainMode { SINGLE, LOOP, REVERSE, LOOP_REVERSE};

    //==============================================================================
    /** Creates a sampled sound from an audio reader.
     
     This will attempt to load the audio from the source into memory and store
     it in this object.
     
     @param name         a name for the sample
     @param source       the audio to load. This object can be safely deleted by the
     caller after this constructor returns
     @param midiNotes    the set of midi keys that this sound should be played on. This
     is used by the SynthesiserSound::appliesToNote() method
     @param midiNoteForNormalPitch   the midi note at which the sample should be played
     with its natural rate. All other notes will be pitched
     up or down relative to this one
     @param attackTimeSecs   the attack (fade-in) time, in seconds
     @param releaseTimeSecs  the decay (fade-out) time, in seconds
     @param maxSampleLengthSeconds   a maximum length of audio to read from the audio
     source, in seconds
     */
    SyncSamplerSound (const String& name,
                  AudioFormatReader& source,
                  const BigInteger& midiNotes,
                  int midiNoteForNormalPitch,
                  double attackTimeSecs,
                  double releaseTimeSecs,
                  double maxSampleLengthSeconds,
                  bool syncOn,
                  double durationRelQuarterNote,
                  SustainMode sustainMode);
    
    /** Destructor. */
    ~SyncSamplerSound();
    
    //==============================================================================
    /** Returns the sample's name */
    const String& getName() const noexcept                  { return name; }
    
    /** Returns the audio sample data.
     This could return nullptr if there was a problem loading the data.
     */
    AudioSampleBuffer* getAudioData() const noexcept        { return data; }
    
    
    //==============================================================================
    bool appliesToNote (int midiNoteNumber) override;
    bool appliesToChannel (int midiChannel) override;
    
    void setRootMidiNote(int midiNote);
    void setSustainMode(SyncSamplerSound::SustainMode mode);
    
private:
    //==============================================================================
    
    
    friend class SyncSamplerVoice;
    
    String name;
    ScopedPointer<AudioSampleBuffer> data;
    double sourceSampleRate;
    BigInteger midiNotes;
    int length, attackSamples, releaseSamples;
    int midiRootNote;
    bool syncOn;
    double durationRelQuarterNote;
    SustainMode sustainMode;
    
    JUCE_LEAK_DETECTOR (SyncSamplerSound)
};


//==============================================================================
/**
 A subclass of SynthesiserVoice that can play a SamplerSound.
 
 To use it, create a Synthesiser, add some SamplerVoice objects to it, then
 give it some SampledSound objects to play.
 
 @see SamplerSound, Synthesiser, SynthesiserVoice
 */
class SyncSamplerVoice    : public SyncSynthesiserVoice
{
public:
    //==============================================================================
    /** Creates a SamplerVoice. */
    SyncSamplerVoice();
    
    /** Destructor. */
    ~SyncSamplerVoice();
    
    //==============================================================================
    bool canPlaySound (SyncSynthesiserSound*) override;
    
    void startNote (int midiNoteNumber, float velocity, SyncSynthesiserSound*, int pitchWheel, AudioPlayHead::CurrentPositionInfo lastPosInfo) override;
    void stopNote (float velocity, bool allowTailOff) override;
    
    void pitchWheelMoved (int newValue);
    void controllerMoved (int controllerNumber, int newValue) override;
    
    void renderNextBlock (AudioSampleBuffer&, int startSample, int numSamples) override;
    
    double getSourceSamplePosition() const;
    
    double getCurrentSampleLength() const;
    
private:
    //==============================================================================
    double pitchRatio;
    double sourceSamplePosition;
    double rightmostSample;
    double currentSampleLength;
    float lgain, rgain, attackReleaseLevel, attackDelta, releaseDelta;
    bool isInAttack, isInRelease;
    
    JUCE_LEAK_DETECTOR (SyncSamplerVoice)
};



#endif  // SYNCSAMPLER_H_INCLUDED
