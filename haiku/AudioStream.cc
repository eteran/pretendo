
#include "AudioStream.h"

#include <OS.h>
#include <MediaDefs.h>
#include <string.h>

AudioStream::AudioStream (const char *name, float sampleRate, int32 sampleBits, 
	int32 channels, int32 bufferSize)
{	
	media_raw_audio_format format;
	format.frame_rate = sampleRate;
	format.channel_count = channels;
	
	if (sampleBits == 16) {
		format.format = media_raw_audio_format::B_AUDIO_SHORT;
		format.byte_order = B_MEDIA_LITTLE_ENDIAN;
	} else {
		format.format = media_raw_audio_format::B_AUDIO_UCHAR;
		format.byte_order = B_MEDIA_LITTLE_ENDIAN;
	}
	
	fSampleBits = sampleBits;

	format.buffer_size = bufferSize;
	
	fSoundPlayer = new BSoundPlayer (&format, name, &sync_hook, NULL, this);
	fSemaphore = create_sem (0, "sound_player");

	fWritePosition = 0;
	fPlayPosition = 0;
	
	fBufferTotal = bufferSize * sampleBits;
	
	fSoundBuffer = new uint8[fBufferTotal];
	memset (fSoundBuffer, 0x80, fBufferTotal);
	
	fStreaming = false;
}


AudioStream::~AudioStream()
{
	if (fSoundPlayer && fStreaming) {
		fSoundPlayer->Stop();
		delete fSoundPlayer;
		
		delete_sem (fSemaphore);
	}
	
	delete[] fSoundBuffer;
}

#include <stdio.h>

void
AudioStream::Stream (const void *stream, size_t samples)
{
	if (! fSoundPlayer || fStreaming == false) {
		return;
	}
	
	if (acquire_sem (fSemaphore) == B_OK) {
		if (fSampleBits == 8) {
			const uint8 *out = reinterpret_cast<const uint8 *>(stream);	
			size_t len = samples * sizeof(uint8);
			size_t pos = fWritePosition + len;
			size_t space = fBufferTotal - fWritePosition;
			
			if (pos > fBufferTotal) {
				memcpy (fSoundBuffer + fWritePosition, out, space);
				out += space;
				len -= space;
				memcpy (fSoundBuffer, out, len);
				fWritePosition = pos - fBufferTotal;
			} else {
				memcpy (fSoundBuffer + fWritePosition, out, len);
				fWritePosition = pos;
			}
		} else {
			const uint16 *out = reinterpret_cast<const uint16 *>(stream);
			size_t len = samples;
			size_t pos = fWritePosition + samples;
			size_t space = fBufferTotal - fWritePosition;

			if (pos > fBufferTotal) {
				memcpy (fSoundBuffer + (fWritePosition * sizeof(uint16)), out, space * sizeof(uint16));
				out += space;
				len -= space;
				memcpy(fSoundBuffer, out, len * sizeof(uint16));
				fWritePosition = pos - fBufferTotal;
			} else {
				memcpy(fSoundBuffer + (fWritePosition * sizeof(uint16)), out, len * sizeof(uint16));
				fWritePosition = pos;
			}
		}

	}
}



void
AudioStream::Start (void)
{
	if (fSoundPlayer->InitCheck() == B_OK) {
		fSoundPlayer->Start();
		fSoundPlayer->SetHasData (true);
		fStreaming = true;
	} else {
		fStreaming = false;
	}
}


void
AudioStream::Stop (void)
{
	if (fStreaming) {
		fStreaming = false;
		fSoundPlayer->Stop();
		fSoundPlayer->SetHasData(false);
		release_sem(fSemaphore);
	}
}


void
AudioStream::InternalSync (void *buffer, size_t size)
{
	if (fSampleBits == 8) {
		uint8 *out = reinterpret_cast<uint8 *>(buffer);
		size_t len = size * sizeof(uint8);
		size_t pos = fPlayPosition + len;
		size_t space = fBufferTotal - fPlayPosition;
		
		if (pos > fBufferTotal) {
			memcpy (out, fSoundBuffer + fPlayPosition, space);
			out += space;
			len -= space;
			memcpy (out, fSoundBuffer, len);
			fPlayPosition = pos - fBufferTotal;
		} else {
			memcpy (out, fSoundBuffer + fPlayPosition, len);
			fPlayPosition = pos;
		}
	} else {
		///uint16 *out = reinterpret_cast<uint16 *>(buffer);
	}
	
	release_sem(fSemaphore);
}


void
AudioStream::sync_hook (void *cookie, void *buffer, size_t size, 
	const media_raw_audio_format &format)
{
	(void)format;
	
	AudioStream *object = reinterpret_cast<AudioStream *>(cookie);
	object->InternalSync (buffer, size);
	
}
