
#ifndef _AUDIO_STREAM_H_
#define _AUDIO_STREAM_H_


#include <SoundPlayer.h>


class AudioStream
{
	public:
			AudioStream (const char *name, float sampleRate, int32 sampleBits, int32 						channels, int32 bufferSize);
	virtual ~AudioStream();
	
	public:
	void Stream (uint8 *stream, size_t numSamples);
	void Start (void);
	void Stop (void);
	
	public:
	void InternalSync (void *buffer, size_t size);
	static void sync_hook (void *cookie, void *buffer, size_t size, 
						   const media_raw_audio_format &format);				   
	private:
	BSoundPlayer *fSoundPlayer;
	sem_id fSemaphore;
	uint32 fWritePosition;
	uint32 fPlayPosition;
	uint32 fBufferTotal;
	uint8 *fSoundBuffer;
	int32 fSampleBits;
	
	private:
	bool fStreaming;
};

#endif // _AUDIO_STREAM_H_
