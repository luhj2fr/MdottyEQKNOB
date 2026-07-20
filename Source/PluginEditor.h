#pragma once
#include "PluginProcessor.h"
#include <juce_audio_processors/juce_audio_processors.h>

class MdottyEQKNOBAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit MdottyEQKNOBAudioProcessorEditor (MdottyEQKNOBAudioProcessor&);
    ~MdottyEQKNOBAudioProcessorEditor() override = default;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    MdottyEQKNOBAudioProcessor& processor;

    juce::Slider knob;
    juce::Label knobLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> knobAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MdottyEQKNOBAudioProcessorEditor)
};
