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
	class iSynthApp : public Engine2D::Application, Engine2D::MidiVisitor
	{
	private:
		std::shared_ptr<Engine2D::Surface> background;
		std::shared_ptr<Piano> piano;
		double midiStartTime = 0, lastMidiTime = 0;
		std::shared_ptr<Engine2D::MidiFile> playingMidi = nullptr;

		static Engine2D::AppSettings GetAppSettings();

		void HandleMidiPlayback();
		virtual void OnNote(Engine2D::MidiNoteMessage msg) override;

	protected:

		void OnShutdown() override;
		void OnRender() override;
		void OnStartup() override;
		void OnFillAudioBuffer(uint64_t audioTime, std::vector<int16_t>& pcmData) override;

		void OnTouchStart(Engine2D::Touch touch) override;
	public:

		static std::shared_ptr<Engine2D::MidiFile> LoadMidi(std::string fileName);
		void PlayTone(Engine2D::MidiNotes note, int octave, float strength = 0.5);
		void PlayMidi(std::shared_ptr<Engine2D::MidiFile> midi);

		iSynthApp();
		~iSynthApp();
	};
}