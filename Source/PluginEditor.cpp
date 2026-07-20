#include "PluginEditor.h"

MdottyEQKNOBAudioProcessorEditor::MdottyEQKNOBAudioProcessorEditor (MdottyEQKNOBAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Standard full-sweep rotary (like a real hardware knob), not the split-direction look
    knob.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    knob.setRotaryParameters (juce::MathConstants<float>::pi * 1.2f,
                               juce::MathConstants<float>::pi * 2.8f,
                               true);
    knob.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    knob.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xff5ec4ff));
    knob.setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (0xff23232e));
    knob.setColour (juce::Slider::thumbColourId, juce::Colours::white);
    knob.onValueChange = [this] { updateReadout(); };
    addAndMakeVisible (knob);

    titleLabel.setText ("MDOTTY EQ KNOB", juce::dontSendNotification);
    titleLabel.setJustificationType (juce::Justification::centred);
    titleLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    titleLabel.setFont (juce::Font (18.0f, juce::Font::bold));
    addAndMakeVisible (titleLabel);

    readoutLabel.setJustificationType (juce::Justification::centred);
    readoutLabel.setColour (juce::Label::textColourId, juce::Colour (0xff5ec4ff));
    readoutLabel.setFont (juce::Font (22.0f, juce::Font::bold));
    addAndMakeVisible (readoutLabel);

    underwaterTag.setText ("UNDERWATER", juce::dontSendNotification);
    underwaterTag.setJustificationType (juce::Justification::centred);
    underwaterTag.setColour (juce::Label::textColourId, juce::Colours::grey);
    underwaterTag.setFont (juce::Font (12.0f, juce::Font::bold));
    addAndMakeVisible (underwaterTag);

    knobAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (
        processor.apvts, "knob", knob);

    setSize (280, 340);
    updateReadout();

    // Poll at ~20fps so the readout also updates during host automation playback,
    // not just when the user is manually dragging the knob.
    startTimerHz (20);
}

MdottyEQKNOBAudioProcessorEditor::~MdottyEQKNOBAudioProcessorEditor()
{
    stopTimer();
}

void MdottyEQKNOBAudioProcessorEditor::timerCallback()
{
    updateReadout();
}

void MdottyEQKNOBAudioProcessorEditor::updateReadout()
{
    const float value = (float) knob.getValue(); // 0.0 to 1.0

    if (value < 0.005f)
    {
        readoutLabel.setText ("CLEAN", juce::dontSendNotification);
    }
    else
    {
        const int percent = juce::roundToInt (value * 100.0f);
        const float cutoffHz = juce::jmap (value, 0.0f, 1.0f, 18000.0f, 100.0f);

        juce::String hzText = cutoffHz >= 1000.0f
            ? juce::String (cutoffHz / 1000.0f, 1) + " kHz"
            : juce::String (juce::roundToInt (cutoffHz)) + " Hz";

        readoutLabel.setText (juce::String (percent) + "%   (" + hzText + ")", juce::dontSendNotification);
    }
}

void MdottyEQKNOBAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Simple vertical gradient background instead of flat colour
    juce::ColourGradient bg (juce::Colour (0xff1e1e28), 0, 0,
                              juce::Colour (0xff14141a), 0, (float) getHeight(), false);
    g.setGradientFill (bg);
    g.fillAll();

    g.setColour (juce::Colour (0xff33333f));
    g.drawRoundedRectangle (getLocalBounds().reduced (10).toFloat(), 14.0f, 1.5f);
}

void MdottyEQKNOBAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (20);

    titleLabel.setBounds (area.removeFromTop (34));
    area.removeFromTop (6);

    knob.setBounds (area.removeFromTop (190).reduced (25));
    area.removeFromTop (4);

    readoutLabel.setBounds (area.removeFromTop (32));
    area.removeFromTop (2);

    underwaterTag.setBounds (area.removeFromTop (20));
}
