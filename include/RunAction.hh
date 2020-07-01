
#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include "g4root.hh"
//#include "g4xml.hh"
#include "HistoManager.hh"

class DetectorConstruction;
class PhysicsList;
class PrimaryGeneratorAction;

class G4Run;
class HistoManager;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class RunAction : public G4UserRunAction
{
public:

  RunAction(DetectorConstruction*,/* PhysicsList*,*/ PrimaryGeneratorAction*);
  virtual ~RunAction();

  virtual void BeginOfRunAction(const G4Run*);
  virtual void   EndOfRunAction(const G4Run*);
    
  inline void FillTallyEdep(G4int n, G4double e);
  inline void FillEdep(G4double de, G4double eni);
  inline void AddProjRange (G4double x);
  inline void AddPrimaryStep();
                   
private:  
    
  //void BookHisto();
  HistoManager*			  histo;
  //G4AnalysisManager*      fAnalysisManager;
  DetectorConstruction*   fDetector;
  //PhysicsList*            fPhysics;
  PrimaryGeneratorAction* fKinematic;
  G4double*               fTallyEdep;   
  G4double                fProjRange, fProjRange2;
  G4double                fEdeptot, fEniel;
  G4int                   fNbPrimarySteps;
  G4int                   fRange;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void RunAction::FillTallyEdep(G4int n, G4double e)  
{
  fTallyEdep[n] += e;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
  
inline void RunAction::FillEdep(G4double de, G4double eni) 
{
  fEdeptot += de; 
  fEniel += eni;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
    
inline void RunAction::AddProjRange (G4double x) 
{
  fProjRange  += x; 
  fProjRange2 += x*x; 
  ++fRange;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void RunAction::AddPrimaryStep() 
{
  ++fNbPrimarySteps;
}
                   

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

