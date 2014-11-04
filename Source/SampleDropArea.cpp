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
    : thumbnailCache(5),
      thumbnail(512, formatManager, thumbnailCache)
{

    //[UserPreSize]
    //[/UserPreSize]

    setSize (200, 200);


    //[Constructor] You can add your own custom stuff here..
    formatManager.registerBasicFormats();
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
    //[/UserPrePaint]

    g.fillAll (Colours::white);

    //[UserPaint] Add your own custom painting code here..
    if (thumbnail.getTotalLength() > 0.0)
    {
        Rectangle<int> thumbArea = getLocalBounds();
        thumbnail.drawChannels(g, thumbArea, 0, thumbnail.getTotalLength(), 1.0f);
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



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
bool SampleDropArea::isInterestedInFileDrag (const StringArray& files)
{
    File potentialFile = File (files[0]);
    return ! potentialFile.isDirectory();
}

void SampleDropArea::filesDropped (const StringArray& files, int /*x*/, int /*y*/)
{
    lastFileDropped = File (files[0]);
    thumbnail.setSource(new FileInputSource(lastFileDropped));
    repaint();
}



//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Introjucer information section --

    This is where the Introjucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="SampleDropArea" componentName=""
                 parentClasses="public Component, public FileDragAndDropTarget"
                 constructorParams="" variableInitialisers="thumbnailCache(5),&#10;thumbnail(512, formatManager, thumbnailCache)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="0" initialWidth="200" initialHeight="200">
  <BACKGROUND backgroundColour="ffffffff"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
