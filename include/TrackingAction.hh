
#ifndef TrackingAction_h
#define TrackingAction_h 1

#include "G4UserTrackingAction.hh"
#include "globals.hh"

class DetectorConstruction;
class RunAction;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class TrackingAction : public G4UserTrackingAction {

public:  
  TrackingAction(DetectorConstruction*, RunAction*);
  virtual ~TrackingAction();
   
  virtual void PostUserTrackingAction(const G4Track*);
    
private:
  DetectorConstruction* fDetector; 
  RunAction*            fRunAction;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
