#include <stdio.h>
#include "ast.h"
#include "dyn_str.h"

#define PROLOG do {printf(".IFJcode24");} while(0)

#define MOVE(var, symb) do {printf("\nMOVE %s %s", var, symb);} while(0)
#define CREATEFRAME do {printf("\nCREATEFRAME");} while(0)
#define PUSHFRAME do {printf("\nPUSHFRAME");} while(0)
#define POPFRAME do {printf("\nPOPFRAME");} while(0)
#define DEFVAR(var) do {printf("\nDEFVAR %s", var);} while(0)
#define CALL(label) do {printf("\nCALL %s", label);} while(0)
#define RETURN do {printf("\nRETURN");} while(0)

#define PUSHS(symb) do {printf("\nPUSHS %s", symb);} while(0)
#define POPS(var) do {printf("\nPOPS %s", var);} while(0)
#define CLEARS do {printf("\nCLEARS");} while(0)

#define ADD(var, symb1, symb2) do {printf("\nADD %s %s %s", var, symb1, symb2);} while(0)
#define SUB(var, symb1, symb2) do {printf("\nSUB %s %s %s", var, symb1, symb2);} while(0)
#define MUL(var, symb1, symb2) do {printf("\nMUL %s %s %s", var, symb1, symb2);} while(0)
#define DIV(var, symb1, symb2) do {printf("\nDIV %s %s %s", var, symb1, symb2);} while(0)
#define IDIV(var, symb1, symb2) do {printf("\nIDIV %s %s %s", var, symb1, symb2);} while(0)
#define ADDS(var, symb1, symb2) do {printf("\nADDS %s %s %s", var, symb1, symb2);} while(0)
#define SUBS(var, symb1, symb2) do {printf("\nSUBS %s %s %s", var, symb1, symb2);} while(0)
#define MULS(var, symb1, symb2) do {printf("\nMULS %s %s %s", var, symb1, symb2);} while(0)
#define DIVS(var, symb1, symb2) do {printf("\nDIVS %s %s %s", var, symb1, symb2);} while(0)
#define IDIVS(var, symb1, symb2) do {printf("\nIDIVS %s %s %s", var, symb1, symb2);} while(0)
#define LT(var, symb1, symb2) do {printf("\nLT %s %s %s", var, symb1, symb2);} while(0)
#define GT(var, symb1, symb2) do {printf("\nGT %s %s %s", var, symb1, symb2);} while(0)
#define EQ(var, symb1, symb2) do {printf("\nEQ %s %s %s", var, symb1, symb2);} while(0)
#define LTS(var, symb1, symb2) do {printf("\nLTS %s %s %s", var, symb1, symb2);} while(0)
#define GTS(var, symb1, symb2) do {printf("\nGTS %s %s %s", var, symb1, symb2);} while(0)
#define EQS(var, symb1, symb2) do {printf("\nEQS %s %s %s", var, symb1, symb2);} while(0)
#define AND(var, symb1, symb2) do {printf("\nAND %s %s %s", var, symb1, symb2);} while(0)
#define OR(var, symb1, symb2) do {printf("\nOR %s %s %s", var, symb1, symb2);} while(0)
#define NOT(var, symb) do {printf("\nNOT %s %s", var, symb);} while(0)
#define ANDS(var, symb1, symb2) do {printf("\nANDS %s %s %s", var, symb1, symb2);} while(0)
#define ORS(var, symb1, symb2) do {printf("\nORS %s %s %s", var, symb1, symb2);} while(0)
#define NOTS(var, symb) do {printf("\nNOTS %s %s", var, symb);} while(0)
#define INT2FLOAT(var, symb) do {printf("\nINT2FLOAT %s %s", var, symb);} while(0)
#define FLOAT2INT(var, symb) do {printf("\nFLOAT2INT %s %s", var, symb);} while(0)
#define INT2CHAR(var, symb) do {printf("\nINT2CHAR %s %s", var, symb);} while(0)
#define STRI2INT(var, symb1, symb2) do {printf("\nSTRI2INT %s %s %s", var, symb1, symb2);} while(0)
#define INT2FLOATS(var, symb) do {printf("\nINT2FLOATS %s %s", var, symb);} while(0)
#define FLOAT2INTS(var, symb) do {printf("\nFLOAT2INTS %s %s", var, symb);} while(0)
#define INT2CHARS(var, symb) do {printf("\nINT2CHARS %s %s", var, symb);} while(0)
#define STRI2INTS(var, symb1, symb2) do {printf("\nSTRI2INTS %s %s %s", var, symb1, symb2);} while(0)

#define READ(var, type) do {printf("\nREAD %s %s", var, type);} while(0)
#define WRITE(symb) do {printf("\nWRITE %s", symb);} while(0)

#define CONCAT(var, symb1, symb2) do {printf("\nCONCAT %s %s %s", var, symb1, symb2);} while(0)
#define STRLEN(var, symb) do {printf("\nSTRLEN %s %s", var, symb);} while(0)
#define GETCHAR(var, symb1, symb2) do {printf("\nGETCHAR %s %s %s", var, symb1, symb2);} while(0)
#define SETCHAR(var1, symb1, symb2) do {printf("\nSETCHAR %s %s %s", var1, symb1, symb2);} while(0)

#define TYPE(var, symb) do {printf("\nTYPE %s %s", var, symb);} while(0)

#define LABEL(label) do {printf("\nLABEL %s", label);} while(0)
#define JUMP(label) do {printf("\nJUMP %s", label);} while(0)
#define JUMPIFEQ(label, symb1, symb2) do {printf("\nJUMPIFEQ %s %s %s", label, symb1, symb2);} while(0)
#define JUMPIFNEQ(label, symb1, symb2) do {printf("\nJUMPIFNEQ %s %s %s", label, symb1, symb2);} while(0)
#define JUMPIFEQS(label, symb1, symb2) do {printf("\nJUMPIFEQS %s %s %s", label, symb1, symb2);} while(0)
#define JUMPIFNEQS(label, symb1, symb2) do {printf("\nJUMPIFNEQS %s %s %s", label, symb1, symb2);} while(0)
#define EXIT(exit_code) do {printf("\nEXIT int@(exit_code)");} while(0)

#ifndef CODEGEN_H
#define CODEGEN_H

#define EPILOG EXIT(0)

int codegen(ASTNode* root);


#endif