/*
  ==============================================================================

	BlobAudioProcessorEditor.cpp
	Created: 21 Sep 2015 11:50:17am
	Author:  stb

  ==============================================================================
*/

#include "BlobAudioProcessorEditor.h"

namespace RNBO {

//==============================================================================
BlobAudioProcessorEditor::BlobAudioProcessorEditor(AudioProcessor* const p, CoreObject& rnboObject)
: AudioProcessorEditor (p)
, _rnboObject(rnboObject)
, _parameterInterface(_rnboObject.createParameterInterface(ParameterEventInterface::SingleProducer, this))
{
	jassert (p != nullptr);
	setOpaque (true);
	p->addListener(this);

	addAndMakeVisible (overblowSlider);
	overblowSlider.addListener (this);

	addAndMakeVisible (harmonicsSlider);
	harmonicsSlider.addListener (this);

	ParameterInfo paramInfo;
	for (ParameterIndex i = 0; i < _rnboObject.getNumParameters(); i++) {
		ConstCharPointer pid = _rnboObject.getParameterId(i);
		if (strcmp(pid, "harmonics") == 0) {
			harmonicsParamIndex = i;
			_rnboObject.getParameterInfo(i, &paramInfo);
			harmonicsSlider.setRange(paramInfo.min, paramInfo.max);
		} else if (strcmp(pid, "overblow") == 0) {
			overblowParamIndex = i;
			_rnboObject.getParameterInfo(i, &paramInfo);
			overblowSlider.setRange(paramInfo.min, paramInfo.max);
		}
	}

	for (int i = 0; i < 8; i++) {
		blobs.add(new BlobComponent(Colour::fromHSV((float) i / 8.0, 1.0, 0.5, 255)));
		addAndMakeVisible( blobs[i] );
	}

	setResizable( true, true );
	setSize (400, 400);
}

BlobAudioProcessorEditor::~BlobAudioProcessorEditor()
{
	processor.removeListener(this);
	for (int i = 0; i < 8; i++) {
		BlobComponent *b = blobs.removeAndReturn(0);
		delete b;
	}
}

void BlobAudioProcessorEditor::paint (Graphics& g)
{
	g.fillAll (Colours::darkgrey);
}

void BlobAudioProcessorEditor::resized()
{
	// position the blobs in the space at the top
	auto blobBounds = getBounds().withBottom(getHeight() - 80);
	auto blobWidth = blobBounds.getWidth() / 4.0;
	auto blobHeight = blobBounds.getHeight() / 2.0;
	for (int i = 0; i < 8; i++) {
		auto myBounds = blobBounds.withX((i % 4) * blobWidth);
		myBounds.setY((i / 4) * blobHeight);
		myBounds.setWidth(blobWidth);
		myBounds.setHeight(blobHeight);
		blobs[i]->setBounds( myBounds );
	}

	// position the sliders at the bottom
	auto bottomBounds = getBounds().withTop(getHeight() - 80);

	overblowSlider.setBounds (bottomBounds.withWidth(getWidth() / 2).withX(0).reduced(4));
	harmonicsSlider.setBounds (bottomBounds.withLeft(getWidth() / 2).withWidth(getWidth() / 2).reduced(4));
}

void BlobAudioProcessorEditor::sliderValueChanged (juce::Slider* slider)
{
	juce::AudioProcessorParameter *param = nullptr;
	int index = -1;
	double value = 0.0;
	if (slider == &overblowSlider) {
		index = overblowParamIndex;
		param = processor.getParameters()[overblowParamIndex];
	} else if (slider == &harmonicsSlider) {
		index = harmonicsParamIndex;
		param = processor.getParameters()[harmonicsParamIndex];
	}

	if (param && index >= 0) {
		value = _rnboObject.convertToNormalizedParameterValue(index, slider->getValue());
		param->beginChangeGesture();
		param->setValueNotifyingHost(value);
		param->endChangeGesture();
	}
}

void BlobAudioProcessorEditor::audioProcessorChanged (AudioProcessor*, const ChangeDetails&) { }

void BlobAudioProcessorEditor::audioProcessorParameterChanged (AudioProcessor*, int parameterIndex, float value)
{
	juce::Slider *slider = nullptr;
	if (parameterIndex == overblowParamIndex) {
		slider = &overblowSlider;
	} else if (parameterIndex == harmonicsParamIndex) {
		slider = &harmonicsSlider;
	}

	if (slider != nullptr) {
		double trueValue = _rnboObject.convertFromNormalizedParameterValue(parameterIndex, value);
		slider->setValue(trueValue, juce::dontSendNotification);

		ParameterValue hv = _rnboObject.getParameterNormalized(harmonicsParamIndex);
		ParameterValue ov = 1.0 - _rnboObject.getParameterNormalized(overblowParamIndex); // invert
		for (BlobComponent *blob: blobs) {
			blob->stretchPointTarget(hv, ov);
		}
	}
}

void BlobAudioProcessorEditor::handleAsyncUpdate()
{
	drainEvents();
}

void BlobAudioProcessorEditor::eventsAvailable()
{
	this->triggerAsyncUpdate();
}

void BlobAudioProcessorEditor::handleParameterEvent(const ParameterEvent&)
{

}

void BlobAudioProcessorEditor::handleMessageEvent(const MessageEvent& event) {
	if (event.getType() == MessageEvent::List) {
		if (event.getTag() == TAG("active")) {
			auto list = event.getListValue().get();
			unsigned int idx = list->operator[](0) - 1;
			bool active = list->operator[](1) > 0;
			blobs[idx]->active(active);
		}
	}
}

} // namespace RNBO
