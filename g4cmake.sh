#!/bin/bash
G4DIR=/usr/local/geant4/geant4.9.6.p02-install/lib/Geant4-9.6.2/
MODULEPATH=/usr/local/geant4/geant4.9.6.p02-install/lib/Geant4-9.6.2/Modules/
SRCDIR="../VoxelizedHumanDoseMultiSDv1"

cmake -DGeant4_DIR=$G4DIR -DCMAKE_MODULE_PATH=$MODULEPATH $SRCDIR
