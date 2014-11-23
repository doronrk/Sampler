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

#include "Gui.h"


//[MiscUserDefs] You can add your own user definitions and misc code here...
//[/MiscUserDefs]

//==============================================================================
Gui::Gui (SamplerAudioProcessor &p, SampleDropArea &sampleDropArea_)
    : AudioProcessorEditor (&p)
{
    addAndMakeVisible (rootMidiNoteComboBox = new ComboBox ("root midi note"));
    rootMidiNoteComboBox->setEditableText (false);
    rootMidiNoteComboBox->setJustificationType (Justification::centredLeft);
    rootMidiNoteComboBox->setTextWhenNothingSelected (String::empty);
    rootMidiNoteComboBox->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    rootMidiNoteComboBox->addListener (this);

    addAndMakeVisible (midiRootNoteLabel = new Label ("midi root note label",
                                                      TRANS("root note")));
    midiRootNoteLabel->setFont (Font (15.00f, Font::plain));
    midiRootNoteLabel->setJustificationType (Justification::centredLeft);
    midiRootNoteLabel->setEditable (false, false, false);
    midiRootNoteLabel->setColour (TextEditor::textColourId, Colours::black);
    midiRootNoteLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (numVoicesLabel = new Label ("num voices label",
                                                   TRANS("num voices")));
    numVoicesLabel->setFont (Font (15.00f, Font::plain));
    numVoicesLabel->setJustificationType (Justification::centredLeft);
    numVoicesLabel->setEditable (false, false, false);
    numVoicesLabel->setColour (TextEditor::textColourId, Colours::black);
    numVoicesLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (numVoicesComboBox = new ComboBox ("num voices combo box"));
    numVoicesComboBox->setEditableText (false);
    numVoicesComboBox->setJustificationType (Justification::centredLeft);
    numVoicesComboBox->setTextWhenNothingSelected (String::empty);
    numVoicesComboBox->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    numVoicesComboBox->addListener (this);

    addAndMakeVisible (sustainModeComboBox = new ComboBox ("sustain mode combo box"));
    sustainModeComboBox->setEditableText (false);
    sustainModeComboBox->setJustificationType (Justification::centredLeft);
    sustainModeComboBox->setTextWhenNothingSelected (TRANS("sustain mode"));
    sustainModeComboBox->setTextWhenNoChoicesAvailable (TRANS("(no choices)"));
    sustainModeComboBox->addItem (TRANS("single"), 1);
    sustainModeComboBox->addItem (TRANS("loop"), 2);
    sustainModeComboBox->addItem (TRANS("reverse"), 3);
    sustainModeComboBox->addItem (TRANS("loopReverse"), 4);
    sustainModeComboBox->addListener (this);

    addAndMakeVisible (sustainModeLabel = new Label ("sustain mode label",
                                                     TRANS("sustain mode")));
    sustainModeLabel->setFont (Font (15.00f, Font::plain));
    sustainModeLabel->setJustificationType (Justification::centredLeft);
    sustainModeLabel->setEditable (false, false, false);
    sustainModeLabel->setColour (TextEditor::textColourId, Colours::black);
    sustainModeLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (syncToggleButton = new ToggleButton ("sync toggle button"));
    syncToggleButton->setButtonText (TRANS("sync mode"));
    syncToggleButton->addListener (this);
    syncToggleButton->setToggleState (true, dontSendNotification);

    addAndMakeVisible (dummySampleArea = new Component());
    dummySampleArea->setName ("sample drop dummy area");

    addAndMakeVisible (durationTextEditor = new TextEditor ("duration text editor"));
    durationTextEditor->setMultiLine (false);
    durationTextEditor->setReturnKeyStartsNewLine (false);
    durationTextEditor->setReadOnly (false);
    durationTextEditor->setScrollbarsShown (true);
    durationTextEditor->setCaretVisible (true);
    durationTextEditor->setPopupMenuEnabled (true);
    durationTextEditor->setText (TRANS("1.0"));

    addAndMakeVisible (durationLabel = new Label ("duration label",
                                                  TRANS("sync duration\n")));
    durationLabel->setFont (Font (15.00f, Font::plain));
    durationLabel->setJustificationType (Justification::centredLeft);
    durationLabel->setEditable (false, false, false);
    durationLabel->setColour (TextEditor::textColourId, Colours::black);
    durationLabel->setColour (TextEditor::backgroundColourId, Colour (0x00000000));

    addAndMakeVisible (durationDescription = new Label ("new label",
                                                        TRANS("1.0 = 1/4 note\n"
                                                        "1.5 = dotted 1/4 note\n"
                                                        "2.0 = 1/2 note\n"
                                                        "etc.")));
    durationDescription->setFont (Font (14.00f, Font::plain));
    durationDescription->setJustificationType (Justification::topLeft);
    durationDescription->setEditable (false, false, false);
    durationDescription->setColour (TextEditor::textColourId, Colours::black);
    durationDescription->setColour (TextEditor::backgroundColourId, Colour (0x00000000));


    //[UserPreSize]
    sampleDropArea = &sampleDropArea_;
    addAndMakeVisible (sampleDropArea);
    //[/UserPreSize]

    setSize (600, 300);


    //[Constructor] You can add your own custom stuff here..
    for (int noteNum = 0; noteNum < 128; noteNum++)
    {
        String midiNote = MidiMessage::getMidiNoteName(noteNum, true, true, 3);
        rootMidiNoteComboBox->addItem(midiNote, noteNum + 1);
    }
    for (int voiceNum = 1; voiceNum <= 16; voiceNum++)
    {
        numVoicesComboBox->addItem(String(voiceNum), voiceNum);
    }
    numVoicesComboBox->setSelectedId(p.getNumVoices());
    rootMidiNoteComboBox->setSelectedItemIndex(p.getRootMidiNote());
    sustainModeComboBox->setSelectedItemIndex(p.getSustainMode());
    syncToggleButton->setToggleState(p.getSyncState(), sendNotification);
    //[/Constructor]
}

Gui::~Gui()
{
    //[Destructor_pre]. You can add your own custom destruction code here..
    //[/Destructor_pre]

    rootMidiNoteComboBox = nullptr;
    midiRootNoteLabel = nullptr;
    numVoicesLabel = nullptr;
    numVoicesComboBox = nullptr;
    sustainModeComboBox = nullptr;
    sustainModeLabel = nullptr;
    syncToggleButton = nullptr;
    dummySampleArea = nullptr;
    durationTextEditor = nullptr;
    durationLabel = nullptr;
    durationDescription = nullptr;


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
    rootMidiNoteComboBox->setBounds (10 + 2, (10 + proportionOfHeight (0.5000f)) + 25, 100, 24);
    midiRootNoteLabel->setBounds (10 + 2, 10 + proportionOfHeight (0.5000f), 100, 25);
    numVoicesLabel->setBounds ((10 + 2) + 100 - -10, (10 + proportionOfHeight (0.5000f)) + 0, 100, 24);
    numVoicesComboBox->setBounds ((10 + 2) + 100 - -10, ((10 + proportionOfHeight (0.5000f)) + 25) + 0, 100, 24);
    sustainModeComboBox->setBounds (((10 + 2) + 100 - -10) + 100 - -10, (((10 + proportionOfHeight (0.5000f)) + 25) + 0) + 0, 100, 24);
    sustainModeLabel->setBounds (((10 + 2) + 100 - -10) + 100 - -10, ((10 + proportionOfHeight (0.5000f)) + 0) + 0, 100, 24);
    syncToggleButton->setBounds ((((10 + 2) + 100 - -10) + 100 - -10) + 100 - -10, ((((10 + proportionOfHeight (0.5000f)) + 25) + 0) + 0) + 0, 100, 24);
    dummySampleArea->setBounds (10, 10, getWidth() - 20, proportionOfHeight (0.5000f));
    durationTextEditor->setBounds (((((10 + 2) + 100 - -10) + 100 - -10) + 100 - -10) + 100 - -10, (((((10 + proportionOfHeight (0.5000f)) + 25) + 0) + 0) + 0) + 0, 100, 24);
    durationLabel->setBounds ((((((10 + 2) + 100 - -10) + 100 - -10) + 100 - -10) + 100 - -10) + 0, ((((((10 + proportionOfHeight (0.5000f)) + 25) + 0) + 0) + 0) + 0) + -24, 100, 24);
    durationDescription->setBounds ((((((10 + 2) + 100 - -10) + 100 - -10) + 100 - -10) + 100 - -10) + 0, ((((((10 + proportionOfHeight (0.5000f)) + 25) + 0) + 0) + 0) + 0) + 31, 140, 80);
    //[UserResized] Add your own custom resize handling here..
    sampleDropArea->setBounds (10, 10, getWidth() - 20, proportionOfHeight (0.5000f));
    //[/UserResized]
}

void Gui::comboBoxChanged (ComboBox* comboBoxThatHasChanged)
{
    //[UsercomboBoxChanged_Pre]
    SamplerAudioProcessor &p = getSamplerAudioProcessor();
    //[/UsercomboBoxChanged_Pre]

    if (comboBoxThatHasChanged == rootMidiNoteComboBox)
    {
        //[UserComboBoxCode_rootMidiNoteComboBox] -- add your combo box handling code here..
        int midiNum = rootMidiNoteComboBox->getSelectedItemIndex();
        p.setRootMidiNote(midiNum);
        //[/UserComboBoxCode_rootMidiNoteComboBox]
    }
    else if (comboBoxThatHasChanged == numVoicesComboBox)
    {
        //[UserComboBoxCode_numVoicesComboBox] -- add your combo box handling code here..
        int numVoices = numVoicesComboBox->getSelectedId();
        p.setNumVoices(numVoices);
        //[/UserComboBoxCode_numVoicesComboBox]
    }
    else if (comboBoxThatHasChanged == sustainModeComboBox)
    {
        //[UserComboBoxCode_sustainModeComboBox] -- add your combo box handling code here..
        int mode = sustainModeComboBox->getSelectedItemIndex();
        p.setSustainMode(SyncSamplerSound::SustainMode(mode));
        //[/UserComboBoxCode_sustainModeComboBox]
    }

    //[UsercomboBoxChanged_Post]
    //[/UsercomboBoxChanged_Post]
}

void Gui::buttonClicked (Button* buttonThatWasClicked)
{
    //[UserbuttonClicked_Pre]
    SamplerAudioProcessor &p = getSamplerAudioProcessor();
    //[/UserbuttonClicked_Pre]

    if (buttonThatWasClicked == syncToggleButton)
    {
        //[UserButtonCode_syncToggleButton] -- add your button handler code here..
        bool isOn = syncToggleButton->getToggleState();
        durationTextEditor->setVisible(isOn);
        durationLabel->setVisible(isOn);
        durationDescription->setVisible(isOn);
        p.setSyncState(isOn);
        //[/UserButtonCode_syncToggleButton]
    }

    //[UserbuttonClicked_Post]
    //[/UserbuttonClicked_Post]
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
                 constructorParams="SamplerAudioProcessor &amp;p, SampleDropArea &amp;sampleDropArea_"
                 variableInitialisers="AudioProcessorEditor (&amp;p)" snapPixels="8"
                 snapActive="1" snapShown="1" overlayOpacity="0.330" fixedSize="1"
                 initialWidth="600" initialHeight="300">
  <BACKGROUND backgroundColour="fffff8dc"/>
  <COMBOBOX name="root midi note" id="f6fc173651f2492f" memberName="rootMidiNoteComboBox"
            virtualName="" explicitFocusOrder="0" pos="2 0R 100 24" posRelativeX="fc1386177eee9413"
            posRelativeY="4975a32468005f6" editable="0" layout="33" items=""
            textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <LABEL name="midi root note label" id="4975a32468005f6" memberName="midiRootNoteLabel"
         virtualName="" explicitFocusOrder="0" pos="2 0R 100 25" posRelativeX="fc1386177eee9413"
         posRelativeY="fc1386177eee9413" edTextCol="ff000000" edBkgCol="0"
         labelText="root note" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <LABEL name="num voices label" id="264096f26a51251d" memberName="numVoicesLabel"
         virtualName="" explicitFocusOrder="0" pos="-10R 0 100 24" posRelativeX="4975a32468005f6"
         posRelativeY="4975a32468005f6" edTextCol="ff000000" edBkgCol="0"
         labelText="num voices" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <COMBOBOX name="num voices combo box" id="ddcb0aec310aa6ce" memberName="numVoicesComboBox"
            virtualName="" explicitFocusOrder="0" pos="-10R 0 100 24" posRelativeX="f6fc173651f2492f"
            posRelativeY="f6fc173651f2492f" editable="0" layout="33" items=""
            textWhenNonSelected="" textWhenNoItems="(no choices)"/>
  <COMBOBOX name="sustain mode combo box" id="b102947079127670" memberName="sustainModeComboBox"
            virtualName="" explicitFocusOrder="0" pos="-10R 0 100 24" posRelativeX="ddcb0aec310aa6ce"
            posRelativeY="ddcb0aec310aa6ce" editable="0" layout="33" items="single&#10;loop&#10;reverse&#10;loopReverse"
            textWhenNonSelected="sustain mode" textWhenNoItems="(no choices)"/>
  <LABEL name="sustain mode label" id="4de0cd67965de3f2" memberName="sustainModeLabel"
         virtualName="" explicitFocusOrder="0" pos="-10R 0 100 24" posRelativeX="264096f26a51251d"
         posRelativeY="264096f26a51251d" edTextCol="ff000000" edBkgCol="0"
         labelText="sustain mode" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <TOGGLEBUTTON name="sync toggle button" id="7e6e8650b47058e5" memberName="syncToggleButton"
                virtualName="" explicitFocusOrder="0" pos="-10R 0 100 24" posRelativeX="b102947079127670"
                posRelativeY="b102947079127670" buttonText="sync mode" connectedEdges="0"
                needsCallback="1" radioGroupId="0" state="1"/>
  <GENERICCOMPONENT name="sample drop dummy area" id="fc1386177eee9413" memberName="dummySampleArea"
                    virtualName="" explicitFocusOrder="0" pos="10 10 20M 50%" class="Component"
                    params=""/>
  <TEXTEDITOR name="duration text editor" id="c25124e384d357b1" memberName="durationTextEditor"
              virtualName="" explicitFocusOrder="0" pos="-10R 0 100 24" posRelativeX="7e6e8650b47058e5"
              posRelativeY="7e6e8650b47058e5" initialText="1.0" multiline="0"
              retKeyStartsLine="0" readonly="0" scrollbars="1" caret="1" popupmenu="1"/>
  <LABEL name="duration label" id="9221a40d06867547" memberName="durationLabel"
         virtualName="" explicitFocusOrder="0" pos="0 -24 100 24" posRelativeX="c25124e384d357b1"
         posRelativeY="c25124e384d357b1" edTextCol="ff000000" edBkgCol="0"
         labelText="sync duration&#10;" editableSingleClick="0" editableDoubleClick="0"
         focusDiscardsChanges="0" fontname="Default font" fontsize="15"
         bold="0" italic="0" justification="33"/>
  <LABEL name="new label" id="ba07cc446b44bd0a" memberName="durationDescription"
         virtualName="" explicitFocusOrder="0" pos="0 31 140 80" posRelativeX="c25124e384d357b1"
         posRelativeY="c25124e384d357b1" edTextCol="ff000000" edBkgCol="0"
         labelText="1.0 = 1/4 note&#10;1.5 = dotted 1/4 note&#10;2.0 = 1/2 note&#10;etc."
         editableSingleClick="0" editableDoubleClick="0" focusDiscardsChanges="0"
         fontname="Default font" fontsize="14" bold="0" italic="0" justification="9"/>
</JUCER_COMPONENT>

END_JUCER_METADATA
*/
#endif


//[EndFile] You can add extra defines here...
//[/EndFile]
