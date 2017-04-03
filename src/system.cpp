/******************************************************************************
 * File:    system.cpp
 * Project: CMSC 411 Project, Summer 2016
 * Author:  Thomas Wallace
 * Date:    25 June 2016
 * Section:
 * E-mail:  thwalla1@umbc.edu
 *
 * Description:
 *          Definition file for simplified MIPS computer system. 
 *
 *****************************************************************************/
#include "system.h"

using namespace std;

/*VARIABLE DECLARATION********************************************************/

CPU cpu;

/*FUNCTION DEFINITION*********************************************************/

//ParseLabel
bool SYSTEM::ParseLabel(string &line, int pc) {

  //Local Variable Definition
  istringstream input(line);
  LABEL ltemp;
  pc *= 4;

  //Parse for Label
  try {
    input >> ws;
    if(line.find(":") != string::npos) {
      input >> ws;
      getline(input, ltemp.label, ':');
      ltemp.pc = pc;
      ltable.push_back(ltemp);
    }
  }

  catch(...) {
    return true;
  }

  return false;
}

//ParseInstruction
bool SYSTEM::ParseInstruction(string &line, int pc) {

  //Local Variable Definition
  istringstream input(line);
  string token;
  INSTRUCTION itemp;
  bool lowerR = false;
  pc *= 4;

  //Initialize Temp Instruction as Invalid
  itemp.operation = -1;

  //Parse for Label and Discard
  bool nolabel = false;
  try {
    if(line.find(":") != string::npos) {
      getline(input, token, ':');
    }
    else {
      nolabel = true;
    }

    /*Parse for Operation*****************************************************/
    input >> ws;
    if((line.find("\t") != string::npos)) {
      getline(input, token, '\t');
      input >> ws;
    }
    else {
      getline(input, token, ' ');
      input >> ws;
    }
    input >> ws;
    transform(token.begin(), token.end(), token.begin(), ::toupper);

    //Check Operation Valid
    for(int i = 0; i < ICOUNT; i++) {
      if(token.compare(ITABLE[i]) == 0) {
        itemp.operation = i;
        break;
      }
    }
    //Not a Valid Operation
    if(itemp.operation == -1) {
      return true;
    }
  }
  catch(...) {
    //Unexpected Parse Error
    return true;
  }
  input >> ws;

  /*Parse for Jump************************************************************/
  try {
    if(itemp.operation == 15) {
      input >> ws;
      getline(input, token, ' ');
      input >> ws;
      
      //Check for Pre-existing Label
      itemp.operand[2] = -1;
      for(unsigned int i = 0; i < ltable.size(); i++) {
	if(token == ltable[i].label) {
	  itemp.operand[2] = ltable[i].pc;
	  break;
	}
      }
      if(itemp.operand[2] == -1) {
	throw;
      }
      else {
	//Load Instruction
	LoadInstruction(itemp, pc);
	return false;
      }
    }
  }
  catch(...) {
    //Unexpected Parse Error
    return true;
  }
  input >> ws;

  /*Parse for Single-Word Instructions****************************************/
  try{
    //Discard Register Character
    if(line.find("R") != string::npos) {
      getline(input, token, 'R');
    }
    else {
      lowerR = true;
      getline(input, token, 'r');
    }
    //Parse for First Operand
    getline(input, token, ',');
    itemp.operand[0] = atoi(token.c_str());
    input >> ws;
    
    //Check Register is Valid
    if((itemp.operand[0] < 0) || (itemp.operand[0] > 31)) {
      throw;
    }
  }
  catch(...) {
    //Unexpected Parse Error
    return true;
  }
  input >> ws;

  /*Parse for Load Immediate**************************************************/
  try {
    if(itemp.operation == 3) {
      getline(input, token, ' ');
      itemp.operand[2] = atoi(token.c_str());
      
      //Load Instruction to Memory
      LoadInstruction(itemp, pc);
      return false;
    }
  }
  catch(...){
    return true;
  }
  input >> ws;

  /*Parse for Load Word and Store Word****************************************/
  try {
    if((itemp.operation == 1) || (itemp.operation == 2)) {
      getline(input, token, '(');
      itemp.operand[2] = atoi(token.c_str());
      
      //Discard Register Character
      if(lowerR == false) {
	getline(input, token, 'R');
      }
      else {
	getline(input, token, 'r');
      }
      //Parse for Load Store Register
      getline(input, token, ')');
      itemp.operand[1] = atoi(token.c_str());
    
      //Check Register is Valid
      if((itemp.operand[1] < 0) || (itemp.operand[1] > 31)) {
	throw;
      }
      else {
	//Load Instruction to Memory
	LoadInstruction(itemp, pc);
	return false;
      }
    }
  }
  catch(...) {
    //Unexpected Parse Error
    return true;
  }
  input >> ws;

  /*Parse for Two-Register Instructions***************************************/
  try{

    //Discard Register Character
    if(lowerR == false) {
      getline(input, token, 'R');
    }
    else {
      getline(input, token, 'r');
    }
    //Parse for Second Operand
    getline(input, token, ',');
    itemp.operand[1] = atoi(token.c_str());
    input >> ws;

    //Check Register is Valid
    if((itemp.operand[1] < 0) || (itemp.operand[1] > 31)) {
      throw;
    }
  }
  catch(...) {
    //Unexpected Parse Error
    return true;
  }
  input >> ws;

  /*Parse for Branch Instructions*********************************************/
  try {
    if((itemp.operation == 16) || (itemp.operation == 17)) {
      input >> ws;
      getline(input, token, ' ');

      //Check for Pre-existing Label
      itemp.operand[2] = -1;
      for(unsigned int i = 0; i < ltable.size(); i++) {
        if(token == ltable[i].label) {
          itemp.operand[2] = ltable[i].pc;
          break;
        }
      }
      if(itemp.operand[2] == -1) {
        throw;
      }
      else {
        //Load Instruction
        LoadInstruction(itemp, pc);
        return false;
      }
    }
  }
  catch(...) {
    //Unexpected Parse Error
    return true;
  }
  input >> ws;
      
  /*Parse for Three-Register Instructions*************************************/
  try{
    if((itemp.operation == 4) || (itemp.operation == 6) ||
       (itemp.operation == 8) || (itemp.operation == 10) ||
       (itemp.operation == 18)) {
  
      //Discard Register Character
      if(lowerR == false) {
	getline(input, token, 'R');
      }
      else {
	getline(input, token, 'r');
      }
      //Parse for Third Operand
      getline(input, token, ' ');
      itemp.operand[2] = atoi(token.c_str());
      
      //Check Register is Valid
      if((itemp.operand[2] < 0) || (itemp.operand[2] > 31)) {
	throw;
      }
      else {
	//Load Instruction
	LoadInstruction(itemp, pc);
	return false;
      }
    }
  }
  catch(...) {
    //Unexpected Parse Error
    return true;
  }
  input >> ws;

  /*Parse for Two-Register I-type Instructions********************************/
  try {
    getline(input, token, ' ');
    itemp.operand[2] = atoi(token.c_str());

    //Load Instruction to Memory
    LoadInstruction(itemp, pc);
    return false;
  }
  catch(...) {
    //Unexpected Parse Error
    return true;
  }

  //Complete Cluster Catch-all
  return true;
}

//ParseData
bool SYSTEM::ParseData(string &line, int pc) {

  //Local Variable Definition
  INSTRUCTION dtemp;
  int address;

  try {
    dtemp.operand[2] = strtol(line.c_str(),NULL,2);
    address = (DATABASE + (pc * 4));
    
    LoadInstruction(dtemp, address);
  }
  catch(...) {
    return true;
  }
  return false;
}

//LoadInstruction
void SYSTEM::LoadInstruction(INSTRUCTION itemp, int pc) {

  memory[pc+0] = itemp.operation;
  memory[pc+1] = itemp.operand[0];
  memory[pc+2] = itemp.operand[1];
  memory[pc+3] = itemp.operand[2];
}

//ReadInstruction
INSTRUCTION SYSTEM::ReadInstruction(int pc) {

  //Local Variable Definition
  INSTRUCTION itemp;

  itemp.operation = memory[pc+0];
  itemp.operand[0] = memory[pc+1];
  itemp.operand[1] = memory[pc+2];
  itemp.operand[2] = memory[pc+3];

  return itemp;
}

//RunProgram
void SYSTEM::RunProgram(string filename) {

  //Output Variable Definition
  ofstream outfile(filename.c_str());
  bool label = false;
  int lindex = 0;

  //Local Variable Definition
  INSTRUCTION inext;

  if(outfile.is_open()) {

    while(cpu.HLTFLAG == false) {
   
      //Check for Instruction Fetech
      if(cpu.FETCHPC == true) {
	if(cpu.pc < PCEND) {
	  inext = ReadInstruction(cpu.pc);
	  cpu.ICACHE.push(inext);
	}
      }

      //Check for Memory Read
      if(cpu.EX3BUF.MEMREAD == true) {
        inext = ReadInstruction(cpu.EX3BUF.IMM);
        cpu.DCACHE.push(inext);
      }
    
      //Decode and Execute     
      cpu.StepCycle();

      //Check for Memory Write
      if(cpu.MEMBUF.MEMWRITE == true) {
	inext =	cpu.WBBUF.front();
	memory[inext.operand[0]] = inext.operand[2];
      }

      //Check for Output
      if(cpu.OUTFLAG == true) {
	PrintOutput();
        //Output to File
	label = false;

        //Check for Label
        for(unsigned int i = 0; i < ltable.size(); i++) {
          if(ltable[i].pc == cpu.MEMBUF.pc) {
            outfile << ltable[i].label << "\t" << ITABLE[memory[cpu.MEMBUF.pc]]
		    << "\t";
            label = true;
	    lindex = i;
            break;
          }
        }
        if(label == false) {
          outfile << "\t" << ITABLE[memory[cpu.MEMBUF.pc]] << "\t";
        }
        switch(cpu.MEMBUF.type) {
        case 1:
          outfile << "R" << cpu.MEMBUF.RD << ", " << cpu.EX3BUF.IMM
                  << "(" << cpu.MEMBUF.RS << ")\t";
          break;
        case 2:
          outfile << "R" << cpu.MEMBUF.RD << ", " << cpu.MEMBUF.IMM << "\t\t";
          break;
        case 3:
          outfile << "\tPC: " << cpu.MEMBUF.pc << "\t";
          break;
        case 4:
          outfile << "R" << cpu.MEMBUF.RS << ", R" << cpu.MEMBUF.RT << ", PC: "
		  << cpu.MEMBUF.pc << "\t";
          break;
        case 5:
          outfile << "R" << cpu.MEMBUF.RD << ", R" << cpu.MEMBUF.RS << ", R"
                  << cpu.MEMBUF.RT << "\t";
          break;
        case 6:
          outfile << "R" << cpu.MEMBUF.RD << ", R" << cpu.MEMBUF.RS << ", "
                  << cpu.MEMBUF.IMM << "\t";
          break;
        case 7:
          outfile << "\t\t";
          break;
        }
        outfile << "\t";
        //Output Stage Cycle Data
        for(int j = 0; j < 5; j++) {
          if(cpu.MEMBUF.stage[j] > 0) {
            outfile << cpu.MEMBUF.stage[j] << " ";
          }
        }
	outfile << endl;
      }
    }
    //Drain Pipeline for Output
    for(int k = 0; k < 10; k++) {
      cpu.StepCycle();
      if(cpu.OUTFLAG == true) {
	PrintOutput();
	//Output to File
	label = false;

	//Check for Label
	for(unsigned int i = 0; i < ltable.size(); i++) {
	  if(ltable[i].pc == cpu.MEMBUF.pc) {
	    outfile << ltable[i].label << "\t" << ITABLE[memory[cpu.MEMBUF.pc]]
		 << "\t";
	    label = true;
	    lindex = i;
	    break;
	  }
	}
	if(label == false) {
          outfile << "\t" << ITABLE[memory[cpu.MEMBUF.pc]] << "\t";
        }
        switch(cpu.MEMBUF.type) {
        case 1:
          outfile << "R" << cpu.MEMBUF.RD << ", " << cpu.EX3BUF.IMM
                  << "(" << cpu.MEMBUF.RS << ")\t";
          break;
        case 2:
          outfile << "R" << cpu.MEMBUF.RD << ", " << cpu.MEMBUF.IMM << "\t\t";
          break;
        case 3:
          outfile << "\tPC: " << cpu.MEMBUF.pc << "\t";
          break;
        case 4:
          outfile << "R" << cpu.MEMBUF.RS << ", R" << cpu.MEMBUF.RT << ", PC: "
                  << cpu.MEMBUF.pc << "\t";
          break;
        case 5:
          outfile << "R" << cpu.MEMBUF.RD << ", R" << cpu.MEMBUF.RS << ", R"
                  << cpu.MEMBUF.RT << "\t";
          break;
        case 6:
          outfile << "R" << cpu.MEMBUF.RD << ", R" << cpu.MEMBUF.RS << ", "
                  << cpu.MEMBUF.IMM << "\t";
          break;
        case 7:
          outfile << "\t\t";
          break;
        }
        outfile << "\t";
        //Output Stage Cycle Data
        for(int j = 0; j < 5; j++) {
          if(cpu.MEMBUF.stage[j] > 0) {
            outfile << cpu.MEMBUF.stage[j] << " ";
          }
        }
	outfile << endl;
      }
    }
  }

  outfile.close();
}

//PrintOutput
void SYSTEM::PrintOutput() {

  //Local Variable Definition
  bool label = false;

  //Check for Label
  for(unsigned int i = 0; i < ltable.size(); i++) {
    if(ltable[i].pc == cpu.MEMBUF.pc) {
      cout << ltable[i].label << "\t" << ITABLE[memory[cpu.MEMBUF.pc]]
	   << "\t";
      label = true;
      break;
    }
  }
  if(label == false) {
    cout << "\t" << ITABLE[memory[cpu.MEMBUF.pc]] << "\t";
  }
  //Output Stage Cycle Data
  for(int j = 0; j < 5; j++) {
    if(cpu.MEMBUF.stage[j] > 0) {
      cout << cpu.MEMBUF.stage[j] << " ";
    }
  }
  cout << endl;
}

//DumpMem
void SYSTEM::DumpMem(int word) {

  if((word % 4) != 0) {
    return;
  }

  int l = 1;
  int k = 0;
  for(int i = 0; i < word; i += 4) {
    k = i + 4;
    cout << "Word " << l << ": ";
    for(int j = i; j < k; j++) {
      cout << memory[j] << " ";
    }
    cout << endl;
    l++;
  }
}

//DumpReg
void SYSTEM::DumpReg() {

  int k = 0;
  for(int i = 0; i < 32; i += 4) {
    k = i + 3;
    cout << "Registers " << i << "-" << k << ":";
    for(int j = i; j <= k; j++) {
      cout << " " << cpu.reg[j];
    }
    cout << endl;
  }
}
