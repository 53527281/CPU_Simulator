/******************************************************************************
 * File:    simulator.h
 * Project: CMSC 411 Project, Summer 2016
 * Author:  Thomas Wallace
 * Date:    25 June 2016
 * Section:
 * E-mail:  thwalla1@umbc.edu
 *
 * Description:
 *          Header file for simplified MIPS computer simulator. For detailed
 * description see simulator.cpp
 *
 *****************************************************************************/
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>

#include "system.h"

/*DATA STRUCTURES DECLARATION*************************************************/



/*FUNCTION DECLARATION********************************************************/

//PrintError - Print prespecified error message
void PrintError(const int id, const int num = 0,
                const std::string tag = std::string());
