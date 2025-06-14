#include "ZeeEditGui.h"

#include "WidgetPanel.h"
#include "LayoutProcessor.h"
#include "../ParameterMap.h"
#include "../PluginProcessorBase.h"
#include "../ThreadSafeQueue.h"

#include <melatonin_inspector/melatonin_inspector.h>

ZeeEditGui::ZeeEditGui(PluginProcessorBase& pluginProcessor, juce::AudioProcessorValueTreeState& valueTreeState, ThreadSafeQueue<juce::MidiBuffer>& inputMidiMessageQueue) :
    AudioProcessorEditor(&pluginProcessor),
    m_inputMidiMessageQueue(inputMidiMessageQueue)
{
    for (const settings::WidgetPanel& panel : ParameterMap::getPanels())
    {
        m_widgetPanels.push_back(std::make_unique<WidgetPanel>(panel, valueTreeState));
        addAndMakeVisible(*m_widgetPanels.back());
    }

    LayoutProcessor layoutProcessor(1500); // 1500 pixels wide maximum for the panel
    for (auto& panel : m_widgetPanels)
    {
        layoutProcessor.insert(*panel, panel->getLabelHeight());
    }
    setSize(layoutProcessor.getSize().getX(), layoutProcessor.getSize().getY());

#if JUCE_DEBUG
    m_inspector = std::make_unique<melatonin::Inspector>(*this);
    m_inspector->onClose = [this]() { m_inspector.reset(); };
    m_inspector->setVisible(true);
#endif
}

ZeeEditGui::~ZeeEditGui() = default;

void ZeeEditGui::paint(juce::Graphics& g)
{
    g.fillAll(getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    g.setFont(g.getCurrentFont().withHeight(11.0f));
}

void ZeeEditGui::resized()
{
}

void ZeeEditGui::changeListenerCallback (juce::ChangeBroadcaster* /*source*/)
{
    m_inputMidiMessageQueue.popAll([this](const juce::MidiBuffer& midiBuffer)
    {
        // TODO
    });
}
