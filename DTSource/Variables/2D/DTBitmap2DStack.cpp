//
//  DTBitmap2DStack.cpp
//  test task
//
//  Created by David Adalsteinsson on 9/14/15.
//
//

#include "DTBitmap2DStack.h"
#include "DTUtilities.h"
#include "DTDataFile.h"
#include "DTMatlabDataFile.h"
#include "DTError.h"

DTBitmap2DStack::DTBitmap2DStack()
{
    numberOfBitmaps = 0;
}

DTBitmap2DStack::DTBitmap2DStack(const DTDataStorage &file,const std::string &name)
{
    theDataFile =file.AsPointer();

    // dataFile = file;
    baseName = name;
    // Count how many
    numberOfBitmaps = 0;
    std::string test;
    while (numberOfBitmaps<1000*1000) {
        test = name + "_B" + DTInt2String(numberOfBitmaps);
        if (file.Contains(test)==false) break;
        numberOfBitmaps++;
    }
}

void DTBitmap2DStack::pinfo(void) const
{
    if (numberOfBitmaps==0) {
        
    }
}

DTBitmap2D DTBitmap2DStack::operator()(int which) const
{
    if (which<0 || which>=numberOfBitmaps) {
        DTErrorMessage("DTBitmap2DStack(number)","Index out of bounds");
        return DTBitmap2D();
    }
    
    DTBitmap2D toReturn;
    std::string savedAs = baseName + "_B" + DTInt2String(which);
    Read(*theDataFile,savedAs,toReturn);
    return toReturn;
}
    
DTMutableBitmap2DStack::DTMutableBitmap2DStack(const DTDataStorage &file,const std::string &name)
{
    theDataFile =file.AsPointer();
    baseName = name;
    numberOfBitmaps = 0;
    Write(*theDataFile,baseName,DTCharArray()); // Just to indicate that something is there.
}

void Read(const DTDataStorage &input,const std::string &name,DTBitmap2DStack &toReturn)
{
    std::string theName = input.ReadString(name);
    if (theName.length()==0) {
        toReturn = DTBitmap2DStack(input,name);
    }
    else {
        // Saved in a separate data file
        std::string fileName = input.ReadString(name+"_file");
        if (fileName.length()==0) {
            DTErrorMessage("Read(DataStorage,name,DTBitmap2DStack","Invalid format");
            toReturn = DTBitmap2DStack();
            return;
        }
        
        if (fileName.length()>=4 && fileName.substr(fileName.length()-4,4)==".mat") {
            DTMatlabDataFile theFile(fileName,DTFile::ReadOnly);
            toReturn = DTBitmap2DStack(theFile,theName);
        }
        else {
            DTDataFile theFile(fileName,DTFile::ReadOnly);
            toReturn = DTBitmap2DStack(theFile,theName);
        }
    }
}

void DTMutableBitmap2DStack::AddFrame(const DTBitmap2D &bitmap)
{
    Write(*theDataFile,baseName+"_B"+DTInt2String(numberOfBitmaps++),bitmap);
}
    
