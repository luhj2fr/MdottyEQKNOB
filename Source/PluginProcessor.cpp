#include "PluginProcessor.h"
#include "PluginEditor.h"

MdottyEQKNOBAudioProcessor::MdottyEQKNOBAudioProcessor()
    : AudioProcessor (BusesProperties()
                        .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      apvts (*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout MdottyEQKNOBAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // The one and only knob. 0.5 = center/clean, matching how it'll look on the GUI.
    params.push_back (std::make_unique<juce::AudioParameterFloat> (
        juce::ParameterID { "knob", 1 },
        "Filter",
        juce::NormalisableRange<float> (0.0f, 1.0f, 0.001f),
        0.5f));

    return { params.begin(), params.end() };
}

void MdottyEQKNOBAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = (juce::uint32) samplesPerBlock;
    spec.numChannels = 1;

    filterL.prepare (spec);
    filterR.prepare (spec);
    filterL.reset();
    filterR.reset();

    smoothedKnob.reset (sampleRate, 0.02); // 20ms smoothing so the sweep doesn't click/zipper
    smoothedKnob.setCurrentAndTargetValue (*apvts.getRawParameterValue ("knob"));
}

bool MdottyEQKNOBAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo()
        && layouts.getMainInputChannelSet()  == juce::AudioChannelSet::stereo();
}

void MdottyEQKNOBAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;

    const float knobTarget = *apvts.getRawParameterValue ("knob");
    smoothedKnob.setTargetValue (knobTarget);

    auto* left  = buffer.getWritePointer (0);
    auto* right = buffer.getNumChannels() > 1 ? buffer.getWritePointer (1) : nullptr;
    const int numSamples = buffer.getNumSamples();

    for (int i = 0; i < numSamples; ++i)
    {
        const float knob = smoothedKnob.getNextValue();

        // Dead zone around center so "clean" actually sounds clean, not almost-clean
        const float deadZone = 0.05f;

        float sampleL = left[i];
        float sampleR = right != nullptr ? right[i] : 0.0f;

        if (knob < 0.5f - deadZone)
        {
            // LEFT HALF: sweep into low-pass. Further left = darker/more underwater.
            const float amount = juce::jmap (knob, 0.0f, 0.5f - deadZone, 0.0f, 1.0f);
            const float cutoff = juce::jmap (1.0f - amount, 0.0f, 1.0f, 80.0f, 18000.0f);

            filterL.setType (Filter::Type::lowpass);
            filterR.setType (Filter::Type::lowpass);
            filterL.setCutoffFrequency (cutoff);
            filterR.setCutoffFrequency (cutoff);
            filterL.setResonance (0.5f + amount * 0.6f); // a bit of resonance boost as it sweeps = more character

            sampleL = filterL.processSample (0, sampleL);
            sampleR = filterR.processSample (0, sampleR);
        }
        else if (knob > 0.5f + deadZone)
        {
            // RIGHT HALF: sweep into high-pass. Further right = thinner/more telephone.
            const float amount = juce::jmap (knob, 0.5f + deadZone, 1.0f, 0.0f, 1.0f);
            const float cutoff = juce::jmap (amount, 0.0f, 1.0f, 20.0f, 4000.0f);

            filterL.setType (Filter::Type::highpass);
            filterR.setType (Filter::Type::highpass);
            filterL.setCutoffFrequency (cutoff);
            filterR.setCutoffFrequency (cutoff);
            filterL.setResonance (0.5f + amount * 0.4f);

            sampleL = filterL.processSample (0, sampleL);
            sampleR = filterR.processSample (0, sampleR);
        }
        // else: inside the dead zone, pass through untouched = truly clean center

        left[i] = sampleL;
        if (right != nullptr) right[i] = sampleR;
    }
}

void MdottyEQKNOBAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto state = apvts.copyState(); state.isValid())
        if (auto xml = state.createXml())
            copyXmlToBinary (*xml, destData);
}

void MdottyEQKNOBAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    if (auto xml = getXmlFromBinary (data, sizeInBytes))
        apvts.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessorEditor* MdottyEQKNOBAudioProcessor::createEditor()
{
    return new MdottyEQKNOBAudioProcessorEditor (*this);
}

// This creates new instances of the plugin
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new MdottyEQKNOBAudioProcessor();
}
