// Part of DTSource. Copyright 2014. David Adalsteinsson.
// see http://www.visualdatatools.com/DTSource/license.html for more information.

#ifndef DTMesh3DOnDisk_H
#define DTMesh3DOnDisk_H

#include "DTFile.h"
#include "DTPointer.h"
#include "DTMesh3DGrid.h"
#include "DTUtilities.h"

class DTDoubleArray;
class DTFloatArray;
class DTIntArray;
class DTUCharArray;
class DTCharArray;
class DTUShortIntArray;
class DTShortIntArray;
class DTDataStorage;

class DTMutableShortIntArray;
class DTMutableIntArray;
class DTMutableCharArray;
class DTMutableDoubleArray;

// Mesh that is stored on disk.  Not intended to be read in all at once.
// The format would allow for arbitrary layouts, but now it expects offsetToNextI<offsetToNextJ<offsetToNextK

// At the bottom of this file are some usage examples

class DTMesh3DOnDiskStorage
{
public:
    enum EndianType {LittleEndian, BigEndian};
    enum NumberType {Double, Float, UInt64, Int64, UInt32, Int32, UInt16, Int16, UInt8, Int8};
    
    ~DTMesh3DOnDiskStorage();
private:
    friend class DTMesh3DOnDisk;
    friend class DTMutableMesh3DOnDisk;

    DTMesh3DOnDiskStorage();
    int SizeOfEntry(void) const;
    
    // Methods ***********************************
    
    bool ReadZRange(DTRange range,DTMutableShortIntArray &arr) const;
    bool ReadZRange(DTRange range,DTMutableFloatArray &arr) const;

    // Read and convert.  Work needs to be big enough to accept the initial read
    bool ReadZRange(DTRange range,DTMutableCharArray &work,DTMutableDoubleArray &arr) const;
    bool ReadZRange(DTRange range,DTMutableCharArray &work,DTMutableFloatArray &arr) const;

    void RawAppend(char *,ssize_t howMany,ssize_t slices);

    // Low level methods
    bool ReadRaw(DTRange range,char *buffer) const;
    
    // Data ***********************************
    DTFile file;

    DTFile::Endian endian;
    NumberType type;
    
    // Where the data starts on disk
    off_t offsetOfStart;
    
    // The values are then stored with a stride between numbers (in bytes)
    off_t offsetToNextI; // How many bytes between (i,j,k) and (i+1,j,k)
    off_t offsetToNextJ; // How many bytes between (i,j,k) and (i,j+1,k)
    off_t offsetToNextK; // How many bytes between (i,j,k) and (i,j,k+1)
    
    // The size is stored in the grid.
    
    bool currentlyWriting;
    int howManyKWritten;
    
    DTMesh3DGrid grid;
};

class DTMesh3DOnDisk
{
public:
    DTMesh3DOnDisk() : storage(new DTMesh3DOnDiskStorage()) {}
    // Connect to a file.  Need to specify the stride information if the file exists
    DTMesh3DOnDisk(DTFile,DTMesh3DGrid,DTFile::Endian,DTMesh3DOnDiskStorage::NumberType,off_t off,off_t offI,off_t offJ,off_t offK);

    ssize_t m(void) const {return storage->grid.m();}
    ssize_t n(void) const {return storage->grid.n();}
    ssize_t o(void) const {return storage->grid.o();}
    
    DTMesh3DGrid Grid(void) const {return storage->grid;}
    
    void pinfo(void) const;
    
    DTMesh3DOnDiskStorage::NumberType Type(void) const {return storage->type;}
    DTFile::Endian Endian(void) const {return storage->endian;}
    
    // Read in chunks.  No conversion, since that would cause constant reallocations
    bool ReadZRange(DTRange range,DTMutableShortIntArray &arr) const {return storage->ReadZRange(range,arr);}
    bool ReadZRange(DTRange range,DTMutableFloatArray &arr) const {return storage->ReadZRange(range,arr);}

    // Read in chunks.  Need to specify a work array to do conversion
    bool ReadZRange(DTRange range,DTMutableCharArray &work,DTMutableDoubleArray &final) const {return storage->ReadZRange(range,work,final);}
    bool ReadZRange(DTRange range,DTMutableCharArray &work,DTMutableFloatArray &final) const {return storage->ReadZRange(range,work,final);}

    // Use Write instead of this function
    void WriteIntoFile(DTDataStorage &output,const std::string &name) const;

protected:
    DTPointer<DTMesh3DOnDiskStorage> storage;
    
};

class DTMutableMesh3DOnDisk : public DTMesh3DOnDisk
{
public:
    DTMutableMesh3DOnDisk() {}
    DTMutableMesh3DOnDisk(std::string,DTMesh3DGrid,DTMesh3DOnDiskStorage::NumberType); // Creates a new file
    DTMutableMesh3DOnDisk(DTFile,DTMesh3DGrid,DTMesh3DOnDiskStorage::NumberType);
    
    // Write the data in chunks.  The chunk needs to have the right dimension in x,y.
    // The type needs to match
    void Append(DTRange range,const DTIntArray &);
    void Append(DTRange range,const DTFloatArray &);
    void Append(DTRange range,const DTDoubleArray &);

private:
    DTMutablePointer<DTMesh3DOnDiskStorage> mutableStorage;
};

extern void Read(const DTDataStorage &input,const std::string &name,DTMesh3DOnDisk &toReturn);
extern void Write(DTDataStorage &output,const std::string &name,const DTMesh3DOnDisk &theVar);
extern void WriteOne(DTDataStorage &output,const std::string &name,const DTMesh3DOnDisk &toWrite); // One time value, self documenting.


#endif

// Examples:

// Just map every value v -> fabs(v)
/*
 int m = mesh.m();
 int n = mesh.n();
 int o = mesh.o();

 int howManySlices = 20*1000*1000 / (4*m*n); // Read in about 20MB at a time.

 DTMutableFloatArray inputValues(m,n,howManySlices);
 DTMutableFloatArray outputValues(m,n,howManySlices);
 DTMutableCharArray workArray(m,n,howManySlices*2); // *8 if the input could be a double.  Allows short and bytes

 DTMutableMesh3DOnDisk toReturn("storage",mesh.Grid(),DTMesh3DOnDiskStorage::Float); // Output file

 int startSlice = 0;
 int howManyZ;
 int i,len;
 DTProgress progress; // If DT is monitoring this.
 const float *fromP = inputValues.Pointer();
 float *toP = outputValues.Pointer();
 while (startSlice<o) {
     progress.UpdatePercentage(startSlice/double(o));
     howManyZ = std::min(howManySlices,o-startSlice);
     if (mesh.ReadZRange(DTRange(startSlice,howManyZ),workArray,inputValues)==false) break; // In case the file is too short
     len = howManyZ*m*n;
     for (i=0;i<len;i++)
         toP[i] = fabs(fromP[i]); // Simple mapping
     toReturn.Append(DTRange(0,howManyZ),outputValues);
     startSlice+=howManyZ;
 }

 */
