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

#ifndef __JUCE_HEADER_1B7D314A9B5D26__
#define __JUCE_HEADER_1B7D314A9B5D26__

//[Headers]     -- You can add your own extra header files here --
#include "JuceHeader.h"
#include "SampleDropArea.h"
//[/Headers]



//==============================================================================
/**
                                                                    //[Comments]
    An auto-generated component, created by the Introjucer.

    Describe your class and how it works here!
                                                                    //[/Comments]
*/
class Gui  : public AudioProcessorEditor,
             public ComboBoxListener,
             public ButtonListener
{
public:
    //==============================================================================
    Gui (SamplerAudioProcessor &p, SampleDropArea &sampleDropArea_);
    ~Gui();

    //==============================================================================
    //[UserMethods]     -- You can add your own custom methods in this section.
    //[/UserMethods]

    void paint (Graphics& g);
    void resized();
    void comboBoxChanged (ComboBox* comboBoxThatHasChanged);
    void buttonClicked (Button* buttonThatWasClicked);



private:
    //[UserVariables]   -- You can add your own custom variables in this section.
    SamplerAudioProcessor& getSamplerAudioProcessor()
    {
        return static_cast<SamplerAudioProcessor&>(processor);
    }
    ReferenceCountedObjectPtr<SampleDropArea> sampleDropArea;
    //[/UserVariables]

    //==============================================================================
    ScopedPointer<ComboBox> rootMidiNoteComboBox;
    ScopedPointer<Label> midiRootNoteLabel;
    ScopedPointer<Label> numVoicesLabel;
    ScopedPointer<ComboBox> numVoicesComboBox;
    ScopedPointer<ComboBox> sustainModeComboBox;
    ScopedPointer<Label> sustainModeLabel;
    ScopedPointer<ToggleButton> syncToggleButton;
    ScopedPointer<Component> dummySampleArea;
    ScopedPointer<TextEditor> durationTextEditor;
    ScopedPointer<Label> durationLabel;
    ScopedPointer<Label> durationDescription;


    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Gui)
};

//[EndFile] You can add extra defines here...
//[/EndFile]

#endif   // __JUCE_HEADER_1B7D314A9B5D26__
