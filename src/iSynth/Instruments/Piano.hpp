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

#include "iSynth.hpp"

namespace iSynth
{
	class PianoTone : Framework::NonCopyable, Framework::NonMovable
	{
	private:
		friend class PianoToneSignal;

		std::string low, mid, fast;
		Engine2D::MidiNotes note;
		int octave;
	public:

		static std::string NoteToString(Engine2D::MidiNotes note);

		Engine2D::MidiNotes GetNote() const { return note; }
		int GetOctave() const { return octave; }
		PianoTone(Engine2D::MidiNotes note, int octave);
	};

	class PianoToneSignal final : public Signal
	{
	private:
		friend class Piano;

		OggVorbisSignal low, mid, fast;
		float lowWeight = 0, midWeight = 0, fastWeight = 0;
		bool useLow = false, useMid = false, useFast = false;

		virtual void NextSample(uint64_t c2i44100, std::vector<int16_t>& pcmBuffer) override;

	public:
		bool HasFinished() const;

		PianoToneSignal(std::shared_ptr<PianoTone> tone, float strength);
	};

	class Piano final : Framework::NonCopyable, Framework::NonMovable, public Signal
	{
	private:
		std::vector<std::shared_ptr<PianoTone>> tones;
		std::array<std::shared_ptr<PianoToneSignal>, 128> enqueuedTones;

	public:

		Piano();

		void Cleanup();
		void Prefetch(MidiNotes tone, int octave);
		void PlayTone(Engine2D::MidiNotes notes, int octave, float strength = 0.5);

		virtual void NextSample(uint64_t c2i44100, std::vector<int16_t>& pcmBuffer) override;
		std::shared_ptr<PianoTone> GetTone(Engine2D::MidiNotes notes, int octave);
	};
}