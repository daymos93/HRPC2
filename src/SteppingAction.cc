
#include "SteppingAction.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "DetectorConstruction.hh"
#include "RunAction.hh"
#include "Randomize.hh"
#include "RunAction.hh"

#include "HistoManager.hh"
#include "G4UnitsTable.hh"
#include "G4Track.hh"
#include "G4RunManager.hh"
#include "HistoManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction(DetectorConstruction* det, RunAction* RuAct)
:G4UserSteppingAction(),fDetector(det), fRunAction(RuAct),  histo(), edep(0),
 p_incident{0}, p_incident_top{0}, p_incident_bot{0},
 p_gap{0}, p_gap_top{0}, p_gap_bot{0},
 //p_charge{0}, p_charge_gap1{0}, p_charge_gap2{0},
 //in_signal_status{0}, Ek_in_signal{0},
 particle_incident_TOT(), particle_incident_TOP(), particle_incident_BOT(),
 particle_gap_TOT(), particle_gap_TOP(), particle_gap_BOT()
 //particle_charge_TOT(), particle_charge_Gap1(), particle_charge_Gap2(),
 //particleKODELStep()
 { }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::~SteppingAction()
{ }

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step* step)
{
  edep = step->GetTotalEnergyDeposit();
  //if (edep <= 0.) return;

  G4StepPoint* prePoint  = step->GetPreStepPoint();
  G4StepPoint* postPoint = step->GetPostStepPoint();

  G4int copyNb = prePoint->GetTouchableHandle()->GetCopyNumber();
  if (copyNb > 0) { fRunAction->FillTallyEdep(copyNb-1, edep); }

  G4double niel = step->GetNonIonizingEnergyDeposit();
  fRunAction->FillEdep(edep, niel);
  
  if (step->GetTrack()->GetTrackID() == 1) {
    fRunAction->AddPrimaryStep();
    /*
    G4cout << step->GetTrack()->GetMaterial()->GetName()
           << "  E1= " << step->GetPreStepPoint()->GetKineticEnergy()
           << "  E2= " << step->GetPostStepPoint()->GetKineticEnergy()
           << " Edep= " << edep 
           << " Q= " << step->GetTrack()->GetDynamicParticle()->GetCharge()
           << " Qp= " << step->GetPostStepPoint()->GetCharge()
           << G4endl;
    */
  } 

  G4AnalysisManager* analysisManager = G4AnalysisManager::Instance();
  if (edep > 0.) {
	  //Bragg curve
	  //
	  G4double xmax = fDetector->GetAbsorSizeX();

	  G4double yz = fDetector->GetAbsorSizeYZ();
	  G4double dens = fDetector->GetAbsorMaterial()->GetDensity();

	  G4double x1 = prePoint->GetPosition().x() + xmax*0.5;
	  G4double x2 = postPoint->GetPosition().x() + xmax*0.5;
	  if(x1 >= 0.0 && x2 <= xmax) {
		  G4double x  = x1 + G4UniformRand()*(x2-x1);

		  if (step->GetTrack()->GetDefinition()->GetPDGCharge() == 0.) x = x2;

		  analysisManager->FillH1(1, x, edep);
		  analysisManager->FillH1(2, x, edep);

		  G4double dose = (edep/(yz*yz*dens));

		  //G4cout<<dose<<"		"<<G4BestUnit(dose,"Dose")<<G4endl;
		  //G4cout<<edep<<"		"<<G4BestUnit(edep,"Energy")<<"		"<<edep*CLHEP::joule<<G4endl;
		  //G4cout<<yz*yz<<"		"<<G4BestUnit(yz*yz,"Area")<<G4endl;
		  //G4cout<<dens<<"		"<<G4BestUnit(dens,"Density")<<G4endl;
		  //G4cout<<yz*yz*dens<<G4endl;
		  //G4cout<<G4endl;

		  analysisManager->FillH1(4, x, dose);

  	  }

  }

  //RESET
  //
  p_incident = 0; p_incident_top = 0; p_incident_bot = 0;
  p_gap = 0; p_gap_top = 0; p_gap_bot = 0;
  //p_charge = 0; p_charge_gap1 = 0; p_charge_gap2 = 0;
  //Ek_in_signal = 0; in_signal_status = 0;
  particle_incident_TOT = {}; particle_incident_TOP = {}; particle_incident_BOT = {};
  particle_gap_TOT = {}; particle_gap_TOP = {}; particle_gap_BOT = {};
  //particle_charge_TOT = {}; particle_charge_Gap1 = {}; particle_charge_Gap2 = {};
  //particleKODELStep = {};

  //Filling NTuples and 2d histograms
  //
  G4double particleCharge= step->GetTrack()->GetParticleDefinition()->GetPDGCharge();		// carica particella
  const G4VTouchable* touchable = step->GetTrack()->GetTouchable();			 //prende volume
  //G4int volCopyNum = touchable->GetVolume()->GetCopyNo();
  G4String volName = touchable->GetVolume()->GetName();
  //G4int PDG = step->GetTrack()->GetParticleDefinition()->GetPDGEncoding(); 	//aggiunto dopo per essere sicuri di particelle incidenti sono quelle desiderate
  G4String particleName = step->GetTrack()->GetParticleDefinition()->GetParticleName();
  G4int parentID = step->GetTrack()->GetParentID();


  G4double radio = fDetector->GetRPCRadioPos();
  G4double phi = fDetector->GetRPCPhiPos();

  G4double Rx = radio*cos(phi);
  G4double Rz = -1*radio*sin(phi);

  //if (volName != "World" && volName != "Absorber"){G4cout<<volName<<G4endl;}



  ////// //////		Secondary particle incident on RPC		////// ///////
  if (parentID != 1) return;
  //counter particle incident
  //
  if ( ((volName == "fakeBottom") || (volName == "fakeTop")) && (p_incident == 0) ) {    // particella incidente o sul falso layer top o bottom (per contare numero particelle incidenti)
  	  //G4cout<<volName<<G4endl;
	  p_incident	=	1;
	  particle_incident_TOT.p_counter	=	p_incident;														// quando c'è particella incidente, la variabile p_incidente deiventa 1  (x contatore)
	  particle_incident_TOT.ID	= 	step->GetTrack()->GetTrackID() ;							//ID particelle incidenti sia top che bot
	  particle_incident_TOT.PID	=	step->GetTrack()->GetParentID() ;									//PID parent ID particelle incidenti sia top che bot
	  particle_incident_TOT.PDG	=	step->GetTrack()->GetParticleDefinition()->GetPDGEncoding()	; 		//PDG particelle incidenti sia top che bot
	  particle_incident_TOT.Ek	= 	step->GetTrack()->GetKineticEnergy();								//Energia cinetica particelle incidenti si top che bot

	  particle_incident_TOT.z 	= 	(step->GetTrack()->GetPosition().z() - Rz)*cos(phi) + (step->GetTrack()->GetPosition().x() - Rx)*sin(phi) ;							//posizione sulla superficie top sulla quale incidono particelle
	  particle_incident_TOT.x 	= 	(step->GetTrack()->GetPosition().x() - Rx)*cos(phi) - (step->GetTrack()->GetPosition().z() - Rz)*sin(phi) ;
	  particle_incident_TOT.y	= 	step->GetTrack()->GetPosition().y() ;

	  if (particleName == "neutron") {
		  histo->FillCounterNtuple(particle_incident_TOT,0);
		  	//  analysisManager->FillH2(0,particle_incident_TOT.z,particle_incident_TOT.y);
		  }
	  if (particleName == "gamma") {
		  histo->FillCounterNtuple(particle_incident_TOT,3);
	      	//  analysisManager->FillH2(3,particle_incident_TOT.z,particle_incident_TOT.y);
		  }
  }

  // counter only incident by top
  //
  if ( (volName == "fakeTop") && (p_incident_top == 0) && (p_incident_bot == 0) ) {    // particella incidente  sul falso layer top (per contare numero particelle incidenti)
	  p_incident_top	=	1;
	  particle_incident_TOP.p_counter	=	p_incident_top;														// quando c'è particella incidente, la variabile p_incidente deiventa 1  (x contatore)
	  particle_incident_TOP.ID	= 	step->GetTrack()->GetTrackID() ;							//ID particelle incidenti sia top che bot
	  particle_incident_TOP.PID	=	step->GetTrack()->GetParentID() ;									//PID parent ID particelle incidenti sia top che bot
	  particle_incident_TOP.PDG	=	step->GetTrack()->GetParticleDefinition()->GetPDGEncoding()	; 		//PDG particelle incidenti sia top che bot
	  particle_incident_TOP.Ek	= 	step->GetTrack()->GetKineticEnergy();								//Energia cinetica particelle incidenti si top che bot

	  particle_incident_TOP.z 	= 	step->GetTrack()->GetPosition().z() ;							//posizione sulla superficie top sulla quale incidono particelle
	  particle_incident_TOP.x 	= 	step->GetTrack()->GetPosition().x() ;
	  particle_incident_TOP.y	= 	step->GetTrack()->GetPosition().y() ;

	  if (particleName == "neutron") {
		  histo->FillCounterNtuple(particle_incident_TOP,1);
		  	//  analysisManager->FillH2(1,particle_incident_TOP.z,particle_incident_TOP.y,1.);
		  }
	  if (particleName == "gamma") {
		  histo->FillCounterNtuple(particle_incident_TOP,4);
		  	//  analysisManager->FillH2(4,particle_incident_TOP.z,particle_incident_TOP.y,1.);
		  }
  }

  // counter only incident by bottom
  //
  if ( (volName == "fakeBottom") && (p_incident_top == 0) && (p_incident_bot == 0) ) {    	// particella incidente  sul falso layer top (per contare numero particelle incidenti)
   	  p_incident_bot	=	1;
	  particle_incident_BOT.p_counter	=	p_incident_bot;														// quando c'è particella incidente, la variabile p_incidente deiventa 1  (x contatore)
	  particle_incident_BOT.ID	= 	step->GetTrack()->GetTrackID() ;							//ID particelle incidenti sia top che bot
	  particle_incident_BOT.PID	=	step->GetTrack()->GetParentID() ;									//PID parent ID particelle incidenti sia top che bot
	  particle_incident_BOT.PDG	=	step->GetTrack()->GetParticleDefinition()->GetPDGEncoding()	; 		//PDG particelle incidenti sia top che bot
	  particle_incident_BOT.Ek	= 	step->GetTrack()->GetKineticEnergy();								//Energia cinetica particelle incidenti si top che bot

	  particle_incident_BOT.z 	= 	step->GetTrack()->GetPosition().z() ;							//posizione sulla superficie top sulla quale incidono particelle
	  particle_incident_BOT.x 	= 	step->GetTrack()->GetPosition().x() ;
	  particle_incident_BOT.y	= 	step->GetTrack()->GetPosition().y() ;

	  if (particleName == "neutron") {
		  histo->FillCounterNtuple(particle_incident_BOT,2);
		  	//  analysisManager->FillH2(2,particle_incident_BOT.z,particle_incident_BOT.y,1.);
		  }
	  if (particleName == "gamma") {
		  histo->FillCounterNtuple(particle_incident_BOT,5);
		  	//  analysisManager->FillH2(5,particle_incident_BOT.z,particle_incident_BOT.y,1.);
		  }
  }




  //counter particle gap
  //
  if ( ((volName == "GasGap1") || (volName == "GasGap2")) && (p_gap == 0) ) {
  	  //G4cout<<volName<<G4endl;
	  p_gap	=	1;
	  particle_gap_TOT.p_counter	=	p_gap;
	  particle_gap_TOT.ID	= 	step->GetTrack()->GetTrackID() ;
	  particle_gap_TOT.PID	=	step->GetTrack()->GetParentID() ;
	  particle_gap_TOT.PDG	=	step->GetTrack()->GetParticleDefinition()->GetPDGEncoding()	;
	  particle_gap_TOT.Ek	= 	step->GetTrack()->GetKineticEnergy();

	  particle_gap_TOT.z 	= 	(step->GetTrack()->GetPosition().z() - Rz)*cos(phi) + (step->GetTrack()->GetPosition().x() - Rx)*sin(phi) ;	 //step->GetTrack()->GetPosition().z() ;
	  particle_gap_TOT.x 	= 	(step->GetTrack()->GetPosition().x() - Rx)*cos(phi) - (step->GetTrack()->GetPosition().z() - Rz)*sin(phi) ;  //step->GetTrack()->GetPosition().x() ;
	  particle_gap_TOT.y	= 	step->GetTrack()->GetPosition().y() ;

	  if (particleName == "neutron") {
		  histo->FillCounterNtuple(particle_gap_TOT,6); }
	  if (particleName == "gamma") {
		  histo->FillCounterNtuple(particle_gap_TOT,9); }
  }

  // counter only incident by top
  //
  if ( (volName == "GasGap1") && (p_gap_top == 0) ) {
	  p_gap_top	=	1;
	  particle_gap_TOP.p_counter	=	p_gap_top;
	  particle_gap_TOP.ID	= 	step->GetTrack()->GetTrackID() ;
	  particle_gap_TOP.PID	=	step->GetTrack()->GetParentID() ;
	  particle_gap_TOP.PDG	=	step->GetTrack()->GetParticleDefinition()->GetPDGEncoding()	;
	  particle_gap_TOP.Ek	= 	step->GetTrack()->GetKineticEnergy();

	  particle_gap_TOP.z 	= 	step->GetTrack()->GetPosition().z() ;
	  particle_gap_TOP.x 	= 	step->GetTrack()->GetPosition().x() ;
	  particle_gap_TOP.y	= 	step->GetTrack()->GetPosition().y() ;

	  if (particleName == "neutron") {
		  histo->FillCounterNtuple(particle_gap_TOP,7);  }
	  if (particleName == "gamma") {
		  histo->FillCounterNtuple(particle_gap_TOP,10); }
  }

  // counter only incident by bottom
  //
  if ( (volName == "GasGap2") && (p_gap_bot == 0) ) {
   	  p_gap_bot	=	1;
	  particle_gap_BOT.p_counter	=	p_gap_bot;
	  particle_gap_BOT.ID	= 	step->GetTrack()->GetTrackID() ;
	  particle_gap_BOT.PID	=	step->GetTrack()->GetParentID() ;
	  particle_gap_BOT.PDG	=	step->GetTrack()->GetParticleDefinition()->GetPDGEncoding()	;
	  particle_gap_BOT.Ek	= 	step->GetTrack()->GetKineticEnergy();

	  particle_gap_BOT.z 	= 	step->GetTrack()->GetPosition().z() ;
	  particle_gap_BOT.x 	= 	step->GetTrack()->GetPosition().x() ;
	  particle_gap_BOT.y	= 	step->GetTrack()->GetPosition().y() ;

	  if (particleName == "neutron") {
		  histo->FillCounterNtuple(particle_gap_BOT,8);	 }
	  if (particleName == "gamma") {
		  histo->FillCounterNtuple(particle_gap_BOT,11); }
  }








  ////// //////		Secondary particle charge in Gas Gap		////// ///////

  // particle charge counter gas gap 1 and gas gap 2
  //
  /*if ( ((volName == "GasGap1") || (volName == "GasGap2")) && (particleCharge != 0) && (edep > 0) && (p_charge == 0) && (p_incident == 1) ) {  // particella carica e che ha depositato energia in gap 1 o gap 2

	  p_charge	=	1;					 									// quando c'è particella carica, la variabile p_carica deiventa 1 (per contatore)
	  particle_charge_TOT.ID	=	step->GetTrack()->GetTrackID();
	  particle_charge_TOT.PID	=	step->GetTrack()->GetParentID() ;
	  particle_charge_TOT.PDG	=	step->GetTrack()->GetParticleDefinition()->GetPDGEncoding();
	  particle_charge_TOT.Ek	=	step->GetTrack()->GetKineticEnergy();

	  particle_charge_TOT.z		=	 step->GetTrack()->GetPosition().z() ;
	  particle_charge_TOT.x		=	 step->GetTrack()->GetPosition().x() ;
	  particle_charge_TOT.y		=	 step->GetTrack()->GetPosition().y() ;

	  if (step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "eIoni") {particle_charge_TOT.ion_interaction	=	1;}
	  if (step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "msc") {particle_charge_TOT.msc_interaction	=	1;}
	  if (step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "eBrem") {particle_charge_TOT.eBrem_interaction	=	1;}

	  //ricava informazioni (posizione e processo) da cui si è formata particella carica che ha interagito
	  G4ThreeVector posizione_creazione_TOT= step->GetTrack()->GetVertexPosition();    // vettore prende posizione
	  particle_charge_TOT.x_creation	=	posizione_creazione_TOT[0] ;		//posizione sulla superficie gap1 sulla quale si crea particella carica (getvertexposition)
	  particle_charge_TOT.y_creation	=	posizione_creazione_TOT[1] ;
	  particle_charge_TOT.z_creation	=	posizione_creazione_TOT[2] ;

	  if (step->GetTrack()->GetCreatorProcess()->GetProcessName() == "compt") {particle_charge_TOT.compton_creation		=	1;}
	  if (step->GetTrack()->GetCreatorProcess()->GetProcessName() == "phot") {particle_charge_TOT.photo_creation	=	1;}

	  histo-> FillChargeNtuple(particle_charge_TOT,6);
	//  analysisManager->FillH2(6,particle_charge_TOT.z,particle_charge_TOT.y,1.);
	//  analysisManager->FillH2(9,particle_charge_TOT.z_creation,particle_charge_TOT.y_creation,1.);
  }

  // particle charge counter gas gap 1
  //
  if ( (volName == "GasGap1") && (particleCharge != 0) && (p_charge_gap1 == 0) && (edep > 0) && (p_charge_gap2 == 0) && (p_incident == 1) ) {

  	  p_charge_gap1	=	1;					 									// quando c'è particella carica, la variabile p_carica deiventa 1 (per contatore)
  	  particle_charge_Gap1.ID	=	step->GetTrack()->GetTrackID();
  	  particle_charge_Gap1.PID	=	step->GetTrack()->GetParentID() ;
  	  particle_charge_Gap1.PDG	=	step->GetTrack()->GetParticleDefinition()->GetPDGEncoding();
  	  particle_charge_Gap1.Ek	=	step->GetTrack()->GetKineticEnergy();

  	  particle_charge_Gap1.z		=	 step->GetTrack()->GetPosition().z() ;
  	  particle_charge_Gap1.x		=	 step->GetTrack()->GetPosition().x() ;
  	  particle_charge_Gap1.y		=	 step->GetTrack()->GetPosition().y() ;

  	  if (step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "eIoni") {particle_charge_Gap1.ion_interaction	=	1;}
  	  if (step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "msc") {particle_charge_Gap1.msc_interaction	=	1;}
  	  if (step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "eBrem") {particle_charge_Gap1.eBrem_interaction	=	1;}

  	  //ricava informazioni (posizione e processo) da cui si è formata particella carica che ha interagito
  	  G4ThreeVector posizione_creazione_TOT= step->GetTrack()->GetVertexPosition();    // vettore prende posizione
  	  particle_charge_Gap1.x_creation	=	posizione_creazione_TOT[0] ;		//posizione sulla superficie gap1 sulla quale si crea particella carica (getvertexposition)
  	  particle_charge_Gap1.y_creation	=	posizione_creazione_TOT[1] ;
  	  particle_charge_Gap1.z_creation	=	posizione_creazione_TOT[2] ;

  	  if (step->GetTrack()->GetCreatorProcess()->GetProcessName() == "compt") {particle_charge_Gap1.compton_creation		=	1;}
  	  if (step->GetTrack()->GetCreatorProcess()->GetProcessName() == "phot") {particle_charge_Gap1.photo_creation	=	1;}

  	  histo-> FillChargeNtuple(particle_charge_Gap1,7);
  	//  analysisManager->FillH2(7,particle_charge_Gap1.z,particle_charge_Gap1.y,1.);
  	//  analysisManager->FillH2(10,particle_charge_Gap1.z_creation,particle_charge_Gap1.y_creation,1.);
    }

    // particle charge counter gas gap 2
    //
    if ( (volName == "GasGap2") && (particleCharge != 0) && (p_charge_gap1 == 0) && (edep >0) && (p_charge_gap1 == 0) && (p_incident == 1) ) {  // particella carica e che ha depositato energia in gap 1 o gap 2
    	p_charge_gap2	=	1;					 									// quando c'è particella carica, la variabile p_carica deiventa 1 (per contatore)
    	particle_charge_Gap2.ID	=	step->GetTrack()->GetTrackID();
    	particle_charge_Gap2.PID	=	step->GetTrack()->GetParentID() ;
    	particle_charge_Gap2.PDG	=	step->GetTrack()->GetParticleDefinition()->GetPDGEncoding();
    	particle_charge_Gap2.Ek	=	step->GetTrack()->GetKineticEnergy();

    	particle_charge_Gap2.z		=	 step->GetTrack()->GetPosition().z() ;
    	particle_charge_Gap2.x		=	 step->GetTrack()->GetPosition().x() ;
    	particle_charge_Gap2.y		=	 step->GetTrack()->GetPosition().y() ;

    	if (step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "eIoni") {particle_charge_Gap2.ion_interaction	=	1;}
    	if (step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "msc") {particle_charge_Gap2.msc_interaction	=	1;}
    	if (step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "eBrem") {particle_charge_Gap2.eBrem_interaction	=	1;}

    	//ricava informazioni (posizione e processo) da cui si è formata particella carica che ha interagito
    	G4ThreeVector posizione_creazione_TOT= step->GetTrack()->GetVertexPosition();    // vettore prende posizione
    	particle_charge_Gap2.x_creation	=	posizione_creazione_TOT[0] ;		//posizione sulla superficie gap1 sulla quale si crea particella carica (getvertexposition)
    	particle_charge_Gap2.y_creation	=	posizione_creazione_TOT[1] ;
    	particle_charge_Gap2.z_creation	=	posizione_creazione_TOT[2] ;

    	if (step->GetTrack()->GetCreatorProcess()->GetProcessName() == "compt") {particle_charge_Gap2.compton_creation	=	1;}
    	if (step->GetTrack()->GetCreatorProcess()->GetProcessName() == "phot") {particle_charge_Gap2.photo_creation	=	1;}

    	histo-> FillChargeNtuple(particle_charge_Gap2,8);
    //	analysisManager->FillH2(8,particle_charge_Gap2.z,particle_charge_Gap2.y,1.);
    //	analysisManager->FillH2(11,particle_charge_Gap2.z_creation,particle_charge_Gap2.y_creation,1.);
      }

  // energy ONLY of incident photons which then produce signal (to know energy of photons which produce signal)
  //
  if (p_incident == 1 && p_charge == 1 && in_signal_status == 0) {
	  in_signal_status	=	1;
	  Ek_in_signal	=	particle_incident_TOT.Ek;
	  histo->FillIncidentSignal(Ek_in_signal,9);		//metodo per copiare variabile Ek_incident_signal in histomanager, e riempire histo
    	}

  //estrazione info per ogni step ma solo nel volume della kodel
  //
  if ( edep !=0 && (volName =="Honeycomb_skin1_1" || volName =="Honeycomb_core1" || volName =="Honeycomb_skin1_2"
  	|| volName == "PET_insulationTOP"
  	|| volName =="paper"

  	|| volName =="Copper1_1" || volName == "PET1_1" || volName =="PET1_2" || volName == "EVA1_1"|| volName =="graphite1_1"|| volName =="HPL1_1"
  	|| volName =="GasGap1"
  	|| volName =="HPL1_2"|| volName =="graphite1_2"|| volName == "EVA1_2"|| volName == "PET1_3"|| volName =="PET1_4"|| volName =="Copper1_2"

  	|| volName =="PETstrip1"|| volName =="Copper_Strip"|| volName == "PETstrip2"

  	|| volName =="Copper2_1"|| volName == "PET2_1"|| volName =="PET2_2"|| volName == "EVA2_1"|| volName =="graphite2_1"|| volName =="HPL2_1"
  	|| volName =="GasGap2"
  	|| volName =="HPL2_2"|| volName =="graphite2_2"|| volName == "EVA2_2"|| volName == "PET2_3"|| volName =="PET2_4"|| volName =="Copper2_2"

  	|| volName =="PET_insulationBOT"
  	|| volName =="Bakelite"
  	|| volName =="Honeycomb_skin2_1"|| volName == "Honeycomb_core2"|| volName =="Honeycomb_skin2_2") )

  {

	  particleKODELStep.z	=	step->GetTrack()->GetPosition().z() ;					//posizione sulla superficie top sulla quale incidono particelle
	  particleKODELStep.x	=	step->GetTrack()->GetPosition().x() ;
	  particleKODELStep.y	=	step->GetTrack()->GetPosition().y() ;

	  particleKODELStep.edep	=	edep;
	  particleKODELStep.eIon	=	edep - step->GetNonIonizingEnergyDeposit() ;

  	if (step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "compt") {particleKODELStep.compt = 1;}
  	if (step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "phot") {particleKODELStep.photo = 1;}
  	if (step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "eIoni") {particleKODELStep.ion = 1;}
  	if (step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "msc") {particleKODELStep.msc = 1;}
  	if (step->GetPostStepPoint()->GetProcessDefinedStep()->GetProcessName() == "eBrem") {particleKODELStep.eBrem = 1;}

  	histo->FillKODELStep(particleKODELStep,10);
  	//analysisManager->FillH1(4, particleKODELStep.x - (RPCXFirstLayerPos - 2.5), edep); //497.5 = posicion en x de la primera lamina - spesor/2
  	//analysisManager->FillH1(6, particleKODELStep.x, edep); //497.5 = posicion en x de la primera lamina - spesor/2
  	  }*/



}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


  // respuesta de geant4 forum
  //G4VPhysicalVolume* pv = step->GetPostStepPoint()->GetPhysicalVolume(); // Here if you've just entered the volume, or are within it
  //pv = step->GetPreStepPoint()->GetPhysicalVolume(); // Here if you are just leaving the volume (postStep volume will be different)

