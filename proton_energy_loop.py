#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Jul  9 16:57:18 2020

@author: dayron
"""
import math										#importa la libreria matematica
import string										#importa la libreria stringhe
import os										#importa libreria per eseguire comandi bash

#script per eseguire loop di simulazioni cambiando in automatico valore dell'energia nella macro run.mac
#apre file run.mac, setta valore di energia da lista, esegue simulazione,   e ripete questo ciclo per ogni valore di energia nella lista
# ++++++++++ ATTENZIONE INSERIRE NUMERO EVENTI PER OGNI SIMULAZIONE +++++++++++++

#print " ++++++ INSERIRE VALORI ENERGIA, E NUMERO EVENTI!! +++++++"	

#print "     CONTINUAZIONE DELLA SIMUALZIONE RUNNATA SU RECAS, PARTENDO DA 13.6 MEV (VALORI PRECEDENTI SU RECAS)          "



nome_file_in="protonOK.mac"								# geant4 macro file name
n_events= 10000				#+++++++ NUMERO EVENTI DA SIMULARE X OGNI SIMULAZIONE

###########################################################################################################
energy_list=[
100,
150,
200,
250
]	#lista valori energia su cui eseguire loop simulazioni 


for energy in energy_list:      #loop per eseguire simulazione per ogni valore di energia
    file_input = open (nome_file_in , "a+")					#apre file macro per poi settare valore energia (apertura in append x aggiungere riga set energy, ma anche in lettura(a+) per commentare mano a mano le linee vecchie di se energy)


	# Read in the file
    with open(nome_file_in, 'r') as file :					#apre file in lettura in modo da leggere stringhe come file
        filedata = file.read()

    # Replace the target string		#commenta linea che setta energy (perche poi si scrivera nuova linea set energy x ogni valore di energia in lista)
    filedata = filedata.replace("/analysis/setFileName", "#/analysis/setFileName")
    filedata = filedata.replace("/gun/energy", "#/gun/energy")		#commenta linea energy
    filedata = filedata.replace("/run/beamOn", "#/run/beamOn")		#commenta linea beam On, perche questo comando deve essere dopo set energy
    
	
    # Write the file out again				#scrive  linea modificato
    with open(nome_file_in, 'w') as file:
        file.write(filedata)


    set_filename = "\n /analysis/setFileName hrpc_"+str(energy)+"\n"			#linea da scrivere nel file per settare valore energia (preso da lista valori)
    file_input.write(set_filename)
    set_energy = "/gun/energy "+str(energy)+" MeV \n"			#linea da scrivere nel file per settare valore energia (preso da lista valori)
    file_input.write(set_energy)						#scrive linea set energy
    beamOn = "/run/beamOn "+str(n_events)					#linea per beam On
    file_input.write(beamOn)						#scrive liena beam ON

    file_input.close()							#chiude file macro 

    #os.system('./HRPC2 protonOK.mac')					#esegue simulazione
    
    print("running "+str(n_events)+" protons @"+str(energy)+" MeV")













