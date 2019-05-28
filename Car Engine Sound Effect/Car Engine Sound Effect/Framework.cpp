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

 // Win32 version of the Creative Labs OpenAL 1.1 Framework for samples

#define  _CRT_SECURE_NO_WARNINGS
#include<windows.h>
#include<stdio.h>

#include "Framework.h"
#include "CWaves.h"
#include "aldlist.h"
#include <thread>
#include <Python.h>
#include <array>

static CWaves *g_pWaveLoader = NULL;

// Imported EFX functions

// Effect objects
LPALGENEFFECTS alGenEffects = NULL;
LPALDELETEEFFECTS alDeleteEffects = NULL;
LPALISEFFECT alIsEffect = NULL;
LPALEFFECTI alEffecti = NULL;
LPALEFFECTIV alEffectiv = NULL;
LPALEFFECTF alEffectf = NULL;
LPALEFFECTFV alEffectfv = NULL;
LPALGETEFFECTI alGetEffecti = NULL;
LPALGETEFFECTIV alGetEffectiv = NULL;
LPALGETEFFECTF alGetEffectf = NULL;
LPALGETEFFECTFV alGetEffectfv = NULL;

//Filter objects
LPALGENFILTERS alGenFilters = NULL;
LPALDELETEFILTERS alDeleteFilters = NULL;
LPALISFILTER alIsFilter = NULL;
LPALFILTERI alFilteri = NULL;
LPALFILTERIV alFilteriv = NULL;
LPALFILTERF alFilterf = NULL;
LPALFILTERFV alFilterfv = NULL;
LPALGETFILTERI alGetFilteri = NULL;
LPALGETFILTERIV alGetFilteriv = NULL;
LPALGETFILTERF alGetFilterf = NULL;
LPALGETFILTERFV alGetFilterfv = NULL;

// Auxiliary slot object
LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots = NULL;
LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots = NULL;
LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot = NULL;
LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti = NULL;
LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv = NULL;
LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf = NULL;
LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv = NULL;
LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti = NULL;
LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv = NULL;
LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf = NULL;
LPALGETAUXILIARYEFFECTSLOTFV alGetAuxiliaryEffectSlotfv = NULL;

// XRAM functions and enum values

LPEAXSETBUFFERMODE eaxSetBufferMode = NULL;
LPEAXGETBUFFERMODE eaxGetBufferMode = NULL;

ALenum eXRAMSize = 0;
ALenum eXRAMFree = 0;
ALenum eXRAMAuto = 0;
ALenum eXRAMHardware = 0;
ALenum eXRAMAccessible = 0;

// Initialization and enumeration
void ALFWInit()
{
	g_pWaveLoader = new CWaves();
}

void ALFWShutdown()
{
	if (g_pWaveLoader)
	{
		delete g_pWaveLoader;
		g_pWaveLoader = NULL;
	}

	ALFWprintf("\nPress a key to quit\n");
	ALchar ch = _getch();
}

ALboolean ALFWInitOpenAL()
{
	ALDeviceList *pDeviceList = NULL;
	ALCcontext *pContext = NULL;
	ALCdevice *pDevice = NULL;
	ALint i;
	ALboolean bReturn = AL_FALSE;

	pDeviceList = new ALDeviceList();
	if ((pDeviceList) && (pDeviceList->GetNumDevices()))
	{
		ALFWprintf("\nSelect OpenAL Device:\n");
		for (i = 0; i < pDeviceList->GetNumDevices(); i++)
			ALFWprintf("%d. %s%s\n", i + 1, pDeviceList->GetDeviceName(i), i == pDeviceList->GetDefaultDevice() ? "(DEFAULT)" : "");

		do {
			ALchar ch = _getch();
			i = atoi(&ch);
		} while ((i < 1) || (i > pDeviceList->GetNumDevices()));

		pDevice = alcOpenDevice(pDeviceList->GetDeviceName(i - 1));
		if (pDevice)
		{
			pContext = alcCreateContext(pDevice, NULL);
			if (pContext)
			{
				ALFWprintf("\nOpened %s Device\n", alcGetString(pDevice, ALC_DEVICE_SPECIFIER));
				alcMakeContextCurrent(pContext);
				bReturn = AL_TRUE;
			}
			else
			{
				alcCloseDevice(pDevice);
			}
		}

		delete pDeviceList;
	}

	return bReturn;
}

ALboolean ALFWShutdownOpenAL()
{
	ALCcontext *pContext;
	ALCdevice *pDevice;

	pContext = alcGetCurrentContext();
	pDevice = alcGetContextsDevice(pContext);

	alcMakeContextCurrent(NULL);
	alcDestroyContext(pContext);
	alcCloseDevice(pDevice);

	return AL_TRUE;
}

ALboolean ALFWLoadWaveToBuffer(const char *szWaveFile, ALuint uiBufferID, ALenum eXRAMBufferMode)
{
	WAVEID			WaveID;
	ALint			iDataSize, iFrequency;
	ALenum			eBufferFormat;
	ALchar			*pData;
	ALboolean		bReturn;

	bReturn = AL_FALSE;
	if (g_pWaveLoader)
	{
		if (SUCCEEDED(g_pWaveLoader->LoadWaveFile(szWaveFile, &WaveID)))
		{
			if ((SUCCEEDED(g_pWaveLoader->GetWaveSize(WaveID, (unsigned long*)&iDataSize))) &&
				(SUCCEEDED(g_pWaveLoader->GetWaveData(WaveID, (void**)&pData))) &&
				(SUCCEEDED(g_pWaveLoader->GetWaveFrequency(WaveID, (unsigned long*)&iFrequency))) &&
				(SUCCEEDED(g_pWaveLoader->GetWaveALBufferFormat(WaveID, &alGetEnumValue, (unsigned long*)&eBufferFormat))))
			{
				// Set XRAM Mode (if application)
				if (eaxSetBufferMode && eXRAMBufferMode)
					eaxSetBufferMode(1, &uiBufferID, eXRAMBufferMode);

				alGetError();
				alBufferData(uiBufferID, eBufferFormat, pData, iDataSize, iFrequency);
				if (alGetError() == AL_NO_ERROR)
					bReturn = AL_TRUE;

				g_pWaveLoader->DeleteWaveFile(WaveID);
			}
		}
	}

	return bReturn;
}

void ALFWprintf(const char* x, ...)
{
	va_list args;
	va_start(args, x);
	vprintf(x, args);
	va_end(args);
}

ALchar fullPath[_MAX_PATH];
ALchar *ALFWaddMediaPath(const ALchar *filename)
{
	sprintf(fullPath, "%s%s", "..\\..\\Media\\", filename);
	return fullPath;
}

ALint ALFWKeyPress(void)
{
	return _kbhit();
}

// Extension Queries

ALboolean ALFWIsXRAMSupported()
{
	ALboolean bXRAM = AL_FALSE;

	if (alIsExtensionPresent("EAX-RAM") == AL_TRUE)
	{
		// Get X-RAM Function pointers
		eaxSetBufferMode = (EAXSetBufferMode)alGetProcAddress("EAXSetBufferMode");
		eaxGetBufferMode = (EAXGetBufferMode)alGetProcAddress("EAXGetBufferMode");

		if (eaxSetBufferMode && eaxGetBufferMode)
		{
			eXRAMSize = alGetEnumValue("AL_EAX_RAM_SIZE");
			eXRAMFree = alGetEnumValue("AL_EAX_RAM_FREE");
			eXRAMAuto = alGetEnumValue("AL_STORAGE_AUTOMATIC");
			eXRAMHardware = alGetEnumValue("AL_STORAGE_HARDWARE");
			eXRAMAccessible = alGetEnumValue("AL_STORAGE_ACCESSIBLE");

			if (eXRAMSize && eXRAMFree && eXRAMAuto && eXRAMHardware && eXRAMAccessible)
				bXRAM = AL_TRUE;
		}
	}

	return bXRAM;
}

ALboolean ALFWIsEFXSupported()
{
	ALCdevice *pDevice = NULL;
	ALCcontext *pContext = NULL;
	ALboolean bEFXSupport = AL_FALSE;

	pContext = alcGetCurrentContext();
	pDevice = alcGetContextsDevice(pContext);

	if (alcIsExtensionPresent(pDevice, (ALCchar*)ALC_EXT_EFX_NAME))
	{
		// Get function pointers
		alGenEffects = (LPALGENEFFECTS)alGetProcAddress("alGenEffects");
		alDeleteEffects = (LPALDELETEEFFECTS)alGetProcAddress("alDeleteEffects");
		alIsEffect = (LPALISEFFECT)alGetProcAddress("alIsEffect");
		alEffecti = (LPALEFFECTI)alGetProcAddress("alEffecti");
		alEffectiv = (LPALEFFECTIV)alGetProcAddress("alEffectiv");
		alEffectf = (LPALEFFECTF)alGetProcAddress("alEffectf");
		alEffectfv = (LPALEFFECTFV)alGetProcAddress("alEffectfv");
		alGetEffecti = (LPALGETEFFECTI)alGetProcAddress("alGetEffecti");
		alGetEffectiv = (LPALGETEFFECTIV)alGetProcAddress("alGetEffectiv");
		alGetEffectf = (LPALGETEFFECTF)alGetProcAddress("alGetEffectf");
		alGetEffectfv = (LPALGETEFFECTFV)alGetProcAddress("alGetEffectfv");
		alGenFilters = (LPALGENFILTERS)alGetProcAddress("alGenFilters");
		alDeleteFilters = (LPALDELETEFILTERS)alGetProcAddress("alDeleteFilters");
		alIsFilter = (LPALISFILTER)alGetProcAddress("alIsFilter");
		alFilteri = (LPALFILTERI)alGetProcAddress("alFilteri");
		alFilteriv = (LPALFILTERIV)alGetProcAddress("alFilteriv");
		alFilterf = (LPALFILTERF)alGetProcAddress("alFilterf");
		alFilterfv = (LPALFILTERFV)alGetProcAddress("alFilterfv");
		alGetFilteri = (LPALGETFILTERI)alGetProcAddress("alGetFilteri");
		alGetFilteriv = (LPALGETFILTERIV)alGetProcAddress("alGetFilteriv");
		alGetFilterf = (LPALGETFILTERF)alGetProcAddress("alGetFilterf");
		alGetFilterfv = (LPALGETFILTERFV)alGetProcAddress("alGetFilterfv");
		alGenAuxiliaryEffectSlots = (LPALGENAUXILIARYEFFECTSLOTS)alGetProcAddress("alGenAuxiliaryEffectSlots");
		alDeleteAuxiliaryEffectSlots = (LPALDELETEAUXILIARYEFFECTSLOTS)alGetProcAddress("alDeleteAuxiliaryEffectSlots");
		alIsAuxiliaryEffectSlot = (LPALISAUXILIARYEFFECTSLOT)alGetProcAddress("alIsAuxiliaryEffectSlot");
		alAuxiliaryEffectSloti = (LPALAUXILIARYEFFECTSLOTI)alGetProcAddress("alAuxiliaryEffectSloti");
		alAuxiliaryEffectSlotiv = (LPALAUXILIARYEFFECTSLOTIV)alGetProcAddress("alAuxiliaryEffectSlotiv");
		alAuxiliaryEffectSlotf = (LPALAUXILIARYEFFECTSLOTF)alGetProcAddress("alAuxiliaryEffectSlotf");
		alAuxiliaryEffectSlotfv = (LPALAUXILIARYEFFECTSLOTFV)alGetProcAddress("alAuxiliaryEffectSlotfv");
		alGetAuxiliaryEffectSloti = (LPALGETAUXILIARYEFFECTSLOTI)alGetProcAddress("alGetAuxiliaryEffectSloti");
		alGetAuxiliaryEffectSlotiv = (LPALGETAUXILIARYEFFECTSLOTIV)alGetProcAddress("alGetAuxiliaryEffectSlotiv");
		alGetAuxiliaryEffectSlotf = (LPALGETAUXILIARYEFFECTSLOTF)alGetProcAddress("alGetAuxiliaryEffectSlotf");
		alGetAuxiliaryEffectSlotfv = (LPALGETAUXILIARYEFFECTSLOTFV)alGetProcAddress("alGetAuxiliaryEffectSlotfv");

		if (alGenEffects &&	alDeleteEffects && alIsEffect && alEffecti && alEffectiv &&	alEffectf &&
			alEffectfv && alGetEffecti && alGetEffectiv && alGetEffectf && alGetEffectfv &&	alGenFilters &&
			alDeleteFilters && alIsFilter && alFilteri && alFilteriv &&	alFilterf && alFilterfv &&
			alGetFilteri &&	alGetFilteriv && alGetFilterf && alGetFilterfv && alGenAuxiliaryEffectSlots &&
			alDeleteAuxiliaryEffectSlots &&	alIsAuxiliaryEffectSlot && alAuxiliaryEffectSloti &&
			alAuxiliaryEffectSlotiv && alAuxiliaryEffectSlotf && alAuxiliaryEffectSlotfv &&
			alGetAuxiliaryEffectSloti && alGetAuxiliaryEffectSlotiv && alGetAuxiliaryEffectSlotf &&
			alGetAuxiliaryEffectSlotfv)
			bEFXSupport = AL_TRUE;
	}

	return bEFXSupport;
}



boolean updown = FALSE;
int RPM = -100;

void createRPM() {
	while (1) {
		Sleep(100);

		if (!updown) {
			RPM += rand() % 75 + 1;
			//printf("RPM: %d\n", RPM);
			if (RPM > 3000) updown = TRUE;
		}
		else {
			RPM -= rand() % 75 + 1;
			//printf("RPM: %d\n", RPM);
			if (RPM < 1000) updown = FALSE;
		}
	}
}

void pyRPM() {

	const int argc = 3;
	char* argv[3];
	argv[0] = "Car Engine Sound Effect.exe";
	argv[1] = "script";
	argv[2] = "rpm";

	PyObject *pName, *pModule, /**pDict,*/ *pFunc;
	PyObject *pArgs, *pValue;
	int i;

	if (argc < 3) {
		fprintf(stderr, "Usage: app fichier fonction [args]\n");
		return;
	}

	while (1) {

		Py_Initialize();
		pName = PyUnicode_FromString(argv[1]);
		/* Error checking of pName left out */

		pModule = PyImport_Import(pName);
		Py_DECREF(pName);

		if (pModule != NULL) {
			pFunc = PyObject_GetAttrString(pModule, argv[2]);
			/* pFunc is a new reference */

			if (pFunc && PyCallable_Check(pFunc)) {
				pArgs = PyTuple_New(argc - 3);
				for (i = 0; i < argc - 3; ++i) {
					pValue = PyLong_FromLong(atoi(argv[i + 3]));
					if (!pValue) {
						Py_DECREF(pArgs);
						Py_DECREF(pModule);
						fprintf(stderr, "Cannot convert argument\n");
						return;
					}
					/* pValue reference stolen here: */
					PyTuple_SetItem(pArgs, i, pValue);
				}
				pValue = PyObject_CallObject(pFunc, pArgs);
				Py_DECREF(pArgs);
				if (pValue != NULL) {
					//printf("Result of call: %ld\n", PyLong_AsLong(pValue));
					Py_DECREF(pValue);
					RPM = (int)PyLong_AsLong(pValue);
				}
				else {
					Py_DECREF(pFunc);
					Py_DECREF(pModule);
					PyErr_Print();
					fprintf(stderr, "Call failed\n");
					return;
				}
			}
			else {
				if (PyErr_Occurred())
					PyErr_Print();
				fprintf(stderr, "Cannot find function \"%s\"\n", argv[2]);
			}
			Py_XDECREF(pFunc);
			Py_DECREF(pModule);
		}
		else {
			PyErr_Print();
			fprintf(stderr, "Failed to load \"%s\"\n", argv[1]);
			return;
		}
	}
	Py_Finalize();
	return;
}


int getRPM() {
	return RPM;
	//return std::stoi(execute_cmd("python3.5 /home/pi/Desktop/pythOBD.py"));
}

void setRPM(int new_rpm) {
	RPM = new_rpm;
}

