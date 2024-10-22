
 //
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
/**
 * @file   VHDDetectorConstruction.cc
 * @brief  detector construction of simulation.
 *
 * @date   7th Aug 2013
 * @author Shih-ying Huang
 * @name   Geant4.9.6-p02
 */

#include "globals.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4UnitsTable.hh"
#include "VHDDetectorConstruction.hh"
#include "VHDPhantomZSliceHeader.hh"
#include <stdlib.h>
#include <stdio.h>

//the below .hh need to be up here instead of down by the function 'SetMultiSensDet' for the destructor to work properly
#include "G4SDManager.hh"
#include "G4SDParticleWithEnergyFilter.hh"
#include "VHDPSEnergyDeposit_NestedParam.hh"
#include "VHDMSDCellFlux_NestedParam.hh"
#include "VHDPSEnergyDeposit_RegParam.hh"
#include "VHDMSDCellFlux_RegParam.hh"

//-------------------------------------------------------------
VHDDetectorConstruction::VHDDetectorConstruction()
{
  
  //make sure all the pointer address is 0 or NULL
  fZSliceHeaderMerged = 0;
  fMateIDs = 0;
  NEngbin = 0;
  //MFDet = 0;
  
  electronflag = FALSE;
  photonflag = FALSE;
}

//-------------------------------------------------------------
VHDDetectorConstruction::~VHDDetectorConstruction()
{
  delete fZSliceHeaderMerged;
  delete [] fMateIDs;
  
  //delete memory in fZSliceHeaders
  std::vector<VHDPhantomZSliceHeader*>::iterator itr1;
  for(itr1 = fZSliceHeaders.begin(); itr1 != fZSliceHeaders.end(); itr1++)
  {
  	delete (*itr1);
  }
  fZSliceHeaders.clear();
  
  //delete memory in fOriginalMaterials
  std::vector<G4Material*>::iterator itr2;
  for(itr2 = fOriginalMaterials.begin(); itr2 != fOriginalMaterials.end(); itr2++)
  {
  	delete (*itr2);
  }
  fOriginalMaterials.clear();
  
  MaterialsOfInterest.clear();
  
  //delete all primitive scorers by calling the destructor of each primitive scorer!
  G4int Nprim = MFDet->GetNumberOfPrimitives();
  for(G4int n = 0; n < Nprim; n++){
  	delete MFDet->GetPrimitive(n);
  }

  G4cout << "destroy VHDDetectorConstruction" << G4endl;
}

//-------------------------------------------------------------
G4VPhysicalVolume* VHDDetectorConstruction::Construct()
{
  //Initialize and define the elements and tissue composition used in this simulation
  InitialisationOfMaterials();
  DefineMaterialsOfInterest();
  
  //----- Build world
  G4double worldXDimension = 1.*m;
  G4double worldYDimension = 1.*m;
  G4double worldZDimension = 1.*m;

  world_solid = new G4Box( "WorldSolid",
                          worldXDimension,
                          worldYDimension,
                          worldZDimension );

  world_logic = new G4LogicalVolume( world_solid, 
                                    air, 
                                    "WorldLogical", 
                                    0, 0, 0 );

  world_phys = new G4PVPlacement( 0,
                                  G4ThreeVector(0,0,0),
                                  "World",
                                  world_logic,
                                  0,
                                  false,
                                  0 );

  // Read in all the data from voxelized data files
  ReadPhantomData();

  // Construct 
  ConstructPhantomContainer();

  //this function will be defined by another derived class, NestedParamVHDDetectorConstruction or RegularVHDDetectorConsturction
  ConstructPhantom();
  
  return world_phys;
}


//-------------------------------------------------------------
void VHDDetectorConstruction::InitialisationOfMaterials()
{
  // Creating elements :
  G4double z, a;
  G4String name, symbol;

  //====================== define the element with its atomic number and atomic weight ====================== 
  G4int totNelement = 14;
  G4Element* elH = new G4Element( name = "Hydrogen",
				  symbol = "H",
				  z = 1.0, a = 1.008  * g/mole );
  G4Element* elC = new G4Element( name = "Carbon",
                                  symbol = "C",
                                  z = 6.0, a = 12.011 * g/mole );
  G4Element* elN = new G4Element( name = "Nitrogen",
				  symbol = "N",
				  z = 7.0, a = 14.007 * g/mole );
  G4Element* elO = new G4Element( name = "Oxygen",
                                  symbol = "O",
                                  z = 8.0, a = 16.00  * g/mole );
  G4Element* elNa = new G4Element( name = "Sodium",
                                   symbol = "Na",
                                   z= 11.0, a = 22.98977* g/mole );
  G4Element* elMg = new G4Element( name = "Magnesium",
                                   symbol = "Mg",
                                   z = 12.0, a = 24.305* g/mole );
  G4Element* elP = new G4Element( name = "Phosphorus",
                                  symbol = "P",
                                  z = 15.0, a = 30.973762* g/mole );
  G4Element* elS = new G4Element( name = "Sulfur",
                                  symbol = "S",
                                  z = 16.0,a = 32.065* g/mole );
  G4Element* elCl = new G4Element( name = "Chlorine",
                                   symbol = "Cl",
                                   z = 17.0, a = 35.453* g/mole );
  G4Element* elAr = new G4Element( name = "Argon",
                                   symbol = "Ar",
                                   z = 18.0, a = 39.948* g/mole );
  G4Element* elK = new G4Element( name = "Potassium",
                                  symbol = "K",
                                  z = 19.0, a = 39.0983* g/mole );
  G4Element* elCa = new G4Element( name = "Calcium",
                                  symbol = "Ca",
                                  z = 20.0, a = 40.078* g/mole );
  G4Element* elFe = new G4Element( name = "Iron",
                                   symbol = "Fe",
                                   z = 26.0, a = 55.845* g/mole );
  G4Element* elI = new G4Element( name = "Iodine",
                                   symbol = "I",
                                   z = 53.0, a = 126.90447* g/mole );
  
  //====================== define all the tissue materials needed for the digital phantoms ======================
  G4int Nmat,organtag1,organtag2,Nelement,elindx;
  G4double density,frac,fracsum;
  G4Material* tempmat;
  G4String organname,fname1,fname2;

  //Air and push it into fOriginalMaterials vector
  air = new G4Material( "Air",
                        1.290*mg/cm3,
                        Nelement = 2 );
  air->AddElement(elN, 0.7);
  air->AddElement(elO, 0.3); 
  
  fOriginalMaterials.push_back(air);
  organtag1 = 0;
  Organtag2MatIndx[static_cast<unsigned int>(organtag1)] = 0;
  
  //open the file and read the element composition and density informatiion
  fname1 = dirname + "/ECompDensity.txt";
  std::ifstream finDF1(fname1);
  if(finDF1.good() != 1 ){
    G4Exception("VHDDetectorConstruction:InitialisationOfMaterials","",FatalErrorInArgument,G4String("Invalid file name: " + fname1).c_str());
  }

  fname2 = dirname + "/OrgantagvsName.txt";
  std::ifstream finDF2(fname2);
  if(finDF2.good() != 1 ){
    G4Exception("VHDDetectorConstruction:InitialisationOfMaterials","",FatalErrorInArgument,G4String("Invalid file name: " + fname2).c_str());
  }
  
  std::map<G4int,G4double> elspace;
  finDF1 >> Nmat;
  for(unsigned int i = 1; i <= static_cast<unsigned int>(Nmat); i++ ){
    finDF1 >> organtag1;   //read ECompDensity.txt
    fracsum = 0.0;
    for(G4int j=1; j <= totNelement;  j++){
	finDF1 >> frac;
	if(frac > 0.0){  //only store the element that has frac > 0.0
	   elspace[j] = frac/100.0;  //add the element as fraction instead of percent
	   fracsum += elspace[j];
	}
    }
    
    finDF1 >> density;
    finDF2 >> organtag2 >> organname;  // read OrgantagvsName.txt
    Nelement = elspace.size();
   
    if(organtag1 == organtag2){
	tempmat = new G4Material(organname,
				density*g/cm3,
				Nelement);

	//set element composition fraction appropriately
	std::map<G4int,G4double>::iterator it;
        for(it=elspace.begin(); it != elspace.end(); it++){
		elindx = it->first;
		frac = it->second;
		switch(elindx){
		   case 1:
		      tempmat->AddElement(elH,frac);
		      break;
		   case 2:
		      tempmat->AddElement(elC,frac);
		      break;
		   case 3:
		      tempmat->AddElement(elN,frac);
		      break;
		   case 4:
		      tempmat->AddElement(elO,frac);
		      break;
		   case 5:
		      tempmat->AddElement(elNa,frac);
		      break;
		   case 6:
		      tempmat->AddElement(elMg,frac);
		      break;
		   case 7:
		      tempmat->AddElement(elP,frac);
		      break;
		   case 8:
		      tempmat->AddElement(elS,frac);
		      break;
		   case 9:
		      tempmat->AddElement(elCl,frac);
		      break;
		   case 10:
		      tempmat->AddElement(elAr,frac);
		      break;
		   case 11:
		      tempmat->AddElement(elK,frac);
		      break;
		   case 12:
		      tempmat->AddElement(elCa,frac);
		      break;
		   case 13:
		      tempmat->AddElement(elFe,frac);
		      break;
		   case 14:
		      tempmat->AddElement(elI,frac);
		   default:
			;
		}
	}
        Organtag2MatIndx[static_cast<unsigned int>(organtag1)] = i;
	fOriginalMaterials.push_back(tempmat);
    }
    elspace.clear();  //clear the elspace for another tissue material
  }
  finDF1.close();
  finDF2.close();
  
  
  //note: Organtag2MatIndx converts UFH phantom organtag to the material matrix indx stored in this class, fOriginalMaterials
  //fOriginalMaterials store all the G4Material's defined according its corresponding elemental composition for all the definied organ parts in UFH phantoms
}


//-------------------------------------------------------------
void VHDDetectorConstruction::ReadPhantomData()
{
  G4String fname1,fname2,fname3;

  fname1 = dirname + "/Data.dat";
  std::ifstream finDF(fname1);
  
  if(finDF.good() != 1 ) {
     G4Exception("VHDDetectorConstruction:ReadPhantomData()","",FatalErrorInArgument,G4String("Invalid file name: " + fname1).c_str());
  }

  G4int compression;
  finDF >> compression; // not used here

  finDF >> fNoFiles;
  totDensity = 0;
  for(G4int i = 0; i < fNoFiles; i++ ){
    finDF >> fname2;
    //--- Read one data file & collect instances of VHDPhantomZSliceHeader
    fname3 = dirname + "/" + fname2;
    ReadPhantomDataFile(fname3);
  }

  //----- Merge data headers 
  MergeZSliceHeaders();
  finDF.close();

}

//-------------------------------------------------------------
void VHDDetectorConstruction::ReadPhantomDataFile(const G4String& fname)  //the image data contains the header and the material ID (no density)
{ 
  std::ifstream fin(fname.c_str(), std::ios_base::in);  //ios_base::in ==> open file for reading
  //std::ifstream fin(fname.c_str());
  if( !fin.is_open() ) {
    G4Exception("VHDDetectorConstruction:ReadPhantomDataFile(const G4String& fname)","",FatalErrorInArgument,G4String("Invalid file name: " + fname).c_str());
  }
  
  //==================== Read data header ====================
  VHDPhantomZSliceHeader* sliceHeader = new VHDPhantomZSliceHeader( fin );
  fZSliceHeaders.push_back( sliceHeader );  //keep track of all the slice header read in from the .g4m and saved in a vector of std::vector<VHDPhantomZSliceHeader*>
  
  //==================== Read material indices ====================
  G4int nVoxels = sliceHeader->GetNoVoxels();  //total number of voxels needed for this geometry
    
  //--- If first slice, initiliaze fMateIDs
  if( fZSliceHeaders.size() == 1 ){
    fMateIDs = new size_t[fNoFiles*nVoxels];  //size_t is probably the same as using unsigned int (8-bit int)
  }

  //==================== read in all the materialID's from all the voxels of all the files availalbe into fMateIDs[] ====================
  unsigned int mateID;
  std::map<unsigned int,unsigned int>::iterator it;
  G4int voxelCopyNo = (fZSliceHeaders.size()-1)*nVoxels; // number of voxels from previously read slices; perhaps the copy No starts at 0 since using 
  for( G4int ii = 0; ii < nVoxels; ii++, voxelCopyNo++ ){
    fin >> mateID;
    //correspond mateID to the correct G4Material
    it = Organtag2MatIndx.find(mateID);
    fMateIDs[voxelCopyNo] = it->second;
  }

  fin.close();
}


//-------------------------------------------------------------
void VHDDetectorConstruction::MergeZSliceHeaders()
{
  //----- Images must have the same dimension ... 
  fZSliceHeaderMerged = new VHDPhantomZSliceHeader( *fZSliceHeaders[0] );
  for( unsigned int ii = 1; ii < fZSliceHeaders.size(); ii++ ) {
    *fZSliceHeaderMerged += *fZSliceHeaders[ii];
  };
}

//-----------------------------------------------------------------------
void VHDDetectorConstruction::ConstructPhantomContainer()
{
  //---- Extract number of voxels and voxel dimensions
  nVoxelX = fZSliceHeaderMerged->GetNoVoxelX();
  nVoxelY = fZSliceHeaderMerged->GetNoVoxelY();
  nVoxelZ = fZSliceHeaderMerged->GetNoVoxelZ();

  voxelHalfDimX = fZSliceHeaderMerged->GetVoxelHalfX();
  voxelHalfDimY = fZSliceHeaderMerged->GetVoxelHalfY();
  voxelHalfDimZ = fZSliceHeaderMerged->GetVoxelHalfZ();
// #ifdef G4VERBOSE
   G4cout << " nVoxelX " << nVoxelX << " voxelHalfDimX " << voxelHalfDimX <<G4endl;
   G4cout << " nVoxelY " << nVoxelY << " voxelHalfDimY " << voxelHalfDimY <<G4endl;
   G4cout << " nVoxelZ " << nVoxelZ << " voxelHalfDimZ " << voxelHalfDimZ <<G4endl;
   G4cout << " totalPixels " << nVoxelX*nVoxelY*nVoxelZ <<  G4endl;
// #endif

  //----- Define the volume that contains all the voxels
  container_solid = new G4Box("phantomContainer",nVoxelX*voxelHalfDimX,nVoxelY*voxelHalfDimY,nVoxelZ*voxelHalfDimZ);

  
  container_logic = new G4LogicalVolume( container_solid, 
			 fOriginalMaterials[0],  //the material is not important, it will be fully filled by the voxels
			 "phantomContainer", 
			 0, 0, 0 );
  
  //--- Place it on the worlds
  G4double offsetX = (fZSliceHeaderMerged->GetMaxX() + fZSliceHeaderMerged->GetMinX() ) /2.;
  G4double offsetY = (fZSliceHeaderMerged->GetMaxY() + fZSliceHeaderMerged->GetMinY() ) /2.;
  G4double offsetZ = (fZSliceHeaderMerged->GetMaxZ() + fZSliceHeaderMerged->GetMinZ() ) /2.;
  G4ThreeVector posCentreVoxels(offsetX,offsetY,offsetZ);  //should be at (0,0,0), center of the geometry coordinate
// #ifdef G4VERBOSE
   G4cout << " placing voxel container volume at " << posCentreVoxels << G4endl;
// #endif
  container_phys = new G4PVPlacement(0,  // rotation
		      posCentreVoxels,
		      container_logic,     // The logic volume
		      "phantomContainer",  // Name
		      world_logic,  // Mother
		      false,           // No op. bool.
		      1);              // Copy number

}

void VHDDetectorConstruction::SetParticleFlag(G4int isElectron, G4int isPhoton)
{
	if(isElectron == 1)
	{
		electronflag = TRUE;
		G4cout << "In VHDDetectorConstruction....Go Electrons goooo!" << G4endl;
	}
	if(isPhoton == 1)
	{
		photonflag = TRUE;
		G4cout << "In VHDDetectorConstruction....Go Photons goooo!" << G4endl;
	}
}

void VHDDetectorConstruction::DefineMaterialsOfInterest()
 {
 	G4int letag,Norgan;
 	unsigned int leindx;
 	
	G4String fname = dirname + "/OrgantagOfInterest.txt";
	std::ifstream fin(fname);
	if(fin.good() != 1 ){
	   G4Exception("VHDDetectorConstruction:DefineMaterialsOfInterest()","",FatalErrorInArgument,G4String("Invalid file name: " + fname).c_str());
	}
 	fin >> Norgan;
 	for(G4int i=0; i < Norgan; i++)
 	{
 		fin >> letag;
 		leindx = Organtag2MatIndx[static_cast<unsigned int>(letag)];
 		MaterialsOfInterest.push_back(fOriginalMaterials[leindx]);
 	}
 	fin.close();
 }


void VHDDetectorConstruction::SetMultiSensDet_NestedParam(G4LogicalVolume* voxel_logic)
{
  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  G4String phantomSDname = "PhantomSD";

  MFDet = new G4MultiFunctionalDetector(phantomSDname);
  SDman->AddNewDetector(MFDet);             // Register SD to SDManager.
  voxel_logic->SetSensitiveDetector(MFDet);  // Assign SD to the logical volume.

  //==========================Total energy deposit scorer=========================================
  G4String psName;
  VHDPSEnergyDeposit_NestedParam* scorer0 = new VHDPSEnergyDeposit_NestedParam(psName="totalEDep",nVoxelX,nVoxelY,nVoxelZ);
  MFDet->RegisterPrimitive(scorer0);

  //--- Cell flux for photon or electron with energy bin
  //=======================read the energy bin file ==============================//
  G4double tmp;
  std::vector<G4double> engbin;

  G4String fname;
  if(ebin == 0)
	fname = dirname + "/Energybin1.txt";   //25 energy bins; use this energy bin when using the DRFs provided by Choonsik Lee
  else
	fname = dirname + "/Energybin2.txt";  //28 energy bins; use this energy bin when using the DRFs from Wayson et al.'s datafile for newborn phantom
  G4cout << "energybin fname = " << fname << G4endl;
  std::ifstream finDF(fname);
  if(finDF.good() != 1 )
  {
     G4Exception("VHDDetectorConstruction:SetMultiSensDet(G4LogicalVolume* voxel_logic)","",FatalErrorInArgument,G4String("Invalid file name: " + fname).c_str());
  }
  finDF >> NEngbin;
  for(G4int i = 0; i < NEngbin; i++ ){
    finDF >> tmp;
    engbin.push_back(tmp);
  }
  finDF.close();
  //=======================end of reading the energy bin file ==============================//


  //==================== Construct Cell Flux scorers for a number of energy bins============//
  G4double kmin,kmax;
  G4String filterName;
  for (unsigned i = 0; i < engbin.size(); i++){
      char name[17];
      std::sprintf(name,"PhotonCellFlux%02d",i);
      G4String psgName(name);
      if(i == 0)
	kmin = 0.0*keV;
      else
        kmin = engbin[i-1]*keV;
      kmax = engbin[i]*keV;

      //----Set up the energy filter for the CellFlux scorer to tally cellflux for the definied energy bins
      G4SDParticleWithEnergyFilter* pkinEFilter = new G4SDParticleWithEnergyFilter(filterName="photonE filter",kmin,kmax);
      if(electronflag)	pkinEFilter->add("e-");     //accept e- electron
      if(photonflag)	pkinEFilter->add("gamma");  // Accept gamma.
      pkinEFilter->show();        // Show accepting condition to stdout.
      
      
      //-----Cell Flux Scorer that store the total tracklength per volume --> per unit surface
      VHDMSDCellFlux_NestedParam* scorer = new VHDMSDCellFlux_NestedParam(psgName,nVoxelX,nVoxelY,nVoxelZ);
      scorer->SetMaterialsOfInterest(MaterialsOfInterest);  //define the material of interest
      scorer->Weighted(FALSE);
      scorer->SetFilter(pkinEFilter);    // Assign filter
      MFDet->RegisterPrimitive(scorer);  // Register it to MultiFunctionalDetector
  }
  engbin.clear();
  G4cout << "end of setting up the multifunctional detectors..." << G4endl;
}

void VHDDetectorConstruction::SetMultiSensDet_RegParam(G4LogicalVolume* voxel_logic)
{
  G4SDManager* SDman = G4SDManager::GetSDMpointer();
  G4String phantomSDname = "PhantomSD";

  MFDet = new G4MultiFunctionalDetector(phantomSDname);
  SDman->AddNewDetector(MFDet);             // Register SD to SDManager.
  voxel_logic->SetSensitiveDetector(MFDet);  // Assign SD to the logical volume.

  //==========================Total energy deposit scorer=========================================
  G4String psName;
  VHDPSEnergyDeposit_RegParam* scorer0 = new VHDPSEnergyDeposit_RegParam(psName="totalEDep",nVoxelX,nVoxelY,nVoxelZ);
  MFDet->RegisterPrimitive(scorer0);
 

  //--- Cell flux for photon or electron with energy bin
  //=======================read the energy bin file ==============================//
  G4double tmp;
  std::vector<G4double> engbin;

  G4String fname;
  if(ebin == 0)
	fname = dirname + "/Energybin1.txt";   //25 energy bins; use this energy bin when using the DRFs provided by Choonsik Lee
  else
	fname = dirname + "/Energybin2.txt";  //28 energy bins; use this energy bin when using the DRFs from Wayson et al.'s datafile for newborn phantom
  G4cout << "energybin fname = " << fname << G4endl;
  std::ifstream finDF(fname);
  if(finDF.good() != 1 )
  {
     G4Exception("VHDDetectorConstruction:SetMultiSensDet(G4LogicalVolume* voxel_logic)","",FatalErrorInArgument,G4String("Invalid file name: " + fname).c_str());
  }
  finDF >> NEngbin;
  for(G4int i = 0; i < NEngbin; i++ ){
    finDF >> tmp;
    engbin.push_back(tmp);
  }
  finDF.close();
  //=======================end of reading the energy bin file ==============================//


  //==================== Construct Cell Flux scorers for a number of energy bins============//
  G4double kmin,kmax;
  G4String filterName;
  for (unsigned i = 0; i < engbin.size(); i++){
      char name[17];
      std::sprintf(name,"PhotonCellFlux%02d",i);
      G4String psgName(name);
      if(i == 0)
	kmin = 0.0*keV;
      else
        kmin = engbin[i-1]*keV;
      kmax = engbin[i]*keV;

      //-- Particle with kinetic energy filter.
      G4SDParticleWithEnergyFilter* pkinEFilter = new G4SDParticleWithEnergyFilter(filterName="photonE filter",kmin,kmax);
      if(electronflag)	pkinEFilter->add("e-");     //accept e- electron
      if(photonflag)	pkinEFilter->add("gamma");  // Accept gamma.
      pkinEFilter->show();        // Show accepting condition to stdout.
      
      
      //-----Cell Flux Scorer that store the total tracklength per volume --> per unit surface
      VHDMSDCellFlux_RegParam* scorer = new VHDMSDCellFlux_RegParam(psgName,nVoxelX,nVoxelY,nVoxelZ);
      scorer->SetMaterialsOfInterest(MaterialsOfInterest);  //define the material of interest
      scorer->Weighted(FALSE);
      scorer->SetFilter(pkinEFilter);    // Assign filter
      MFDet->RegisterPrimitive(scorer);  // Register it to MultiFunctionalDetector
  }
  engbin.clear();
  G4cout << "end of setting up the multifunctional detectors..." << G4endl;
}







