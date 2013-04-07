
#ifndef	_ROM_FILEPANEL_H_
#define _ROM_FILEPANEL_H_

#include <View.h>
#include <Window.h>
#include <FilePanel.h>
#include <Entry.h>
#include <Path.h>
#include <String.h>


class ROMFilePanel : public BFilePanel
{
	public:
			ROMFilePanel();
	virtual ~ROMFilePanel();
	
	private:
	virtual void SelectionChanged (void);
	
	private:
	void Customize (void);
	
	private:
	entry_ref fPrevRef;
};


class ROMFilter : public BRefFilter
{
	public:
	virtual bool Filter (const entry_ref *ref, BNode *node, struct  stat_beos *st, const char *filetype)
	{
		BString	tempFileName (ref->name);
		BString tempFileType (filetype);
		int32 pos;
	
		// first check the file type.
		// we don't want to filter out directories, symlinks, or volumes.
	
		if (tempFileType.ICompare ("application/x-vnd.Be-directory") == 0 ||
			tempFileType.ICompare ("application/x-vnd.Be-volume") == 0 ||
			tempFileType.ICompare ("application/x-vnd.Be.symlink") == 0) {
				return true;
		}
			
		// otherwise, we'll go ahead, analyse the file's extension 
		// and determine what to do from there.
	
		pos = tempFileName.FindLast ('.');
		if (pos == B_ERROR) {
			return false;
		}
	
		tempFileName.Remove (0, ++pos);
		if (tempFileName.ICompare ("nes") == 0 ||	// iNES format
			tempFileName.ICompare ("unf") == 0 ||	// UNIF archive (DOS)
			tempFileName.ICompare ("unif") == 0 ||	// UNIF archive (UNIX)
			tempFileName.ICompare ("fds") == 0) {	// FDS format
			return true;
		}
	
		// we couldn't catch anything.
		return false;
	}
};

#endif //	_ROM_FILEPANEL_H_


