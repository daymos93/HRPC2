#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Jul 10 10:15:05 2020

@author: dayron
"""


# importar todas las funciones de pylab
from pylab import *

# importar el módulo pyplot
import matplotlib.pyplot as plt
import numpy as np

import seaborn as sns



energy_list = list(range (30,230,10))

nevent = 10000.

proj_range = [4.572,10.27,17.2,25.27,34.42,44.57,55.64,67.69,84.29,98.83,114.4,131.2,148.5,166.7,185.8,205.6,226.3,247.4,269.2,291.6]
neutron = [0,0,0,6,6,8,11,21,48,38,54,95,97,104,139,166,249,309,314,427] 
gamma = [3,17,35,48,60,69,116,97,143,144,168,266,257,300,370,404,504,527,635,726]

n_ratio = []; g_ratio = []

for i in neutron : n_ratio.append(i/nevent)
for i in gamma : g_ratio.append(i/nevent)

#plot data
plt.figure(figsize=(14,9))

#calc ratios / nevents
errorbar(energy_list, n_ratio,fmt='o', label='neutron ratio')
errorbar(energy_list, g_ratio,fmt='o', label='gamma ratio')


sns.set_style()

# Plot set
#semilogy()
plt.grid()
plt.xlabel('energy [MeV]', fontsize=24)
plt.ylabel('', fontsize=24)

tick_params(axis="x", labelsize=18)
tick_params(axis="y", labelsize=18)

#plt.xlim(10, 100)
#plt.ylim(0, 14)

plt.legend(loc='upper left', fontsize=20)
savefig('sec_ratio.jpg')

show()    