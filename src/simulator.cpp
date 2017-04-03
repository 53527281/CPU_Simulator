/******************************************************************************
 * File:    pipeline.cpp
 * Project: CMSC 411 Project, Summer 2016
 * Author:  Thomas Wallace
 * Date:    25 June 2016
 * Section: 
 * E-mail:  thwalla1@umbc.edu
 *
 * Description:
 *          Main file for the simplified MIPS computer simulation project. This
 * file contains the main run loop logic for the simulator.
 *
 *****************************************************************************/
#include "simulator.h"

using namespace std;

void PrintError(int id, int num, string tag) {

  switch(id) {
  case 1:
    cout << "Error: Missing Command Line Arguments" << endl;
    break;
  case 2:
    cout << "Error: Unable to Open " << tag << endl;
    break;
  case 3:
    cout << "Error: Unknown Instruction Format on line " << num << endl;
    cout << "Expected: Tabs following semi-colon and operation" << endl;
    break;
  case 4:
    cout << "Error: Unknown Data Format on line " << num << endl;
    break;
  default:
    cout << "Error: Undefined Exception" << endl;
  }
}

int main(int argc, char* argv[]) {

  //Check for Command Line Arguments
  if(argc != 4) {
    PrintError(1);
    exit(0);
  }

  //Simulator Variable Declaration
  SYSTEM sys;

  //Input Variable Declaration
  ifstream instfile(argv[1]);
  ifstream datafile(argv[2]);
  
  //Check Instruction File Valid
  if(!instfile.is_open()) {
    PrintError(2, 0, string(argv[1]));
  }
  else {
    //Local Variable Definition
    int lineNum = 0;
    string line;
    bool parseErr = true;

    //Build Program Label Table
    while(getline(instfile, line)) {

      parseErr = sys.ParseLabel(line, lineNum);

      if(parseErr == true) {
	PrintError(3, lineNum);
	exit(0);
      }
      lineNum++;
    }
    //Reset File Read
    instfile.clear();
    instfile.seekg(0, ios::beg);
    lineNum = 0;

    //Load Program into Memory
    while(getline(instfile, line)) {

      parseErr = sys.ParseInstruction(line, lineNum);
      
      //Add Instruction
      if(parseErr == true) {
	PrintError(3, (lineNum));
	exit(0);
      }
      lineNum++;
    }
    sys.PCEND = lineNum * 4;
  }
  instfile.close();

  //Check Data File Valid
  if(!datafile.is_open()) {
    PrintError(2, 0, string(argv[1]));
  }
  else {
    //Local Variable Definition
    int lineNum = 0;
    string line;
    bool parseErr = true;

    //Build Program Label Table
    while(getline(datafile, line)) {

      parseErr = sys.ParseData(line, lineNum);

      if(parseErr == true) {
        PrintError(4, (lineNum + 1));
        exit(0);
      }
      lineNum++;
    }
  }
  datafile.close();

  //Run Program
  sys.RunProgram(string(argv[3]));

  //Load Data into Memory

  return 0;
}
