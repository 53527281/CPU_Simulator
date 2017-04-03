/******************************************************************************
 * File:    system.h
 * Project: CMSC 411 Project, Summer 2016
 * Author:  Thomas Wallace
 * Date:    25 June 2016
 * Section:
 * E-mail:  thwalla1@umbc.edu
 *
 * Description:
 *          Header file for simplified MIPS computer system. For detailed
 * description see pipeline.cpp
 *
 *****************************************************************************/
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <string>
#include <queue>

#include "processor.h"

/*GLOBAL VARIABLE DECLARATION*************************************************/

const int MEMSIZE = 512;
const int PCBASE = 0;
const int DATABASE = 256;

/*DATA STRUCTURES DECLARATION*************************************************/

//LABEL - Groups label with associated program count
struct LABEL {

  std::string label;
  int pc;
};


//SYSTEM - Groups system-managed data elements
struct SYSTEM {

  int PCEND;
  int memory[MEMSIZE];
  std::vector<LABEL> ltable;

  /*FUNCTION DECLARATION******************************************************/

  //ParseLabel - Parses given string for semi-colon delimited instruction label
  bool ParseLabel(std::string &line, int pc);
  
  //ParseInstruction - Parses given instruction format string
  bool ParseInstruction(std::string &line, int pc);

  //ParseData - Parses given data format string
  bool ParseData(std::string &line, int pc);
  
  //LoadInstruction - Adds parsed instruction to memory array
  void LoadInstruction(INSTRUCTION itemp, int pc);

  //ReadInstruction - Returns word located at given address
  INSTRUCTION ReadInstruction(int pc);

  //RunProgram - Runs previously parsed program, output to outfile
  void RunProgram(std::string filename);

  //PrintOutput - Prints pipeline output to screen for debug
  void PrintOutput();

  //DumpMem - Displays the contents of memory up to given word address
  void DumpMem(int word);

  //DumpReg - Displays the contents of all general purpose registers
  void DumpReg();

};
