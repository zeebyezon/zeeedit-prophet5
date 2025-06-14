// Created by David Bizon on 09/06/2025.
#pragma once

#include "PluginProcessorBase.h"
#include "juce_audio_basics/juce_audio_basics.h"
#include "ThreadSafeQueue.h"

namespace juce {
class AudioProcessor;
class AudioPlayHead;
class MidiBuffer;
}

class ZeeEdit : public PluginProcessorBase
{
public:
    ZeeEdit();
    ~ZeeEdit() override;

    juce::AudioProcessorEditor* createEditor() override;

    void pushOutputMessage(juce::MidiMessage message);

protected:
    juce::AudioProcessorValueTreeState& getParameters() override { return m_parameters; }
    void processMidiMessages(juce::MidiBuffer& midiMessages) override;

private:
    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void createParameterListeners();
    juce::AudioProcessorValueTreeState::Listener& createParameterListener(int channel, int ccNumber);

private:
    juce::AudioProcessorValueTreeState m_parameters;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::Listener>> m_parameterListeners;
    ThreadSafeQueue<juce::MidiMessage> m_outputMidiMessageQueue;

    ThreadSafeQueue<juce::MidiBuffer> m_inputMidiMessageQueue;
    juce::ChangeBroadcaster m_changeBroadcaster;
};
