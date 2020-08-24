
#include "TrackingAction.hh"
#include "DetectorConstruction.hh"
#include "RunAction.hh"
#include "G4Track.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TrackingAction::TrackingAction(DetectorConstruction* det, RunAction* run)
:G4UserTrackingAction(),fDetector(det), fRunAction(run)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

TrackingAction::~TrackingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void TrackingAction::PostUserTrackingAction(const G4Track* track)
{
  // extract Projected Range of primary particle
  if (track->GetTrackID() == 1) {
    G4double x = track->GetPosition().x() + 0.5*fDetector->GetAbsorSizeX();
    if(x > 0.0) fRunAction->AddProjRange(x);
    G4AnalysisManager::Instance()->FillH1(3, x);
  }  

  ////// //////		Secondary particle produced		////// ///////
  /*G4String volName = track->GetTouchable()->GetVolume()->GetName();
  G4int parentID = track->GetParentID();
  G4double totalEdep = track->GetTotalEnergy();
  if ( (volName == "World") && (parentID != 0) && (totalEdep > 0) ) {
  	G4String particleName = track->GetParticleDefinition()->GetParticleName();
 	if (particleName == "neutron") { G4AnalysisManager::Instance()->FillH1(4,totalEdep);}
 	if (particleName == "gamma") { G4AnalysisManager::Instance()->FillH1(5,totalEdep);}
  }*/


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

