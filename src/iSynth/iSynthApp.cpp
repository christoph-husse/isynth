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

using namespace Framework;
using namespace Engine2D;

namespace iSynth
{
	Engine2D::AppSettings iSynthApp::GetAppSettings()
	{
		Engine2D::AppSettings settings;
		settings.windowTitle = "iSynth";
		return settings;
	}

	iSynthApp::iSynthApp() : Engine2D::Application(GetAppSettings())
	{
		// Here we are still in a transient state ("main()" has not even been called). 
		// Don't do any advanced logic here but rather defer it to OnStartup()!
	}

	iSynthApp::~iSynthApp()
	{
	}

	void iSynthApp::OnTouchStart(Touch touch)
	{
		static int i = 0;

		PlayTone(MidiNotes::C, 1 + (i++ % 6), 1);
	}

	void iSynthApp::OnRender()
	{
		SetLogicalViewportToFill((float)background->GetWidth(), (float)background->GetHeight());
		
		DrawTexture(background, { 0, 0, background->GetWidth(), background->GetHeight() });

		piano->Cleanup();

#if defined(DEBUG) && !defined(IS_EMSCRIPTEN)
		HandleMidiPlayback();
#endif
	} 

	void iSynthApp::OnShutdown()
	{
		piano = nullptr;
	}

	std::shared_ptr<MidiFile> iSynthApp::LoadMidi(std::string fileName)
	{
		std::ifstream file;
		file.open(fileName, std::ios::binary);
		return MidiFile::Load(file);
	}

	void iSynthApp::OnStartup()
	{
		const std::string defaultSong = "Assets/iSynth/Songs/Beethoven - Sonate No. 17 Tempest 3rd Movement.jsmidi";

		background = std::make_shared<Engine2D::Surface>("Assets/iSynth/GUI/Background.jpg");

		piano = std::make_shared<Piano>();
		
		// handle command line arguments
#ifndef IS_EMSCRIPTEN
		auto args = GetEnvironmentArgs();
		if (args.size() == 3)
		{
			// convert user defined midi file to jsmidi
			auto midi = MidiParser::ParseMidiFile(*FileUtils::ReadAllBytes(args[1]));

			std::ofstream ofile;
			ofile.open(args[2].c_str(), std::ios::binary | std::ios::trunc);
			midi->Save(ofile);
			ofile.flush();
			ofile.close();
		}
		else if (args.size() < 3)
		{
			// playback user defined midi file
			auto midiPath = (args.size() == 2) ? args[1] : defaultSong;
			std::shared_ptr<MidiFile> midi;

			if (endsWith(midiPath, ".mid"))
			{
				// convert midi file to jsmidi
				midi = MidiParser::ParseMidiFile(*FileUtils::ReadAllBytes(midiPath));
			}
			else
			{
				// load jsmidi
				midi = LoadMidi(midiPath);
			}

			PlayMidi(midi);
		}

#else
		// play default song
		PlayMidi(LoadMidi(defaultSong));
#endif
	}

	void iSynthApp::PlayMidi(std::shared_ptr<Engine2D::MidiFile> midi)
	{
		playingMidi = midi;


		// prefetch all notes used in this file
		for (auto& track : *playingMidi)
		{
			for (auto& msg : *track)
			{
				if (msg->GetClass() != MidiMessageClass::MidiNoteMessage)
					continue;

				MidiNoteMessage* noteMsg = (MidiNoteMessage*)msg.get();
				piano->Prefetch(noteMsg->GetNote(), noteMsg->GetOctave() - 1);
			}
		}

		midiStartTime = Application::GetTicks() / 1000.0 + 1;
	}

	void iSynthApp::HandleMidiPlayback()
	{
		// is called every rendered frame
		if (playingMidi == nullptr)
			return;

		// gather due notes that were not already scheduled 
		double currentTime = (Application::GetTicks() / 1000.0) * 2.f;
		double progress = currentTime - midiStartTime;

		for (auto& track : *playingMidi)
		{
			for (auto& msg : *track)
			{
				if ((msg->GetTimestamp() <= progress) && (msg->GetTimestamp() > lastMidiTime))
					msg->Visit(*this);
			}
		}

		lastMidiTime = progress;
	}

	void iSynthApp::OnNote(MidiNoteMessage msg)
	{
		if (!msg.IsNoteOn())
			return;

		PlayTone(msg.GetNote(), msg.GetOctave()-1, msg.GetFloatVelocity());
	}

	void iSynthApp::PlayTone(MidiNotes note, int octave, float strength)
	{
		piano->PlayTone(note, octave, strength);
	}

	void iSynthApp::OnFillAudioBuffer(uint64_t audioTime, std::vector<int16_t>& pcmData)
	{
#if defined(IS_EMSCRIPTEN) || !defined(DEBUG)
		HandleMidiPlayback();
#endif

		// ATTENTION: This is called by a dedicated audio thread!
		// Engine2D::Application guarantees that this method won't be called anywmore when
		// iSynthApp::OnShutdown() is invoked.

		piano->NextSample(audioTime, pcmData); // must be thread-safe with respect to application logic
	}
}
