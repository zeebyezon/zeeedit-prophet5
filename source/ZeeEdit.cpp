// Created by David Bizon on 09/06/2025.

#include "ZeeEdit.h"
#include "GUI/ZeeEditGui.h"
#include "ParameterMap.h"

ZeeEdit::ZeeEdit() :
    PluginProcessorBase(),
    m_parameters(*this, nullptr, "parameters", createParameterLayout())
{
    createParameterListeners();
}

ZeeEdit::~ZeeEdit() = default;

juce::AudioProcessorValueTreeState::ParameterLayout ZeeEdit::createParameterLayout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout params;
    for (const settings::WidgetPanel& panel : ParameterMap::getPanels())
    {
        for (const auto& widget : panel.widgets)
        {
            params.add(std::make_unique<juce::AudioParameterInt>(
                ParameterMap::generateParameterID(panel.panel.name, widget.name),
                widget.name,
                widget.valueRange.minValue,
                widget.valueRange.maxValue,
                widget.valueRange.defaultValue));
        }
    }
    return params;
}

void ZeeEdit::createParameterListeners()
{
    for (const settings::WidgetPanel& panel : ParameterMap::getPanels())
    {
        for (const auto& widget : panel.widgets)
        {
            const auto parameterID = ParameterMap::generateParameterID (panel.panel.name, widget.name);
            m_parameters.addParameterListener (parameterID, &createParameterListener (widget.midiConfig.channel, widget.midiConfig.ccNumber));
        }
    }
}

class ParameterListener : public juce::AudioProcessorValueTreeState::Listener
{
public:
    ParameterListener(ZeeEdit& zeeEdit, int channel, int ccNumber) :
        m_parent(zeeEdit),
        m_channel(channel),
        m_ccNumber(ccNumber)
    {
    }

    ~ParameterListener() override = default;

    void parameterChanged(const juce::String& /*parameterID*/, float newValue) override
    {
        m_parent.pushOutputMessage(juce::MidiMessage::controllerEvent(m_channel, m_ccNumber, static_cast<int>(newValue)));
    }

private:
    ZeeEdit& m_parent;
    int m_channel;
    int m_ccNumber;
};

juce::AudioProcessorValueTreeState::Listener& ZeeEdit::createParameterListener(int channel, int ccNumber)
{
    m_parameterListeners.push_back(std::make_unique<ParameterListener>(*this, channel, ccNumber));
    return *m_parameterListeners.back();
}

void ZeeEdit::pushOutputMessage(juce::MidiMessage message)
{
    m_outputMidiMessageQueue.push(std::move(message));
}

void ZeeEdit::processMidiMessages(juce::MidiBuffer& midiMessages)
{
    juce::MidiBuffer inputBuffer;
    midiMessages.swapWith(inputBuffer); // midiMessages is now the output buffer
    if (!inputBuffer.isEmpty())
    {
        m_inputMidiMessageQueue.push(std::move(inputBuffer));
        m_changeBroadcaster.sendChangeMessage();
    }

    m_outputMidiMessageQueue.popAll([&midiMessages](const juce::MidiMessage& midiMessage)
    {
        midiMessages.addEvent(midiMessage, 0);
    });
}

//==============================================================================

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new ZeeEdit();
}

juce::AudioProcessorEditor* ZeeEdit::createEditor()
{
    auto gui = new ZeeEditGui(*this, m_parameters, m_inputMidiMessageQueue);
    m_changeBroadcaster.addChangeListener(gui);
    return gui;
}
