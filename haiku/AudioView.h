
#ifndef _AUDIO_VIEW_H
#define _AUDIO_VIEW_H_

#include <Window.h>
#include <View.h>
#include <string>

class AudioView : public BView
{
	public:
	AudioView();
	virtual ~AudioView();
	
	public:
	virtual void Draw (BRect updateaRect);
	virtual void AttachedToWindow (void);
};
	

class AudioViewWindow : public BWindow
{
	public:
			 AudioViewWindow();
	virtual ~AudioViewWindow();
	
	public:
	virtual void MessageReceived (BMessage *message);
	virtual bool QuitRequested (void);
	
	private:
	AudioView *fAudioView;
};
	

#endif // AUDIO_VIEW_H
