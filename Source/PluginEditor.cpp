#include "PluginEditor.h"

MdottyEQKNOBAudioProcessorEditor::MdottyEQKNOBAudioProcessorEditor (MdottyEQKNOBAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    knob.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    knob.setRotaryParameters (juce::MathConstants<float>::pi * 1.2f,
                               juce::MathConstants<float>::pi * 2.8f,
                               true);
    knob.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    knob.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xff4a9eff));
    knob.setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (0xff2a2a35));
    knob.setColour (juce::Slider::thumbColourId, juce::Colours::white);
    addAndMakeVisible (knob);

    knobLabel.setText ("MDOTTY EQ KNOB", juce::dontSendNotification);
    knobLabel.setJustificationType (juce::Justification::centred);
    knobLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    knobLabel.setFont (juce::Font (16.0f, juce::Font::bold));
    addAndMakeVisible (knobLabel);

    knobAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processor.apvts, "knob", knob);

    setSize (260, 320);
}

void MdottyEQKNOBAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour (0xff1a1a22));

    g.setColour (juce::Colour (0xff33333f));
    g.drawRoundedRectangle (getLocalBounds().reduced (10).toFloat(), 12.0f, 1.5f);

    // Small tick labels so the sweep direction is obvious
    g.setColour (juce::Colours::grey);
    g.setFont (11.0f);
    g.drawText ("UNDERWATER", 20, 250, 100, 20, juce::Justification::left);
    g.drawText ("TELEPHONE", 140, 250, 100, 20, juce::Justification::right);
}

void MdottyEQKNOBAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (20);
    knobLabel.setBounds (area.removeFromTop (30));
    area.removeFromTop (10);
    knob.setBounds (area.removeFromTop (180).reduced (20));
}
