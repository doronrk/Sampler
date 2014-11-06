/*
  ==============================================================================

  This is an automatically generated GUI class created by the Introjucer!

  Be careful when adding custom code to these files, as only the code within
  the "//[xyz]" and "//[/xyz]" sections will be retained when the file is loaded
  and re-saved.

  Created with Introjucer version: 3.1.0

  ------------------------------------------------------------------------------

  The Introjucer is part of the JUCE library - "Jules' Utility Class Extensions"
  Copyright 2004-13 by Raw Material Software Ltd.

  ==============================================================================
*/

#ifndef __JUCE_HEADER_7571728773988918__
#define __JUCE_HEADER_7571728773988918__

//[Headers]     -- You can add your own extra header files here --
#include "JuceHeader.h"
#include "PluginProcessor.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Introjucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class SampleDropArea  : public Component,
                        public FileDragAndDropTarget,
                        public ReferenceCountedObject,
                        private ChangeListener,
                        private ScrollBar::Listener,
                        private Timer
{
    
public:
    //==============================================================================
    SampleDropArea (SamplerAudioProcessor &p);
    ~SampleDropArea();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    File getLastDroppedFile() const noexcept;
    bool isInterestedInFileDrag (const StringArray& /*files*/) override;
    void filesDropped (const StringArray& files, int /*x*/, int /*y*/) override;
    void changeListenerCallback (ChangeBroadcaster*);
    void mouseWheelMove (const MouseEvent&, const MouseWheelDetails& wheel);
    void adjustZoomFactor (const MouseEvent& e, double amount);
    
    void mouseDown(const MouseEvent &event);
    void mouseUp(const MouseEvent &event);

    //[/UserMethods]

    void paint (Graphics& g);
    void resized();

private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    ScrollBar thumbnailScroller;
    Range<double> visibleThumbnailRange;

    File lastFileDropped; // the sample
    AudioThumbnailCache thumbnailCache;
    AudioThumbnail thumbnail;
    AudioFormatManager formatManager;
    
    SamplerAudioProcessor &sampler;
    
    double zoomFactor;
    
    void scrollBarMoved (ScrollBar* scrollBarThatHasMoved, double newRangeStart);
    void setVisibleThumbnailRange (Range<double> newRange);
    void timerCallback();
    //[/UserVariables]

    //==============================================================================


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SampleDropArea)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCE_HEADER_7571728773988918__
