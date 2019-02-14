#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Feb 11 19:21:48 2019

@author: hippo
"""

def genRegistration(filePath):
    qRegisterMetaType = list()
    qRegisterMetaType_containers = list()
    Q_DECLARE_METATYPE = list()
    Q_DECLARE_METATYPE_containers = list()
    
    container_types = {'std::vector', 
#                       'std::set', 
#                       'std::unordered_set'
                       }
    
    with open(filePath, 'r') as fs:
        for line in fs:
            line = str.strip(line)

            if line.startswith("Q_DECLARE_METATYPE("):
                tar = line[len("Q_DECLARE_METATYPE("):-2]
                regstr = "qRegisterMetaType<" + tar + ">();"
                if regstr not in qRegisterMetaType:
                    qRegisterMetaType.append(regstr)
                for container in container_types:
                    qRegisterMetaType_containers.append( "qRegisterMetaType<" + container + "<" + tar + ">>();" )
                    Q_DECLARE_METATYPE_containers.append( "Q_DECLARE_METATYPE(" + container + "<" + tar + ">);" )
#            elif line.startswith("Q_DECLARE_SEQUENTIAL_CONTAINER_METATYPE("):
#                regstr = "qRegisterMetaType<" + line[len("Q_DECLARE_SEQUENTIAL_CONTAINER_METATYPE("):-2] + ">();"
#            elif line.startswith("Q_DECLARE_ASSOCIATIVE_CONTAINER_METATYPE("):
#                regstr = "qRegisterMetaType<" + line[len("Q_DECLARE_ASSOCIATIVE_CONTAINER_METATYPE("):-2] + ">();"
            elif line.startswith("QObject::connect: Cannot queue arguments of type '"):
                tar = line[len("QObject::connect: Cannot queue arguments of type '"):-1]
                regstr = "qRegisterMetaType<" + tar + ">();"
                dclr = "Q_DECLARE_METATYPE(" + tar + ");"
                if regstr not in qRegisterMetaType:
                    qRegisterMetaType.append(regstr)
                if dclr not in Q_DECLARE_METATYPE:
                    Q_DECLARE_METATYPE.append(dclr);
                
    for decl in Q_DECLARE_METATYPE:
        print(decl)
    print()
    for reg in qRegisterMetaType:
        print(reg)
    print()
    for cntr_declare in Q_DECLARE_METATYPE_containers:
        print(cntr_declare)
    print()
    for cntr_reg in qRegisterMetaType_containers:
        print(cntr_reg)
    print()

    


reg_file_path = "/home/hippo/Documents/Desktop/qt_registration.txt"
genRegistration(reg_file_path)