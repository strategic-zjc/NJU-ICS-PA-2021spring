#include "cpu/cpu.h"
#include <assert.h>

void set_ZF(uint32_t result, size_t data_size){
    result = result & (0xFFFFFFFF >> (32 - data_size));
    cpu.eflags.ZF = (result == 0);
}
void set_SF(uint32_t result, size_t data_size){
    result = sign_ext(result & (0xFFFFFFFF >> (32 - data_size)), data_size);
    cpu.eflags.SF = sign(result);
}
void set_PF(uint32_t result){
    uint32_t bit_check = 0x1;
    uint32_t bit_count = 0;
    for(int i = 0; i < 8; i++){
        if((result & bit_check) == bit_check) bit_count++;
        bit_check = bit_check << 1;
    }
    cpu.eflags.PF = ((bit_count % 2) == 0);
}

void set_CF_add(uint32_t result, uint32_t src, size_t data_size){
    result = sign_ext(result & (0xFFFFFFFF >> (32 - data_size)), data_size);
    src = sign_ext(src & (0xFFFFFFFF >> (32 - data_size)), data_size);
    cpu.eflags.CF = result < src;
}
void set_CF_adc(uint32_t result, uint32_t src, size_t data_size){
    result = sign_ext(result & (0xFFFFFFFF >> (32 - data_size)), data_size);
    src = sign_ext(src & (0xFFFFFFFF >> (32 - data_size)), data_size);
    if(cpu.eflags.CF == 1){
        cpu.eflags.CF = (result <= src);
    }else{
        cpu.eflags.CF = (result < src);
    }
}
void set_OF_add(uint32_t result, uint32_t src , uint32_t dest, size_t data_size){
    switch(data_size){
        case 8:
            result = sign_ext(result & 0xFF, 8);
            src = sign_ext(src & 0xFF, 8);
            dest = sign_ext(dest & 0xFF, 8);
            break;
        case 16:
            result = sign_ext(result & 0xFFFF, 16);
            src = sign_ext(src & 0xFFFF, 16);
            dest = sign_ext(dest & 0xFFFF, 16);
            break;
        default:
            break;
    }
    if (sign(src) == sign(dest)){
        if(sign(src) != sign(result)){
            cpu.eflags.OF = 1;
        }else{
            cpu.eflags.OF =0;
        }
    }else{
        cpu.eflags.OF =0;
    }
}
void set_OF_adc(uint32_t result, uint32_t src, uint32_t dest, size_t data_size){
    set_OF_add(result, src, dest, data_size);
}


uint32_t alu_add(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_add(src, dest, data_size);
#else
	/*printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);*/
	uint32_t res = 0;
	res = src + dest;
	set_CF_add(res, src, data_size);
	set_PF(res);
	set_ZF(res, data_size);
	set_SF(res, data_size);
	set_OF_add(res, src, dest, data_size);
	return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}

uint32_t alu_adc(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_adc(src, dest, data_size);
#else
	/*printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);*/
	uint32_t res = 0;
	uint32_t CF = cpu.eflags.CF; 
	res = src + dest + CF;
	set_PF(res);
	set_ZF(res , data_size);
	set_SF(res , data_size);
	set_CF_adc(res, src, data_size);
	set_OF_adc(res, src, dest, data_size);
	return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}

uint32_t to_complement(uint32_t src){
    uint32_t res = ~src + 1;
    return res;
}
void set_CF_sub(uint32_t result, uint32_t dest, size_t data_size){
    dest = sign_ext(dest & (0xFFFFFFFF >> (32 - data_size)), data_size);
    result = sign_ext(result & (0xFFFFFFFF >> (32 - data_size)), data_size);
    cpu.eflags.CF = (result > dest);
    // because when src subtracts more from dest, it comes to set CF to 1
}
void set_OF_sub(uint32_t result, uint32_t src, uint32_t dest, size_t data_size){
    src = ~src;
    set_OF_add(result, src, dest, data_size);
}
uint32_t alu_sub(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sub(src, dest, data_size);
#else
	/*printf("\e[0;31mPlease implement me at al.c\e[0m\n");
	fflush(stdout);
	assert(0);*/
	uint32_t res = 0;
	res = dest + to_complement(src);
	set_PF(res);
	set_SF(res, data_size);
	set_ZF(res, data_size);
	set_CF_sub(res, dest, data_size);
	//set_OF_add(res, ~src + 1, dest, data_size); // see OF's rule in theoretical class. --> why (src - 1)
	set_OF_sub(res, src, dest, data_size); 
	return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}
void set_CF_sbb(uint32_t result, uint32_t dest, size_t data_size){
    dest = sign_ext(dest & (0xFFFFFFFF >> (32 - data_size)), data_size);
    result = sign_ext(result & (0xFFFFFFFF >> (32 - data_size)), data_size);
    cpu.eflags.CF = (result >= dest);
}
void set_OF_sbb(uint32_t result, uint32_t src, uint32_t dest, size_t data_size){
    set_OF_sub(result, src, dest, data_size);
}
uint32_t alu_sbb(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sbb(src, dest, data_size);
#else
	/*printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);*/
	uint64_t CF = cpu.eflags.CF;
	uint64_t res = 0;
	if(CF == 0){
	    res = alu_sub(src, dest, data_size);
	}else{
	    res = dest + (~src);
	    set_PF(res);
	    set_ZF(res, data_size);
	    set_SF(res, data_size);
	    set_CF_sbb(res, dest, data_size);
	    set_OF_sbb(res, src, dest, data_size);
	}
	return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}
void set_CF_OF_mul(uint64_t result,  size_t data_size){
    uint32_t high = 0;
    switch(data_size){
	    case 8:// byte * byte = 16bits
                high =(uint32_t)((result & 0xFF00) >> 8);
		break;
	    case 16:
		high = (uint32_t)((result & 0xFFFF0000) >> 16);
	        break;
	    case 32:
	        high = (uint32_t)((result & 0xFFFFFFFF00000000) >> 32);
	        break;
	    default:break;	
    }
    cpu.eflags.OF = cpu.eflags.CF = (high != 0);
    
}
uint64_t alu_mul(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_mul(src, dest, data_size);
#else
	/*printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;*/
	uint64_t res = ((uint64_t)src * (uint64_t)dest);
	set_CF_OF_mul(res, data_size);
	return res;
#endif
}

int64_t alu_imul(int32_t src, int32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_imul(src, dest, data_size);
#else
	/*printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;*/
	int64_t res = (int64_t)src * (int64_t)dest;
	return res;
#endif
}

// need to implement alu_mod before testing
uint32_t alu_div(uint64_t src, uint64_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_div(src, dest, data_size);
#else
	/*printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;*/
	uint32_t res = dest / src;
	return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}

// need to implement alu_imod before testing
int32_t alu_idiv(int64_t src, int64_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_idiv(src, dest, data_size);
#else
	/*printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;*/
	int64_t res = dest / src;
	return res;
#endif
}

uint32_t alu_mod(uint64_t src, uint64_t dest)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_mod(src, dest);
#else
	/*printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;*/
	uint64_t res = dest % src;
	return res;
#endif
}

int32_t alu_imod(int64_t src, int64_t dest)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_imod(src, dest);
#else
	/*printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);*/
	int64_t res = dest % src;
	return res;
#endif
}

uint32_t alu_and(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_and(src, dest, data_size);
#else
	/*printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;*/
	uint32_t res = src & dest;
	cpu.eflags.CF = 0;
	cpu.eflags.OF = 0;
	set_PF(res);
	set_SF(res, data_size);
	set_ZF(res, data_size);
	return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}

uint32_t alu_xor(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_xor(src, dest, data_size);
#else
	/*printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;*/
	uint32_t res = src ^ dest;
	cpu.eflags.CF = 0;
	cpu.eflags.OF = 0;
	set_PF(res);
	set_SF(res, data_size);
	set_ZF(res, data_size);
	return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}

uint32_t alu_or(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_or(src, dest, data_size);
#else
	/*printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;*/
	uint32_t res = src | dest;
	cpu.eflags.CF = 0;
	cpu.eflags.OF = 0;
	set_PF(res);
	set_SF(res, data_size);
	set_ZF(res, data_size);
	return res & (0xFFFFFFFF >> (32 - data_size));
	
#endif
}
void set_CF_shl(uint32_t src, uint32_t dest, size_t data_size){
    uint32_t res = dest << (src - 1);
    res = sign_ext(res & (0xFFFFFFFF >> (32 - data_size)), data_size);
    cpu.eflags.CF = sign(res);
}
uint32_t alu_shl(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_shl(src, dest, data_size);
#else
	/*printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;*/
	uint32_t res = dest << src;
	set_PF(res);
	set_SF(res, data_size);
	set_ZF(res, data_size);
	set_CF_shl(src, dest ,data_size);
	return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}
void set_CF_shr(uint32_t src, uint32_t dest, size_t data_size){
    uint32_t res = dest >> (src - 1);
    cpu.eflags.CF = res & 0x1;
}
uint32_t alu_shr(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_shr(src, dest, data_size);
#else
	/*printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;*/
	dest = dest & (0xFFFFFFFF >> (32 - data_size)); // add zero in the high bit 
	uint32_t res = dest >> src; //unsigned int right shift
	set_PF(res);
	set_SF(res, data_size);
	set_ZF(res, data_size);
	set_CF_shr(src, dest, data_size);
	return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}
void set_CF_sar(uint32_t src, uint32_t dest, size_t data_size){
    dest = sign_ext(dest & (0xFFFFFFFF >> (32 - data_size)), data_size);
	int32_t signed_dest = (int32_t) dest;
	int32_t signed_res = signed_dest >> (src - 1);
	uint32_t res = (uint32_t) signed_res;
	cpu.eflags.CF = res & 0x1;
}
uint32_t alu_sar(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sar(src, dest, data_size);
#else
	/*printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;*/
	dest = sign_ext(dest & (0xFFFFFFFF >> (32 - data_size)), data_size);
	int32_t signed_dest = (int32_t) dest;
	int32_t signed_res = signed_dest >> src;
	uint32_t res = (uint32_t) signed_res;
	set_PF(res);
	set_SF(res, data_size);
	set_ZF(res, data_size);
	set_CF_sar(src, dest, data_size);
	return res & (0xFFFFFFFF >> (32 - data_size));
	
#endif
}

uint32_t alu_sal(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sal(src, dest, data_size);
#else
	/*printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;*/
	uint32_t res = dest << src;
	set_PF(res);
	set_SF(res, data_size);
	set_ZF(res, data_size);
	set_CF_shl(src, dest ,data_size);
	return res & (0xFFFFFFFF >> (32 - data_size));
#endif
}
