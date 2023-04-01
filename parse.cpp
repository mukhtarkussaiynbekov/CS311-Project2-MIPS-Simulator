/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   CS311 KAIST                                               */
/*   parse.c                                                   */
/*                                                             */
/***************************************************************/

#include <stdio.h>
#include <bits/stdc++.h>

using namespace std;

#include "util.h"
#include "parse.h"

int text_size;
int data_size;

instruction parsing_instr(const char *buffer, const int index)
{
	string line = buffer;
    instruction instr;
	instr.value = fromBinary(const_cast<char*>(line.c_str()));
	instr.opcode = fromBinary(const_cast<char*>(line.substr(0, 6).c_str()));
	switch (instr.opcode) {
		case 0x2: case 0x3:
			// J type
			instr.r_t.target = fromBinary(const_cast<char*>(line.substr(6).c_str()));
			break;

		case 0x0:
			// R type
			instr.r_t.r_i.rs = fromBinary(const_cast<char*>(line.substr(6, 5).c_str()));
			instr.r_t.r_i.rt = fromBinary(const_cast<char*>(line.substr(11, 5).c_str()));
			instr.r_t.r_i.r_i.r.rd = fromBinary(const_cast<char*>(line.substr(16, 5).c_str()));
			instr.r_t.r_i.r_i.r.shamt = fromBinary(const_cast<char*>(line.substr(21, 5).c_str()));
			instr.func_code = fromBinary(const_cast<char*>(line.substr(26).c_str()));
			break;

		case 0x9:		//ADDIU
	    case 0xc:		//ANDI
	    case 0xf:		//LUI	
	    case 0xd:		//ORI
	    case 0xb:		//SLTIU
	    case 0x23:		//LW	
	    case 0x2b:		//SW
	    case 0x4:		//BEQ
	    case 0x5:		//BNE
			// I type
			instr.r_t.r_i.rs = fromBinary(const_cast<char*>(line.substr(6, 5).c_str()));
			instr.r_t.r_i.rt = fromBinary(const_cast<char*>(line.substr(11, 5).c_str()));
			instr.r_t.r_i.r_i.imm = fromBinary(const_cast<char*>(line.substr(16).c_str()));
			break;
		
		default:
			printf("Not available instruction\n");
			assert(0);
	}
    return instr;
}

void parsing_data(const char *buffer, const int index)
{
	mem_write_32(MEM_DATA_START+index, fromBinary(buffer));
}

void print_parse_result()
{
    int i;
    printf("Instruction Information\n");

    for(i = 0; i < text_size/4; i++)
    {
	printf("INST_INFO[%d].value : %x\n",i, INST_INFO[i].value);
	printf("INST_INFO[%d].opcode : %d\n",i, INST_INFO[i].opcode);

	switch(INST_INFO[i].opcode)
	{
	    //I format
	    case 0x9:		//ADDIU
	    case 0xc:		//ANDI
	    case 0xf:		//LUI	
	    case 0xd:		//ORI
	    case 0xb:		//SLTIU
	    case 0x23:		//LW	
	    case 0x2b:		//SW
	    case 0x4:		//BEQ
	    case 0x5:		//BNE
		printf("INST_INFO[%d].rs : %d\n",i, INST_INFO[i].r_t.r_i.rs);
		printf("INST_INFO[%d].rt : %d\n",i, INST_INFO[i].r_t.r_i.rt);
		printf("INST_INFO[%d].imm : %d\n",i, INST_INFO[i].r_t.r_i.r_i.imm);
		break;

    	    //R format
	    case 0x0:		//ADDU, AND, NOR, OR, SLTU, SLL, SRL, SUBU if JR
		printf("INST_INFO[%d].func_code : %d\n",i, INST_INFO[i].func_code);
		printf("INST_INFO[%d].rs : %d\n",i, INST_INFO[i].r_t.r_i.rs);
		printf("INST_INFO[%d].rt : %d\n",i, INST_INFO[i].r_t.r_i.rt);
		printf("INST_INFO[%d].rd : %d\n",i, INST_INFO[i].r_t.r_i.r_i.r.rd);
		printf("INST_INFO[%d].shamt : %d\n",i, INST_INFO[i].r_t.r_i.r_i.r.shamt);
		break;

    	    //J format
	    case 0x2:		//J
	    case 0x3:		//JAL
		printf("INST_INFO[%d].target : %d\n",i, INST_INFO[i].r_t.target);
		break;

	    default:
		printf("Not available instruction\n");
		assert(0);
	}
    }

    printf("Memory Dump - Text Segment\n");
    for(i = 0; i < text_size; i+=4)
	printf("text_seg[%d] : %x\n", i, mem_read_32(MEM_TEXT_START + i));
    for(i = 0; i < data_size; i+=4)
	printf("data_seg[%d] : %x\n", i, mem_read_32(MEM_DATA_START + i));
    printf("Current PC: %x\n", CURRENT_STATE.PC);
}
