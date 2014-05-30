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
//
/**
 * @file   VHDMSDEventAction.cc
 * @brief  event action definition
 *
 * @date   7th Aug 2013
 * @author Shih-ying Huang
 * @name   Geant4.9.6-p02
 */


#include "VHDMultiSDEventAction.hh"

#include "G4Event.hh"
#include "G4EventManager.hh"

VHDMultiSDEventAction::VHDMultiSDEventAction()
{
	evtID = 0;
}


VHDMultiSDEventAction::~VHDMultiSDEventAction()
{
	G4cout << "destroying VHDMultiSDEventAction..." << G4endl;
}


void VHDMultiSDEventAction::BeginOfEventAction(const G4Event* evt)
{
	evtID = evt->GetEventID();
}

//
void VHDMultiSDEventAction::EndOfEventAction(const G4Event* evt)
{
  // periodic printing
  //if (event_id < 100 || event_id%100 == 0) {
  if (evtID%1000 == 0) {
    G4cout << ">>> Event " << evtID << G4endl;
  }

}
