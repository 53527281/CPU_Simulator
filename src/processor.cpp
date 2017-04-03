/******************************************************************************
 * File:    processor.cpp
 * Project: CMSC 411 Project, Summer 2016
 * Author:  Thomas Wallace
 * Date:    25 June 2016
 * Section:
 * E-mail:  thwalla1@umbc.edu
 *
 * Description:
 *          Definition file for simplified MIPS computer processor.
 *
 *****************************************************************************/
#include "processor.h"

using namespace std;

/*FUNCTION DEFINITION*********************************************************/

void CPU::Reset() {


}

void CPU::StepCycle() {

  //Local Variable Definition
  bool INVNEXT = false;
  int prevpc = pc;
  INSTRUCTION itemp;

  //Advance Cycle
  cycle++;

  //Assume Pipeline Stall
  FETCHPC = false;

  /*Step WB*******************************************************************/
  if(MEMBUF.INV == false) {

    //Check for Register Write-Back Operation
    if(MEMBUF.REGWRITE == true) {
      reg[MEMBUF.RD] = MEMBUF.IMM;
    }
  }

  //Check for Output
  if(EX3BUF.type == 0) {
    OUTFLAG = false;
  }
  else {
    OUTFLAG = true;
    WBtemp = MEMBUF;
  }

  /*Step MEM******************************************************************/
  if(EX3BUF.STALL == 0) {
    if(EX3BUF.INV == false) {
      
      //Check for Memory Read
      if(EX3BUF.MEMREAD == true) {
	Interlock(4);
	itemp = DCACHE.front();
	DCACHE.pop();
	EX3BUF.IMM = itemp.operand[2];
      }
      //Check for Memory Write
      if(EX3BUF.MEMWRITE == true) {
	itemp.operand[0] = EX3BUF.IMM;
	itemp.operand[2] = EX3BUF.rd;
	WBBUF.push(itemp);
      }
      //Set MEM Stage Cycle
      EX3BUF.stage[3] = cycle;      
      EX3BUF.stage[4] = (cycle + 1);
    }
    MEMBUF = EX3BUF;
    EX3BUF.SetNOP();
  }
  else {
    MEMBUF.STALL--;
    MEMBUF.SetNOP();
    return;
  }

  //Forward MEM
  if(MEMBUF.REGWRITE) {
    Forward(4);
  }


  /*Step EX3******************************************************************/
  if(EX2BUF.STALL == 0) {
    if(EX2BUF.INV == false) {

      //Check for Interlock
      Interlock(3);
      
      //Check for Multiplication Register
      if(EX2BUF.op == 18) {
	EX2BUF.IMM = EX2BUF.rs * EX2BUF.rt;
      }
      //Check for Multiplication Immediate
      if(EX2BUF.op == 19) {
        EX2BUF.IMM = EX2BUF.rs * EX2BUF.IMM;
      }
      //Set EX Stage Cycle
      EX2BUF.stage[2] = cycle;
    }
    EX3BUF = EX2BUF;
    EX2BUF.SetNOP();
  }
  else {
    EX3BUF.STALL--;
    EX3BUF.SetNOP();
    return;
  }

  //Forward EX3
  if(EX3BUF.REGWRITE) {
    Forward(3);
  }

  /*Step EX2BUF***************************************************************/
  if(EX1BUF.STALL == 0) {
    if(EX1BUF.INV == false) {

      //Check for Interlock
      Interlock(2);
      
      switch(EX1BUF.op){
      //Check for Add Register
      case 4:
        EX1BUF.IMM = EX1BUF.rs + EX1BUF.rt;
	break;
      //Check for Add Immediate
      case 5:
        EX1BUF.IMM = EX1BUF.rs + EX1BUF.IMM;
	break;
      //Check for Subtract Register
      case 6:
        EX1BUF.IMM = EX1BUF.rs - EX1BUF.rt;
	break;
      //Check for Subtract Immediate
      case 7:
        EX1BUF.IMM = EX1BUF.rs - EX1BUF.IMM;
	break;
      }
    }
    EX2BUF = EX1BUF;
    EX1BUF.SetNOP();
  }
  else {
    EX2BUF.STALL--;
    EX2BUF.SetNOP();
    return;
  }

  //Forward EX2
  if(EX2BUF.REGWRITE) {
    Forward(2);
  }

  /*Step EX1BUF***************************************************************/
  if(IDBUF.STALL == 0) {
    if(IDBUF.INV == false) {

      //Local Variable Definition
      int intSize;

      //Check Memory Interlock
      Interlock(1);

      switch(IDBUF.op){
      //Check for Load Word
      case 1:
	IDBUF.IMM = IDBUF.IMM + IDBUF.rs;
	break;
      //Check for Store Word
      case 2:
        IDBUF.IMM = IDBUF.IMM + IDBUF.rs;
	break;
      //Check for Bitwise AND Register
      case 8:
        IDBUF.IMM = IDBUF.rs & IDBUF.rt;
	break;
      //Check for Bitwise AND Immediate
      case 9:
        IDBUF.IMM = IDBUF.rs & IDBUF.IMM;
	break;
      //Check for Bitwise OR Register
      case 10:
        IDBUF.IMM = IDBUF.rs | IDBUF.rt;
	break;
      //Check for Bitwise OR Immediate
      case 11:
        IDBUF.IMM = IDBUF.rs | IDBUF.IMM;
	break;
      //Check for Logical Shift Left
      case 12:
        IDBUF.IMM = IDBUF.rs << IDBUF.IMM;
	break;
      //Check for Logical Shift Right
      case 13:
        IDBUF.IMM = IDBUF.rs >> IDBUF.IMM;
	break;
      //Check for Arithmetic Shift Right
      case 14:
	intSize = sizeof(int) * 8;
	intSize = ~(((0x1 << intSize) >> IDBUF.IMM) << 1);
	IDBUF.IMM = (IDBUF.rs >> IDBUF.IMM) & intSize;
	break;
      } 
    }
    EX1BUF = IDBUF;
    IDBUF.SetNOP();
  }
  else {
    //Insert NOP Bubble
    EX1BUF.STALL--;
    EX1BUF.SetNOP();
    return;
  }

  //Forward EX1
  if(EX1BUF.REGWRITE) {
    Forward(1);
  }

  /*Step IDBUF****************************************************************/
  if(IFBUF.STALL == 0) {

    //Incriment Program Counter
    prevpc = pc;
    pc += 4;
    FETCHPC = true;

    if(IFBUF.INV == false) {
   
      //Set Instruction Types for Output

      //NOP Type
      if(IFBUF.op == 0) {
	IFBUF.type = 0;
      }
      //Memory Access Type
      else if((IFBUF.op == 1) || (IFBUF.op == 2)) {
	IFBUF.type = 1;
      }
      //Load Immediate Type
      else if(IFBUF.op == 3) {
	IFBUF.type = 2;
	reg[IFBUF.RD] = IFBUF.IMM;
      }
      //Jump Type
      else if(IFBUF.op == 15) {
	IFBUF.type = 3;
	pc = IFBUF.IMM;
	IFBUF.INV = true;
	INVNEXT = true;
      }
      //Branch Type
      else if((IFBUF.op == 16) || (IFBUF.op == 17)) {
	IFBUF.type = 4;
      }
      //Halt Type
      else if(IFBUF.op == 20) {
	IFBUF.type = 7;
        HLTFLAG = true;
        FETCHPC = false;
      }
      //Register Type
      else if((IFBUF.op == 4) || (IFBUF.op == 6) || (IFBUF.op == 8) ||
	 (IFBUF.op == 10) || (IFBUF.op == 18)) {
	IFBUF.type = 5;
	IFBUF.RT = IFBUF.IMM;
	IFBUF.IMM = 0;
	IFBUF.rs = reg[IFBUF.RS];
	IFBUF.rt = reg[IFBUF.RT];
	IFBUF.REGWRITE = true;
      }
      //Immediate Type
      else {
	IFBUF.type = 6;
	IFBUF.rs = reg[IFBUF.RS];
	IFBUF.REGWRITE = true;
      }

      //Check Forward
      if(FAFLAG) {
	IFBUF.rs = FWDA;
      }
      if(FBFLAG) {
	IFBUF.rt = FWDB;
      }


      switch(IFBUF.op){
      //Check for Load Word
      case 1:
	IFBUF.MEMREAD = true;
	IFBUF.REGWRITE = true;
	IFBUF.rs = reg[IFBUF.RS];
	break;
      //Check for Store Word
      case 2:
	IFBUF.MEMWRITE = true;
	IFBUF.rs = reg[IFBUF.RS];
	break;
      //Check for Branch Not Equal
      case 16:
	IFBUF.INV = true;
	IFBUF.RT = IFBUF.RS;
	IFBUF.RS = IFBUF.RD;
	IFBUF.rs = reg[IFBUF.RS];
	IFBUF.rt = reg[IFBUF.RT];
	if(IFBUF.rs != IFBUF.rt) {
	  pc = IFBUF.IMM;
	  INVNEXT = true;
	}
	break;
      //Check for Branch Equal
      case 17:
	IFBUF.INV = true;
	IFBUF.RT = IFBUF.RS;
	IFBUF.RS = IFBUF.RD;
	IFBUF.rs = reg[IFBUF.RS];
        IFBUF.rt = reg[IFBUF.RT];
        if(IFBUF.rs == IFBUF.rt) {
          pc = IFBUF.IMM;
	  INVNEXT = true;
        }
	break;
      //Check for Halt
      case 20:

	IFBUF.INV = true;
	INVNEXT = true;

	//Clear Caches
	ICACHE.pop();
	
	break;
      }
      //Set ID Stage Cycle
      IFBUF.stage[1] = cycle;
    }
    IDBUF = IFBUF;
    IFBUF.SetNOP();
    
  }
  else {
    //Insert NOP Bubble
    IDBUF.STALL--;
    IDBUF.SetNOP();
    return;
  }

  //Clear Forward Flags
  FAFLAG = false;
  FBFLAG = false;

  /*Step IF*******************************************************************/
  if(!ICACHE.empty() && (HLTFLAG == false)) {
    
    //Fetch Next Instruction
    itemp = ICACHE.front();
    ICACHE.pop();
    
    //Set Next Instruction
    IFtemp.INV = false;
    IFtemp.STALL = false;
    IFtemp.MEMREAD = false;
    IFtemp.MEMWRITE = false;
    IFtemp.REGWRITE = false;
    IFtemp.pc = prevpc;
    IFtemp.op = itemp.operation;
    IFtemp.RD = itemp.operand[0];
    IFtemp.RS = itemp.operand[1];
    IFtemp.IMM = itemp.operand[2];
    
    IFtemp.stage[0] = cycle;

    //Invalidate if Branch Taken
    if(INVNEXT) {
      IFtemp.INV = true;
    }
    IFBUF = IFtemp;
  }
  else {
    IFBUF.SetNOP();
  }
}

void CPU::Forward(int stage) {

  switch(stage){
  //Forward EX1 to ID
  case 1:
    if(EX1BUF.RD == IFBUF.RS) {
      FAFLAG = true;
      FWDA = EX1BUF.IMM;
    }
    if(EX1BUF.RD == IFBUF.RT) {
      FBFLAG = true;
      FWDB = EX1BUF.IMM;
    }
    break;
  //Forward EX2 to EX1, ID
  case 2:
    if(EX2BUF.RD == IDBUF.RS) {
      IDBUF.rs = EX2BUF.IMM;
    }
    if(EX2BUF.RD == IDBUF.RT) {
      IDBUF.rt = EX2BUF.IMM;
    }
    if(EX2BUF.RD == IFBUF.RS) {
      FAFLAG = true;
      FWDA = EX2BUF.IMM;
    }
    if(EX2BUF.RD == IFBUF.RT) {
      FBFLAG = true;
      FWDB = EX2BUF.IMM;
    }
    break;
  //Forward EX3 to EX1, ID
  case 3:
    if(EX3BUF.RD == IDBUF.RS) {
      IDBUF.rs = EX3BUF.IMM;
    }
    if(EX3BUF.RD == IDBUF.RT) {
      IDBUF.rt = EX3BUF.IMM;
    }
    if(EX3BUF.RD == IFBUF.RS) {
      FAFLAG = true;
      FWDA = EX3BUF.IMM;
    }
    if(EX3BUF.RD == IFBUF.RT) {
      FBFLAG = true;
      FWDB = EX3BUF.IMM;
    }
    break;
  //Forward MEM to EX1, ID
  case 4:
    if(MEMBUF.RD == IDBUF.RS) {
      IDBUF.rs = MEMBUF.IMM;
    }
    if(MEMBUF.RD == IDBUF.RT) {
      IDBUF.rt = MEMBUF.IMM;
    }
    if(MEMBUF.RD == IFBUF.RS) {
      FAFLAG = true;
      FWDA = MEMBUF.IMM;
    }
    if(MEMBUF.RD == IFBUF.RT) {
      FBFLAG = true;
      FWDB = MEMBUF.IMM;
    }
    break;
  }
}

//Interlock
void CPU::Interlock(int stage) {

  switch(stage){
    //Check Stall EX3 Stage
  case 4:
    if(EX3BUF.MEMREAD && ((EX3BUF.RD == IFBUF.RS) ||
			  (EX3BUF.RD == IFBUF.RT)) && IFBUF.INV == false){
      
      //cout << "STALLING ID " << IDBUF.op << " FROM MEM" << endl;
      IFBUF.STALL = 0;
    }
    break;
  case 3:
    if(EX2BUF.MEMREAD && ((EX2BUF.RD == IFBUF.RS) ||
                          (EX2BUF.RD == IFBUF.RT)) && IFBUF.INV == false){

      //cout << "STALLING ID " << IDBUF.op << " FROM MEM" << endl;
      IFBUF.STALL = 1;
    }
    break;
  case 2:
    if(EX1BUF.MEMREAD && ((EX1BUF.RD == IFBUF.RS) ||
                          (EX1BUF.RD == IFBUF.RT)) && IFBUF.INV == false){

      //cout << "STALLING ID " << IDBUF.op << " FROM MEM" << endl;
      IFBUF.STALL = 2;
    }
    break;
  case 1:
    if(IDBUF.MEMREAD && ((IDBUF.RD == IFBUF.RS) ||
			 (IDBUF.RD == IFBUF.RT)) && IFBUF.INV == false){

      //cout << "STALLING ID " << IDBUF.op << " FROM MEM" << endl;
      IFBUF.STALL = 3;
    }
    break;
  }
}
