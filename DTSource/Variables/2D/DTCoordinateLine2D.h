// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTCoordinateLine2D_H
#define DTCoordinateLine2D_H

#include "DTDataStorage.h"

class DTCoordinateLine2D {
public:
    enum LineType {Empty, XFixed, YFixed};

    DTCoordinateLine2D() : type(DTCoordinateLine2D::Empty), value(0.0) {}
    DTCoordinateLine2D(LineType t,double v) : type(t), value(v) {}
    
    LineType Type(void) const {return type;}
    double Value(void) const {return value;}

    void pinfo(void) const;
    
private:
    LineType type;
    double value;
};

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTCoordinateLine2D &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTCoordinateLine2D &theVar);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTCoordinateLine2D &toWrite); // One time value, self documenting.

#endif
