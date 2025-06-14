#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

namespace melatonin {
class Inspector;
}

class PluginProcessorBase;
class WidgetPanel;
template<class T> class ThreadSafeQueue;

//==============================================================================
class ZeeEditGui : public juce::AudioProcessorEditor, public juce::ChangeListener
{
public:
    ZeeEditGui(PluginProcessorBase& pluginProcessor, juce::AudioProcessorValueTreeState& valueTreeState, ThreadSafeQueue<juce::MidiBuffer>& inputMidiMessageQueue);
    ~ZeeEditGui() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    //==============================================================================
    void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
    std::vector<std::unique_ptr<WidgetPanel>> m_widgetPanels;
    ThreadSafeQueue<juce::MidiBuffer>& m_inputMidiMessageQueue;

    std::unique_ptr<melatonin::Inspector> m_inspector;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ZeeEditGui)
};
