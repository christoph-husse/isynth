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
#include "Engine2D.hpp"

namespace Engine2D
{
	void MidiUnhandledMessage::Visit(MidiVisitor& visitor) { visitor.OnUnhandled(*this); }
	void MidiCommandMessage::Visit(MidiVisitor& visitor) { visitor.OnCommand(*this); }
	void MidiNoteMessage::Visit(MidiVisitor& visitor) { visitor.OnNote(*this); }
	void MidiKeySignatureMessage::Visit(MidiVisitor& visitor) { visitor.OnChangeKeySignature(*this); }
	void MidiTempoMessage::Visit(MidiVisitor& visitor) { visitor.OnChangeTempo(*this); }
	void MidiTimeSignatureMessage::Visit(MidiVisitor& visitor) { visitor.OnChangeTimeSignature(*this); }

	std::shared_ptr<MidiMessage> MidiMessage::Load(std::istream& stream)
	{
		BinaryReader reader{ stream };

		if (reader.ReadByte() != 197)
			THROW InvalidDataException();

		auto cls = (MidiMessageClass)reader.ReadByte();
		auto type = (MidiMessageType)reader.ReadByte();
		auto timestamp = reader.ReadFloat();
		std::shared_ptr<MidiMessage> result = nullptr;

		switch (cls)
		{
		case MidiMessageClass::MidiUnhandledMessage: result = std::make_shared<MidiUnhandledMessage>(type, stream); break;
		case MidiMessageClass::MidiCommandMessage: result = std::make_shared<MidiCommandMessage>(type, stream); break;
		case MidiMessageClass::MidiKeySignatureMessage: result = std::make_shared<MidiKeySignatureMessage>(type, stream); break;
		case MidiMessageClass::MidiNoteMessage: result = std::make_shared<MidiNoteMessage>(type, stream); break;
		case MidiMessageClass::MidiTempoMessage: result = std::make_shared<MidiTempoMessage>(type, stream); break;
		case MidiMessageClass::MidiTimeSignatureMessage: result = std::make_shared<MidiTimeSignatureMessage>(type, stream); break;
		default:
			THROW InvalidDataException();
		}

		result->SetTimestamp(timestamp);
		return result;
	}
}