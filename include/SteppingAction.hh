
#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"
#include "HistoManager.hh"

class DetectorConstruction;
class RunAction;
class HistoManager;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class SteppingAction : public G4UserSteppingAction
{
  public:
    SteppingAction(DetectorConstruction*, RunAction*);
    virtual ~SteppingAction();

    virtual void UserSteppingAction(const G4Step*);
    
    /*struct particleData{
    	G4String	pName;
    	G4int 		p_counter;
    	G4int 		ID; 	//ID particelle
    	G4int 		PID; 	//PID parent ID particelle
    	G4int 		PDG;	//PDG particelle
    	G4double 	Ek;		//Energia cinetica
    	G4double 	x;		//posizione sulla superficie
		G4double 	y;
    	G4double 	z;
    };*/

  private:
    DetectorConstruction* fDetector;
    RunAction*            fRunAction;
    HistoManager*		  histo;
    G4double edep;

    G4int p_incident, p_incident_top, p_incident_bot, p_charge, p_charge_gap1, p_charge_gap2, p_gap, p_gap_top, p_gap_bot;			// variabile 0/1  particelle incidenti sia bot che top

    //energia SOLO dei fotoni incidenti che poi producono segnale
    //G4double Ek_in_signal;
    //G4int in_signal_status;

    HistoManager::particleData 	particle_incident_TOT, particle_incident_TOP, particle_incident_BOT;
    HistoManager::particleData 	particle_gap_TOT, particle_gap_TOP, particle_gap_BOT;
	//HistoManager::particleChargeData particle_charge_TOT, particle_charge_Gap1, particle_charge_Gap2;
	//HistoManager::particleKODELData particleKODELStep;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif
