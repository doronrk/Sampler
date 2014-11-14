/*
 ==============================================================================
 
 SyncSynthesiser.cpp
 Created: 13 Nov 2014 8:50:09pm
 Author:  Doron Roberts-Kedes
 
 ==============================================================================
 */

#include "SyncSynthesiser.h"



SyncSynthesiserSound::SyncSynthesiserSound() {}
SyncSynthesiserSound::~SyncSynthesiserSound() {}

//==============================================================================
SyncSynthesiserVoice::SyncSynthesiserVoice()
: currentSampleRate (44100.0),
currentlyPlayingNote (-1),
noteOnTime (0),
keyIsDown (false),
sostenutoPedalDown (false)
{
}

SyncSynthesiserVoice::~SyncSynthesiserVoice()
{
}

bool SyncSynthesiserVoice::isPlayingChannel (const int midiChannel) const
{
    return currentlyPlayingSound != nullptr
    && currentlyPlayingSound->appliesToChannel (midiChannel);
}

void SyncSynthesiserVoice::setCurrentPlaybackSampleRate (const double newRate)
{
    currentSampleRate = newRate;
}

void SyncSynthesiserVoice::clearCurrentNote()
{
    currentlyPlayingNote = -1;
    currentlyPlayingSound = nullptr;
}

void SyncSynthesiserVoice::aftertouchChanged (int) {}

bool SyncSynthesiserVoice::wasStartedBefore (const SyncSynthesiserVoice& other) const noexcept
{
    return noteOnTime < other.noteOnTime;
}
    
    //==============================================================================
SyncSynthesiser::SyncSynthesiser()
: sampleRate (0),
lastNoteOnCounter (0),
shouldStealNotes (true)
{
    for (int i = 0; i < numElementsInArray (lastPitchWheelValues); ++i)
        lastPitchWheelValues[i] = 0x2000;
}
    
SyncSynthesiser::~SyncSynthesiser()
{
}
    
    //==============================================================================
SyncSynthesiserVoice* SyncSynthesiser::getVoice (const int index) const
{
    const ScopedLock sl (lock);
    return voices [index];
}
    
void SyncSynthesiser::clearVoices()
{
    const ScopedLock sl (lock);
    voices.clear();
}
    
SyncSynthesiserVoice* SyncSynthesiser::addVoice (SyncSynthesiserVoice* const newVoice)
{
    const ScopedLock sl (lock);
    return voices.add (newVoice);
}
    
void SyncSynthesiser::removeVoice (const int index)
{
    const ScopedLock sl (lock);
    voices.remove (index);
}
    
void SyncSynthesiser::clearSounds()
{
    const ScopedLock sl (lock);
    sounds.clear();
}
    
SyncSynthesiserSound* SyncSynthesiser::addSound (const SyncSynthesiserSound::Ptr& newSound)
{
    const ScopedLock sl (lock);
    return sounds.add (newSound);
}
    
void SyncSynthesiser::removeSound (const int index)
{
    const ScopedLock sl (lock);
    sounds.remove (index);
}
    
void SyncSynthesiser::setNoteStealingEnabled (const bool shouldSteal)
{
    shouldStealNotes = shouldSteal;
}
    
    //==============================================================================
void SyncSynthesiser::setCurrentPlaybackSampleRate (const double newRate)
{
    if (sampleRate != newRate)
    {
        const ScopedLock sl (lock);
        
        allNotesOff (0, false);
            
        sampleRate = newRate;
            
        for (int i = voices.size(); --i >= 0;)
            voices.getUnchecked (i)->setCurrentPlaybackSampleRate (newRate);
    }
}
    
void SyncSynthesiser::renderNextBlock (AudioSampleBuffer& outputBuffer, const MidiBuffer& midiData,
                                       int startSample, int numSamples, AudioPlayHead::CurrentPositionInfo lastPosInfo)
{
        // must set the sample rate before using this!
    jassert (sampleRate != 0);
    
    const ScopedLock sl (lock);
        
    MidiBuffer::Iterator midiIterator (midiData);
    midiIterator.setNextSamplePosition (startSample);
    MidiMessage m (0xf4, 0.0);
        
    while (numSamples > 0)
    {
        int midiEventPos;
        const bool useEvent = midiIterator.getNextEvent (m, midiEventPos)
        && midiEventPos < startSample + numSamples;
        
        const int numThisTime = useEvent ? midiEventPos - startSample
        : numSamples;
        
        if (numThisTime > 0)
            renderVoices (outputBuffer, startSample, numThisTime);
        
        if (useEvent)
            handleMidiEvent (m, lastPosInfo);
        
        startSample += numThisTime;
        numSamples -= numThisTime;
    }
}
    
void SyncSynthesiser::renderVoices (AudioSampleBuffer& buffer, int startSample, int numSamples)
{
    for (int i = voices.size(); --i >= 0;)
        voices.getUnchecked (i)->renderNextBlock (buffer, startSample, numSamples);
}
    
void SyncSynthesiser::handleMidiEvent (const MidiMessage& m, AudioPlayHead::CurrentPositionInfo lastPosInfo)
{
    if (m.isNoteOn())
    {
        noteOn (m.getChannel(), m.getNoteNumber(), m.getFloatVelocity(), lastPosInfo);
    }
    else if (m.isNoteOff())
    {
        noteOff (m.getChannel(), m.getNoteNumber(), m.getFloatVelocity(), true);
    }
    else if (m.isAllNotesOff() || m.isAllSoundOff())
    {
        allNotesOff (m.getChannel(), true);
    }
    else if (m.isPitchWheel())
    {
        const int channel = m.getChannel();
        const int wheelPos = m.getPitchWheelValue();
        lastPitchWheelValues [channel - 1] = wheelPos;
        
        handlePitchWheel (channel, wheelPos);
    }
    else if (m.isAftertouch())
    {
        handleAftertouch (m.getChannel(), m.getNoteNumber(), m.getAfterTouchValue());
    }
    else if (m.isController())
    {
        handleController (m.getChannel(), m.getControllerNumber(), m.getControllerValue());
    }
}

//==============================================================================
void SyncSynthesiser::noteOn (const int midiChannel,
                          const int midiNoteNumber,
                          const float velocity,
                          AudioPlayHead::CurrentPositionInfo lastPosInfo)
{
     const ScopedLock sl (lock);
    
    for (int i = sounds.size(); --i >= 0;)
    {
        SyncSynthesiserSound* const sound = sounds.getUnchecked(i);
        
        if (sound->appliesToNote (midiNoteNumber)
            && sound->appliesToChannel (midiChannel))
        {
            // If hitting a note that's still ringing, stop it first (it could be
            // still playing because of the sustain or sostenuto pedal).
            for (int j = voices.size(); --j >= 0;)
            {
                SyncSynthesiserVoice* const voice = voices.getUnchecked (j);
                
                if (voice->getCurrentlyPlayingNote() == midiNoteNumber
                    && voice->isPlayingChannel (midiChannel))
                    stopVoice (voice, 1.0f, true);
            }
            
            startVoice (findFreeVoice (sound, midiChannel, midiNoteNumber, shouldStealNotes),
                        sound, midiChannel, midiNoteNumber, velocity, lastPosInfo);
        }
    }
}

void SyncSynthesiser::startVoice (SyncSynthesiserVoice* const voice,
                              SyncSynthesiserSound* const sound,
                              const int midiChannel,
                              const int midiNoteNumber,
                              const float velocity,
                              AudioPlayHead::CurrentPositionInfo lastPosInfo)
{
    if (voice != nullptr && sound != nullptr)
    {
        if (voice->currentlyPlayingSound != nullptr)
            voice->stopNote (0.0f, false);
        
        voice->startNote (midiNoteNumber, velocity, sound, lastPitchWheelValues [midiChannel - 1], lastPosInfo);
        
        voice->currentlyPlayingNote = midiNoteNumber;
        voice->noteOnTime = ++lastNoteOnCounter;
        voice->currentlyPlayingSound = sound;
        voice->keyIsDown = true;
        voice->sostenutoPedalDown = false;
    }
}

void SyncSynthesiser::stopVoice (SyncSynthesiserVoice* voice, float velocity, const bool allowTailOff)
{
    jassert (voice != nullptr);
    
    voice->stopNote (velocity, allowTailOff);
    
    // the subclass MUST call clearCurrentNote() if it's not tailing off! RTFM for stopNote()!
    jassert (allowTailOff || (voice->getCurrentlyPlayingNote() < 0 && voice->getCurrentlyPlayingSound() == 0));
}

void SyncSynthesiser::noteOff (const int midiChannel,
                           const int midiNoteNumber,
                           const float velocity,
                           const bool allowTailOff)
{
    const ScopedLock sl (lock);
    
    for (int i = voices.size(); --i >= 0;)
    {
        SyncSynthesiserVoice* const voice = voices.getUnchecked (i);
        
        if (voice->getCurrentlyPlayingNote() == midiNoteNumber)
        {
            if (SyncSynthesiserSound* const sound = voice->getCurrentlyPlayingSound())
            {
                if (sound->appliesToNote (midiNoteNumber)
                    && sound->appliesToChannel (midiChannel))
                {
                    voice->keyIsDown = false;
                    
                    if (! (sustainPedalsDown [midiChannel] || voice->sostenutoPedalDown))
                        stopVoice (voice, velocity, allowTailOff);
                }
            }
        }
    }
}

void SyncSynthesiser::allNotesOff (const int midiChannel, const bool allowTailOff)
{
    const ScopedLock sl (lock);
    
    for (int i = voices.size(); --i >= 0;)
    {
        SyncSynthesiserVoice* const voice = voices.getUnchecked (i);
        
        if (midiChannel <= 0 || voice->isPlayingChannel (midiChannel))
            voice->stopNote (1.0f, allowTailOff);
    }
    
    sustainPedalsDown.clear();
}

void SyncSynthesiser::handlePitchWheel (const int midiChannel, const int wheelValue)
{
    const ScopedLock sl (lock);
    
    for (int i = voices.size(); --i >= 0;)
    {
        SyncSynthesiserVoice* const voice = voices.getUnchecked (i);
        
        if (midiChannel <= 0 || voice->isPlayingChannel (midiChannel))
            voice->pitchWheelMoved (wheelValue);
    }
}

void SyncSynthesiser::handleController (const int midiChannel,
                                    const int controllerNumber,
                                    const int controllerValue)
{
    switch (controllerNumber)
    {
        case 0x40:  handleSustainPedal   (midiChannel, controllerValue >= 64); break;
        case 0x42:  handleSostenutoPedal (midiChannel, controllerValue >= 64); break;
        case 0x43:  handleSoftPedal      (midiChannel, controllerValue >= 64); break;
        default:    break;
    }
    
    const ScopedLock sl (lock);
    
    for (int i = voices.size(); --i >= 0;)
    {
        SyncSynthesiserVoice* const voice = voices.getUnchecked (i);
        
        if (midiChannel <= 0 || voice->isPlayingChannel (midiChannel))
            voice->controllerMoved (controllerNumber, controllerValue);
    }
}

void SyncSynthesiser::handleAftertouch (int midiChannel, int midiNoteNumber, int aftertouchValue)
{
    const ScopedLock sl (lock);
    
    for (int i = voices.size(); --i >= 0;)
    {
        SyncSynthesiserVoice* const voice = voices.getUnchecked (i);
        
        if (voice->getCurrentlyPlayingNote() == midiNoteNumber
            && (midiChannel <= 0 || voice->isPlayingChannel (midiChannel)))
            voice->aftertouchChanged (aftertouchValue);
    }
}

void SyncSynthesiser::handleSustainPedal (int midiChannel, bool isDown)
{
    jassert (midiChannel > 0 && midiChannel <= 16);
    const ScopedLock sl (lock);
    
    if (isDown)
    {
        sustainPedalsDown.setBit (midiChannel);
    }
    else
    {
        for (int i = voices.size(); --i >= 0;)
        {
            SyncSynthesiserVoice* const voice = voices.getUnchecked (i);
            
            if (voice->isPlayingChannel (midiChannel) && ! voice->keyIsDown)
                stopVoice (voice, 1.0f, true);
        }
        
        sustainPedalsDown.clearBit (midiChannel);
    }
}

void SyncSynthesiser::handleSostenutoPedal (int midiChannel, bool isDown)
{
    jassert (midiChannel > 0 && midiChannel <= 16);
    const ScopedLock sl (lock);
    
    for (int i = voices.size(); --i >= 0;)
    {
        SyncSynthesiserVoice* const voice = voices.getUnchecked (i);
        
        if (voice->isPlayingChannel (midiChannel))
        {
            if (isDown)
                voice->sostenutoPedalDown = true;
            else if (voice->sostenutoPedalDown)
                stopVoice (voice, 1.0f, true);
        }
    }
}

void SyncSynthesiser::handleSoftPedal (int midiChannel, bool /*isDown*/)
{
    (void) midiChannel;
    jassert (midiChannel > 0 && midiChannel <= 16);
}

//==============================================================================
SyncSynthesiserVoice* SyncSynthesiser::findFreeVoice (SyncSynthesiserSound* soundToPlay,
                                              int midiChannel, int midiNoteNumber,
                                              const bool stealIfNoneAvailable) const
{
    const ScopedLock sl (lock);
    
    for (int i = 0; i < voices.size(); ++i)
    {
        SyncSynthesiserVoice* const voice = voices.getUnchecked (i);
        
        if (voice->getCurrentlyPlayingNote() < 0  && voice->canPlaySound (soundToPlay))
            return voice;
    }
    
    if (stealIfNoneAvailable)
        return findVoiceToSteal (soundToPlay, midiChannel, midiNoteNumber);
    
    return nullptr;
}

struct VoiceAgeSorter
{
    static int compareElements (SyncSynthesiserVoice* v1, SyncSynthesiserVoice* v2) noexcept
    {
        return v1->wasStartedBefore (*v2) ? 1 : (v2->wasStartedBefore (*v1) ? -1 : 0);
    }
};

SyncSynthesiserVoice* SyncSynthesiser::findVoiceToSteal (SyncSynthesiserSound* soundToPlay,
                                                 int /*midiChannel*/, int midiNoteNumber) const
{
    SyncSynthesiserVoice* bottom = nullptr;
    SyncSynthesiserVoice* top    = nullptr;
    
    // this is a list of voices we can steal, sorted by how long they've been running
    Array<SyncSynthesiserVoice*> usableVoices;
    usableVoices.ensureStorageAllocated (voices.size());
    
    for (int i = 0; i < voices.size(); ++i)
    {
        SyncSynthesiserVoice* const voice = voices.getUnchecked (i);
        
        if (voice->canPlaySound (soundToPlay))
        {
            VoiceAgeSorter sorter;
            usableVoices.addSorted (sorter, voice);
            
            const int note = voice->getCurrentlyPlayingNote();
            
            if (bottom == nullptr || note < bottom->getCurrentlyPlayingNote())
                bottom = voice;
            
            if (top == nullptr || note > top->getCurrentlyPlayingNote())
                top = voice;
        }
    }
    
    jassert (bottom != nullptr && top != nullptr);
    
    // The oldest note that's playing with the target pitch playing is ideal..
    for (int i = 0; i < usableVoices.size(); ++i)
    {
        SyncSynthesiserVoice* const voice = usableVoices.getUnchecked (i);
        
        if (voice->getCurrentlyPlayingNote() == midiNoteNumber)
            return voice;
    }
    
    // ..otherwise, look for the oldest note that isn't the top or bottom note..
    for (int i = 0; i < usableVoices.size(); ++i)
    {
        SyncSynthesiserVoice* const voice = usableVoices.getUnchecked (i);
        
        if (voice != bottom && voice != top)
            return voice;
    }
    
    // ..otherwise, there's only one or two voices to choose from - we'll return the top one..
    return top;
}