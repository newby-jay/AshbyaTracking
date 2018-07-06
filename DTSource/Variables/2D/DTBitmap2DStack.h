//
//  DTBitmap2DStack.h
//  test task
//
//  Created by David Adalsteinsson on 9/14/15.
//
//

#ifndef DTBitmap2DStack_Header
#define DTBitmap2DStack_Header

#include "DTBitmap2D.h"
#include "DTDataStorage.h"

class DTBitmap2DStack
{
public:
    DTBitmap2DStack();
    DTBitmap2DStack(const DTDataStorage &,const std::string &);
    
    void pinfo(void) const;
    
    // Access elements as if this was a list of bitmaps
    // but the bitmap is stored on disk.
    DTBitmap2D operator()(int) const;
    
    int HowManyFrames(void) const {return numberOfBitmaps;}
    
protected:
    DTMutablePointer<DTDataStorage> theDataFile;
    std::string baseName;
    int numberOfBitmaps;
};

extern void Read(const DTDataStorage &input,const std::string &name,DTBitmap2DStack &toReturn);

// Create a bitmap stack

class DTMutableBitmap2DStack : DTBitmap2DStack
{
public:
    DTMutableBitmap2DStack() {}
    DTMutableBitmap2DStack(const DTDataStorage &,const std::string &);
    
    void AddFrame(const DTBitmap2D &);

private:
    
};

#endif /* defined(__test_task__DTBitmap2DStack__) */
