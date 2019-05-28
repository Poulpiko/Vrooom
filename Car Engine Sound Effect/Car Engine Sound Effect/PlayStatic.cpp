/*
 * Copyright (c) 2006, Creative Labs Inc.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided
 * that the following conditions are met:
 * 
 *     * Redistributions of source code must retain the above copyright notice, this list of conditions and
 * 	     the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright notice, this list of conditions
 * 	     and the following disclaimer in the documentation and/or other materials provided with the distribution.
 *     * Neither the name of Creative Labs Inc. nor the names of its contributors may be used to endorse or
 * 	     promote products derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include	"Framework.h"

#define		K			3
#define		RPM_MAX		3000

int PlayStatic()
{
	ALint       iState;

	ALuint buffer[K];
	ALuint source[K];


	char* file[K];

	file[0] = "200Hz.wav";
	file[1] = "400Hz.wav";
	file[2] = "600Hz.wav";
	
	// Initialize Framework
	ALFWInit();

	ALFWprintf("PlayStatic Test Application\n");

	if (!ALFWInitOpenAL())
	{
		ALFWprintf("Failed to initialize OpenAL\n");
		ALFWShutdown();
		return 0;
	}

	
	for (int i(0); i < K; ++i) {			// Pour chaque fichier son
		// Generate an AL Buffer
		alGenBuffers(i+1, &buffer[i]);
		// Load Wave file into OpenAL Buffer
		if (!ALFWLoadWaveToBuffer(file[i], buffer[i]))
			ALFWprintf("Failed to load %s\n", ALFWaddMediaPath(file[i]));
		// Generate a Source to playback the Buffer
		alGenSources(i+1, &source[i]);
		// Attach Source to Buffer
		alSourcei(source[i], AL_BUFFER, buffer[i]);
	}

	// Play Source
	ALFWprintf("Playing Source ");
	for (int i(0); i < K; ++i) {
		alSourcePlay(source[i]);
		alSourcei(source[i], AL_LOOPING, 1);
	}

	ALfloat pitch = 1.f;
	ALfloat gain = 1.f;
	int rpm = 0;
	char* title = "";
		
	do
	{
		Sleep(100);

		// Get Source State

		//////////////////////////////////////////////////////////////////////
		// Changer Pitch

		rpm = getRPM();
		pitch = 0.7f + (ALfloat)rpm * 0.6f / (ALfloat)RPM_MAX;

		for (int i(0); i < K; ++i) {
			alSourcef(source[i], AL_PITCH, pitch);
			alSourcef(source[i], AL_GAIN, 0);
		}

		if (K >= 1 && rpm < RPM_MAX/K) // De 0 à 1000 
		{
			for (int i(0); i < K; ++i) {
				alSourcef(source[i], AL_GAIN, 0);
			}
			alSourcef(source[0], AL_GAIN, 1);

			GainMixage(source[0], source[1], rpm, 500 / 19, 500);	//RPM_MAX / K - (RPM_MAX / K - RPM_MAX / 2*K)) / 19, RPM_MAX / K - RPM_MAX / 2 * K);
			title = file[0];
		}

		else if (K >= 2 && rpm < 2 * RPM_MAX/K) // De 1000 à 2000
		{
			for (int i(0); i < K; ++i) {
				alSourcef(source[i], AL_GAIN, 0);
			}
			alSourcef(source[1], AL_GAIN, 1);

			GainMixage(source[1], source[2], rpm, 500 / 19, 1500);	// (2 * RPM_MAX / K - (2 * RPM_MAX / K - RPM_MAX / 2 * K)) / 19, 2 * RPM_MAX / K - RPM_MAX / 2 * K);
			title = file[1];
		}
		/*
		else if (K >= 3 && rpm < 3 * RPM_MAX / K) // De 2000 à 3000
		{
			for (int i(0); i < K; ++i) {
				alSourcef(source[i], AL_GAIN, 0);
			}
			alSourcef(source[2], AL_GAIN, 1);

			GainMixage(source[2], source[3], rpm, 500 / 19, 2500);	//(3*RPM_MAX / K - (3*RPM_MAX / K - RPM_MAX / 2 * K)) / 19, 3* RPM_MAX / K - RPM_MAX / 2 * K);
			title = file[2];
		}
		
		else if (K >= 4 && rpm < 4 * RPM_MAX / K) // De 3000 à 4000
		{
			for (int i(0); i < K; ++i) {
				alSourcef(source[i], AL_GAIN, 0);
			}
			alSourcef(source[3], AL_GAIN, 1);

			GainMixage(source[3], source[4], rpm, 500 / 19, 3500);	//(4*RPM_MAX / K - (4*RPM_MAX / K - RPM_MAX / 2 * K)) / 19, 4* RPM_MAX / K - RPM_MAX / 2 * K);
			title = file[3];
		}*/

		else // De 4000 à 5000
		{
			for (int i(0); i < K; ++i) {
				alSourcef(source[i], AL_GAIN, 0);
			}
			alSourcef(source[2], AL_GAIN, 1);

			title = file[2];
		}
	
		if (rpm && pitch && title)
			ALFWprintf("\nRPM: %d  \tpitch: %f \tsource: %s", rpm, pitch, title);

		//////////////////////////////////////////////////////////////////////

		alGetSourcei(source[0], AL_SOURCE_STATE, &iState);
	} while (iState == AL_PLAYING);

	ALFWprintf("\n");

	// Clean up by deleting Source(s) and Buffer(s)
	for (int i(0); i < K; ++i) {
		alSourceStop(source[i]);
		alDeleteSources(i+1, &source[i]);
		alDeleteBuffers(i+1, &buffer[i]);
	}

	ALFWShutdownOpenAL();
	ALFWShutdown();
	setRPM(0);

	return 0;
}

void GainMixage(ALuint source_a, ALuint source_b, int rpm, int marge, int base)
{
	if (rpm > base && rpm <= base + marge) {
		alSourcef(source_a, AL_GAIN, 0.95f);
		alSourcef(source_b, AL_GAIN, 0.05f);
	}
	if (rpm > base + marge && rpm <= base + marge * 2) {
		alSourcef(source_a, AL_GAIN, 0.9f);
		alSourcef(source_b, AL_GAIN, 0.1f);
	}
	if (rpm > base + marge * 2 && rpm <= base + marge * 3) {
		alSourcef(source_a, AL_GAIN, 0.85f);
		alSourcef(source_b, AL_GAIN, 0.15f);
	}
	if (rpm > base + marge * 3 && rpm <= base + marge * 4) {
		alSourcef(source_a, AL_GAIN, 0.8f);
		alSourcef(source_b, AL_GAIN, 0.2f);
	}
	if (rpm > base + marge * 4 && rpm <= base + marge * 5) {
		alSourcef(source_a, AL_GAIN, 0.75f);
		alSourcef(source_b, AL_GAIN, 0.25f);
	}
	if (rpm > base + marge * 5 && rpm <= base + marge * 6) {
		alSourcef(source_a, AL_GAIN, 0.7f);
		alSourcef(source_b, AL_GAIN, 0.3f);
	}
	if (rpm > base + marge * 6 && rpm <= base + marge * 7) {
		alSourcef(source_a, AL_GAIN, 0.65f);
		alSourcef(source_b, AL_GAIN, 0.35f);
	}
	if (rpm > base + marge * 7 && rpm <= base + marge * 8) {
		alSourcef(source_a, AL_GAIN, 0.6f);
		alSourcef(source_b, AL_GAIN, 0.4f);
	}
	if (rpm > base + marge * 8 && rpm <= base + marge * 9) {
		alSourcef(source_a, AL_GAIN, 0.55f);
		alSourcef(source_b, AL_GAIN, 0.45f);
	}
	if (rpm > base + marge * 9 && rpm <= base + marge * 10) {
		alSourcef(source_a, AL_GAIN, 0.5f);
		alSourcef(source_b, AL_GAIN, 0.5f);
	}
	if (rpm > base + marge * 10 && rpm <= base + marge * 11) {
		alSourcef(source_a, AL_GAIN, 0.45f);
		alSourcef(source_b, AL_GAIN, 0.55f);
	}
	if (rpm > base + marge * 11 && rpm <= base + marge * 12) {
		alSourcef(source_a, AL_GAIN, 0.4f);
		alSourcef(source_b, AL_GAIN, 0.6f);
	}
	if (rpm > base + marge * 12 && rpm <= base + marge * 13) {
		alSourcef(source_a, AL_GAIN, 0.35f);
		alSourcef(source_b, AL_GAIN, 0.65f);
	}
	if (rpm > base + marge * 13 && rpm <= base + marge * 14) {
		alSourcef(source_a, AL_GAIN, 0.3f);
		alSourcef(source_b, AL_GAIN, 0.7f);
	}
	if (rpm > base + marge * 14 && rpm <= base + marge * 15) {
		alSourcef(source_a, AL_GAIN, 0.25f);
		alSourcef(source_b, AL_GAIN, 0.75f);
	}
	if (rpm > base + marge * 15 && rpm <= base + marge * 16) {
		alSourcef(source_a, AL_GAIN, 0.2f);
		alSourcef(source_b, AL_GAIN, 0.8f);
	}
	if (rpm > base + marge * 16 && rpm <= base + marge * 17) {
		alSourcef(source_a, AL_GAIN, 0.15f);
		alSourcef(source_b, AL_GAIN, 0.85f);
	}
	if (rpm > base + marge * 17 && rpm <= base + marge * 18) {
		alSourcef(source_a, AL_GAIN, 0.1f);
		alSourcef(source_b, AL_GAIN, 0.9f);
	}
	if (rpm > base + marge * 18) {
		alSourcef(source_a, AL_GAIN, 0.05f);
		alSourcef(source_b, AL_GAIN, 0.95f);
	}
}