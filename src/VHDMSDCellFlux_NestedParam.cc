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
 * @file   VHDMSDCellFlux.cc
 * @brief  score cell flux similar to G4PSCellFlux class with modification of ProcessHits(...) with voxel definitions defined according to NestedParamVHDDetectorConstruction
 *
 * @date   7th Aug 2013
 * @author Shih-ying Huang
 * @name   Geant4.9.6-p02
 */
 
 #include "VHDMSDCellFlux_NestedParam.hh"
 #include "G4Track.hh"
 #include "G4SystemOfUnits.hh"
 #include "G4VSolid.hh"
 #include "G4VPVParameterisation.hh"
 #include "G4UnitsTable.hh"


 VHDMSDCellFlux_NestedParam::VHDMSDCellFlux_NestedParam(G4String name,G4int nx, G4int ny, G4int nz,G4int depth):G4VPrimitiveScorer(name,depth),HCID(-1),weighted(TRUE),fNx(nx),fNy(ny),fNz(nz)
 {
 	fNxNy = fNx*fNy;
 	DefineUnitAndCategory();
 	SetUnit("percm2");  //default unit if cm-2
 }
 
 VHDMSDCellFlux_NestedParam::VHDMSDCellFlux_NestedParam(G4String name, const G4String& unit,G4int depth):G4VPrimitiveScorer(name,depth),HCID(-1),weighted(TRUE)
 {
 	DefineUnitAndCategory();
 	SetUnit(unit);  //set a preferred unit to use!
 }
 
 VHDMSDCellFlux_NestedParam::~VHDMSDCellFlux_NestedParam()
 {
 	if(filter){  //'filter' is a private variable that is definied in G4VPrimitiveScorer
		delete filter;  //delete it if it's defined
	}
	
	MaterialsOfInterest.clear();
  	
	G4cout << "destroying VHDMSDCellFlux..." << G4endl;
 }
 
 G4bool VHDMSDCellFlux_NestedParam::ProcessHits(G4Step* aStep,G4TouchableHistory*)
 {	
 	G4double steplen = aStep->GetStepLength();
 	if(steplen == 0.)	return FALSE;
 	
 	if(!MaterialsOfInterest.empty()){
		//use String comparison when tallying ==> can slow down CPU performance
		//Do the following when steplen > 0
		G4Material* lemat = aStep->GetPreStepPoint()->GetMaterial();
		std::vector<G4Material*>::iterator itr;
		itr = find(MaterialsOfInterest.begin(),MaterialsOfInterest.end(),lemat); 
		if(itr != MaterialsOfInterest.end())
		{ 	
			//tally cell flux!! it's one of the material of interest!
			G4int idx = ((G4TouchableHistory*)(aStep->GetPreStepPoint()->GetTouchable()))->GetReplicaNumber(indexDepth);
			G4double cubicVolume = ComputeVolume(aStep,idx);
			G4double CellFlux = steplen/cubicVolume;
			if(weighted)	CellFlux *= aStep->GetPreStepPoint()->GetWeight();
			G4int index = GetIndex(aStep);
			EvtMap->add(index,CellFlux);
			return TRUE;
		}
	}
	else
	{
		//G4cout << "This hit/step is not in one of the materials of interest!" << G4endl;
		return FALSE;
	}
	
	return FALSE;
 }
 
 void VHDMSDCellFlux_NestedParam::Initialize(G4HCofThisEvent* HCE)
 {
 	EvtMap = new G4THitsMap<G4double>(detector->GetName(),GetName());
 	if(HCID < 0)	HCID = GetCollectionID(0);
 	HCE->AddHitsCollection(HCID,EvtMap);
 }
 
 void VHDMSDCellFlux_NestedParam::EndOfEvent(G4HCofThisEvent*)
 {;}
 
 
 void VHDMSDCellFlux_NestedParam::clear()
 {
 	EvtMap->clear();
 }
 
 void VHDMSDCellFlux_NestedParam::DrawAll()
 {;}
 
 void VHDMSDCellFlux_NestedParam::PrintAll()
 {
 	G4cout << "MultiFunctionalDet " << detector->GetName() << G4endl;
 	G4cout << "PrimitiveScorer " << GetName() << G4endl;
 	G4cout << "Number of entries " << EvtMap->entries() << G4endl;
 	std::map<G4int,G4double*>::iterator itr = EvtMap->GetMap()->begin();
 	for(; itr != EvtMap->GetMap()->end(); itr++)
 	{
 		G4cout << " copy no.: " << itr->first << " cell flux: " << *(itr->second)/GetUnitValue() << " [" << GetUnit() << "]" << G4endl;
 	}
 }
 
 
 void VHDMSDCellFlux_NestedParam::SetUnit(const G4String& unit)
 {
 	CheckAndSetUnit(unit,"Per Unit Surface");
 }
 
 void VHDMSDCellFlux_NestedParam::DefineUnitAndCategory()
 {
 	//per unit surface
 	new G4UnitDefinition("percentimeter2","percm2","Per Unit Surface",(1./cm2));
 	new G4UnitDefinition("permillimeter2","permm2","Per Unit Surface",(1./mm2));
 	new G4UnitDefinition("permeter2","perm2","Per Unit Surface",(1./m2));
 }
 
 
 G4double VHDMSDCellFlux_NestedParam::ComputeVolume(G4Step* aStep, G4int idx)
 {
 	G4VPhysicalVolume* physVol = aStep->GetPreStepPoint()->GetPhysicalVolume();
 	G4VPVParameterisation* physParam = physVol->GetParameterisation();
 	G4VSolid* solid = 0;
 	if(physParam)
 	{  //for parameterised volume
 		if(idx < 0)
 		{
 			G4Exception("VHDMSDCellFlux","VHDMSDCellFlux::ComputeVolume",JustWarning,"Incorrect replica number");
 			G4cerr << " ------- GetReplicaNumber : " << idx << G4endl;
 		}	
 		solid = physParam->ComputeSolid(idx,physVol);
 		solid->ComputeDimensions(physParam,idx,physVol);
 	}
 	else
 	{  //for ordinary volume
 		solid = physVol->GetLogicalVolume()->GetSolid();
 	}
 	return solid->GetCubicVolume();
 }
 
 
 G4int VHDMSDCellFlux_NestedParam::GetIndex(G4Step* aStep) 
 {
 	const G4VTouchable* touchable = aStep->GetPreStepPoint()->GetTouchable();
 	G4int ix = touchable->GetReplicaNumber(1);
 	G4int iy = touchable->GetReplicaNumber(2);
 	G4int iz = touchable->GetReplicaNumber(0);
 	return ix + iy*fNx + iz*fNxNy;
 }
 
