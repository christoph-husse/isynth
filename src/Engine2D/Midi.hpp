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

#pragma once

#include "Engine2D.hpp"

namespace Engine2D
{

	class MidiVisitor;

	enum class MidiMessageType
	{
		SysEx,
		NoteOn,
		NoteOff,
		NoteOnOrOff,
		SustainPedalOn,
		SustainPedalOff,
		SostenutoPedalOn,
		SostenutoPedalOff,
		SoftPedalOn,
		SoftPedalOff,
		ProgramChange,
		PitchWheel,
		Aftertouch,
		ChannelPressure,
		Controller,
		AllNotesOff,
		AllSoundOff,
		MetaEvent,
		TrackMetaEvent,
		EndOfTrackMetaEvent,
		TrackNameEvent,
		TextMetaEvent,
		TempoMetaEvent,
		TimeSignatureMetaEvent,
		KeySignatureMetaEvent,
		MidiChannelMetaEvent,
		ActiveSense,
		MidiStart,
		MidiContinue,
		MidiStop,
		MidiClock,
		SongPositionPointer,
		QuarterFrame,
		FullFrame,
		MidiMachineControlMessage,
	};

	enum class MidiMessageClass
	{
		MidiMessage,
		MidiUnhandledMessage,
		MidiCommandMessage,
		MidiTempoMessage,
		MidiTimeSignatureMessage,
		MidiKeySignatureMessage,
		MidiNoteMessage,
	};

	class MidiMessage
	{
	private:
		MidiMessageType type;
		MidiMessageClass cppClass = MidiMessageClass::MidiMessage;
		float timestamp = 0;

	protected:
		MidiMessage(MidiMessageType type, MidiMessageClass cppClass) : type(type), cppClass(cppClass)
		{

		}

	public:

		virtual void Visit(MidiVisitor& visitor) = 0;
		virtual ~MidiMessage() { }
		MidiMessageType GetType() const { return type; }
		MidiMessageClass GetClass() const { return cppClass; }

		float GetTimestamp() const { return timestamp; }
		void SetTimestamp(float seconds) { timestamp = seconds; }

		static std::shared_ptr<MidiMessage> Load(std::istream& stream);

		virtual void Save(std::ostream& stream)
		{
			BinaryWriter writer{ stream };

			writer.WriteByte((unsigned char)197);
			writer.WriteByte((unsigned char)cppClass);
			writer.WriteByte((unsigned char)type);
			writer.WriteFloat(timestamp);
		}
	};

	// currently not implemented but stored for the record...
	class MidiUnhandledMessage final : public MidiMessage
	{
	private:
		std::string typeStr;

	public:
		virtual void Visit(MidiVisitor& visitor) override;

		// convenience method to make it easier to output what exactly was unhandled...
		// It's just the name of the enum class member.
		std::string GetTypeString() const { return typeStr; }

		MidiUnhandledMessage(MidiMessageType type, std::string typeStr) : MidiMessage(type, MidiMessageClass::MidiUnhandledMessage), typeStr(typeStr)
		{

		}

		MidiUnhandledMessage(MidiMessageType type, std::istream& stream) : MidiMessage(type, MidiMessageClass::MidiUnhandledMessage)
		{
			BinaryReader reader{ stream };
			typeStr = reader.ReadString();
		}

		virtual void Save(std::ostream& stream) override
		{
			BinaryWriter writer{ stream };

			MidiMessage::Save(stream);
			writer.WriteString(typeStr);
		}
	};

	enum class MidiCommands
	{
		MidiStart,
		MidiContinue,
		MidiStop,
		SustainPedalOn,
		SustainPedalOff,
		SostenutoPedalOn,
		SostenutoPedalOff,
		SoftPedalOn,
		SoftPedalOff,
		AllNotesOff,
		AllSoundOff,
		EndOfTrack,
	};

	class MidiCommandMessage final : public MidiMessage
	{
	private:
		MidiCommands command;

	public:

		MidiCommands GetCommand() const { return command; }
		virtual void Visit(MidiVisitor& visitor) override;

		MidiCommandMessage(MidiMessageType type, MidiCommands command) : MidiMessage(type, MidiMessageClass::MidiCommandMessage), command(command) { }

		MidiCommandMessage(MidiMessageType type, std::istream& stream) : MidiMessage(type, MidiMessageClass::MidiCommandMessage)
		{
			BinaryReader reader{ stream };
			command = (MidiCommands)reader.ReadByte();
		}

		virtual void Save(std::ostream& stream) override
		{
			BinaryWriter writer{ stream };

			MidiMessage::Save(stream);
			writer.WriteByte((unsigned char)command);
		}
	};

	class MidiTempoMessage final : public MidiMessage
	{
	private:
		double tempo;

	public:

		double GetSecondsPerQuarterNote() const { return tempo; }
		virtual void Visit(MidiVisitor& visitor) override;

		MidiTempoMessage(MidiMessageType type, double tempo) : MidiMessage(type, MidiMessageClass::MidiTempoMessage), tempo(tempo) { }

		MidiTempoMessage(MidiMessageType type, std::istream& stream) : MidiMessage(type, MidiMessageClass::MidiTempoMessage)
		{
			BinaryReader reader{ stream };
			tempo = reader.ReadDouble();
		}

		virtual void Save(std::ostream& stream) override
		{
			BinaryWriter writer{ stream };

			MidiMessage::Save(stream);
			writer.WriteDouble(tempo);
		}
	};

	class MidiTimeSignatureMessage final : public MidiMessage
	{
	private:
		int num, denom;

	public:

		int GetNumerator() const { return num; }
		int GetDenominator() const { return denom; }
		virtual void Visit(MidiVisitor& visitor) override;

		MidiTimeSignatureMessage(MidiMessageType type, int numerator, int denominator) : MidiMessage(type, MidiMessageClass::MidiTimeSignatureMessage), num(numerator), denom(denominator) {}

		MidiTimeSignatureMessage(MidiMessageType type, std::istream& stream) : MidiMessage(type, MidiMessageClass::MidiTimeSignatureMessage)
		{
			BinaryReader reader{ stream };
			num = reader.ReadByte();
			denom = reader.ReadByte();
		}

		virtual void Save(std::ostream& stream) override
		{
			BinaryWriter writer{ stream };

			MidiMessage::Save(stream);
			writer.WriteByte((unsigned char)num);
			writer.WriteByte((unsigned char)denom);
		}
	};

	class MidiKeySignatureMessage final : public MidiMessage
	{
	private:
		int sharps = 0;
		int flats = 0;
		bool isMajor = false;
	public:

		int GetSharps() const { return sharps; }
		int GetFlats() const { return flats; }
		bool IsMajor() const { return isMajor; }
		bool IsMinor() const { return !isMajor; }
		virtual void Visit(MidiVisitor& visitor) override;

		MidiKeySignatureMessage(MidiMessageType type, bool isMajor, int sharps, int flats)
			: MidiMessage(type, MidiMessageClass::MidiKeySignatureMessage), sharps(sharps), flats(flats), isMajor(isMajor)
		{

		}

		MidiKeySignatureMessage(MidiMessageType type, std::istream& stream) : MidiMessage(type, MidiMessageClass::MidiKeySignatureMessage)
		{
			BinaryReader reader{ stream };
			sharps = reader.ReadByte();
			flats = reader.ReadByte();
			isMajor = reader.ReadBoolean();
		}

		virtual void Save(std::ostream& stream) override
		{
			BinaryWriter writer{ stream };

			MidiMessage::Save(stream);
			writer.WriteByte((unsigned char)sharps);
			writer.WriteByte((unsigned char)flats);
			writer.WriteBoolean(isMajor);
		}
	};

	enum class MidiNotes
	{
		C,
		Db,
		D,
		Eb,
		E,
		F,
		Gb,
		G,
		Ab,
		A,
		Bb,
		B,
	};

	class MidiNoteMessage final : public MidiMessage
	{
	private:
		bool isNoteOn;
		int noteNumber;
		float velocity;
		std::string noteName;
		MidiNotes note;
		int octave;
		bool isFlat, isSharp;
	public:

		bool IsNoteOn() const { return isNoteOn; }
		bool IsNoteOff() const { return !isNoteOn; }
		bool IsFlat() const { return isFlat; }
		bool IsSharp() const { return isSharp; }
		int GetNoteNumber() const { return noteNumber; }
		MidiNotes GetNote() const { return note; }
		int GetOctave() const { return octave; }
		float GetFloatVelocity() const { return velocity; }
		std::string GetNoteName() const { return noteName; }
		virtual void Visit(MidiVisitor& visitor) override;

		MidiNoteMessage(MidiMessageType type, bool isNoteOn, int noteNumber, float velocity, std::string noteName)
			: MidiMessage(type, MidiMessageClass::MidiNoteMessage), isNoteOn(isNoteOn), noteNumber(noteNumber), velocity(velocity), noteName(noteName)
		{
			isFlat = std::find(noteName.begin(), noteName.end(), 'b') != noteName.end();
			isSharp = std::find(noteName.begin(), noteName.end(), '#') != noteName.end();
			
			switch (noteName[0])
			{
			case 'A': note = MidiNotes::A; break;
			case 'B': note = MidiNotes::B; break;
			case 'C': note = MidiNotes::C; break;
			case 'D': note = MidiNotes::D; break;
			case 'E': note = MidiNotes::E; break;
			case 'F': note = MidiNotes::F; break;
			case 'G': note = MidiNotes::G; break;
			default:
				THROW Framework::ApplicationException("Unknown note.");
			}

			octave = noteName[noteName.size() - 1] - '0';
			if ((octave <= 0) || (octave > 8))
				THROW Framework::ApplicationException("Invalid octave.");

			if (isFlat)
				note = (MidiNotes)((int)note - 1);

			if (isSharp)
			{
				int raised = (int)note + 1;
				if (raised > (int)MidiNotes::B)
				{
					octave++;
					note = MidiNotes::C;
				}
				else
					note = (MidiNotes)raised;
			}

			if ((octave == 8) && (note != MidiNotes::C))
				THROW Framework::ApplicationException("Octave 8 has only C as valid note.");
		}

		MidiNoteMessage(MidiMessageType type, std::istream& stream) : MidiMessage(type, MidiMessageClass::MidiNoteMessage)
		{
			BinaryReader reader{ stream };
			noteNumber = reader.ReadInt32();
			velocity = reader.ReadFloat();
			isNoteOn = reader.ReadBoolean();
			noteName = reader.ReadString();
			isFlat = reader.ReadBoolean();
			isSharp = reader.ReadBoolean();
			octave = reader.ReadByte();
			note = (MidiNotes)reader.ReadByte();
		}

		virtual void Save(std::ostream& stream) override
		{
			BinaryWriter writer{ stream };

			MidiMessage::Save(stream);
			writer.WriteInt32((int32_t)noteNumber);
			writer.WriteFloat(velocity);
			writer.WriteBoolean(isNoteOn);
			writer.WriteString(noteName);
			writer.WriteBoolean(isFlat);
			writer.WriteBoolean(isSharp);
			writer.WriteByte((unsigned char)octave);
			writer.WriteByte((unsigned char)note);
		}
	};

	class MidiTrack
	{
		typedef std::vector<std::shared_ptr<MidiMessage>> message_container;

	private:
		message_container messages;

	public:

		message_container::iterator begin() { return messages.begin(); }
		message_container::iterator end() { return messages.end(); }

		template<class TMessage>
		std::shared_ptr<TMessage> AddMessage(const TMessage& message)
		{
			auto msg = std::make_shared<TMessage>(message);
			messages.push_back(msg);
			return msg;
		}

		void RemoveMessage(std::shared_ptr<MidiMessage> msg)
		{
			messages.erase(std::find(messages.begin(), messages.end(), msg));
		}

		void Visit(MidiVisitor& visitor)
		{
			for (auto& msg : messages)
				msg->Visit(visitor);
		}

		void Save(std::ostream& stream)
		{
			BinaryWriter writer{ stream };

			writer.WriteString("Engine2D::MidiTrack");
			writer.WriteUInt32((uint32_t)messages.size());

			for (int i = 0; i < (int)messages.size(); i++)
			{
				messages[i]->Save(stream);
			}
		}

		void Load(std::istream& stream)
		{
			BinaryReader reader{ stream };

			if (reader.ReadString() != "Engine2D::MidiTrack")
				THROW InvalidDataException();

			messages.clear();

			int msgNum = reader.ReadInt32();
			for (int i = 0; i < msgNum; i++)
			{
				messages.emplace_back(MidiMessage::Load(stream));
			}
		}
	};

	class MidiVisitor
	{
	private:
	public:
		virtual ~MidiVisitor() { }

		virtual void OnUnhandled(MidiUnhandledMessage msg) { }
		virtual void OnCommand(MidiCommandMessage msg) { }
		virtual void OnNote(MidiNoteMessage msg) { }
		virtual void OnChangeKeySignature(MidiKeySignatureMessage msg) { }
		virtual void OnChangeTempo(MidiTempoMessage msg) { }
		virtual void OnChangeTimeSignature(MidiTimeSignatureMessage msg) { }
	};


	class MidiFile : Framework::NonCopyable
	{
		typedef std::vector<std::shared_ptr<MidiTrack>> tracks_container;

	private:
		tracks_container tracks;

	public:

		tracks_container::iterator begin() { return tracks.begin(); }
		tracks_container::iterator end() { return tracks.end(); }

		std::shared_ptr<MidiTrack> AddTrack()
		{
			auto track = std::make_shared<MidiTrack>();
			tracks.push_back(track);
			return track;
		}

		void RemoveTrack(std::shared_ptr<MidiTrack> track)
		{
			tracks.erase(std::find(tracks.begin(), tracks.end(), track));
		}

		void Save(std::ostream& stream)
		{
			BinaryWriter writer{ stream };
			
			writer.WriteString("Engine2D::MidiFile");
			writer.WriteUInt16((uint16_t)tracks.size());

			for (int i = 0; i < (int)tracks.size(); i++)
			{
				tracks[i]->Save(stream);
			}
		}

		static std::shared_ptr<MidiFile> Load(std::istream& stream)
		{
			BinaryReader reader{ stream };
			auto result = std::make_shared<MidiFile>();

			if (reader.ReadString() != "Engine2D::MidiFile")
				THROW InvalidDataException();

			int trackNum = reader.ReadInt16();
			for (int i = 0; i < trackNum; i++)
			{
				auto track = result->AddTrack();
				track->Load(stream);
			}

			return result;
		}

	};

}