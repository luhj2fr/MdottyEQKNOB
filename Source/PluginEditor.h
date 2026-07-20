#pragma once
#include "PluginProcessor.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_gui_basics/juce_gui_basics.h>

class MdottyEQKNOBAudioProcessorEditor : public juce::AudioProcessorEditor,
                                          private juce::Timer
{
public:
    explicit MdottyEQKNOBAudioProcessorEditor (MdottyEQKNOBAudioProcessor&);
    ~MdottyEQKNOBAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void updateReadout();

    MdottyEQKNOBAudioProcessor& processor;

    juce::Slider knob;
    juce::Label titleLabel;
    juce::Label readoutLabel;   // shows current % / Hz live
    juce::Label underwaterTag;

    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> knobAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MdottyEQKNOBAudioProcessorEditor)
};
