#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Jul  9 16:57:18 2020

@author: dayron
"""
import math										
import string									
import os										



nome_file_in="protonOK.mac"								# geant4 macro file name
n_events= 10000				                            # NUMERO EVENTI 

###########################################################################################################

energy_list = list(range (30,240,10))
angle_list = list(range (0,165,15))

for angle in angle_list:
    for energy in energy_list:      
        file_input=open (nome_file_in , "a+")					
        
        # Read in the file
        with open(nome_file_in, 'r') as file :					
            filedata = file.read()

        # Replace the target string		
        filedata = filedata.replace("/analysis/setFileName", "#/analysis/setFileName")
        filedata = filedata.replace("/gun/energy", "#/gun/energy")		#commenta linea energy
        filedata = filedata.replace("/run/beamOn", "#/run/beamOn")		#commenta linea beam On, 
        filedata = filedata.replace("/hrpc/det/setRPCPhiPos", "#/hrpc/det/setRPCPhiPos")
        
        filedata = filedata.replace("/hrpc/gun/rndm", "#/hrpc/gun/rndm")
        filedata = filedata.replace("/run/setCut", "#/run/setCut")
        filedata = filedata.replace("/run/printProgress", "#/run/printProgress")
        filedata = filedata.replace("/analysis/h1/set", "#/analysis/h1/set")
        filedata = filedata.replace("/gun/particle proton", "#/gun/particle proton")
        filedata = filedata.replace("/run/initialize", "#/run/initialize")

        # Write the file out again				
        with open(nome_file_in, 'w') as file:
            file.write(filedata)

        set_angle = "\n/hrpc/det/setRPCPhiPos "+str(angle)+". deg\n"		
        file_input.write(set_angle)

        initialize = '''\n/run/initialize\n
\n/hrpc/gun/rndm 20 cm\n
\n/run/setCut 1 mm\n
\n/run/printProgress 1000\n
\n/analysis/h1/set 2 100 220 280 mm\n
\n/analysis/h1/set 3 100 220 280 mm\n
\n/analysis/h1/set 4 300 0 300 mm\n
\n/gun/particle proton\n'''
        file_input.write(initialize)
        
        set_filename = "\n/analysis/setFileName hrpc_"+str(angle)+"deg_"+str(energy)+"MeV\n"			
        file_input.write(set_filename)
        set_energy = "\n/gun/energy "+str(energy)+" MeV \n"			
        file_input.write(set_energy)						
        beamOn = "\n/run/beamOn "+str(n_events)					
        file_input.write(beamOn)						

        file_input.close()							

        print("\n ")
        print("=======================================")
        print("====== Run "+str(n_events)+" protons @"+str(angle)+" deg and"+str(energy)+" MeV =====")
        print("=======================================")
        print("\n ")

        os.system('./HRPC2 protonOK.mac')					#esegue simulazione
