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
#include "PluginProcessor.h"
//[/Headers]

#include "Gui.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
Gui::Gui (SamplerAudioProcessor &p)
    : AudioProcessorEditor (&p)
{
    addAndMakeVisible (sampleDropArea = new SampleDropArea (p));

    //[UserPreSize]
    //[/UserPreSize]

    setSize (600, 300);


    //[Constructor] You can add your own custom stuff here..
    //[/Constructor]
}

Gui::~Gui()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    sampleDropArea = nullptr;


    //[Destructor]. You can add your own custom destruction code here..
    //[/Destructor]
}

//==============================================================================
void Gui::paint (Graphics& g)
{
    //[UserPrePaint] Add your own custom painting code here..
    //[/UserPrePaint]

    g.fillAll (Colours::cornsilk);

    //[UserPaint] Add your own custom painting code here..
    //[/UserPaint]
}

void Gui::resized()
{
    sampleDropArea->setBounds (10, 10, getWidth() - 20, proportionOfHeight (0.5000f));
    //[UserResized] Add your own custom resize handling here..
    //[/UserResized]
}



//[MiscUserCode] You can add your own definitions of your custom methods or any other code here...
//[/MiscUserCode]


//==============================================================================
#if 0
/*  -- Introjucer information section --

    This is where the Introjucer stores the metadata that describe this GUI layout, so
    make changes in here at your peril!

BEGIN_JUCER_METADATA

<JUCER_COMPONENT documentType="Component" className="Gui" componentName="" parentClasses="public AudioProcessorEditor"
                 constructorParams="SamplerAudioProcessor &amp;p" variableInitialisers="AudioProcessorEditor (&amp;p)"
                 snapPixels="8" snapActive="1" snapShown="1" overlayOpacity="0.330"
                 fixedSize="1" initialWidth="600" initialHeight="300">
  <BACKGROUND backgroundColour="fffff8dc"/>
  <JUCERCOMP name="" id="cf0ea896cd8ceeb0" memberName="sampleDropArea" virtualName="SampleDropArea"
             explicitFocusOrder="0" pos="10 10 20M 50%" sourceFile="SampleDropArea.cpp"
             constructorParams="p"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
