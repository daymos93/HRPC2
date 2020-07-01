
#ifndef DetectorMessenger_h
#define DetectorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"

class DetectorConstruction;
class G4UIdirectory;
class G4UIcommand;
class G4UIcmdWithAString;
class G4UIcmdWithAnInteger;
class G4UIcmdWithADoubleAndUnit;
class G4UIcmdWithoutParameter;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class DetectorMessenger: public G4UImessenger
{
public:
  
  DetectorMessenger(DetectorConstruction* );
  virtual ~DetectorMessenger();
    
  virtual void SetNewValue(G4UIcommand*, G4String);
    
private:
  
  DetectorConstruction*      fDetector;
    
  G4UIdirectory*             fTestemDir;
  G4UIdirectory*             fDetDir;    
  G4UIcmdWithAString*        fMaterCmd;
  G4UIcmdWithAString*        fWMaterCmd;
  G4UIcmdWithADoubleAndUnit* fSizeXCmd;
  G4UIcmdWithADoubleAndUnit* fSizeYZCmd;    
  G4UIcmdWithADoubleAndUnit* fMagFieldCmd;
    
  G4UIcmdWithAnInteger*      fTalNbCmd;    
  G4UIcommand*               fTalDefCmd;
  G4UIcommand*               fTalPosiCmd;

  G4UIcmdWithADoubleAndUnit* fgapThicknessCMD;
  G4UIcmdWithADoubleAndUnit* ftrdRadioPositionCMD;
  G4UIcmdWithADoubleAndUnit* ftrdPhiPositionCMD;


};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

