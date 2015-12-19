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
#include "iSynth.hpp"

using namespace Engine2D;
using namespace Framework;

namespace iSynth
{
	PianoTone::PianoTone(MidiNotes note, int octave) : note(note), octave(octave)
	{
		fast = strconcat("Assets/iSynth/Tones/Piano/Piano.ff.", NoteToString(note), (int)octave, ".ogg");
		mid = strconcat("Assets/iSynth/Tones/Piano/Piano.mf.", NoteToString(note), (int)octave, ".ogg");
		low = strconcat("Assets/iSynth/Tones/Piano/Piano.pp.", NoteToString(note), (int)octave, ".ogg");
	}


	std::string PianoTone::NoteToString(MidiNotes tone)
	{
		std::string toneStr;

		switch (tone)
		{
		case MidiNotes::A: toneStr = "A"; break;
		case MidiNotes::Ab: toneStr = "Ab"; break;
		case MidiNotes::B: toneStr = "B"; break;
		case MidiNotes::Bb: toneStr = "Bb"; break;
		case MidiNotes::C: toneStr = "C"; break;
		case MidiNotes::Db: toneStr = "Db"; break;
		case MidiNotes::D: toneStr = "D"; break;
		case MidiNotes::E: toneStr = "E"; break;
		case MidiNotes::Eb: toneStr = "Eb"; break;
		case MidiNotes::F: toneStr = "F"; break;
		case MidiNotes::G: toneStr = "G"; break;
		case MidiNotes::Gb: toneStr = "Gb"; break;
		default:
			THROW ApplicationException("Unknown enumeration value.");
		}

		return toneStr;
	}

	Piano::Piano()
	{
		for (int i = 1; i <= 7; i++)
		{
			auto octave = i;

			tones.push_back(std::make_shared<PianoTone>(MidiNotes::A, octave));
			tones.push_back(std::make_shared<PianoTone>(MidiNotes::Ab, octave));
			tones.push_back(std::make_shared<PianoTone>(MidiNotes::B, octave));
			tones.push_back(std::make_shared<PianoTone>(MidiNotes::Bb, octave));
			tones.push_back(std::make_shared<PianoTone>(MidiNotes::C, octave));
			tones.push_back(std::make_shared<PianoTone>(MidiNotes::D, octave));
			tones.push_back(std::make_shared<PianoTone>(MidiNotes::Db, octave));
			tones.push_back(std::make_shared<PianoTone>(MidiNotes::E, octave));
			tones.push_back(std::make_shared<PianoTone>(MidiNotes::Eb, octave));
			tones.push_back(std::make_shared<PianoTone>(MidiNotes::F, octave));
			tones.push_back(std::make_shared<PianoTone>(MidiNotes::G, octave));
			tones.push_back(std::make_shared<PianoTone>(MidiNotes::Gb, octave));
		}

		tones.push_back(std::make_shared<PianoTone>(MidiNotes::C, 8));
	}

	std::shared_ptr<PianoTone> Piano::GetTone(MidiNotes note, int octave)
	{
		for (auto& tone : tones)
		{
			if ((tone->GetNote() == note) && (tone->GetOctave() == octave))
				return tone;
		}

		THROW ArgumentException("No suitable tone could be found.");
	}


	void Piano::Prefetch(MidiNotes tone, int octave)
	{
		std::make_shared<PianoToneSignal>(GetTone(tone, octave), 0.0f);
		std::make_shared<PianoToneSignal>(GetTone(tone, octave), 1.0f);
	}

	void Piano::Cleanup()
	{
		for (auto& e : enqueuedTones)
		{
			if (e == nullptr)
				continue;

			if (e->HasFinished())
				e = nullptr;
		}

	}

	void Piano::PlayTone(MidiNotes tone, int octave, float strength)
	{
		Cleanup();

		auto signal = std::make_shared<PianoToneSignal>(GetTone(tone, octave), strength);
		for (auto& e : enqueuedTones)
		{
			if (e == nullptr)
			{
				e = signal;
				break;
			}
		}
	}

	void Piano::NextSample(uint64_t c2i44100, std::vector<int16_t>& pcmBuffer)
	{
		static std::vector<int32_t> accu;
		static int min = 0, max = 0;

		accu.resize(pcmBuffer.size());

		std::fill_n(accu.begin(), accu.size(), 0);

		for (std::shared_ptr<PianoToneSignal> tone : enqueuedTones)
		{
			if (tone == nullptr)
				continue;

			tone->NextSample(c2i44100, pcmBuffer);

			for (int i = 0, n = (int)pcmBuffer.size(); i < n; i++)
				accu[i] += pcmBuffer[i];
		}

		for (int i = 0, n = (int)pcmBuffer.size(); i < n; i++)
		{
			min = std::min(min, accu[i]);
			max = std::max(max, accu[i]);

			pcmBuffer[i] = (int16_t)std::max(
				(int32_t)std::numeric_limits<int16_t>::min(),
				std::min(accu[i] / 2, (int32_t)std::numeric_limits<int16_t>::max()));
		}
	}

	PianoToneSignal::PianoToneSignal(std::shared_ptr<PianoTone> tone, float strength) : 
		low(OggVorbisCache::LoadFile(tone->low)),
		mid(OggVorbisCache::LoadFile(tone->mid)),
		fast(OggVorbisCache::LoadFile(tone->fast))
	{
		strength = std::max(0.0f, std::min(1.0f, strength));

		if (strength > 0.66f)
		{
			// mid high
			useFast = true;
			useMid = true;
			useLow = false;

			strength = (strength - 0.33f) * (1.0f/0.66f);
			lowWeight = 0;
			midWeight = 1 - strength;
			fastWeight = strength;
		}
		else
		{
			// low mid
			useFast = false;
			useMid = true;
			useLow = true;

			strength = strength * (1.0f / 0.66f);
			fastWeight = 0;
			lowWeight = 1 - strength;
			midWeight = strength;
		}
	}

	void PianoToneSignal::NextSample(uint64_t c2i44100, std::vector<int16_t>& pcmBuffer)
	{
		static std::vector<int16_t> accu;
		accu.resize(pcmBuffer.size());

		if (useFast)
		{
			fast.NextSample(c2i44100, accu);
			mid.NextSample(c2i44100, pcmBuffer);

			for (size_t i = 0; i < pcmBuffer.size(); i++)
				pcmBuffer[i] = (int16_t)(accu[i] * fastWeight + pcmBuffer[i] * midWeight);
		}
		else
		{
			mid.NextSample(c2i44100, accu);
			low.NextSample(c2i44100, pcmBuffer);

			for (size_t i = 0; i < pcmBuffer.size(); i++)
				pcmBuffer[i] = (int16_t)(accu[i] * midWeight + pcmBuffer[i] * lowWeight);
		}
	}

	bool PianoToneSignal::HasFinished() const
	{
		return (!useFast || fast.HasFinished()) && (!useMid || mid.HasFinished()) && (!useLow || low.HasFinished());
	}
}