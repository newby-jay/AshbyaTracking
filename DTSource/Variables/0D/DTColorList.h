// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTColorList_H
#define DTColorList_H

#include "DTFloatArray.h"

#include <string>
using namespace std;

class DTDataStorage;
class DTMutableUCharArray;

class DTColorList
{
public:
    DTColorList() {}
    DTColorList(const DTFloatArray &rgb);
    
    bool IsEmpty(void) const {return rgbArray.IsEmpty();}
    DTFloatArray RGB(void) const {return rgbArray;}
    
    DTMutableUCharArray EightBitColors(void) const;
    
    void pinfo(void) const;
    
private:
    DTFloatArray rgbArray;
};

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTColorList &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTColorList &theVar);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTColorList &toWrite); // One time value, self documenting.

#endif
