// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTMesh1DGrid_H
#define DTMesh1DGrid_H

#include "DTDataStorage.h"

class DTMesh1DGrid {
public:
    DTMesh1DGrid() : _m(0), _origin(0), _h(1.0) {};
    DTMesh1DGrid(double origin,double h,int m);

    int m(void) const {return _m;}

    double Origin(void) const {return _origin;}
    double h(void) const {return _h;}

    bool IsStandard(void) const {return (_origin==0.0 && _h==1.0);}
    double FromSpaceToGrid(double v);
    
    void pinfo(void) const;

private:
    int _m;
    double _origin;
    double _h;
};

bool operator==(const DTMesh1DGrid &,const DTMesh1DGrid &);
bool operator!=(const DTMesh1DGrid &,const DTMesh1DGrid &);

// Conversion

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTMesh1DGrid &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTMesh1DGrid &theVar);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTMesh1DGrid &toWrite); // One time value, self documenting.

#endif
