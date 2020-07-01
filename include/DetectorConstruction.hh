
#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4Trd.hh"
#include "G4RotationMatrix.hh"

class G4LogicalVolume;
class G4Material;
class G4UniformMagField;
class DetectorMessenger;

const G4int kMaxTally = 20;
//const G4int Nstrati = 41;						//costante per definire NUMERO DI STRATI (con int non funziona)
const G4int Nstrati = 35;						//without honeycomb panel



//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
  
  DetectorConstruction();
  virtual ~DetectorConstruction();

  void SetSizeX    (G4double);
  void SetSizeYZ   (G4double);              
  void SetMaterial (const G4String&);            
  void SetWorldMaterial (const G4String&);            
  void SetMagField (G4double);
     
  void SetTallyNumber   (G4int);     
  void SetTallySize     (G4int, const G4ThreeVector&);
  void SetTallyPosition (G4int, const G4ThreeVector&);

  virtual G4VPhysicalVolume* Construct();
     
  inline G4double GetWorldSizeX()  const {return fWorldSizeX;};
  inline G4double GetWorldSizeYZ() const {return fWorldSizeYZ;};
  inline G4double GetAbsorSizeX()  const {return fAbsorSizeX;};
  inline G4double GetAbsorSizeYZ() const {return fAbsorSizeYZ;};
  inline G4int    GetTallyNumber() const {return fTallyNumber;};

  //G4double GetRPCSizeYZ()   const {return fRPCSizeYZ;};
  void     SetGasGapThickness(const G4double thick) { fgapThickness = thick; }
  G4double GetGasGapThickness() const               { return fgapThickness;  }

  // Public methods to set/get the detector distance to (0,0,0)
  void     SetRPCRadioPos(const G4double distance) { ftrdRadioPosition = distance; }
  G4double GetRPCRadioPos() const               { return ftrdRadioPosition;  }

  // Public methods to set/get the detector angle
  void     SetRPCPhiPos(const G4double angle) { ftrdPhiPosition = angle; }
  G4double GetRPCPhiPos() const               { return ftrdPhiPosition;  }


  G4double GetAbsorMass() const;

  inline const G4Material* GetWorldMaterial() const {return fWorldMaterial;};
  inline const G4Material* GetAbsorMaterial() const {return fAbsorMaterial;};
     
  G4double GetTallyMass(G4int n) const;
  const G4LogicalVolume* GetLogicalTally(G4int n) const;
     
  void PrintParameters() const;
  
private:
  
  G4double            fWorldSizeX;
  G4double            fWorldSizeYZ;
  G4double            fAbsorSizeX;
  G4double            fAbsorSizeYZ;     

  G4Material*         fAbsorMaterial;
  G4Material*         fWorldMaterial;
  G4Material*		  fMatStrati[Nstrati];

  G4double			  fAbsorMass;

  G4UniformMagField*  fMagField;
  G4LogicalVolume*    fLAbsor;
  G4LogicalVolume*    fLWorld;
     
  G4int               fTallyNumber;                   
  G4ThreeVector       fTallySize[kMaxTally];
  G4double            fTallyMass[kMaxTally]; 
  G4ThreeVector       fTallyPosition[kMaxTally];
  G4LogicalVolume*    fLTally[kMaxTally];
     
  DetectorMessenger*  fDetectorMessenger;


  //G4double 		  	  minorBase; 		//dimensioni trapezio camera (area effettiva)
  //G4double 		      majorBase;
  //G4double 		      Height;
  G4double            fRPCSizeYZ;
  G4double 			  ftrdRadioPosition;
  G4double 			  ftrdPhiPosition;

  G4double			  fgapThickness;

  std::vector<G4Trd*>           trdCollection ;			//collezione del nome dei volumi
  std::vector<G4LogicalVolume*> trdLogCollection ;		//collezione dei volumi logici

  //per controllare che nessun volume sia sovrapposto ad un altro
  G4bool  fCheckOverlaps;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

