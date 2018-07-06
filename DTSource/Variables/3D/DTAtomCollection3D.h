// Part of DTSource. Copyright 2004-2017. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTAtomCollection3D_H
#define DTAtomCollection3D_H

#include "DTFloatArray.h"
#include "DTIntArray.h"
#include "DTUCharArray.h"

#include "DTDataStorage.h"

struct DTRegion3D;

class DTAtomCollection3D {
public:
    DTAtomCollection3D() : locations(), atoms(), charges(), bonds() {};
    DTAtomCollection3D(const DTFloatArray &locations,const DTUCharArray &atoms);

    DTAtomCollection3D(const DTFloatArray &locations,const DTUCharArray &atoms,
                       const DTIntArray &,const DTFloatArray &);

    bool IsEmpty() const {return locations.IsEmpty();}

    DTFloatArray Locations(void) const {return locations;}
    DTUCharArray Atoms(void) const {return atoms;}
    DTIntArray Bonds(void) const {return bonds;}
    DTFloatArray Charges(void) const {return charges;}

    void pinfo(void) const;
    
private:

    DTFloatArray locations; // 3xN array
    DTUCharArray atoms; // N entries
    DTFloatArray charges; // optional N array
    DTIntArray bonds; // Optional 2xM array of location offsets.
};

extern DTAtomCollection3D SetBonds(const DTAtomCollection3D &,const DTIntArray &);
extern DTAtomCollection3D SetCharges(const DTAtomCollection3D &,const DTFloatArray &);

extern DTRegion3D BoundingBox(const DTAtomCollection3D &);

// Reading and writing
extern void Read(const DTDataStorage &input,const std::string &name,DTAtomCollection3D &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTAtomCollection3D &thePath);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTAtomCollection3D &toWrite); // One time value, self documenting.

#endif
