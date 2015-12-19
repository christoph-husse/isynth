/*
The MIT License (MIT)

Copyright (c) 2014 Christoph Husse

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

#include "stdafx.h"

#ifndef IS_EMSCRIPTEN

#define DONT_SET_USING_JUCE_NAMESPACE 1
#include "juce/juce.h"

#include "Engine2D.hpp"

using namespace Framework;
using namespace Engine2D;

template<class TMessage, class ...TArgs>
void AddMessage(std::shared_ptr<MidiMessage>& msg, std::shared_ptr<MidiTrack> track, TArgs... args)
{
	assert(msg == nullptr);

	msg = track->AddMessage<TMessage>(TMessage{ args... });
}

namespace MidiParser
{
	std::shared_ptr<MidiFile> ParseMidiFile(std::vector<unsigned char> fileBytes)
	{
		juce::MidiFile midiFile;
		juce::MemoryInputStream fileData(fileBytes.data(), fileBytes.size(), false);

		if (!midiFile.readFrom(fileData))
			return nullptr;

		auto result = std::make_shared<MidiFile>();
		MidiFile& midi = *result;

		auto format = midiFile.getTimeFormat();

		for (int iTrack = 0; iTrack < midiFile.getNumTracks(); iTrack++)
		{
			auto juceTrack = midiFile.getTrack(iTrack);
			auto track = midi.AddTrack();

			for (int iEvent = 0; iEvent < juceTrack->getNumEvents(); iEvent++)
			{
				auto juceEvent = juceTrack->getEventPointer(iEvent);
				auto juceMsg = juceEvent->message;
				auto juceOffMsg = (juceEvent->noteOffObject != nullptr) ? juceEvent->noteOffObject->message : juce::MidiMessage();
				std::shared_ptr<MidiMessage> msg = nullptr;

#define VALUE_AND_STRING(val) val, #val

				// unhandled
				if (juceMsg.isActiveSense())
					AddMessage<MidiUnhandledMessage>(msg, track, VALUE_AND_STRING(MidiMessageType::ActiveSense));
				if (juceMsg.isAftertouch())
					AddMessage<MidiUnhandledMessage>(msg, track, VALUE_AND_STRING(MidiMessageType::Aftertouch));
				if (juceMsg.isChannelPressure())
					AddMessage<MidiUnhandledMessage>(msg, track, VALUE_AND_STRING(MidiMessageType::ChannelPressure));
				if (juceMsg.isFullFrame())
					AddMessage<MidiUnhandledMessage>(msg, track, VALUE_AND_STRING(MidiMessageType::FullFrame));
				if (juceMsg.isMidiClock())
					AddMessage<MidiUnhandledMessage>(msg, track, VALUE_AND_STRING(MidiMessageType::MidiClock));
				if (juceMsg.isMidiMachineControlMessage())
					AddMessage<MidiUnhandledMessage>(msg, track, VALUE_AND_STRING(MidiMessageType::MidiMachineControlMessage));
				if (juceMsg.isPitchWheel())
					AddMessage<MidiUnhandledMessage>(msg, track, VALUE_AND_STRING(MidiMessageType::PitchWheel));
				if (juceMsg.isProgramChange())
					AddMessage<MidiUnhandledMessage>(msg, track, VALUE_AND_STRING(MidiMessageType::ProgramChange));
				if (juceMsg.isQuarterFrame())
					AddMessage<MidiUnhandledMessage>(msg, track, VALUE_AND_STRING(MidiMessageType::QuarterFrame));
				if (juceMsg.isSongPositionPointer())
					AddMessage<MidiUnhandledMessage>(msg, track, VALUE_AND_STRING(MidiMessageType::SongPositionPointer));
				if (juceMsg.isSysEx())
					AddMessage<MidiUnhandledMessage>(msg, track, VALUE_AND_STRING(MidiMessageType::SysEx));

				// special
				if (juceMsg.isNoteOnOrOff())
				{
					bool isNoteOn = juceMsg.isNoteOn();
					AddMessage<MidiNoteMessage>(msg, track,
						isNoteOn ? MidiMessageType::NoteOn : MidiMessageType::NoteOff,
						isNoteOn,
						juceMsg.getNoteNumber(),
						juceMsg.getFloatVelocity(),
						juceMsg.getMidiNoteName(juceMsg.getNoteNumber(), true, true, 5).toStdString());
				}

				// commands
				if (juceMsg.isAllNotesOff())
					AddMessage<MidiCommandMessage>(msg, track, MidiMessageType::AllNotesOff, MidiCommands::AllNotesOff);
				else if (juceMsg.isAllSoundOff())
					AddMessage<MidiCommandMessage>(msg, track, MidiMessageType::AllSoundOff, MidiCommands::AllSoundOff);
				else if (juceMsg.isMidiContinue())
					AddMessage<MidiCommandMessage>(msg, track, MidiMessageType::MidiContinue, MidiCommands::MidiContinue);
				else if (juceMsg.isMidiStart())
					AddMessage<MidiCommandMessage>(msg, track, MidiMessageType::MidiStart, MidiCommands::MidiStart);
				else if (juceMsg.isMidiStop())
					AddMessage<MidiCommandMessage>(msg, track, MidiMessageType::MidiStop, MidiCommands::MidiStop);
				else if (juceMsg.isSoftPedalOff())
					AddMessage<MidiCommandMessage>(msg, track, MidiMessageType::SoftPedalOff, MidiCommands::SoftPedalOff);
				else if (juceMsg.isSoftPedalOn())
					AddMessage<MidiCommandMessage>(msg, track, MidiMessageType::SoftPedalOn, MidiCommands::SoftPedalOn);
				else if (juceMsg.isSostenutoPedalOff())
					AddMessage<MidiCommandMessage>(msg, track, MidiMessageType::SostenutoPedalOff, MidiCommands::SostenutoPedalOff);
				else if (juceMsg.isSostenutoPedalOn())
					AddMessage<MidiCommandMessage>(msg, track, MidiMessageType::SostenutoPedalOn, MidiCommands::SostenutoPedalOn);
				else if (juceMsg.isSustainPedalOff())
					AddMessage<MidiCommandMessage>(msg, track, MidiMessageType::SustainPedalOff, MidiCommands::SustainPedalOff);
				else if (juceMsg.isSustainPedalOn())
					AddMessage<MidiCommandMessage>(msg, track, MidiMessageType::SustainPedalOn, MidiCommands::SustainPedalOn);
				else if (juceMsg.isController())
					AddMessage<MidiUnhandledMessage>(msg, track, VALUE_AND_STRING(MidiMessageType::Controller));

				// meta events
				if (juceMsg.isEndOfTrackMetaEvent())
					AddMessage<MidiCommandMessage>(msg, track, MidiMessageType::EndOfTrackMetaEvent, MidiCommands::EndOfTrack);
				else if (juceMsg.isKeySignatureMetaEvent())
				{
					AddMessage<MidiKeySignatureMessage>(msg, track,
						MidiMessageType::KeySignatureMetaEvent,
						juceMsg.isKeySignatureMajorKey(),
						std::max(0, (int)(int8_t)juceMsg.getKeySignatureNumberOfSharpsOrFlats()),
						-std::min((int)(int8_t)juceMsg.getKeySignatureNumberOfSharpsOrFlats(), 0));
				}
				else if (juceMsg.isTempoMetaEvent())
					AddMessage<MidiTempoMessage>(msg, track, MidiMessageType::TempoMetaEvent, juceMsg.getTempoSecondsPerQuarterNote());
				else if (juceMsg.isTrackNameEvent())
					AddMessage<MidiUnhandledMessage>(msg, track, VALUE_AND_STRING(MidiMessageType::TrackNameEvent)); 
				else if (juceMsg.isTextMetaEvent())
					AddMessage<MidiUnhandledMessage>(msg, track, VALUE_AND_STRING(MidiMessageType::TextMetaEvent));
				else if (juceMsg.isTimeSignatureMetaEvent())
				{
					int num = 1, denom = 1;
					juceMsg.getTimeSignatureInfo(num, denom);
					AddMessage<MidiTimeSignatureMessage>(msg, track, MidiMessageType::TimeSignatureMetaEvent, num, denom);
				}
				else if (juceMsg.isTrackMetaEvent())
					AddMessage<MidiUnhandledMessage>(msg, track, VALUE_AND_STRING(MidiMessageType::TrackMetaEvent));
				else if (juceMsg.isMetaEvent())
					AddMessage<MidiUnhandledMessage>(msg, track, VALUE_AND_STRING(MidiMessageType::MetaEvent));

#undef VALUE_AND_STRING

				auto timestamp = juceTrack->getEventTime(iEvent);
				timestamp /= format;
				msg->SetTimestamp((float)timestamp);

				assert(msg != nullptr);
			}
		}

		return result;
	}
}
#endif