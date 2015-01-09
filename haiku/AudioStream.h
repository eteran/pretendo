
#ifndef _AUDIO_STREAM_H_
#define _AUDIO_STREAM_H_

#include <SoundPlayer.h>

class AudioStream
{
	public:
			AudioStream (float sampleRate, int32 sampleBits, int32 	channels, int32 bufferSize);
	virtual ~AudioStream();
	
	private:
		AudioStream(const AudioStream &) = delete;
		AudioStream& operator=(const AudioStream &) = delete;
	
	public:
	void Stream (const void *stream, size_t numSamples);
	void Start (void);
	void Stop (void);
	uint8 *SoundBuffer (void) { return fSoundBuffer; };
	
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
