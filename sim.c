#include <stdio.h>
#include "shell.h"

void process_instruction()
{
    /* execute one instruction here. You should use CURRENT_STATE and modify
     * values in NEXT_STATE. You can call mem_read_32() and mem_write_32() to
     * access memory. */
    //取指令
    uint32_t inst=mem_read_32(CURRENT_STATE.PC);
    //解析指令
    uint8_t
        op=inst>>26,
        rs=(inst>>21)& 0x1F,
        rt=(inst>>16)& 0x1F,
        rd=(inst>>11)& 0x1F,
        shamt=(inst>>6)& 0x1F,
        func=inst& 0x3F;
    uint16_t imm=inst& 0xFFFF;
    uint32_t target=inst& 0x3FFFFFFF;
    //判断指令类型并执行
    switch (op)
    {
    case 0x00:
        switch (func)
        {
        case 0x00: // sll
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << shamt;
            break;
        case 0x02: // srl
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> shamt;
            break;
        case 0x03: // sra
            NEXT_STATE.REGS[rd] = (int32_t)CURRENT_STATE.REGS[rt] >> shamt;
            break;
        case 0x04: // sllv
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] << (CURRENT_STATE.REGS[rs] & 0x1F);
            break;
        case 0x06: // srlv
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rt] >> (CURRENT_STATE.REGS[rs] & 0x1F);
            break;
        case 0x07: // srav
            NEXT_STATE.REGS[rd] = (int32_t)CURRENT_STATE.REGS[rt] >> (CURRENT_STATE.REGS[rs] & 0x1F);
            break;
        case 0x08: // jr
            NEXT_STATE.PC = CURRENT_STATE.REGS[rs];
            break;
        case 0x09: // jalr
            NEXT_STATE.PC = CURRENT_STATE.REGS[rs];
            NEXT_STATE.REGS[rd] = CURRENT_STATE.PC + 4;
            break;
        case 0x0C: // syscall
            if (CURRENT_STATE.REGS[2] = 0x0A)
                RUN_BIT = 0;
            break;
        case 0x10: // mfhi
            NEXT_STATE.REGS[rd] = CURRENT_STATE.HI;
            break;
        case 0x12: // mflo
            NEXT_STATE.REGS[rd] = CURRENT_STATE.LO;
            break;
        case 0x11: // mthi
            NEXT_STATE.HI = CURRENT_STATE.REGS[rs];
            break;
        case 0x13: // mtlo
            NEXT_STATE.LO = CURRENT_STATE.REGS[rs];
            break;
        case 0x18: // mult
            int64_t result0 = (int64_t)CURRENT_STATE.REGS[rs] * (int64_t)CURRENT_STATE.REGS[rt];
            NEXT_STATE.HI = (uint32_t)(result0 >> 32);
            NEXT_STATE.LO = (uint32_t)result0;
            break;
        case 0x19: // multu
            uint64_t result1 = (uint64_t)CURRENT_STATE.REGS[rs] * (uint64_t)CURRENT_STATE.REGS[rt];
            NEXT_STATE.HI = (uint32_t)(result1 >> 32);
            NEXT_STATE.LO = (uint32_t)result1;
            break;
        case 0x1A: // div
            if (CURRENT_STATE.REGS[rt] != 0)
            {
                NEXT_STATE.LO = (int32_t)CURRENT_STATE.REGS[rs] / (int32_t)CURRENT_STATE.REGS[rt];
                NEXT_STATE.HI = (int32_t)CURRENT_STATE.REGS[rs] % (int32_t)CURRENT_STATE.REGS[rt];
            }
            break;
        case 0x1B: // divu
            if (CURRENT_STATE.REGS[rt] != 0)
            {
                NEXT_STATE.LO = CURRENT_STATE.REGS[rs] / CURRENT_STATE.REGS[rt];
                NEXT_STATE.HI = CURRENT_STATE.REGS[rs] % CURRENT_STATE.REGS[rt];
            }
            break;
        case 0x20://add
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
            break;
        case 0x21: // addu
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] + CURRENT_STATE.REGS[rt];
            break;
        case 0x22: // sub
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
            break;
        case 0x23: // subu
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] - CURRENT_STATE.REGS[rt];
            break;
        case 0x25: //or
            NEXT_STATE.REGS[rd] = CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt];
            break;
        case 0x27: //nor
            NEXT_STATE.REGS[rd] = ~(CURRENT_STATE.REGS[rs] | CURRENT_STATE.REGS[rt]);
        default:
            printf("未知R型指令\n");
        }
        break;
    case 0x02:
        NEXT_STATE.PC = (CURRENT_STATE.PC & 0xF0000000) | target;
        break;
    case 0x03:
        NEXT_STATE.PC = (CURRENT_STATE.PC & 0xF0000000) | target;
        NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
        break;
    case 0x01: // bltz, bgez, bltzal, bgezal
        uint16_t offset = imm << 2;
        switch (rt)
        {
        case 0x00: // bltz
            if ((int32_t)CURRENT_STATE.REGS[rs] < 0)
            {
                NEXT_STATE.PC = CURRENT_STATE.PC + 4 + offset;
            }
            break;
        case 0x01: // bgez
            if ((int32_t)CURRENT_STATE.REGS[rs] >= 0)
            {
                NEXT_STATE.PC = CURRENT_STATE.PC + 4 + offset;
            }
            break;
        case 0x10: // bltzal
            if ((int32_t)CURRENT_STATE.REGS[rs] < 0)
            {
                NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
                NEXT_STATE.PC = CURRENT_STATE.PC + 4 + offset;
            }
            break;
        case 0x11: // bgezal
            if ((int32_t)CURRENT_STATE.REGS[rs] >= 0)
            {
                NEXT_STATE.REGS[31] = CURRENT_STATE.PC + 4;
                NEXT_STATE.PC = CURRENT_STATE.PC + 4 + offset;
            }
            break;
        default:
            printf("未知的分支指令\n");
            break;
        }
    case 0x04: // beq
        if (CURRENT_STATE.REGS[rs] == CURRENT_STATE.REGS[rt])
        {
            NEXT_STATE.PC = CURRENT_STATE.PC + 4 + (imm << 2);
        }
        break;
    case 0x05: // bne
        if (CURRENT_STATE.REGS[rs] != CURRENT_STATE.REGS[rt])
        {
            NEXT_STATE.PC = CURRENT_STATE.PC + 4 + (imm << 2);
        }
        break;
    case 0x06: // blez
        if (CURRENT_STATE.REGS[rs] <= 0)
        {
            NEXT_STATE.PC = CURRENT_STATE.PC + 4 + (imm << 2);
        }
        break;
    case 0x07: // bgtz
        if (CURRENT_STATE.REGS[rs] > 0)
        {
            NEXT_STATE.PC = CURRENT_STATE.PC + 4 + (imm << 2);
        }
        break;
    case 0x08: // addi
        NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + imm;
        break;
    case 0x09: // addiu
        NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] + imm;
        break;
    case 0x0A: // slti
        NEXT_STATE.REGS[rt] = (int32_t)CURRENT_STATE.REGS[rs] < imm ? 1 : 0;
        break;
    case 0x0B: // sltiu
        NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] < (uint32_t)imm ? 1 : 0;
        break;
    case 0x0C: // andi
        NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] & (uint32_t)imm;
        break;
    case 0x0D: // ori
        NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] | (uint32_t)imm;
        break;
    case 0x0E: // xori
        NEXT_STATE.REGS[rt] = CURRENT_STATE.REGS[rs] ^ (uint32_t)imm;
        break;
    case 0x0F: // lui
        NEXT_STATE.REGS[rt] = imm << 16;
        break;
    case 0x20: // lb
        int32_t address0 = CURRENT_STATE.REGS[rs] + imm;
        uint32_t data0 = mem_read_32(address0);
        if (data0 & 0x80)
        {
            data0 |= 0xFFFFFF00;
        }
        NEXT_STATE.REGS[rt] = data0;
        break;
    case 0x21: // lh
        int32_t address1 = CURRENT_STATE.REGS[rs] + imm;
        uint32_t data1 = mem_read_32(address1);
        if (data1 & 0x8000)
        {
            data1 |= 0xFFFF0000;
        }
        NEXT_STATE.REGS[rt] = data1;
        break;
    case 0x23: // lw
        int32_t address2 = CURRENT_STATE.REGS[rs] + imm;
        uint32_t data2 = mem_read_32(address2);
        NEXT_STATE.REGS[rt] = data2;
        break;
    case 0x24: // lbu
        int32_t address3 = CURRENT_STATE.REGS[rs] + imm;
        uint32_t data3 = mem_read_32(address3);
        NEXT_STATE.REGS[rt] = data3;
        break;
    case 0x25: // lhu
        int32_t address4 = CURRENT_STATE.REGS[rs] + imm;
        uint32_t data4 = mem_read_32(address4);
        NEXT_STATE.REGS[rt] = data4;
        break;
    case 0x28: // sb
        int32_t address5 = CURRENT_STATE.REGS[rs] + imm;
        uint8_t data5 = CURRENT_STATE.REGS[rt] & 0xFF;
        mem_write_32(address5, data5);
        break;
    case 0x29: // sh
        int32_t address6 = CURRENT_STATE.REGS[rs] + imm;
        uint16_t data6 = CURRENT_STATE.REGS[rt] & 0xFFFF;
        mem_write_32(address6, data6);
        break;
    case 0x2B: // sw
        int32_t address7 = CURRENT_STATE.REGS[rs] + imm;
        uint32_t data7 = CURRENT_STATE.REGS[rt];
        mem_write_32(address7, data7);
        break;
    default:
        printf("未知指令\n");
    }
    //下一条指令
    NEXT_STATE.PC = CURRENT_STATE.PC + 4;

}
