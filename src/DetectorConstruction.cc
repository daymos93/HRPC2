
#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"

#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4UniformMagField.hh"

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

#include "G4NistManager.hh"
#include "G4UnitsTable.hh"

#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4RunManager.hh" 

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

#include "G4VisAttributes.hh"

#include "G4RotationMatrix.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
  : G4VUserDetectorConstruction(), 
    fMagField(nullptr), 
    fLAbsor(nullptr),
    fLWorld(nullptr),
	minorBase(0),
	majorBase(0),
	Height(0),
	ftrdRPosition(0),
	ftrdThetaPosition(0)
	{
  // default parameter values
  fAbsorSizeX = 30*cm;
  fAbsorSizeYZ = 80*cm;
  fWorldSizeX = fWorldSizeYZ = 3*m;
  majorBase	=	584.1*mm;				//definizione valori dimensioni trapezio camera
  minorBase	=	866.3*mm;
  Height	=	1613.0*mm;
  ftrdRPosition	=	1000.0*mm;			//distancia detector to origin
  ftrdThetaPosition	=	90.0*deg;

  
  fTallyNumber = 0;
  for (G4int j=0; j<kMaxTally; j++) {
    fTallySize[j] = fTallyPosition[j] = G4ThreeVector(0.,0.,0.);
    fTallyMass[j]     = 0.;
    fLTally[j]        = nullptr; 
  }
  // create commands for interactive definition of the detector  
  fDetectorMessenger = new DetectorMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{ 
  delete fDetectorMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
	//
	// Define Elements
	//
	G4double z, a;
	G4NistManager* manager = G4NistManager::Instance();

	G4Element* H = new G4Element("Hydrogen", "H", z= 1, a= 1.008*g/mole);
	G4Element* N = new G4Element("Nitrogen", "N", z= 7, a= 14.01*g/mole);
	G4Element* O = new G4Element("Oxygen"  , "O", z= 8, a= 16.00*g/mole);
	G4Element* C  = manager->FindOrBuildElement("C");

	G4Element* F  = manager->FindOrBuildElement("F");
	G4Element* S  = manager->FindOrBuildElement("S");

	G4Material *Al = G4NistManager::Instance()->FindOrBuildMaterial("G4_Al") ;
	G4Material *Cu = G4NistManager::Instance()->FindOrBuildMaterial("G4_Cu") ;
	G4Material *Graphite = G4NistManager::Instance()->FindOrBuildMaterial("G4_GRAPHITE") ;
	G4Material *Air = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR") ;


	// gases at STP conditions
	G4Material* empty = manager->FindOrBuildMaterial("G4_Galactic");

	//
	// define Materials.
	//
	G4double density, temperature, pressure;
	G4int    ncomponents, natoms, numel;
	G4double fractionmass;

	G4Material* H2O =
			new G4Material("Water", density= 1.0*g/cm3, ncomponents=2);
	  	  H2O->AddElement(H, natoms=2);
	  	  H2O->AddElement(O, natoms=1);
	  	  H2O->GetIonisation()->SetMeanExcitationEnergy(78.0*eV);

	// In this line both G4_WATER and Water_1.05 will be constructed
	G4NistManager::Instance()->
			BuildMaterialWithNewDensity("Water_1.05","G4_WATER",1.05*g/cm3);

	//Air
	//G4Material* Air =
	//		  new G4Material("Air", density= 1.29*mg/cm3, numel=2);
	//Air->AddMaterial(G4NistManager::Instance()->FindOrBuildMaterial("G4_N"), fractionmass = 70.*perCent);
	//Air->AddMaterial(G4NistManager::Instance()->FindOrBuildMaterial("G4_O"), fractionmass = 30.*perCent);


	G4Material* vac =
			new G4Material( "TechVacuum", density     = 1.e-5*g/cm3, 1, kStateGas, temperature = STP_Temperature, pressure    = 2.e-2*bar );
	  	  vac->AddMaterial( Air, 1. );


	G4Material* vacuum =
			new G4Material("Galactic",z= 1,a= 1.008*g/mole,density     = universe_mean_density,
		  kStateGas,temperature,pressure    = 3.e-18*pascal);

	//TISSUE_SOFT
	G4Material* Tissue = G4NistManager::Instance()->FindOrBuildMaterial("G4_TISSUE_SOFT_ICRP");

	//Aluminum core (per honeycomb core) alluminio con densità inferiore
	G4Material* AlCore = G4NistManager::Instance()->BuildMaterialWithNewDensity("AlCore","G4_Al",density=0.5*g/cm3);

	//paper (cellulosa)
	G4Material* paper = new G4Material("paper" , density = 0.3*g/cm3, numel=3);
	paper->AddElement(C, natoms=6);
	paper->AddElement(H, natoms=10);
	paper->AddElement(O, natoms=5);

	//PET
	G4Material* PET = new G4Material("PET" , density = 1.37*g/cm3, numel=3);
	PET->AddElement(C, natoms=10);
	PET->AddElement(H, natoms=8);
	PET->AddElement(O, natoms=4);

	//High Pressure Laminate HPL
	G4Material* HPL = new G4Material("HPL", density= 1.4*g/cm3, numel=3);
	HPL->AddMaterial(G4NistManager::Instance()->FindOrBuildMaterial("G4_H"), fractionmass = 5.74*perCent);
	HPL->AddMaterial(G4NistManager::Instance()->FindOrBuildMaterial("G4_C"), fractionmass = 77.46*perCent);
	HPL->AddMaterial(G4NistManager::Instance()->FindOrBuildMaterial("G4_O"), fractionmass = 16.8*perCent);

	//EVA (Etilene vinil acetato)
//-Etilene
	G4Material* etylene = new G4Material("etylene", density= 1.2612*g/cm3, numel=2);
	etylene->AddElement(C, natoms=2);
	etylene->AddElement(H, natoms=4);

// vinyl acetato
	G4Material* vinyl_acetato = new G4Material("vinyl_acetato", density= 0.93*g/cm3, numel=3);
	vinyl_acetato->AddElement(C, natoms=4);
	vinyl_acetato->AddElement(H, natoms=6);
	vinyl_acetato->AddElement(O, natoms=2);

//EVA
	G4Material* EVA = new G4Material("EVA", density= 0.934*g/cm3, numel=2);
	EVA->AddMaterial(etylene,fractionmass = 10* perCent);
	EVA->AddMaterial(vinyl_acetato,fractionmass = 90* perCent);

	// RPC mixture gas components
// iso-Butane (methylpropane), STP
	G4Material* isobutane = new G4Material("isoC4H10", density = 2.67*mg/cm3, numel=2) ;
	isobutane->AddElement(C,4);
	isobutane->AddElement(H,10);

// Freon
	G4Material* C2H2F4 = new G4Material("Freon", density = 4.55*mg/cm3, numel=3) ;
	C2H2F4->AddElement(C, natoms=2);
	C2H2F4->AddElement(H, natoms=2);
	C2H2F4->AddElement(F, natoms=4);

// esafluoruro di zolfo SF6
	G4Material* SF6 = new G4Material("SF6", density = 6.14*mg/cm3, numel=2) ;
	SF6->AddElement(S, natoms=1);
	SF6->AddElement(F, natoms=6);

//mixure RPC
	G4Material* RPCgas = new G4Material("RPCGas", density = 3.569*mg/cm3, numel=3) ;
	RPCgas->AddMaterial(isobutane, fractionmass = 4.5* perCent) ;
	RPCgas->AddMaterial(C2H2F4, fractionmass = 95.2* perCent) ;
	RPCgas->AddMaterial(SF6, fractionmass = 0.3* perCent) ;

	G4Material* MatStrati[Nstrati] =				//array materiali
	{

	  		Air,  								//falso layer di aria
	  		Al, AlCore ,Al,								//honeycomb panel  TOP
	  		PET,								//pet insulation TOP
	  		paper,								//layer paper

	  		Cu, PET,PET, EVA, Graphite, HPL,				//"Copper1_1", "PET1_1","PET1_2", "EVA1_1","graphite1_1","HPL1_1"
	  		RPCgas,								//GASGAP 1
	  		HPL, Graphite, EVA, PET, PET, Cu,				//"HPL1_2","graphite1_2", "EVA1_2", "PET1_3","PET1_4","Copper1_2"

	  		PET, Cu, PET,							//"PETstrip1","CuStrip", "PETstrip2"

	  		Cu, PET, PET, EVA, Graphite, HPL,				//"Copper2_1", "PET2_1","PET2_2", "EVA2_1","graphite2_1","HPL2_1"
	  		RPCgas,								//GASGAP 2
	  		HPL, Graphite, EVA, PET, PET, Cu,				// HPL2_2

	  		PET,								//pet insulation BOT
	  		HPL,								//bakelite layer
	  		Al, AlCore, Al,								//honeycomb panel BOTTOM
	  		Air,								//falso layer di aria

	};

	//default materials
	fAbsorMaterial = Tissue;
	fWorldMaterial = Air;

  // WORLD
  //
  G4Box*
  sWorld = new G4Box("World",                                      //name
                   fWorldSizeX/2,fWorldSizeYZ/2,fWorldSizeYZ/2);   //dimensions

  fLWorld = new G4LogicalVolume(sWorld,                        //shape
                                fWorldMaterial,                //material
                                "World");                      //name

  G4VPhysicalVolume*                                   
  pWorld = new G4PVPlacement(0,                           //no rotation
                             G4ThreeVector(0.,0.,0.),     //at (0,0,0)
                             fLWorld,                     //logical volume
                             "World",                     //name
                             0,                           //mother  volume
                             false,                       //no boolean operation
                             0);                          //copy number
  //                           
  // ABSORBER
  //                           
  G4Box*
  sAbsor = new G4Box("Absorber",                                 //name
                   fAbsorSizeX/2,fAbsorSizeYZ/2,fAbsorSizeYZ/2); //dimensions
                                                                 
  fLAbsor = new G4LogicalVolume(sAbsor,                   //shape
                                fAbsorMaterial,           //material
                                "Absorber");              //name
  
                              
  new G4PVPlacement(0,                           //no rotation
                    G4ThreeVector(0.,0.,0.),     //at (0,0,0)
                    fLAbsor,                     //logical volume
                    "Absorber",                  //name
                    fLWorld,                     //mother  volume
                    false,                       //no boolean operation
                    0);                          //copy number
  //
  // TALLIES (optional)
  //
  if (fTallyNumber > 0) {
    for (G4int j=0; j<fTallyNumber; ++j) {
            
       G4Box* sTally = new G4Box("Tally",
                   fTallySize[j].x()/2,fTallySize[j].y()/2,fTallySize[j].z()/2);
                      
       fLTally[j] = new G4LogicalVolume(sTally,fAbsorMaterial,"Tally");
           
       new G4PVPlacement(0,                        //no rotation
                         fTallyPosition[j],        //position
                         fLTally[j],               //logical volume
                         "Tally",                  //name
                         fLAbsor,                  //mother  volume
                         false,                    //no boolean operation
                         j+1);                     //copy number
       
      fTallyMass[j] = fTallySize[j].x()*fTallySize[j].y()*fTallySize[j].z()
               *(fAbsorMaterial->GetDensity());
    }               
  } 

  //TRAPEZI CAMERA (UNO DI SEGUITO ALL'ALTRO),   RE4-2 KODEL DG 1.4 1.4
  std::string NomeStrati[Nstrati]= 					//array nome strati
  {

  		"fakeTop",  								//falso layer di aria
  		"Honeycomb_skin1_1", "Honeycomb_core1","Honeycomb_skin1_2",		//honeycomb panel  TOP
  		"PET_insulationTOP",							//pet insulation TOP
  		"paper",								//layer paper

  		"Copper1_1", "PET1_1","PET1_2", "EVA1_1","graphite1_1","HPL1_1",	//HPL1_1
  		"GasGap1",								//GASGAP 1
  		"HPL1_2","graphite1_2", "EVA1_2", "PET1_3","PET1_4","Copper1_2",	// HPL1_2

  		"PETstrip1","Copper_Strip", "PETstrip2",					//STRIP

  		"Copper2_1", "PET2_1","PET2_2", "EVA2_1","graphite2_1","HPL2_1",	//HPL2_1
  		"GasGap2",								//GASGAP 2
  		"HPL2_2","graphite2_2", "EVA2_2", "PET2_3","PET2_4","Copper2_2",	// HPL2_2

  		"PET_insulationBOT",							//pet insulation BOT
  		"Bakelite",								//bakelite layer
  		"Honeycomb_skin2_1", "Honeycomb_core2","Honeycomb_skin2_2",		//honeycomb panel BOTTOM
  		"fakeBottom",								//falso layer di aria

  };

  std::string NomeStratiLog[Nstrati];				//ciclo per aggiungere Log a nome strati

  for(size_t A=0; A<Nstrati; A++) {
	  NomeStratiLog[A]=NomeStrati[A]+"Log";
  }

  G4double spessoreStrati[Nstrati]=				//array spessori
  {

  		0.01*mm,								//falso layer di aria
  		0.5*mm,	5.0*mm, 0.5*mm,						//"Honeycomb_skin1_1", "Honeycomb_core1","Honeycomb_skin1_2"
  		0.188*mm,							//pet insulation TOP
  		1.0*mm,								//layer paper

  		0.38*mm, 0.18*mm, 0.18*mm, 0.15*mm, 0.001*mm, 1.35*mm,	      	//"Copper1_1", "PET1_1","PET1_2", "EVA1_1","graphite1_1","HPL1_1"
  		1.0*mm,								//GASGAP 1
  		1.35*mm, 0.001*mm, 0.15*mm, 0.18*mm, 0.18*mm,0.38*mm,		//"HPL1_2","graphite1_2", "EVA1_2", "PET1_3","PET1_4","Copper1_2"

  		0.2*mm, 0.17*mm, 0.2*mm,					//"PETstrip1","CuStrip", "PETstrip2"

  		0.38*mm, 0.18*mm, 0.18*mm, 0.15*mm, 0.001*mm, 1.35*mm,		//"Copper2_1", "PET2_1","PET2_2", "EVA2_1","graphite2_1","HPL2_1"
  		1.0*mm,								//GASGAP 2
  		1.35*mm, 0.001*mm, 0.15*mm, 0.18*mm, 0.18*mm,0.38*mm,		//"HPL2_2","graphite2_2", "EVA2_2", "PET2_3","PET2_4","Copper2_2"

  		0.188*mm,							//pet insulation BOT
  		2.0*mm,								//bakelite layer
  		0.5*mm,	5.0*mm, 0.5*mm,						//"Honeycomb_skin2_1", "Honeycomb_core2","Honeycomb_skin2_2"
  		0.01*mm,							//falso layer di aria

  };

  //----- VISUALIZZAZIONE ATTRIBUTI (colore layer)
  // Visual attributes
  G4VisAttributes *electrodeAttributes = new G4VisAttributes(G4Color::Grey()) ;
  //cathodeAttributes->SetForceWireframe(true) ;
  G4VisAttributes *gasAttributes = new G4VisAttributes(G4Color::White()) ;
  //g10Attributes->SetForceWireframe(true) ;
  G4VisAttributes *petAttributes = new G4VisAttributes(G4Color::Blue()) ;
  //gasAttributes->SetForceWireframe(true) ;
  G4VisAttributes *evaAttributes = new G4VisAttributes(G4Color::Red()) ;
  //gemAttributes->SetForceWireframe(true) ;
  G4VisAttributes *cuAttributes = new G4VisAttributes(G4Color::Yellow()) ;
  //insAttributes->SetForceWireframe(true) ;
  G4VisAttributes *honeyAttributes = new G4VisAttributes(G4Color::Green()) ;
  //insAttributes->SetForceWireframe(true) ;
  G4VisAttributes *graphiteAttributes = new G4VisAttributes(G4Color::Magenta()) ;
  //insAttributes->SetForceWireframe(true) ;
  G4VisAttributes *fakeAttributes = new G4VisAttributes(G4Color::Cyan()) ;
  //insAttributes->SetForceWireframe(true) ;


  //VOLUMI LOGICI
  std::string HPLstring="HPL";					//stringhe per controllo e assegnazione colore a volumi
  std::string Bakelitestring="Bakelite";
  std::string GasGapstring="GasGap";
  std::string PETstring="PET";
  std::string EVAstring="EVA";
  std::string Copperstring="Copper";
  std::string Honeycombstring="Honeycomb";
  std::string Graphitestring="graphite";
  std::string Fakestring="fake";

  for(G4int lyr=0;lyr<Nstrati;lyr++){				//loop per assegnare a ogni nome strato il proprio volume logico e forma e spessore
	  	  G4Trd* strato= new G4Trd(NomeStrati[lyr],spessoreStrati[lyr]/2, spessoreStrati[lyr]/2,majorBase/2, majorBase/2, Height/2); //per invertire posizione base maggiore e minore basta invertirli nell'inserimento
	  	  G4LogicalVolume* logicStrato = new G4LogicalVolume(strato, MatStrati[lyr], NomeStratiLog[lyr]) ;

	  	  if (NomeStrati[lyr].find(HPLstring)==0 or NomeStrati[lyr].find(Bakelitestring)==0) {  		//ciclo per settare colore volumi
			logicStrato->SetVisAttributes(new G4VisAttributes(*electrodeAttributes));}


	  	  else if (NomeStrati[lyr].find(GasGapstring)==0) {
			logicStrato->SetVisAttributes(new G4VisAttributes(*gasAttributes));}


	  	  else if (NomeStrati[lyr].find(PETstring)==0) {
			logicStrato->SetVisAttributes(new G4VisAttributes(*petAttributes));}


	  	  else if (NomeStrati[lyr].find(EVAstring)==0) {
			logicStrato->SetVisAttributes(new G4VisAttributes(*evaAttributes));}


	  	  else if (NomeStrati[lyr].find(Copperstring)==0) {
			logicStrato->SetVisAttributes(new G4VisAttributes(*cuAttributes));}


	  	  else if (NomeStrati[lyr].find(Honeycombstring)==0) {
			logicStrato->SetVisAttributes(new G4VisAttributes(*honeyAttributes));}


	  	  else if (NomeStrati[lyr].find(Graphitestring)==0) {
			logicStrato->SetVisAttributes(new G4VisAttributes(*graphiteAttributes));}


	  	  else if (NomeStrati[lyr].find(Fakestring)==0) {
			logicStrato->SetVisAttributes(new G4VisAttributes(*fakeAttributes));}




		trdCollection.push_back(strato) ;				//mette forma volume nel vettore
		trdLogCollection.push_back(logicStrato) ;		     	//mette ogni volume logico nel vettore
	}
  //VOLUMI FISICI
  // Rotation Matrix for layers   (ruota elementi disegnati)
  G4RotationMatrix* rotationPlacement = new G4RotationMatrix();
  rotationPlacement->rotateX(ftrdThetaPosition+90*deg) ;		//rotazione asse x, per avere asse trapezio verticale
  rotationPlacement->rotateY(90*deg) ;		//ruota asse y, per avere x larghezza e z spessore
  rotationPlacement->rotateZ(90*deg) ;


  //-- variabile per shift rispetto a world

  G4cout << "----------------------RE4-2 KODEL ------------------------------------" << G4endl ;

  G4double ZTranslation= ftrdRPosition*cos(ftrdThetaPosition);	//posizione di partenza su asse z da dove posizionare primo layer
  G4double YTranslation= ftrdRPosition*sin(ftrdThetaPosition);


  G4int cpN=1;		// indice numero strato per stampare a video informazioni

  for(size_t i=0 ; i<trdCollection.size() ; i++) {   		// i counts as the copyNo per loop

	ZTranslation += trdCollection.at(i)->GetXHalfLength1() ;//trasla di mezzo spessore(volume posizionato metà prima e metà dopo)(usa il segno - xchè upstream z negativa)
	G4double thickness=trdCollection.at(i)->GetXHalfLength1()*2 ;		//prende spessore del volume per stamparlo a video

    G4cout << "Volume (" << cpN << ") " << trdCollection.at(i)->GetName() << " The thickness is " << thickness << " mm" << " The mean Z position is " << G4BestUnit(ZTranslation,"Length") << " And the start Z thickness position is " << G4BestUnit((ZTranslation-(thickness/2)),"Length") << G4endl ;

    G4VPhysicalVolume* trapezoidalPhysic = new G4PVPlacement(		//crea volume fisico
	  rotationPlacement,							//rotazione
	  G4ThreeVector(0,YTranslation,ZTranslation),					//vettore posizione, varia solo posizione in Z (distanza sorgente)
	  trdLogCollection.at(i),						//nome del volume logico
	  trdCollection.at(i)->GetName(),					//nome volume fisico
	  fLWorld,								//volume madre in cui posizionare
	  false,								//pmany
	  cpN,									//copia numero cpN
	  fCheckOverlaps) ;							// controllo overlap (se attivato all'inizio file)

     ZTranslation += trdCollection.at(i)->GetXHalfLength1() ;			// incremento traslazione della seconda metà dello spessore
     cpN++;								//incremento indice layer  (si usa il segno - perchè upstream convenzione z è negativa)
  }

G4cout << "-------------------------------------------------------------------------" << G4endl ;


  PrintParameters();
    
  //
  //always return the World volume
  //  
  return pWorld;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::PrintParameters() const
{
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
  G4cout << "\n---------------------------------------------------------\n";
  G4cout << "---> The Absorber is " << G4BestUnit(fAbsorSizeX,"Length")
         << " of " << fAbsorMaterial->GetName() << G4endl;
  G4cout << "\n---------------------------------------------------------\n";
  
  if (fTallyNumber > 0) {
    G4cout << "---> There are " << fTallyNumber << " tallies : " << G4endl;    
    for (G4int j=0; j<fTallyNumber; ++j) {
      G4cout << "fTally " << j << ": "
             << fAbsorMaterial->GetName()
             << ",  mass = " << G4BestUnit(fTallyMass[j],"Mass")
             << " size = "   << G4BestUnit(fTallySize[j],"Length")           
             << " position = " << G4BestUnit(fTallyPosition[j],"Length")
             << G4endl;
    }                 
    G4cout << "\n---------------------------------------------------------\n";
  }  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetSizeX(G4double value)
{
  fAbsorSizeX = value; 
  fWorldSizeX = 3*m;
}
  
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetSizeYZ(G4double value)
{
  fAbsorSizeYZ = value; 
  fWorldSizeYZ = 3*m;
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetMaterial(const G4String& materialChoice)
{
  // search the material by its name   
  G4Material* pttoMaterial =
    G4NistManager::Instance()->FindOrBuildMaterial(materialChoice);
  if (pttoMaterial && pttoMaterial != fAbsorMaterial) {
    // change target material everywhere
    fAbsorMaterial = pttoMaterial;
    for (G4int j=0; j<fTallyNumber; ++j) {
      if(fLTally[j]) { 
        fLTally[j]->SetMaterial(pttoMaterial); 
        fTallyMass[j] = fTallySize[j].x()*fTallySize[j].y()*fTallySize[j].z()
          *(pttoMaterial->GetDensity());
      }
    } 
    if(fLAbsor) {
      fLAbsor->SetMaterial(fAbsorMaterial);
      G4RunManager::GetRunManager()->PhysicsHasBeenModified();
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetWorldMaterial(const G4String& materialChoice)
{
  // search the material by its name   
  G4Material* pttoMaterial =
    G4NistManager::Instance()->FindOrBuildMaterial(materialChoice);
  if (pttoMaterial && pttoMaterial != fWorldMaterial) {
    fWorldMaterial = pttoMaterial;
    if(fLWorld) {
      fLWorld->SetMaterial(fAbsorMaterial);
      G4RunManager::GetRunManager()->PhysicsHasBeenModified();
    }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetMagField(G4double fieldValue)
{
  //apply a global uniform magnetic field along Z axis
  G4FieldManager* fieldMgr 
   = G4TransportationManager::GetTransportationManager()->GetFieldManager();
    
  if (fMagField) delete fMagField;        //delete the existing magn field  

  if (fieldValue!=0.)                        // create a new one if non nul
    {
      fMagField = new G4UniformMagField(G4ThreeVector(0.,0.,fieldValue));
      fieldMgr->SetDetectorField(fMagField);
      fieldMgr->CreateChordFinder(fMagField);
    }
   else
    {
      fMagField = nullptr;
      fieldMgr->SetDetectorField(fMagField);
    }
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetTallyNumber(G4int value)
{
  if(value >= 0 && value <kMaxTally) {
    fTallyNumber = value;
  } else {
    G4cout << "### DetectorConstruction::SetTallyNumber WARNING: wrong tally "
           << "number " << value << " is ignored" << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetTallySize(G4int j, const G4ThreeVector& value)
{
  if(j >= 0 && j < kMaxTally) {
    fTallySize[j] = value;
  } else {
    G4cout << "### DetectorConstruction::SetTallyNumber WARNING: wrong tally "
           << "number " << j << " is ignored" << G4endl;
  } 
}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void DetectorConstruction::SetTallyPosition(G4int j, const G4ThreeVector& value)
{
  if(j >= 0 && j < kMaxTally) {
    fTallyPosition[j] = value; 
  } else {
    G4cout << "### DetectorConstruction::SetTallyPosition WARNING: wrong tally "
           << "number " << j << " is ignored" << G4endl;
  } 
}  

G4double DetectorConstruction::GetTallyMass(G4int j) const
{
  if(j >= 0 && j < kMaxTally) {
    return fTallyMass[j];
  } else {
    G4cout << "### DetectorConstruction::GetTallyMass WARNING: wrong tally "
           << "number " << j << " is ignored" << G4endl;
    return 0.0;
  } 
}

const G4LogicalVolume* DetectorConstruction::GetLogicalTally(G4int j) const 
{
  if(j >= 0 && j < kMaxTally) {
    return fLTally[j];
  } else {
    G4cout << "### DetectorConstruction::GetLOgicalTally WARNING: wrong tally "
           << "number " << j << " is ignored" << G4endl;
    return nullptr;
  } 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
