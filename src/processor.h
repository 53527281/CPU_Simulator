/******************************************************************************
 * File:    processor.h
 * Project: CMSC 411 Project, Summer 2016
 * Author:  Thomas Wallace
 * Date:    25 June 2016
 * Section:
 * E-mail:  thwalla1@umbc.edu
 *
 * Description:
 *          Header file for simplified MIPS computer processor. For detailed
 * description see processor.cpp
 *
 *****************************************************************************/
#include <iostream>
#include <string>
#include <queue>

/*DATA STRUCTURES DECLARATION*************************************************/

//ITABLE - Reference table of implemented instructions
const std::string ITABLE[] = {"NOP","LW","SW","LI","ADD","ADDI","SUB",
                              "SUBI","AND","ANDI","OR","ORI","SLL","SRL",
                              "SRA","J","BNE","BEQ","MULT","MULTI","HLT"};
const int ICOUNT = 21;

//INSTRUCTION - Groups MIPS instruction elements
struct INSTRUCTION {

  int operation;
  int operand[3];

  INSTRUCTION() {
    operation = 0;
    operand[0] = 0;
    operand[1] = 0;
    operand[2] = 0;
  }
};

//BUFFER - Groups pipeline buffer data elements
struct BUFFER {

  bool INV, REGWRITE, MEMWRITE, MEMREAD;
  int STALL;
  int type, op;
  int pc;
  int RD, RS, RT, IMM, rd, rs, rt;
  int stage[5];

  BUFFER(){
    INV = true;
    MEMREAD = false;
    MEMWRITE = false;
    REGWRITE = false;
    STALL = 0;
    type = 0;
    op = 0;
    pc = 0;
    RD = 0;
    RS = 0;
    RS = 0;
    IMM = 0;
    stage[0] = -1;
    stage[1] = -1;
    stage[2] = -1;
    stage[3] = -1;
    stage[4] = -1;
  }

  void SetNOP(){
    INV = true;
    MEMREAD = false;
    MEMWRITE = false;
    REGWRITE = false;
    STALL = 0;
    type = 0;
    op = 0;
    pc = 0;
    RD = 0;
    RS = 0;
    RS = 0;
    IMM = 0;
    stage[0] = -1;
    stage[1] = -1;
    stage[2] = -1;
    stage[3] = -1;
    stage[4] = -1;
  }
};

//IBLOCK - Groups instruction cache data elements
struct BLOCK {

  
  
};

//CACHE - Groups cache data elements
struct CACHE {

  BLOCK block1, block2;

};

//CPU - Groups CPU data elements
struct CPU {

  int cycle;
  int reg[32];
  int pc;
  int FWDA, FWDB;
  bool OUTFLAG, HLTFLAG, FAFLAG, FBFLAG, FETCHPC;
  BUFFER IFtemp, IFBUF, IDBUF, EX1BUF, EX2BUF, EX3BUF, MEMBUF, WBtemp;
  std::queue<INSTRUCTION> ICACHE;
  std::queue<INSTRUCTION> DCACHE;
  std::queue<INSTRUCTION> WBBUF;

  /*FUNCTION DECLARATION******************************************************/
  
  CPU() {
    cycle = 0;
    pc = 0;
    OUTFLAG = true;
    HLTFLAG = false;
    FETCHPC = true;
  }

  //Reset - Resets all buffers and caches
  void Reset();
  
  //StepCycle - Performs one complete pipeline compute cycle
  void StepCycle();

  //Forward - Checks for forwardable data dependancies
  void Forward(int stage);

  //StallCheck - Checks for non-forwardable stall conditions
  void Interlock(int stage);
};
