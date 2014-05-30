#ifndef VHDDetectorConstruction_h
#define VHDDetectorConstruction_h 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include "VHDPhantomZSliceHeader.hh"
#include <map>

#include "G4MultiFunctionalDetector.hh"

class G4Material;
class G4Box;
class G4LogicalVolume;

class VHDDetectorConstruction : public G4VUserDetectorConstruction
{
public:

  VHDDetectorConstruction();
  virtual ~VHDDetectorConstruction();

  G4VPhysicalVolume* Construct();
  // trigger the construction of the geometry

  G4int GetNX() const {return nVoxelX;}
  G4int GetNY() const {return nVoxelY;}
  G4int GetNZ() const {return fNoFiles;}  //number of file is the same as the number of voxels in the z-direction
  G4int GetNEngbin() const {return NEngbin;}
  void SetDirName(G4String name) {dirname = name;}
  void SetParticleFlag(G4int isElectron, G4int isPhoton);
  void DefineMaterialsOfInterest();
  void SetEnergyBinOption (G4int ieng) {ebin = ieng;};
  //G4double GetObjMass() const;

protected:
  void InitialisationOfMaterials();
  // create the original materials

  void ReadPhantomData();
  // read the DICOM files describing the phantom

  void ReadPhantomDataFile(const G4String& fname);
  // read one of the DICOM files describing the phantom (usually one per Z slice). Build a VHDPhantomZSliceHeader for each file

  void MergeZSliceHeaders();
  // merge the slice headers of all the files

  void ConstructPhantomContainer();
  virtual void ConstructPhantom() = 0;  //syntax "=0" indicates that ConstructPhantom() is an abstract member function!!
  // construct the phantom volumes. This method should be implemented for each of the derived classes
 
  void SetMultiSensDet_NestedParam(G4LogicalVolume* voxel_logic);
  void SetMultiSensDet_RegParam(G4LogicalVolume* voxel_logic);


protected:   //define all "private"-like variables in "protected" since there is class inheritence going on in this class
  G4Material* air;

  // World ...
  G4Box* world_solid;
  G4LogicalVolume* world_logic;
  G4VPhysicalVolume* world_phys;

  G4Box* container_solid;
  G4LogicalVolume* container_logic;
  G4VPhysicalVolume* container_phys;

  G4int fNoFiles; // number of DICOM files
  std::map<unsigned int,unsigned int> Organtag2MatIndx;
  std::vector<G4Material*> fOriginalMaterials;  // list of original materials
  //std::vector<G4Material*> fMaterials;  // list of new materials created to distinguish different density voxels that have the same original materials
  std::vector<G4Material*> MaterialsOfInterest;
  
  std::vector<VHDPhantomZSliceHeader*> fZSliceHeaders; // list of z slice header (one per DICOM files)
  VHDPhantomZSliceHeader* fZSliceHeaderMerged; // z slice header resulted from merging all z slice headers
  size_t* fMateIDs; // index of material of each voxel
  //VHDPhantomZSliceHeader* sliceHeader;

  //unsigned int* fMateIDs; // index of material of each voxel
  //std::map<G4int,G4double> fDensityDiffs; // Density difference to distinguish material for each original material (by index)

  G4int nVoxelX, nVoxelY, nVoxelZ, NEngbin;
  G4double voxelHalfDimX,  voxelHalfDimY, voxelHalfDimZ,totDensity;
  G4String dirname;
  //G4int isVis;
  G4MultiFunctionalDetector* MFDet;
  G4bool electronflag, photonflag;   //flag to see if one wants to track electron or gamma rays or not in SetMultiSensDet function
  G4int ebin;  //flag to select the energy bin used in the simulation
};

#endif

