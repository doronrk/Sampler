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

//[Headers] You can add your own extra header files here...
//[/Headers]

#include "SampleDropArea.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
SampleDropArea::SampleDropArea ()
    : thumbnailScroller(false),
      thumbnailCache(5),
      thumbnail(512, formatManager, thumbnailCache),
      zoomFactor(0.0)
{

    //[UserPreSize]
    //[/UserPreSize]

    setSize (200, 200);


    //[Constructor] You can add your own custom stuff here..
    DBG("sample drop area constructor called");
    formatManager.registerBasicFormats();

    // scrollbar
    addAndMakeVisible(thumbnailScroller);
    thumbnailScroller.addListener(this);
    thumbnailScroller.setAutoHide(false);
    thumbnailScroller.setBounds (getLocalBounds().removeFromBottom (14).reduced (2));

    thumbnail.addChangeListener(this);
    //[/Constructor]
}

SampleDropArea::~SampleDropArea()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]



    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void SampleDropArea::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    g.fillAll (Colours::darkgrey);
    g.setColour (Colours::lightblue);
    //[/UserPrePaint]

    

    //[UserPaint] Add your own custom painting code here..
    if (thumbnail.getTotalLength() > 0.0)
    {
        DBG("painting thumbnail");
        Rectangle<int> thumbArea = getLocalBounds();
        thumbArea.removeFromBottom(thumbnailScroller.getHeight() + 4);
        thumbnail.drawChannels(g, thumbArea.reduced(2), visibleThumbnailRange.getStart(), visibleThumbnailRange.getEnd(), 1.0f);
        thumbnailScroller.setBounds (getLocalBounds().removeFromBottom (14).reduced (2));
    }
    else
    {
        DBG("not painting thumbnail since no thumbnail length < 0.0");
        g.setFont(14.0f);
        g.drawFittedText("no audio file selected", getLocalBounds(), Justification::centred, 2);
    }
    //[/UserPaint]
}

void SampleDropArea::resized()
{
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}

void SampleDropArea::mouseWheelMove (const MouseEvent& e, const MouseWheelDetails& wheel)
{
    if (thumbnail.getTotalLength() > 0.0)
    {
        if ( wheel.deltaX * wheel.deltaX > wheel.deltaY * wheel.deltaY)
        {
            if (wheel.deltaX != 0.0)
            {
                double newStart = visibleThumbnailRange.getStart() - wheel.deltaX * (visibleThumbnailRange.getLength()) / 10.0;
                newStart = jlimit (0.0, jmax (0.0, thumbnail.getTotalLength() - (visibleThumbnailRange.getLength())), newStart);
                setVisibleThumbnailRange (Range<double> (newStart, newStart + visibleThumbnailRange.getLength()));
            }
        }
        else
        {
            if (wheel.deltaY != 0.0f)
            {
                adjustZoomFactor(e, wheel.deltaY);
            }
        }
        repaint();
    }
}

void SampleDropArea::adjustZoomFactor (const MouseEvent& e, double amount)
{
    if (thumbnail.getTotalLength() > 0)
    {
        
        zoomFactor = jlimit(0.0, .99, zoomFactor - amount);
        double newScale = jmax (0.001, thumbnail.getTotalLength() * (1.0 - zoomFactor));
        const double timeAtCentre = visibleThumbnailRange.getStart() + .5 * visibleThumbnailRange.getLength();
        double newLeft = timeAtCentre - newScale * 0.5;
        double newRight = timeAtCentre + newScale * 0.5;
        if (newLeft < 0.0)
        {
            newRight = newScale;
            newLeft = 0.0;
        }
        else if (newRight > thumbnail.getTotalLength())
        {
            newRight = thumbnail.getTotalLength();
            newLeft = newRight - newScale;
        }
        setVisibleThumbnailRange (Range<double> (newLeft, newRight));
    }
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
bool SampleDropArea::isInterestedInFileDrag (const StringArray& files)
{
    File potentialFile = File (files[0]);
    return ! potentialFile.isDirectory();
}

void SampleDropArea::filesDropped (const StringArray& files, int /*x*/, int /*y*/)
{
    DBG("files dropped");
    lastFileDropped = File (files[0]);
    thumbnail.setSource(new FileInputSource(lastFileDropped));
    const Range<double> newRange (0.0, thumbnail.getTotalLength());
    thumbnailScroller.setRangeLimits(newRange);
    setVisibleThumbnailRange(newRange);
    repaint();
}

void SampleDropArea::changeListenerCallback (ChangeBroadcaster* source)
{
    // this method is called by the thumbnail when it has changed, so we should repaint it..
    DBG("change listener callback");
    if (source == &thumbnail)
    {
        DBG("changebroadcaster source as the thumbnail");
        repaint();
    }
}

void SampleDropArea::scrollBarMoved (ScrollBar* scrollBarThatHasMoved, double newRangeStart)
{
    DBG ("scrollBarMoved ccalled");
    if (scrollBarThatHasMoved == &thumbnailScroller)
    {
        setVisibleThumbnailRange(visibleThumbnailRange.movedToStartAt (newRangeStart));
    }
}

void SampleDropArea::setVisibleThumbnailRange (Range<double> newRange)
{
    DBG ("setVisibleThumbnailRange called");
    visibleThumbnailRange = newRange;
    thumbnailScroller.setCurrentRange (visibleThumbnailRange, dontSendNotification);
    repaint();
}



//[/MiscUserCode]


//==============================================================================


//[EndFile] You can add extra defines here...
//[/EndFile]
