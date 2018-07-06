// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTAudio_H
#define DTAudio_H

// Lightweight wrapper around an AIFF file.

#include <string>
using namespace std;

class DTCharArray;
class DTUCharArray;
class DTShortIntArray;
class DTIntArray;

class DTMutableCharArray;
class DTMutableUCharArray;
class DTMutableShortIntArray;
class DTMutableIntArray;
 
class DTAudio {
public:
    DTAudio(string fileName);
    ~DTAudio();

    short int NumberOfChannels(void) const {return numChannels;}
    int NumberOfSamples(void) const {return numSampleFrames;}
    short int SampleSize(void) const {return sampleSize;} // In bits
    double SampleRate(void) const {return sampleRate;}

    unsigned long int StartOfSample(void) const {return startOfSampledSound;}
    std::string FileName(void) const {return aiffFileName;}
    FILE *FilePointer(void) const {return filePointer;}
    
private:
    // Not allowed to copy this object.  Pass it in by reference.
    DTAudio(const DTAudio &);
    void operator=(const DTAudio &);
    
    std::string aiffFileName;
    FILE *filePointer;

    // Sampling information.
    short int numChannels;
    int       numSampleFrames;
    short int sampleSize;
    double    sampleRate;

    unsigned long int startOfSampledSound;
};

// High level access to data.
// The into array will be truncated for the last access if needed, so the pointer might change.
// The size and type needs to match.
extern bool ReadInSamples(const DTAudio &,int sampleNumber,DTMutableUCharArray &into);    // Raw
extern bool ReadInSamples(const DTAudio &,int sampleNumber,DTMutableCharArray &into);     // 8
extern bool ReadInSamples(const DTAudio &,int sampleNumber,DTMutableShortIntArray &into); // 16
extern bool ReadInSamples(const DTAudio &,int sampleNumber,DTMutableIntArray &into);      // 32

class DTAudioOutput
{
public:
    ~DTAudioOutput() {Close();}
    DTAudioOutput(string fName,short int channels,short int sampleSize,double sampleRate);

    int NumberOfChannels(void) const {return numberOfChannels;}
    int SampleSize(void) const {return sampleSize;} // In bits

    FILE *FilePointer(void) const {return filePointer;}

    void Close(void); // Wraps things up.
    
private:
    // Not allowed to copy this object.
    DTAudioOutput(const DTAudioOutput &);
    DTAudioOutput &operator=(const DTAudioOutput &);
    
    FILE *filePointer;
    int numberOfChannels;
    int sampleSize;
};

extern bool WriteSamples(DTAudioOutput &,const DTUCharArray &);    // Raw access.
extern bool WriteSamples(DTAudioOutput &,const DTCharArray &);     // 8 bit
extern bool WriteSamples(DTAudioOutput &,const DTShortIntArray &); // 16 bit
extern bool WriteSamples(DTAudioOutput &,const DTIntArray &);      // 32 bit

#endif
