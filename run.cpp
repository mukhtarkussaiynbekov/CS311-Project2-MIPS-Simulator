/***************************************************************/
/*                                                             */
/*   MIPS-32 Instruction Level Simulator                       */
/*                                                             */
/*   CS311 KAIST                                               */
/*   run.c                                                     */
/*                                                             */
/***************************************************************/

#include <stdio.h>

#include "util.h"
#include "run.h"
#include <iostream>
using namespace std;

uint32_t sign_ext_imm(short);
uint32_t zero_ext_imm(short);
uint32_t calculate_branch_address(short imm);

/***************************************************************/
/*                                                             */
/* Procedure: get_inst_info                                    */
/*                                                             */
/* Purpose: Read insturction information                       */
/*                                                             */
/***************************************************************/
instruction* get_inst_info(uint32_t pc) 
{ 
    return &INST_INFO[(pc - MEM_TEXT_START) >> 2];
}

/***************************************************************/
/*                                                             */
/* Procedure: process_instruction                              */
/*                                                             */
/* Purpose: Process one instrction                             */
/*                                                             */
/***************************************************************/
void process_instruction(){
    uint32_t upper_bits;
    uint32_t lower_bits;
    
	instruction *inst_ptr = get_inst_info(CURRENT_STATE.PC);
    switch (inst_ptr->opcode) {
        case 0x3:
            // JAL
            CURRENT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
        case 0x2:
            // J
            upper_bits = (CURRENT_STATE.PC + 4) & (0xf << 28);
            lower_bits = inst_ptr->r_t.target << 2;
            CURRENT_STATE.PC = upper_bits | lower_bits;
            CURRENT_STATE.PC -= 4;
            break;
        case 0x4:
            // BEQ
            if (CURRENT_STATE.REGS[inst_ptr->r_t.r_i.rs] == CURRENT_STATE.REGS[inst_ptr->r_t.r_i.rt])
                CURRENT_STATE.PC += calculate_branch_address(inst_ptr->r_t.r_i.r_i.imm);
            break;
        case 0x5:
            // BNE
            if (CURRENT_STATE.REGS[inst_ptr->r_t.r_i.rs] != CURRENT_STATE.REGS[inst_ptr->r_t.r_i.rt])
                CURRENT_STATE.PC += calculate_branch_address(inst_ptr->r_t.r_i.r_i.imm);
            break;
        case 0x9:
            // ADDIU
            CURRENT_STATE.REGS[inst_ptr->r_t.r_i.rt] = CURRENT_STATE.REGS[inst_ptr->r_t.r_i.rs] + sign_ext_imm(inst_ptr->r_t.r_i.r_i.imm);
            break;
        case 0xc:
            // ANDI
            CURRENT_STATE.REGS[inst_ptr->r_t.r_i.rt] = CURRENT_STATE.REGS[inst_ptr->r_t.r_i.rs] & zero_ext_imm(inst_ptr->r_t.r_i.r_i.imm);
            break;
        case 0xf:
            // LUI
            CURRENT_STATE.REGS[inst_ptr->r_t.r_i.rt] = zero_ext_imm(inst_ptr->r_t.r_i.r_i.imm) << 16;
            break;
        case 0x23:
            // LW
            CURRENT_STATE.REGS[inst_ptr->r_t.r_i.rt] = mem_read_32(CURRENT_STATE.REGS[inst_ptr->r_t.r_i.rs] + sign_ext_imm(inst_ptr->r_t.r_i.r_i.imm));
            break;
        case 0xd:
            // ORI
            CURRENT_STATE.REGS[inst_ptr->r_t.r_i.rt] = CURRENT_STATE.REGS[inst_ptr->r_t.r_i.rs] | zero_ext_imm(inst_ptr->r_t.r_i.r_i.imm);
            break;
        case 0xb:
            // SLTIU
            CURRENT_STATE.REGS[inst_ptr->r_t.r_i.rt] = CURRENT_STATE.REGS[inst_ptr->r_t.r_i.rs] < sign_ext_imm(inst_ptr->r_t.r_i.r_i.imm) ? 1 : 0;
            break;
        case 0x2b:
            mem_write_32(CURRENT_STATE.REGS[inst_ptr->r_t.r_i.rs] + sign_ext_imm(inst_ptr->r_t.r_i.r_i.imm), CURRENT_STATE.REGS[inst_ptr->r_t.r_i.rt]);
            break;
        case 0x0:
            // R type
            switch (inst_ptr->func_code) {
                case 0x8:
                    // JR
                    CURRENT_STATE.PC = CURRENT_STATE.REGS[inst_ptr->r_t.r_i.rs];
                    CURRENT_STATE.PC -= 4;
                    break;
                case 0x21:
                    // ADDU
                    CURRENT_STATE.REGS[inst_ptr->r_t.r_i.r_i.r.rd] = CURRENT_STATE.REGS[inst_ptr->r_t.r_i.rs] + CURRENT_STATE.REGS[inst_ptr->r_t.r_i.rt];
                    break;
                case 0x24:
                    // AND
                    CURRENT_STATE.REGS[inst_ptr->r_t.r_i.r_i.r.rd] = CURRENT_STATE.REGS[inst_ptr->r_t.r_i.rs] & CURRENT_STATE.REGS[inst_ptr->r_t.r_i.rt];
                    break;
                case 0x25:
                    // OR
                    CURRENT_STATE.REGS[inst_ptr->r_t.r_i.r_i.r.rd] = CURRENT_STATE.REGS[inst_ptr->r_t.r_i.rs] | CURRENT_STATE.REGS[inst_ptr->r_t.r_i.rt];
                    break;
                case 0x27:
                    // NOR
                    CURRENT_STATE.REGS[inst_ptr->r_t.r_i.r_i.r.rd] = ~(CURRENT_STATE.REGS[inst_ptr->r_t.r_i.rs] | CURRENT_STATE.REGS[inst_ptr->r_t.r_i.rt]);
                    break;
                case 0x00:
                    // SLL
                    CURRENT_STATE.REGS[inst_ptr->r_t.r_i.r_i.r.rd] = CURRENT_STATE.REGS[inst_ptr->r_t.r_i.rt] << inst_ptr->r_t.r_i.r_i.r.shamt;
                    break;
                case 0x02:
                    // SRL
                    CURRENT_STATE.REGS[inst_ptr->r_t.r_i.r_i.r.rd] = CURRENT_STATE.REGS[inst_ptr->r_t.r_i.rt] >> inst_ptr->r_t.r_i.r_i.r.shamt;
                    break;
                case 0x23:
                    // SUBU
                    CURRENT_STATE.REGS[inst_ptr->r_t.r_i.r_i.r.rd] = CURRENT_STATE.REGS[inst_ptr->r_t.r_i.rs] - CURRENT_STATE.REGS[inst_ptr->r_t.r_i.rt];
                    break;
                case 0x2b:
                    // SLTU
                    CURRENT_STATE.REGS[inst_ptr->r_t.r_i.r_i.r.rd] = CURRENT_STATE.REGS[inst_ptr->r_t.r_i.rs] < CURRENT_STATE.REGS[inst_ptr->r_t.r_i.rt] ? 1 : 0;
                    break;
            }
            break;
        
        default:
			printf("Not available instruction\n");
			assert(0);
    }
    if (((CURRENT_STATE.PC - MEM_TEXT_START) >> 2) == NUM_INST) {
        RUN_BIT = FALSE;
    } else {
        CURRENT_STATE.PC += 4;
    }
}

uint32_t calculate_branch_address(short imm) {
    return ((int) imm) << 2;
}

uint32_t sign_ext_imm(short imm) {
    return (int) imm;
}

uint32_t zero_ext_imm(short imm) {
    return (uint16_t) imm;
}