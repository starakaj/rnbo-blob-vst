/*
 ==============================================================================

 RNBO_JuceAudioProcessorEditor.h
 Created: 21 Sep 2015 11:50:17am
 Author:  stb

 ==============================================================================
 */

#ifndef RNBO_JUCEAUDIOPROCESSOREDITOR_H_INCLUDED
#define RNBO_JUCEAUDIOPROCESSOREDITOR_H_INCLUDED

#include "JuceHeader.h"
#include "RNBO.h"
#include "BlobComponent.h"

namespace RNBO {

	//==============================================================================
	class BlobAudioProcessorEditor :
		public AudioProcessorEditor,
		public AsyncUpdater,
		public RNBO::EventHandler,
		public juce::Slider::Listener,
		public juce::AudioProcessorListener
	{
	public:
		//==============================================================================
		BlobAudioProcessorEditor (AudioProcessor* owner, CoreObject& rnboObject);
		~BlobAudioProcessorEditor() override;

		//==============================================================================
		void paint (Graphics&) override;
		void resized() override;

		void handleAsyncUpdate() override;

		void eventsAvailable() override;
		void handleParameterEvent(const RNBO::ParameterEvent& event) override;
		void handleMessageEvent(const MessageEvent& event) override;

		// Slider listener
		void sliderValueChanged (juce::Slider* slider) override;

		// AudioProcessorListener
		void audioProcessorChanged (AudioProcessor*, const ChangeDetails&) override;
		void audioProcessorParameterChanged (AudioProcessor*, int parameterIndex, float value) override;
		
	private:

		//==============================================================================
		CoreObject&								_rnboObject;
		ParameterEventInterfaceUniquePtr		_parameterInterface;

		juce::Slider							overblowSlider;
		juce::Slider							harmonicsSlider;
		ParameterIndex							overblowParamIndex;
		ParameterIndex							harmonicsParamIndex;

		Array <BlobComponent*>					blobs;	
	};

} // namespace RNBO

#endif  // RNBO_JUCEAUDIOPROCESSOREDITOR_H_INCLUDED
