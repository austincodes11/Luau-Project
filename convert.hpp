#pragma once
#include "r_lua.h"

#define proto_do(p, v) *(int*)(int)(p) = (int)(p) - (int)(v)
#define closure_do(p, v) *(int*)(p) = (int)(v) + (int)(p)

#define c1_do(addr, val) (*(DWORD*)(addr) = (DWORD)(val) - (DWORD)(addr))
#define c1_undo(addr) (((DWORD)(addr)) - (*(DWORD*)(addr)))

#define c2_do(addr, val) (*(DWORD*)(addr) = (DWORD)(val) - (DWORD)(addr))
#define c2_undo(addr) ((DWORD)(addr) + *(DWORD*)(addr))

#define c3_do(addr, val) (*(DWORD*)(addr) = (DWORD)(val) ^ (DWORD)(addr))
#define c3_undo(addr) ((DWORD)(addr) ^ *(DWORD*)(addr))
enum
{
	NOP = 0x90,
	MUL = 0x5,
	NTEST = 0xE8,
	TFORPREP = 0x17,
	SETGLOBAL = 0x18,
	LEN = 0xC,
	SUB = 0x6,
	TEST = 0x2a,
	SETTABLEK = 0x30,
	GETGLOBAL = 0x35,
	UNM = 0x39,
	ADD = 0x43,
	GETTABLEK = 0x4D,
	MOVE = 0x52,
	NOT = 0x56,
	NLT = 0x60,
	JMP = 0x65,
	SETTABLE = 0x6A,
	LONGJMP = 0x69,
	LOADK = 0x6F,
	CONCAT = 0x73,
	NLE = 0x7D,
	RETURN = 0x82,
	GETTABLE = 0x7,
	FORLOOP = 0xB,
	LOADNUMBER = 0xAC,
	NEQ = 0x8A,
	CALL = 0x8F,
	CLEARSTACK = 0xA3,
	FORPREP = 0xA4,
	LOADBOOL = 0x59,
	POW = 0x72,
	LT = 0x17,
	SELF = 0x4C,
	CLOSE = 0x31,
	SETLIST = 0xC3,
	LOADNIL = 0xC2,
	MOD = 0xC1,
	LE = 0xD4,
	SETUPVAL = 0xDE,
	CLOSURE = 0xAB,
	VARARG = 0xDC,
	DIV = 0xEA,
	EQ = 0xF3,
	TFORLOOP = 0xF8,
	GETUPVAL = 0xFC,
	NEWTABLE = 0xFF
};

void PushTable(DWORD L, TValue* vanillaK, TValue* rbxK)
{
	//std::cout << "[LOGGING PUSH CONSTANTS]: " << vanillaK->tt << std::endl;
	switch (vanillaK->tt)
	{
	case LUA_TNIL:
		rbxK->tt = 0;
		break;
	case LUA_TBOOLEAN:
		rbxK->tt = RLUA_TBOOLEAN;
		rbxK->value.b = rbxK->value.b;
		break;
	case LUA_TNUMBER:
	{
		DWORD64 raw_n = *reinterpret_cast<DWORD64*>(&vanillaK->value.n) ^ *reinterpret_cast<DWORD64*>(XorConstant);
		rbxK->tt = RLUA_TNUMBER;
		rbxK->value.n = raw_n;
		break;

	}

	case LUA_TSTRING:
	{
		std::cout << "Logging tstring\n";

		TString* ts = (TString*)(vanillaK->value.gc);
		const char* str = (const char*)(ts + 1);
		rbxK->tt = RLUA_TSTRING;
		rbxK->value.gc = reinterpret_cast<GCObject*>(rluaS_newlstr(L, str, ts->tsv.len));
		std::cout << "finished\n";
		break;
	}
	case LUA_TUSERDATA:
		std::cout << "logging tud!\n";
		rbxK->tt = RLUA_TUSERDATA;
		rbxK->value.gc = reinterpret_cast<GCObject*>(vanillaK->value.gc);
		break;
	default:
		break;


	}
}

const char rbxOpCodes[]
{
	0x90,//1
	0x5,//2
	0xE8,//3
	0x17,//4
	0x18,//5
	0xC,//6
	0x6,//7
	0x2a,//8
	0x30,//9
	0x35,//10
	0x39,//11
	0x43,//12
	0x4D,//13
	0x52,//14
	0x56,//15
	0x60,//16
	0x65,//17
	0x6A,//18
	0x69,//19
	0x6F,//20
	0x73,//21
	0x7D,//22
	0x82,//23
	0x7,//24
	0xB,//25
	0xAC,//26
	0x8A,//27
	0x8F,//28
	0xA3,//29
	0xA4,//30
	0x59,//31
	0x72,//32
	0x17,//33
	0x4C,//34
	0x31,//35
	0xC3,//36
	0xC2,//36
	0xC1,//37
	0xD4,//38
	0xDE,//39
	0xAB,//40
	0xDC,//41
	0xEA,//42
	0xF3,//43
	0xF8,//44
	0xFC,///45
	0xFF//46
};

#define RBX_SET_OPCODE(ins, op) ( (ins) = ((ins) & 0x3FFFFFF) | ((op) << 24) )

#define RA(i)	(base+GETARG_A(i))
/* to be used after possible stack reallocation */
#define RB(i)	check_exp(getBMode(GET_OPCODE(i)) == OpArgR, base+GETARG_B(i))
#define RC(i)	check_exp(getCMode(GET_OPCODE(i)) == OpArgR, base+GETARG_C(i))
#define RKB(i)	check_exp(getBMode(GET_OPCODE(i)) == OpArgK, \
	ISK(GETARG_B(i)) ? k+INDEXK(GETARG_B(i)) : base+GETARG_B(i))
#define RKC(i)	check_exp(getCMode(GET_OPCODE(i)) == OpArgK, \
	ISK(GETARG_C(i)) ? k+INDEXK(GETARG_C(i)) : base+GETARG_C(i))
#define KBx(i)	check_exp(getBMode(GET_OPCODE(i)) == OpArgK, k+GETARG_Bx(i))


#define dojump(L,pc,i)	{(pc) += (i); luai_threadyield(L);}


#define Protect(x)	{ L->savedpc = pc; {x;}; base = L->base; }

Instruction Convert_Instruction(DWORD rL, lua_State* L, Proto* p, Instruction instr)
{

	uint8_t op = GET_OPCODE(instr);

	DWORD rp = rluaF_newproto(rL);

	TValue* k = (TValue*)rluaM_realloc(rL, 0, 0, 16 * p->sizek, 3);
	const Instruction* pc = L->savedpc;
	
	for (auto i = 0; i < p->sizek; i++)
	{
		PushTable(rL, &p->k[i], &k[i]);
	}


	StkId base = L->base;


	std::cout << "Logging Instructions: " << luaP_opnames[GET_OPCODE(instr)] << std::endl;

	TValue* ra = base + GETARG_A(instr);
	switch (op)
	{
	case OP_LOADK:
	{
		std::cout << "OP_LOADK" << std::endl;
		ra->tt = KBx(instr)->tt;
		ra->value = KBx(instr)->value;
		break;
	}
	case OP_GETGLOBAL:
	{
		std::cout << "-> running getglobal...\n";
		rlua_gettable(rL, reinterpret_cast<TValue*>(rL + 72), KBx(instr), ra);
		std::cout << "-> done\n";
		break;
	}

	case OP_GETTABLE:
	{
		TValue* rb = RB(instr);
		std::cout << "RB: " << rb << std::endl;
		Protect(rlua_gettable(rL, reinterpret_cast<TValue*>(rb), RKC(instr), ra));
		std::cout << "got??\n";
		break;

	}

	case OP_SETTABLE:
	{
		std::cout << "ok man, starting settable\n";
		Protect(rlua_settable(rL, reinterpret_cast<TValue*>(ra), RKB(instr), RKC(instr)));
		std::cout << "-> done\n";
		break;
	}

	case OP_CALL:
	{
		std::cout << "-> OP_CALL\n";
		int b = GETARG_B(instr);
		int nresults = GETARG_C(instr) - 1;
		L->savedpc = pc;
		if (b != 0)
		{
			*(StkId*)(rL + 16) = ra + b;
		}
		int err = rluaD_precall(rL, ra, nresults);
		if (err == 0)
		{
			std::cout << "Err == 0\n";
			break;
		}

		if (err == 1)
		{
			if (nresults >= 0) {
				*(DWORD*)(rL + 16) = **(DWORD**)(rL + 8);
			}
			break;
		}
		break;

	}

	case OP_RETURN:
	{

		std::cout << "running -> OP_RETURN" << std::endl;
		return 0;


	}
	default:
		break;


	}

}

uintptr_t FullyConverted(uintptr_t rL, lua_State* L, Proto* p)
{

	DWORD rp = rluaF_newproto(rL);
	*(DWORD*)(rp + 68) = p->sizecode;
	*(DWORD*)(rp + 8) = p->sizek;
	*(DWORD*)(rp + 72) = p->sizep;
	*(DWORD*)(rp + 52) = p->sizelineinfo;

	uintptr_t* lineinfo = reinterpret_cast<uintptr_t*>(rluaM_realloc(rL, 0, 0, sizeof(DWORD) * p->sizelineinfo, 3));
	TValue* k = reinterpret_cast<TValue*>(rluaM_realloc(rL, 0, 0, sizeof(TValue) * p->sizek, 3));
	Instruction* code = reinterpret_cast<Instruction*>(rluaM_realloc(rL, 0, 0, sizeof(Instruction) * p->sizecode, 3));
	uintptr_t* _p = reinterpret_cast<uintptr_t*>(rluaM_realloc(rL, 0, 0, sizeof(Proto) * p->sizep, 3));

	for (auto i = 0; i < p->sizep; i++)
	{
		_p[i] = FullyConverted(rL, L, p->p[i]);
	}

	for (auto i = 0; i < p->sizek; i++)
	{
		PushTable(rL, &p->k[i], &k[i]);
	}

	for (auto i = 0; i < p->sizecode; i++)
	{
		Convert_Instruction(rL, L, p, p->code[i]);
	}

	for (auto i = 0; i < p->sizelineinfo; i++)
	{
		std::cout << "ok line info\n";

		lineinfo[i] = p->lineinfo[i];
	}

	return rp;
}

void lua_pushlclosure(uintptr_t rL, DWORD closure)
{
	TValue* top = *(TValue**)(rL + 16);
	top->tt = RLUA_TFUNCTION;
	top->value.gc = reinterpret_cast<GCObject*>(closure);
	*(uint32_t*)(rL + 16) += sizeof(TValue);
}
VOID Initiate_Proto(DWORD rL, lua_State* L, LClosure* _p)
{

	Proto* vp = _p->p;

	DWORD rproto = FullyConverted(rL, L, vp);

	std::cout << "rproto: " << rproto << '\n';
	std::cout << "[ LOGGING PROTO CONVERTING ] Converting proto nested stuffs... " << std::endl;


	std::cout << " [ LOGGED CONVERTED NESTED ] " << std::endl;

	std::cout << " [ CREATING NEW CLOSURE ] " << std::endl;

	uintptr_t rlcl = rluaF_newLclosure(rL, vp->nups, *(int*)(rL + 72));
	*(DWORD*)(rlcl + 20) = rproto + rlcl + 20;
	lua_pushlclosure(rL, rlcl);
	std::cout << "[ CREATED NEW CLOSURE ] " << std::endl;
	
	std::cout << "spawned??\n";
	r_spawn(rL);


}

void runScript(DWORD rL, lua_State* L, const char* source)
{

	if (luaL_loadbuffer(L, source, strlen(source), source))
	{
		std::cout << "[ LOGGER ] ERROR: " << lua_tolstring(L, -1, 0) << std::endl;
		lua_pop(L, 1);
	}

	else
	{
		TValue* o = (L->top) - 1;
		LClosure* lcl = &clvalue(o)->l;
		DWORD rbxThread = r_lua_newthread(rL);
		Initiate_Proto(rbxThread, L, lcl);
		std::cout << "[ LOGGER ] Execution Finished!\n";
	}

}