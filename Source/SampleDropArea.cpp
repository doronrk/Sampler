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
// MultiTimer Map:
    // 0 -> scroll bar remover
    // 1 -> repaint sample positions

//[/MiscUserDefs]

//==============================================================================
SampleDropArea::SampleDropArea (SamplerAudioProcessor &p)
    : thumbnailScroller(false),
      thumbnailCache(5),
      thumbnail(512, formatManager, thumbnailCache),
      sampler(p),
      zoomFactor(0.0)
{

    //[UserPreSize]
    //[/UserPreSize]

    setSize (200, 200);


    //[Constructor] You can add your own custom stuff here..
    formatManager.registerBasicFormats();

    // scrollbar
    addChildComponent(thumbnailScroller);
    thumbnailScroller.addListener(this);
    thumbnailScroller.setBounds (getLocalBounds().removeFromBottom (14).reduced (2));
    thumbnailScroller.setVisible(false);

    thumbnail.addChangeListener(this);
    
    startTimer(0, 1000);
    startTimer(1, 50);
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
        Rectangle<int> thumbArea = getLocalBounds();
        thumbArea.removeFromBottom(thumbnailScroller.getHeight() + 4);
        thumbnail.drawChannels(g, thumbArea.reduced(2), visibleThumbnailRange.getStart(), visibleThumbnailRange.getEnd(), 1.0f);
        thumbnailScroller.setBounds (getLocalBounds().removeFromBottom (14).reduced (2));
    }
    else
    {
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
        thumbnailScroller.setVisible(true);
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
    File lastFileDropped = File (files[0]);
    thumbnail.setSource(new FileInputSource(lastFileDropped));
    const Range<double> newRange (0.0, thumbnail.getTotalLength());
    thumbnailScroller.setRangeLimits(newRange);
    setVisibleThumbnailRange(newRange);
    repaint();
    reader = formatManager.createReaderFor(lastFileDropped);
    sampler.setNewSample(*reader);
}

void SampleDropArea::changeListenerCallback (ChangeBroadcaster* source)
{
    // this method is called by the thumbnail when it has changed, so we should repaint it..
    if (source == &thumbnail)
    {
        repaint();
    }
}

void SampleDropArea::scrollBarMoved (ScrollBar* scrollBarThatHasMoved, double newRangeStart)
{
    if (scrollBarThatHasMoved == &thumbnailScroller)
    {
        setVisibleThumbnailRange(visibleThumbnailRange.movedToStartAt (newRangeStart));
    }
}

void SampleDropArea::setVisibleThumbnailRange (Range<double> newRange)
{
    visibleThumbnailRange = newRange;
    thumbnailScroller.setCurrentRange (visibleThumbnailRange, dontSendNotification);
    repaint();
}

void SampleDropArea::timerCallback(int timerID)
{
    switch(timerID)
    {
        case 0:
            thumbnailScroller.setVisible(false);
            break;
        default:
            drawSamplePositions();
    }
    
}

void SampleDropArea::drawSamplePositions()
{
    Array<double> *positions = sampler.getSamplePositions();
    double thumbnailLength = thumbnail.getTotalLength();
    double visibleStart = visibleThumbnailRange.getStart() / thumbnailLength;
    double visibleLength = visibleThumbnailRange.getLength() / thumbnailLength;
    
    
    
    int numToDraw = jmin(positions->size(), positionMarkers.size());
    for (int i = 0; i < numToDraw; i++)
    {
        double relPosition = (positions->getUnchecked(i) - visibleStart) / visibleLength;
        double thumbnailPosition = relPosition * getWidth();
        DrawableRectangle *positionMarker = positionMarkers.getUnchecked(i);
        Rectangle<float> rect = Rectangle<float> (thumbnailPosition, 0.0, 1.5f, (float) (getHeight()));
        positionMarker->setRectangle(rect);
        positionMarker->setFill(FillType(Colours::yellow));
        positionMarker->setVisible(true);
    }
    // clear the voices not being played
    for (int i = numToDraw; i < positionMarkers.size(); i++)
    {
        DrawableRectangle *positionMarker = positionMarkers.getUnchecked(i);
        positionMarker->setVisible(false);
    }
}


void SampleDropArea::mouseDown(const MouseEvent &event)
{
    sampler.beginPreviewSound();
    
}

void SampleDropArea::mouseUp(const MouseEvent &event)
{
    sampler.endPreviewSound();
}

void SampleDropArea::clearPositionMarkers()
{
    positionMarkers.clearQuick(true);
}

void SampleDropArea::addPositionMarker()
{
    DrawableRectangle *rect = new DrawableRectangle();
    addAndMakeVisible(rect);
    positionMarkers.add(rect);
}


//[/MiscUserCode]


//==============================================================================


//[EndFile] You can add extra defines here...
//[/EndFile]
