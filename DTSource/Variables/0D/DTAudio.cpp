// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#include "DTAudio.h"

#include "DTError.h"
#include "DTUCharArray.h"
#include "DTCharArray.h"
#include "DTShortIntArray.h"
#include "DTIntArray.h"

#include <stdio.h>
#include <string.h>

/*
 static void printbits(unsigned long long int number)
{
    int i,oneOrZero;
    for (i=0;i<64;i++) {
        oneOrZero = ((unsigned long long int)(number << i) >> 63);
        printf("%d",oneOrZero);
    }
    printf("\n");
}

*/

static double DTConvert80ToDouble(unsigned char *buffer)
{
    // 64 bit number - s e..11..e   f..52..f
    // 80 bit number - s e..15..e i f..63..f

    // Where i is an integer bit.
    // The buffer has 10 bytes - 80 bits.
    // seeeeeee
    // eeeeeeee
    // ifffffff
    // ffffffff
    // ...

    // A double precision number is
    // seeeeeee
    // eeeeffff
    // ffffffff
    // ...

    // Act on the bits to shuffle them in place.  Will truncate the fraction, and expects val > 1
    unsigned long long int theExponent = *((unsigned short int *)buffer);
    theExponent = (theExponent << 53) >> 1; // Strip the sign bit.  Last 4 bits are 0.
    theExponent = theExponent | (((unsigned long long int)1) << 62 ); // Clean up the exp - sign = +
    unsigned long long int fraction = *(unsigned long long int *)(buffer+2); // iff..63..ff
    fraction = (fraction << 1) >> 12; // Aligns the fraction into place a double.  Strips the i bit
    unsigned long long int combined = theExponent | fraction; // Merge the two two bit patterns.
    double toReturn = *((double *)&combined);

    return toReturn;
}

static void DTConvertDoubleTo80(double sampleRate,unsigned char *buffer)
{
    // Again, only works for sampleRate>1
    unsigned long long int doubleBits = *((unsigned long long int *)&sampleRate);

    // First deal with the exponent.
    unsigned long long int exponent = ((doubleBits << 2) >> 6);
    buffer[0] = ((unsigned char *)&exponent)[0] | 64;
    buffer[1] = ((unsigned char *)&exponent)[1];

    unsigned long long int fraction = (doubleBits << 11);
    fraction = fraction | ((unsigned long long int)1 << 63);
    *((unsigned long long int *)(buffer+2)) = fraction;
}

DTAudio::~DTAudio(void)
{
    if (filePointer) {
        fclose(filePointer);
        filePointer = NULL;
    }
}

DTAudio::DTAudio(string fileName)
{
    // Read in the common chunk, to get structure information.
    aiffFileName = fileName;
    numChannels = 0;
    numSampleFrames = 0;
    sampleSize = 0;
    sampleRate = 0.0;
    startOfSampledSound = 0;
    
    filePointer = fopen(aiffFileName.c_str(),"rb");
    if (filePointer==NULL) {
        DTErrorMessage("DTAudio(file name)","Could not open file");
        aiffFileName = std::string();
        return;
    }

    // Search out the common block.
    char ID[4];
    bool failed = true;
    bool foundCOMM = false;
    bool foundSSND = false;
    unsigned long int lengthOfBlock;
    unsigned char rateBuffer[10];
    unsigned long offset,blockSize;
    
    while (1) {
        // Read the ID
        if (fread(ID,1,4,filePointer)!=4)
            break;
        // Read the block length
        if (fread(&lengthOfBlock,4,1,filePointer)!=1)
            break;
        if (strncmp(ID,"FORM",4)!=0) {
            // Skip this  block.
            fseek(filePointer,lengthOfBlock,SEEK_CUR);
            continue;
        }
        
        // See if this block is the AIFF block.
        if (fread(ID,1,4,filePointer)!=4)
            break;
        if (strncmp(ID,"AIFF",4)!=0) {
            // Skip this  block.
            fseek(filePointer,lengthOfBlock-4,SEEK_CUR);
            continue;
        }

        // This is an AIFF block.  Search for the "COMM" block and "SSND" block.
        while (1) {
            if (foundCOMM && foundSSND) {
                failed = false;
                break;
            }
            if (fread(ID,1,4,filePointer)!=4) break;
            if (fread(&lengthOfBlock,4,1,filePointer)!=1) break;
            
            if (strncmp(ID,"COMM",4)==0) {
                // The common block.  Read it in and parse it.
                //   short          numChannels;
                //   unsigned long  numSampleFrames;
                //   short          sampleSize;
                //   extended       sampleRate; - this is a 80 bit double precision number
                if (fread(&numChannels,2,1,filePointer)!=1 ||
                    fread(&numSampleFrames,4,1,filePointer)!=1 ||
                    fread(&sampleSize,2,1,filePointer)!=1 ||
                    fread(&rateBuffer,1,10,filePointer)!=10)
                    break;
                // Convert the rate
                sampleRate = DTConvert80ToDouble(rateBuffer);
                // Skip any
                foundCOMM = true;
                continue; // Next block in the AIFF block.
            }
            if (strncmp(ID,"SSND",4)==0) {
                // Sampled sound
                //   unsigned long  offset;
                //   unsigned long  blockSize;
                // At this point, expect both to be 0.
                if (fread(&offset,4,1,filePointer)!=1 ||
                    fread(&blockSize,4,1,filePointer)!=1)
                    break;

                startOfSampledSound = ftell(filePointer);
                fseek(filePointer,lengthOfBlock-8,SEEK_CUR);
                foundSSND = true;
                
                continue; // Next block in the AIFF block.
            }
            
            // Skip over this block.
            fseek(filePointer,lengthOfBlock,SEEK_CUR);
        }

        break;
    }

    if (failed) {
        DTErrorMessage("DTAudio(file)","Invalid format");
        aiffFileName = std::string();
        fclose(filePointer);
        filePointer = NULL;
        numChannels = 0;
        numSampleFrames = 0;
        sampleSize = 0;
        sampleRate = 0.0;
        startOfSampledSound = 0;
        return;
    }
}

bool ReadInSamples(const DTAudio &audio,int sampleNumber,DTMutableUCharArray &into)
{
    // Check input variables.
    if (audio.NumberOfChannels()*audio.SampleSize()!=into.m()*8) {
        DTErrorMessage("ReadInSamples(audio,#,UCharArray)","Number of channels is incompatible.");
        return false;
    }

    int numSamples = audio.NumberOfSamples();
    if (sampleNumber<0 || numSamples<=sampleNumber) {
        DTErrorMessage("ReadInSamples(audio,#,UCharArray)","sampleNumber out of bounds.");
        return false;
    }

    if (into.o()>1) {
        DTErrorMessage("ReadInSamples(audio,#,UCharArray)","Passing in a 3D array.");
        return false;
    }

    // How many to read.
    if (sampleNumber + into.n() > numSamples) {
        // Resize the array.  Should only happen at the very end.
        into = DTMutableUCharArray(into.m(),numSamples-sampleNumber);
    }

    size_t howMuchToRead = into.n();
    FILE *filePointer = audio.FilePointer();
    fseek(filePointer,audio.StartOfSample() + sampleNumber*into.m(),SEEK_SET);
    size_t howMuchWasRead = fread(into.Pointer(),1,howMuchToRead,filePointer);
    if (howMuchWasRead!=howMuchToRead) {
        DTErrorMessage("ReadInSamples(audio,#,UCharArray)","Failed when reading from file.");
        return false;
    }

    return true;
}

bool ReadInSamples(const DTAudio &audio,int sampleNumber,DTMutableCharArray &into)
{
    // Check input variables.
    if (audio.NumberOfChannels()!=into.m()) {
        DTErrorMessage("ReadInSamples(audio,#,CharArray)","Number of channels is incompatible.");
        return false;
    }

    if (audio.SampleSize()!=8) {
        DTErrorMessage("ReadInSamples(audio,#,CharArray)","Sample depth is incompatiable.");
        // Should call it with a different array type.  Doesn't do hidden type conversion.
        return false;
    }

    int numSamples = audio.NumberOfSamples();
    if (sampleNumber<0 || numSamples<=sampleNumber) {
        DTErrorMessage("ReadInSamples(audio,#,CharArray)","sampleNumber out of bounds.");
        return false;
    }

    if (into.o()>1) {
        DTErrorMessage("ReadInSamples(audio,#,CharArray)","Passing in a 3D array.");
        return false;
    }

    // How many to read.
    if (sampleNumber + into.n() > numSamples) {
        // Resize the array.  Should only happen at the very end.
        into = DTMutableCharArray(into.m(),numSamples-sampleNumber);
    }

    size_t howMuchToRead = into.n();
    FILE *filePointer = audio.FilePointer();
    fseek(filePointer,audio.StartOfSample() + sampleNumber*into.m(),SEEK_SET);
    size_t howMuchWasRead = fread(into.Pointer(),1,howMuchToRead,filePointer);
    if (howMuchWasRead!=howMuchToRead) {
        DTErrorMessage("ReadInSamples(audio,#,CharArray)","Failed when reading from file.");
        return false;
    }

    return true;
}

bool ReadInSamples(const DTAudio &audio,int sampleNumber,DTMutableShortIntArray &into)
{
    // Check input variables.
    if (audio.NumberOfChannels()!=into.m()) {
        DTErrorMessage("ReadInSamples(audio,#,array)","Number of channels is incompatible.");
        return false;
    }

    if (audio.SampleSize()!=sizeof(short int)*8) {
        DTErrorMessage("ReadInSamples(audio,#,array)","Sample depth is incompatiable.");
        // Should call it with a different array type.  Doesn't do hidden type conversion.
        return false;
    }

    int numSamples = audio.NumberOfSamples();
    if (sampleNumber<0 || numSamples<=sampleNumber) {
        DTErrorMessage("ReadInSamples(audio,#,array)","sampleNumber out of bounds.");
        return false;
    }

    if (into.o()>1) {
        DTErrorMessage("ReadInSamples(audio,#,array)","Passing in a 3D array.");
        return false;
    }

    // How many to read.
    if (sampleNumber + into.n() > numSamples) {
        // Resize the array.  Should only happen at the very end.
        into = DTMutableShortIntArray(into.m(),numSamples-sampleNumber);
    }

    size_t howMuchToRead = 2*into.n();
    FILE *filePointer = audio.FilePointer();
    fseek(filePointer,audio.StartOfSample() + sampleNumber*sizeof(short int)*into.m(),SEEK_SET);
    size_t howMuchWasRead = fread(into.Pointer(),2,howMuchToRead,filePointer);
    if (howMuchWasRead!=howMuchToRead) {
        DTErrorMessage("ReadInSamples(audio,#,array)","Failed when reading from file.");
        return false;
    }

    return true;
}

bool ReadInSamples(const DTAudio &audio,int sampleNumber,DTMutableIntArray &into)
{
    // Check input variables.
    if (audio.NumberOfChannels()!=into.m()) {
        DTErrorMessage("ReadInSamples(audio,#,IntArray)","Number of channels is incompatible.");
        return false;
    }

    if (audio.SampleSize()!=sizeof(int)*4) {
        DTErrorMessage("ReadInSamples(audio,#,IntArray)","Sample depth is incompatiable.");
        // Should call it with a different array type.  Doesn't do hidden type conversion.
        return false;
    }

    int numSamples = audio.NumberOfSamples();
    if (sampleNumber<0 || numSamples<=sampleNumber) {
        DTErrorMessage("ReadInSamples(audio,#,IntArray)","sampleNumber out of bounds.");
        return false;
    }

    if (into.o()>1) {
        DTErrorMessage("ReadInSamples(audio,#,IntArray)","Passing in a 3D array.");
        return false;
    }

    // How many to read.
    if (sampleNumber + into.n() > numSamples) {
        // Resize the array.  Should only happen at the very end.
        into = DTMutableIntArray(into.m(),numSamples-sampleNumber);
    }

    size_t howMuchToRead = 4*into.n();
    FILE *filePointer = audio.FilePointer();
    fseek(filePointer,audio.StartOfSample() + sampleNumber*sizeof(short int)*into.m(),SEEK_SET);
    size_t howMuchWasRead = fread(into.Pointer(),4,howMuchToRead,filePointer);
    if (howMuchWasRead!=howMuchToRead) {
        DTErrorMessage("ReadInSamples(audio,#,IntArray)","Failed when reading from file.");
        return false;
    }

    return true;
}

DTAudioOutput::DTAudioOutput(string fName,short int channels,short int sampleS,double sampleRate)
{
    numberOfChannels = 0;
    sampleSize = 0;

    filePointer = fopen(fName.c_str(),"wb");
    if (filePointer==NULL)
        return;

    numberOfChannels = channels;
    sampleSize = sampleS;

    unsigned int fourZeros = 0;
    // Write the header, but leave the lengths we don't know as 0.
    fwrite("FORM",1,4,filePointer);
    fwrite(&fourZeros,4,1,filePointer); // Size info
    fwrite("AIFFCOMM",1,8,filePointer);
    unsigned long int eighteen = 18;
    fwrite(&eighteen,4,1,filePointer);
    fwrite(&channels,2,1,filePointer); // Number of Channels
    fwrite(&fourZeros,4,1,filePointer); // Number of Sample Frames
    fwrite(&sampleS,2,1,filePointer); // Sample Size

    unsigned char buffer[10];
    DTConvertDoubleTo80(sampleRate,buffer);
    fwrite(buffer,1,10,filePointer);

    // Now write the sound block.
    fwrite("SSND",1,4,filePointer);
    fwrite(&fourZeros,4,1,filePointer); // Size
    fwrite(&fourZeros,4,1,filePointer); // Offset
    fwrite(&fourZeros,4,1,filePointer); // Block size

    // Now add the samples.  
}

void DTAudioOutput::Close(void)
{
    if (filePointer) {
        // Write the length, and close the file.

        size_t endingSize = ftell(filePointer);

        size_t writeNumber;
        writeNumber = endingSize-8;
        fseek(filePointer,4,SEEK_SET);
        fwrite(&writeNumber,4,1,filePointer);

        writeNumber = (endingSize-54)/(numberOfChannels*sampleSize/8);
        fseek(filePointer,22,SEEK_SET);
        fwrite(&writeNumber,4,1,filePointer);

        writeNumber = endingSize-46;
        fseek(filePointer,42,SEEK_SET);
        fwrite(&writeNumber,4,1,filePointer);

        fclose(filePointer);
    }
}

bool WriteSamples(DTAudioOutput &audio,const DTUCharArray &Arr)
{
    FILE *filePointer = audio.FilePointer();
    if (filePointer==NULL)
        return false;
    if (Arr.IsEmpty())
        return true;
    if (Arr.o()>1) {
        DTErrorMessage("WriteSamples(Audio,DTUCharArray)","Passing in a 3D array.");
        return false;
    }
    if (audio.NumberOfChannels()*audio.SampleSize()!=Arr.m()*8) {
        DTErrorMessage("WriteSamples(Audio,DTUCharArray)","Incorrect number of bytes per sample.");
        return false;
    }

    // Append it to the file
    fwrite(Arr.Pointer(),1,Arr.Length(),filePointer);

    return true;
}

bool WriteSamples(DTAudioOutput &audio,const DTCharArray &Arr)
{
    FILE *filePointer = audio.FilePointer();
    if (filePointer==NULL)
        return false;
    if (Arr.IsEmpty())
        return true;
    if (Arr.o()>1) {
        DTErrorMessage("WriteSamples(Audio,DTCharArray)","Passing in a 3D array.");
        return false;
    }
    if (audio.NumberOfChannels()!=Arr.m()) {
        DTErrorMessage("WriteSamples(Audio,DTCharArray)","Incorrect number of samples.");
        return false;
    }
    if (audio.SampleSize()!=16) {
        DTErrorMessage("WriteSamples(Audio,DTCharArray)","Only works for 8 bits per sample.");
        return false;
    }

    // Append it to the file
    fwrite(Arr.Pointer(),1,Arr.Length(),filePointer);

    return true;
}

bool WriteSamples(DTAudioOutput &audio,const DTShortIntArray &Arr)
{
    FILE *filePointer = audio.FilePointer();
    if (filePointer==NULL)
        return false;
    if (Arr.IsEmpty())
        return true;
    if (Arr.o()>1) {
        DTErrorMessage("WriteSamples(Audio,ShortIntArr)","Passing in a 3D array.");
        return false;
    }
    if (audio.NumberOfChannels()!=Arr.m()) {
        DTErrorMessage("WriteSamples(Audio,ShortIntArr)","Incorrect number of samples.");
        return false;
    }
    if (audio.SampleSize()!=16) {
        DTErrorMessage("WriteSamples(Audio,ShortIntArr)","Only works for 16 bits per sample.");
        return false;
    }

    // Append it to the file
    fwrite(Arr.Pointer(),2,Arr.Length(),filePointer);
    
    return true;
}

bool WriteSamples(DTAudioOutput &audio,const DTIntArray &Arr)
{
    FILE *filePointer = audio.FilePointer();
    if (filePointer==NULL)
        return false;
    if (Arr.IsEmpty())
        return true;
    if (Arr.o()>1) {
        DTErrorMessage("WriteSamples(Audio,DTIntArray)","Passing in a 3D array.");
        return false;
    }
    if (audio.NumberOfChannels()!=Arr.m()) {
        DTErrorMessage("WriteSamples(Audio,DTIntArray)","Incorrect number of samples.");
        return false;
    }
    if (audio.SampleSize()!=16) {
        DTErrorMessage("WriteSamples(Audio,DTIntArray)","Only works for 32 bits per sample.");
        return false;
    }

    // Append it to the file
    fwrite(Arr.Pointer(),4,Arr.Length(),filePointer);

    return true;
}

