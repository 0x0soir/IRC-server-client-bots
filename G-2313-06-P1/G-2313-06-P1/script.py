#!/usr/bin/env python
import os

exe = '06_redes2'

os.system("clear")
print("********** Limpiando el directorio **********")
os.system("make clean")
print("********** Compilando **********")
os.system("make all")
print("********** Ejecutando el Servidor **********")
os.system("./" + exe)
