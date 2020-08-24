
import math										#importa la libreria matematica
import string										#importa la libreria stringhe
import glob


#librerie pyROOT per disegnare grafici
import ROOT
from ROOT import gROOT
from ROOT import TCanvas, TGraph, gPad, TF1, TF2 , kRed, TMultiGraph, TLegend, gStyle, TPaveStats, TStyle, TText, TList, TLatex, TMath, TFormula, TGraphErrors, TTree, TDatime, TMath
from ROOT import TCanvas, TGraph, gPad, TF1, kRed, TMultiGraph, TLegend, gStyle, TPaveStats, TStyle, TText, TList, TLatex, TMultiGraph, TH1F, TFile
from array import array

#spazio = "  "

#file_out=open("range_extracted.csv", "w")		#crea file output con valori sensistività estratti da ogni rootfile
#intestazione="energy range PDGcode \n"				#intestazione file out
#file_out.write(intestazione)

list_rootfile= glob.glob('*.root') 						#lista file root da cui prendere grafici


for rootfile in list_rootfile:							#loop su ogni rootfile
	input_root = TFile(rootfile)							#apre root file

	#prende valore energia
	imported_tree= input_root.Get("neutron_incident_TOT")		#prende ttree
	imported_tree.Draw("KINETIC_ENERGY","","")	#disegna plot da ttree per poter prendere e modificare
	histo= imported_tree.GetHistogram().Clone()
	mean_energy=histo.GetMean()

	#prende PDG CODE
	imported_tree.Draw("PARTICLE_DATA_GROUP","","")	#disegna plot da ttree per poter prendere e modificare
	histo= imported_tree.GetHistogram().Clone()
	PDG=histo.GetMean()

	#prende valore sensitività
	proj_range= input_root.Get("projectile_range")		#prende grafico sensitivity (se c'è)

	if proj_range:					#se c'è grafico sensitivity estrae valore
		mean= proj_range.GetMean()
		xbin=proj_range.GetXaxis().FindBin(mean)
		range = proj_range.GetBinContent(xbin)
	else:							#se non c'è grafico sensitivity la mette a 0
		range =0

    print("ok")
	#scrive su file
	#dati= str(mean_energy) +spazio+ str(range)+spazio+ str(PDG)+"\n"
	#file_out.write(dati)
