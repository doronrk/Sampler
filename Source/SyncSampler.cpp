/*
  ==============================================================================

    SyncSampler.cpp
    Created: 12 Nov 2014 10:20:34pm
    Author:  Doron Roberts-Kedes

  ==============================================================================
*/

#include "SyncSampler.h"


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

SyncSamplerSound::SyncSamplerSound (const String& soundName,
                            AudioFormatReader& source,
                            const BigInteger& notes,
                            const int midiNoteForNormalPitch,
                            const double attackTimeSecs,
                            const double releaseTimeSecs,
                            const double maxSampleLengthSeconds,
                            const double durationRelQuarterNote_,
                            const SustainMode sustainMode_)
: name (soundName),
midiNotes (notes),
midiRootNote (midiNoteForNormalPitch),
durationRelQuarterNote(durationRelQuarterNote_),
sustainMode(sustainMode_)
{
    sourceSampleRate = source.sampleRate;
    if (sourceSampleRate <= 0 || source.lengthInSamples <= 0)
    {
        length = 0;
        attackSamples = 0;
        releaseSamples = 0;
    }
    else
    {
        length = jmin ((int) source.lengthInSamples,
                       (int) (maxSampleLengthSeconds * sourceSampleRate));
        data = new AudioSampleBuffer (jmin (2, (int) source.numChannels), length + 4);
        source.read (data, 0, length + 4, 0, true, true);
        attackSamples = roundToInt (attackTimeSecs * sourceSampleRate);
        releaseSamples = roundToInt (releaseTimeSecs * sourceSampleRate);
    }
}

SyncSamplerSound::~SyncSamplerSound()
{
}

bool SyncSamplerSound::appliesToNote (int midiNoteNumber)
{
    return midiNotes [midiNoteNumber];
}

bool SyncSamplerSound::appliesToChannel (int /*midiChannel*/)
{
    return true;
}

void SyncSamplerSound::setRootMidiNote(int midiNote)
{
    midiRootNote = midiNote;
}

void SyncSamplerSound::setSustainMode(SyncSamplerSound::SustainMode mode)
{
    sustainMode = mode;
}

//==============================================================================
SyncSamplerVoice::SyncSamplerVoice()
    : pitchRatio (0.0),
    sourceSamplePosition (0.0),
    rightmostSample(0.0),
    currentSampleLength(0.0),
    lgain (0.0f), rgain (0.0f),
    attackReleaseLevel (0), attackDelta (0), releaseDelta (0),
    isInAttack (false), isInRelease (false)
{
}

SyncSamplerVoice::~SyncSamplerVoice()
{
}

bool SyncSamplerVoice::canPlaySound (SyncSynthesiserSound* sound)
{
    return dynamic_cast<const SyncSamplerSound*> (sound) != nullptr;
}

void SyncSamplerVoice::startNote (const int midiNoteNumber,
                              const float velocity,
                              SyncSynthesiserSound* s,
                              const int /*currentPitchWheelPosition*/,
                              AudioPlayHead::CurrentPositionInfo lastPosInfo)
{
    if (const SyncSamplerSound* const sound = dynamic_cast <const SyncSamplerSound*> (s))
    {
        pitchRatio = pow (2.0, (midiNoteNumber - sound->midiRootNote) / 12.0)
        * sound->sourceSampleRate / getSampleRate();
        currentSampleLength = sound->length;
        
        double bpm = lastPosInfo.bpm;
        double secondsPerBeat = 60.0 / bpm;
        rightmostSample = secondsPerBeat * getSampleRate() * pitchRatio * sound->durationRelQuarterNote;
        
        if (rightmostSample >= sound->length)
        {
            DBG("duration requires too many samples");
            DBG("sound->durationRelQuarterNote " + String(sound->durationRelQuarterNote));
            DBG("rightmostSample " + String(rightmostSample));
            DBG("sound->length " + String(sound->length));
            return;
        }
        
        SyncSamplerSound::SustainMode sustainMode = sound->sustainMode;
        switch (sustainMode)
        {
            case SyncSamplerSound::SustainMode::SINGLE:
                sourceSamplePosition = 0.0;
                break;
            case SyncSamplerSound::SustainMode::LOOP:
                sourceSamplePosition = 0.0;
                break;
            case SyncSamplerSound::SustainMode::REVERSE:
                sourceSamplePosition = rightmostSample;
                pitchRatio = -pitchRatio;
                break;
            default: // LOOP_REVERSE
                sourceSamplePosition = 0.0;
        }

        lgain = velocity;
        rgain = velocity;
        
        isInAttack = (sound->attackSamples > 0);
        isInRelease = false;
        
        if (isInAttack)
        {
            attackReleaseLevel = 0.0f;
            attackDelta = (float) (pitchRatio / sound->attackSamples);
        }
        else
        {
            attackReleaseLevel = 1.0f;
            attackDelta = 0.0f;
        }
        
        if (sound->releaseSamples > 0)
            releaseDelta = (float) (-pitchRatio / sound->releaseSamples);
        else
            releaseDelta = -1.0f;
    }
    else
    {
        jassertfalse; // this object can only play SamplerSounds!
    }
}

void SyncSamplerVoice::stopNote (float /*velocity*/, bool allowTailOff)
{
    if (allowTailOff)
    {
        isInAttack = false;
        isInRelease = true;
    }
    else
    {
        clearCurrentNote();
    }
}

void SyncSamplerVoice::pitchWheelMoved (const int /*newValue*/)
{
}

void SyncSamplerVoice::controllerMoved (const int /*controllerNumber*/,
                                    const int /*newValue*/)
{
}

double SyncSamplerVoice::getSourceSamplePosition() const
{
    return sourceSamplePosition;
}

double SyncSamplerVoice::getCurrentSampleLength() const
{
    return currentSampleLength;
}

//==============================================================================
void SyncSamplerVoice::renderNextBlock (AudioSampleBuffer& outputBuffer, int startSample, int numSamples)
{
    if (const SyncSamplerSound* const playingSound = static_cast <SyncSamplerSound*> (getCurrentlyPlayingSound().get()))
    {
        const float* const inL = playingSound->data->getReadPointer (0);
        const float* const inR = playingSound->data->getNumChannels() > 1
        ? playingSound->data->getReadPointer (1) : nullptr;
        
        float* outL = outputBuffer.getWritePointer (0, startSample);
        float* outR = outputBuffer.getNumChannels() > 1 ? outputBuffer.getWritePointer (1, startSample) : nullptr;
        
        SyncSamplerSound::SustainMode sustainMode = playingSound->sustainMode;
        
        while (--numSamples >= 0)
        {
            const int pos = (int) sourceSamplePosition;
            const float alpha = (float) (sourceSamplePosition - pos);
            const float invAlpha = 1.0f - alpha;
            
            // just using a very simple linear interpolation here..
            float l = (inL [pos] * invAlpha + inL [pos + 1] * alpha);
            float r = (inR != nullptr) ? (inR [pos] * invAlpha + inR [pos + 1] * alpha)
            : l;
            
            l *= lgain;
            r *= rgain;
            
            if (isInAttack)
            {
                l *= attackReleaseLevel;
                r *= attackReleaseLevel;
                
                attackReleaseLevel += attackDelta;
                
                if (attackReleaseLevel >= 1.0f)
                {
                    attackReleaseLevel = 1.0f;
                    isInAttack = false;
                }
            }
            else if (isInRelease)
            {
                l *= attackReleaseLevel;
                r *= attackReleaseLevel;
                
                attackReleaseLevel += releaseDelta;
                
                if (attackReleaseLevel <= 0.0f)
                {
                    stopNote (0.0f, false);
                    break;
                }
            }
            
            if (outR != nullptr)
            {
                *outL++ += l;
                *outR++ += r;
            }
            else
            {
                *outL++ += (l + r) * 0.5f;
            }
            
            sourceSamplePosition += pitchRatio;
            
            if (sourceSamplePosition > rightmostSample)
            {
                switch (sustainMode)
                {
                    case SyncSamplerSound::SustainMode::SINGLE:
                        stopNote (0.0f, false);
                        break;
                    case SyncSamplerSound::SustainMode::LOOP:
                        sourceSamplePosition = 0;
                        break;
                    case SyncSamplerSound::SustainMode::REVERSE:
                        DBG("Warning! sourceSamplePosition > rightmostSample in REVERSE mode");
                        sourceSamplePosition = rightmostSample;
                        break;
                    default: // LOOP_REVERSE
                        pitchRatio = - pitchRatio;
                }
            }
            if (sourceSamplePosition < 0)
            {
                switch (sustainMode)
                {
                    case SyncSamplerSound::SustainMode::SINGLE:
                        DBG("Warning! sourceSamplePosition < 0 in SINGLE mode");
                        sourceSamplePosition = 0;
                        break;
                    case SyncSamplerSound::SustainMode::LOOP:
                        DBG("Warning! sourceSamplePosition < 0 in LOOP mode");
                        sourceSamplePosition = 0;
                        break;
                    case SyncSamplerSound::SustainMode::REVERSE:
                        stopNote (0.0f, false);
                        break;
                    default: // LOOP_REVERSE
                        pitchRatio = - pitchRatio;
                }
            }
        }
    }
}
