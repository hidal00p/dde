#ifndef PIN_H
#define PIN_H

#include <cstdint>
#include <string>

typedef uint64_t ADDRINT;
typedef void (*AFUNPTR)();

typedef enum {
  REG_INVALID_ = 0,
  REG_NONE = 1,
  REG_FIRST = 2,

  // base for all kinds of registers (application, machine, pin)
  REG_RBASE,

  // Machine registers are individual real registers on the machine
  REG_MACHINE_BASE = REG_RBASE,

  // Application registers are registers used in the application binary
  // Application registers include all machine registers. In addition,
  // they include some aggregrate registers that can be accessed by
  // the application in a single instruction
  // Essentially, application registers = individual machine registers +
  // aggregrate registers

  REG_APPLICATION_BASE = REG_RBASE,

  /* !@ todo: should save scratch mmx and fp registers */
  // The machine registers that form a context. These are the registers
  // that need to be saved in a context switch.
  REG_PHYSICAL_INTEGER_BASE = REG_RBASE,

  REG_TO_SPILL_BASE = REG_RBASE,

  REG_GR_BASE = REG_RBASE,
#if defined(TARGET_IA32E)
  // Context registers in the Intel(R) 64 architecture
  REG_RDI = REG_GR_BASE,   ///< rdi
  REG_GDI = REG_RDI,       ///< edi on a 32 bit machine, rdi on 64
  REG_RSI,                 ///< rsi
  REG_GSI = REG_RSI,       ///< esi on a 32 bit machine, rsi on 64
  REG_RBP,                 ///< rbp
  REG_GBP = REG_RBP,       ///< ebp on a 32 bit machine, rbp on 64
  REG_RSP,                 ///< rsp
  REG_STACK_PTR = REG_RSP, ///< esp on a 32 bit machine, rsp on 64
  REG_RBX,                 ///< rbx
  REG_GBX = REG_RBX,       ///< ebx on a 32 bit machine, rbx on 64
  REG_RDX,                 ///< rdx
  REG_GDX = REG_RDX,       ///< edx on a 32 bit machine, rdx on 64
  REG_RCX,                 ///< rcx
  REG_GCX = REG_RCX,       ///< ecx on a 32 bit machine, rcx on 64
  REG_RAX,                 ///< rax
  REG_GAX = REG_RAX,       ///< eax on a 32 bit machine, rax on 64
  REG_R8,
  REG_R9,
  REG_R10,
  REG_R11,
  REG_R12,
  REG_R13,
  REG_R14,
  REG_R15,
  REG_GR_LAST = REG_R15,

  REG_SEG_BASE,
  REG_SEG_CS = REG_SEG_BASE,
  REG_SEG_SS,
  REG_SEG_DS,
  REG_SEG_ES,
  REG_SEG_FS,
  REG_SEG_GS,
  REG_SEG_LAST = REG_SEG_GS,

  REG_RFLAGS,              ///< rflags
  REG_GFLAGS = REG_RFLAGS, ///< eflags on a 32 bit machine, rflags on 64
  REG_RIP,
  REG_INST_PTR = REG_RIP,
#else  // not defined(TARGET_IA32E)
  // Context registers in the IA-32 architecture
  REG_EDI = REG_GR_BASE,
  REG_GDI = REG_EDI,
  REG_ESI,
  REG_GSI = REG_ESI,
  REG_EBP,
  REG_GBP = REG_EBP,
  REG_ESP,
  REG_STACK_PTR = REG_ESP,
  REG_EBX,
  REG_GBX = REG_EBX,
  REG_EDX,
  REG_GDX = REG_EDX,
  REG_ECX,
  REG_GCX = REG_ECX,
  REG_EAX,
  REG_GAX = REG_EAX,
  REG_GR_LAST = REG_EAX,

  REG_SEG_BASE,
  REG_SEG_CS = REG_SEG_BASE,
  REG_SEG_SS,
  REG_SEG_DS,
  REG_SEG_ES,
  REG_SEG_FS,
  REG_SEG_GS,
  REG_SEG_LAST = REG_SEG_GS,

  REG_EFLAGS,
  REG_GFLAGS = REG_EFLAGS,
  REG_EIP,
  REG_INST_PTR = REG_EIP,
#endif // not defined(TARGET_IA32E)

  REG_PHYSICAL_INTEGER_END = REG_INST_PTR,
  // partial registers common to both the IA-32 and Intel(R) 64 architectures.
  REG_AL,
  REG_AH,
  REG_AX,

  REG_CL,
  REG_CH,
  REG_CX,

  REG_DL,
  REG_DH,
  REG_DX,

  REG_BL,
  REG_BH,
  REG_BX,

  REG_BP,
  REG_SI,
  REG_DI,

  REG_SP,
  REG_FLAGS,
  REG_IP,

#if defined(TARGET_IA32E)
  // partial registers in the Intel(R) 64 architecture
  REG_EDI,
  REG_DIL,
  REG_ESI,
  REG_SIL,
  REG_EBP,
  REG_BPL,
  REG_ESP,
  REG_SPL,
  REG_EBX,
  REG_EDX,
  REG_ECX,
  REG_EAX,
  REG_EFLAGS,
  REG_EIP,

  REG_R8B,
  REG_R8W,
  REG_R8D,
  REG_R9B,
  REG_R9W,
  REG_R9D,
  REG_R10B,
  REG_R10W,
  REG_R10D,
  REG_R11B,
  REG_R11W,
  REG_R11D,
  REG_R12B,
  REG_R12W,
  REG_R12D,
  REG_R13B,
  REG_R13W,
  REG_R13D,
  REG_R14B,
  REG_R14W,
  REG_R14D,
  REG_R15B,
  REG_R15W,
  REG_R15D,
#endif // not defined(TARGET_IA32E)

  REG_MM_BASE,
  REG_MM0 = REG_MM_BASE,
  REG_MM1,
  REG_MM2,
  REG_MM3,
  REG_MM4,
  REG_MM5,
  REG_MM6,
  REG_MM7,
  REG_MM_LAST = REG_MM7,

  REG_XMM_BASE,
  REG_FIRST_FP_REG = REG_XMM_BASE,
  REG_XMM0 = REG_XMM_BASE,
  REG_XMM1,
  REG_XMM2,
  REG_XMM3,
  REG_XMM4,
  REG_XMM5,
  REG_XMM6,
  REG_XMM7,

#if defined(TARGET_IA32E)
  // additional xmm registers in the Intel(R) 64 architecture
  REG_XMM8,
  REG_XMM9,
  REG_XMM10,
  REG_XMM11,
  REG_XMM12,
  REG_XMM13,
  REG_XMM14,
  REG_XMM15,
  REG_XMM_SSE_LAST = REG_XMM15,
  REG_XMM_AVX_LAST = REG_XMM_SSE_LAST,
  REG_XMM_AVX512_HI16_FIRST,
  REG_XMM16 = REG_XMM_AVX512_HI16_FIRST,
  REG_XMM17,
  REG_XMM18,
  REG_XMM19,
  REG_XMM20,
  REG_XMM21,
  REG_XMM22,
  REG_XMM23,
  REG_XMM24,
  REG_XMM25,
  REG_XMM26,
  REG_XMM27,
  REG_XMM28,
  REG_XMM29,
  REG_XMM30,
  REG_XMM31,
  REG_XMM_AVX512_HI16_LAST = REG_XMM31,
  REG_XMM_AVX512_LAST = REG_XMM_AVX512_HI16_LAST,
  REG_XMM_LAST = REG_XMM_AVX512_LAST,
#else  // not TARGET_IA32E
  REG_XMM_SSE_LAST = REG_XMM7,
  REG_XMM_AVX_LAST = REG_XMM_SSE_LAST,
  REG_XMM_AVX512_LAST = REG_XMM_AVX_LAST,
  REG_XMM_LAST = REG_XMM_AVX512_LAST,
#endif // not TARGET_IA32E

  REG_YMM_BASE,
  REG_YMM0 = REG_YMM_BASE,
  REG_YMM1,
  REG_YMM2,
  REG_YMM3,
  REG_YMM4,
  REG_YMM5,
  REG_YMM6,
  REG_YMM7,

#if defined(TARGET_IA32E)
  // additional ymm registers in the Intel(R) 64 architecture
  REG_YMM8,
  REG_YMM9,
  REG_YMM10,
  REG_YMM11,
  REG_YMM12,
  REG_YMM13,
  REG_YMM14,
  REG_YMM15,
  REG_YMM_AVX_LAST = REG_YMM15,
  REG_YMM_AVX512_HI16_FIRST,
  REG_YMM16 = REG_YMM_AVX512_HI16_FIRST,
  REG_YMM17,
  REG_YMM18,
  REG_YMM19,
  REG_YMM20,
  REG_YMM21,
  REG_YMM22,
  REG_YMM23,
  REG_YMM24,
  REG_YMM25,
  REG_YMM26,
  REG_YMM27,
  REG_YMM28,
  REG_YMM29,
  REG_YMM30,
  REG_YMM31,
  REG_YMM_AVX512_HI16_LAST = REG_YMM31,
  REG_YMM_AVX512_LAST = REG_YMM_AVX512_HI16_LAST,
  REG_YMM_LAST = REG_YMM_AVX512_LAST,
#else  // not TARGET_IA32E
  REG_YMM_AVX_LAST = REG_YMM7,
  REG_YMM_AVX512_LAST = REG_YMM_AVX_LAST,
  REG_YMM_LAST = REG_YMM_AVX512_LAST,
#endif // not TARGET_IA32E

  REG_ZMM_BASE,
  REG_ZMM0 = REG_ZMM_BASE,
  REG_ZMM1,
  REG_ZMM2,
  REG_ZMM3,
  REG_ZMM4,
  REG_ZMM5,
  REG_ZMM6,
  REG_ZMM7,
#if defined(TARGET_IA32E)
  REG_ZMM8,
  REG_ZMM9,
  REG_ZMM10,
  REG_ZMM11,
  REG_ZMM12,
  REG_ZMM13,
  REG_ZMM14,
  REG_ZMM15,
  REG_ZMM_AVX512_SPLIT_LAST = REG_ZMM15,
  REG_ZMM_AVX512_HI16_FIRST,
  REG_ZMM16 = REG_ZMM_AVX512_HI16_FIRST,
  REG_ZMM17,
  REG_ZMM18,
  REG_ZMM19,
  REG_ZMM20,
  REG_ZMM21,
  REG_ZMM22,
  REG_ZMM23,
  REG_ZMM24,
  REG_ZMM25,
  REG_ZMM26,
  REG_ZMM27,
  REG_ZMM28,
  REG_ZMM29,
  REG_ZMM30,
  REG_ZMM31,
  REG_ZMM_AVX512_HI16_LAST = REG_ZMM31,
  REG_ZMM_AVX512_LAST = REG_ZMM_AVX512_HI16_LAST,
  REG_ZMM_LAST = REG_ZMM_AVX512_LAST,
#else  // not defined(TARGET_IA32E)
  REG_ZMM_AVX512_SPLIT_LAST = REG_ZMM7,
  REG_ZMM_AVX512_LAST = REG_ZMM_AVX512_SPLIT_LAST,
  REG_ZMM_LAST = REG_ZMM_AVX512_LAST,
#endif // not defined(TARGET_IA32E)

  REG_K_BASE,
  REG_K0 = REG_K_BASE,
  // The K0 opmask register cannot be used as the write mask operand of an
  // AVX512 instruction. However the encoding of K0 as the write mask operand is
  // legal and is used as an implicit full mask.
  REG_IMPLICIT_FULL_MASK = REG_K0,
  REG_K1,
  REG_K2,
  REG_K3,
  REG_K4,
  REG_K5,
  REG_K6,
  REG_K7,
  REG_K_LAST = REG_K7,

#if defined(TARGET_IA32E)
  REG_TMM0, ///< tmm0 on a 64 bit machine
  REG_TMM1, ///< tmm1 on a 64 bit machine
  REG_TMM2, ///< tmm2 on a 64 bit machine
  REG_TMM3, ///< tmm3 on a 64 bit machine
  REG_TMM4, ///< tmm4 on a 64 bit machine
  REG_TMM5, ///< tmm5 on a 64 bit machine
  REG_TMM6, ///< tmm6 on a 64 bit machine
  REG_TMM7, ///< tmm7 on a 64 bit machine
  REG_TMM_FIRST = REG_TMM0,
  REG_TMM_LAST = REG_TMM7,

  /*!
   * Virtual register representing CPU internal tile control register (AMX tile
   * configuration) Contains AMX metadata about palette, tiles sizes and so on.
   * See SDM's LDTILECFG instruction for more details.
   */
  REG_TILECONFIG,

#endif

  REG_MXCSR,
  REG_MXCSRMASK,

// This corresponds to the "orig_eax" register that is visible
// to some debuggers.
#if defined(TARGET_IA32E)
  REG_ORIG_RAX,
  REG_ORIG_GAX = REG_ORIG_RAX,
#else  // not defined(TARGET_IA32E)
  REG_ORIG_EAX,
  REG_ORIG_GAX = REG_ORIG_EAX,
#endif // not defined(TARGET_IA32E)

  REG_FPST_BASE,
  REG_FPSTATUS_BASE = REG_FPST_BASE,
  REG_FPCW = REG_FPSTATUS_BASE,
  REG_FPSW,
  REG_FPTAG, ///< Abridged 8-bit version of x87 tag register.
  REG_FPIP_OFF,
  REG_FPIP_SEL,
  REG_FPOPCODE,
  REG_FPDP_OFF,
  REG_FPDP_SEL,
  REG_FPSTATUS_LAST = REG_FPDP_SEL,

  REG_ST_BASE,
  REG_ST0 = REG_ST_BASE,
  REG_ST1,
  REG_ST2,
  REG_ST3,
  REG_ST4,
  REG_ST5,
  REG_ST6,
  REG_ST7,
  REG_ST_LAST = REG_ST7,
  REG_FPST_LAST = REG_ST_LAST,

  REG_DR_BASE,
  REG_DR0 = REG_DR_BASE,
  REG_DR1,
  REG_DR2,
  REG_DR3,
  REG_DR4,
  REG_DR5,
  REG_DR6,
  REG_DR7,
  REG_DR_LAST = REG_DR7,

  REG_CR_BASE,
  REG_CR0 = REG_CR_BASE,
  REG_CR1,
  REG_CR2,
  REG_CR3,
  REG_CR4,
  REG_CR_LAST = REG_CR4,
#if defined(TARGET_IA32E)
  REG_CR8,
#endif

  REG_TSSR,
  REG_LDTR,

  REG_TR_BASE,
  REG_TR = REG_TR_BASE,
  REG_TR3,
  REG_TR4,
  REG_TR5,
  REG_TR6,
  REG_TR7,
  REG_TR_LAST = REG_TR7,

  REG_MACHINE_LAST = REG_TR_LAST, /* last machine register */

  /* these are the two registers implementing the eflags in pin
     REG_STATUS_FLAGS represents the OF, SF, ZF, AF, PF and CF flags.
     REG_DF_FLAG      represents the DF flag.
     flag splitting is done because the DF flag spilling and filling is rather
     expensive, and the DF flag is not read/written by most instructions -
     therefore it is not necessary to spill/fill it on most instructions that
     read/write the flags. (prior to flag splitting, whenever any of the flags
     needed to be spilled/filled both the DF and all the above status flags were
     spilled/filled). NOTE - this flag splitting is not done if the pushf/popf
     sequence is being used rather than the sahf/lahf sequence (some early
     Intel64 processors do not support sahf/lahf instructions). Also the
     KnobRegFlagsSplit can be used to disable the flags splitting when the
     sahf/lahf sequence is being used Flags splitting is not done at the INS
     operand level - it is done when building the vreglist for register
     allocation. So tools see the architectural flags registers in INSs. See the
     functions MakeRegisterList and REG_InsertReadRegToVreglist to see how the
     split flags are inserted into the vreglist. See
     jit_flags_spillfill_ia32.cpp file comments to learn how they are
     spilled/filled
   */
  REG_STATUS_FLAGS,
  REG_DF_FLAG,

  // NOTE: although REG_X87 is outside REG_APPLICATION_LAST scope it is part of
  // app registers therefore any traversal of all application regs need to have
  // special handling for REG_X87
  REG_APPLICATION_LAST =
      REG_DF_FLAG, /* last register name used by the application */

  /* Pin's virtual register names */
  REG_TOOL_BASE,

  /**
   * *** Segment registers (FS/GS) handling in Pin ***
   *
   * Background about segment virtualization support in Pin:
   * Segment virtualization was introduced in Pin in the past in order to
   * support Pin on OS's which didn't contain old (without segment usage) libc
   * in a standard installation. Application and Pin were using 2 different
   * TLS's, however access to them was done through segment registers (Also
   * known as thread-self-pointer registers) (actually through their matching
   * segment descriptor which contain the segment base address). Segment
   * register (selector) can have one value at a time. Changing segment register
   * value back and forth (from application to Pin and the other way around) is
   * very costly performance wise (involve system calls). Also there may have
   * been other limitations. Therefore it was decided to emulate application
   * instructions which use segments registers. This is done by saving segment
   * selector value and segment base address in the spill area (i.e
   * REG_PIN_SEG_GS_VAL and REG_SEG_GS_BASE ) for each thread and performing all
   * segment related instruction (of the application) using their values (by
   * emulating instructions that set these registers and translate instructions
   * that are accessing the memory using fs or gs prefix - we call this
   * virtualizing segment). (This also help when passing effective address of
   * memory operands to analysis routines) In Linux 32 bit the segment base
   * address changes every time we write to a segment register (every time we
   * load GS/FS, the hidden part is also loaded with the segment base address -
   * it's kind of a cache for optimization, to save bus cycles) In order to
   * support this beside emulating these in instructions we also tracked GDT/LDT
   * tables (We store these tables inside Pin and update them every time
   * needed).
   *
   * Today we have PinCRT which doesn't use segment registers, therefore we
   * don't have to virtualize application segment usage and just let application
   * execute these instructions in their original form (without modifying them
   * or without emulating them).
   *
   * Linux
   * In Linux we no longer virtualize application handling of segments:
   * application instructions which uses segments or segments prefix now runs in
   * their original form. In Linux 64 bits we now only track segment base
   * address virtual register by emulating the system call which changes the
   * segment base address (track application segment base address inside a
   * virtual register in addition to updating the application one). In Linux 32
   * bits it's more complicated: It's hard to track the segment address without
   * fully emulating all writes to segment registers + tracking the GDT/LDT
   * which is a lot of work. Instead we're using GsBaseAddress()/FsBaseAddress()
   * where needed including in PrecomputeSegBaseAddressIfNeeded() which is
   * called from SetupArgumentEa() when needing to compute
   * REG_SEG_FS_BASE/REG_SEG_GS_BASE value (holds the segment base address)
   *
   * macOS
   * In macOS, PIN still use (at least) the system loader which uses the GS
   * segment register, therefore segment virtualization is still used.
   *
   * Windows
   * In Windows we compute segment base address at the beginning (assume it
   * doens't change) and use its value when needed. REG_PIN_SEG_GS_VAL and
   * REG_PIN_SEG_FS_VAL are unused in this platform
   *
   */

  // Virtual registers reg holding memory addresses pointed by GS/FS registers
  // These registers are visible for tool writers
  REG_SEG_GS_BASE = REG_TOOL_BASE, ///< Base address for GS segment
  REG_SEG_FS_BASE,                 ///< Base address for FS segment

  // ISA-independent Pin virtual regs needed for instrumentation
  // These are pin registers visible to the pintool writers.
  REG_INST_BASE,
  REG_INST_SCRATCH_BASE = REG_INST_BASE, ///< First available scratch register
  REG_INST_G0 = REG_INST_SCRATCH_BASE,   ///< Scratch register used in pintools
  REG_INST_G1,                           ///< Scratch register used in pintools
  REG_INST_G2,                           ///< Scratch register used in pintools
  REG_INST_G3,                           ///< Scratch register used in pintools
  REG_INST_G4,                           ///< Scratch register used in pintools
  REG_INST_G5,                           ///< Scratch register used in pintools
  REG_INST_G6,                           ///< Scratch register used in pintools
  REG_INST_G7,                           ///< Scratch register used in pintools
  REG_INST_G8,                           ///< Scratch register used in pintools
  REG_INST_G9,                           ///< Scratch register used in pintools
  REG_INST_G10,                          ///< Scratch register used in pintools
  REG_INST_G11,                          ///< Scratch register used in pintools
  REG_INST_G12,                          ///< Scratch register used in pintools
  REG_INST_G13,                          ///< Scratch register used in pintools
  REG_INST_G14,                          ///< Scratch register used in pintools
  REG_INST_G15,                          ///< Scratch register used in pintools
  REG_INST_G16,                          ///< Scratch register used in pintools
  REG_INST_G17,                          ///< Scratch register used in pintools
  REG_INST_G18,                          ///< Scratch register used in pintools
  REG_INST_G19,                          ///< Scratch register used in pintools
  REG_INST_G20,                          ///< Scratch register used in pintools
  REG_INST_G21,                          ///< Scratch register used in pintools
  REG_INST_G22,                          ///< Scratch register used in pintools
  REG_INST_G23,                          ///< Scratch register used in pintools
  REG_INST_G24,                          ///< Scratch register used in pintools
  REG_INST_G25,                          ///< Scratch register used in pintools
  REG_INST_G26,                          ///< Scratch register used in pintools
  REG_INST_G27,                          ///< Scratch register used in pintools
  REG_INST_G28,                          ///< Scratch register used in pintools
  REG_INST_G29,                          ///< Scratch register used in pintools
  REG_INST_TOOL_FIRST = REG_INST_G0,
  REG_INST_TOOL_LAST = REG_INST_G29,

  REG_BUF_BASE0,
  REG_BUF_BASE1,
  REG_BUF_BASE2,
  REG_BUF_BASE3,
  REG_BUF_BASE4,
  REG_BUF_BASE5,
  REG_BUF_BASE6,
  REG_BUF_BASE7,
  REG_BUF_BASE8,
  REG_BUF_BASE9,
  REG_BUF_BASE_LAST = REG_BUF_BASE9,

  REG_BUF_END0,
  REG_BUF_END1,
  REG_BUF_END2,
  REG_BUF_END3,
  REG_BUF_END4,
  REG_BUF_END5,
  REG_BUF_END6,
  REG_BUF_END7,
  REG_BUF_END8,
  REG_BUF_END9,
  REG_BUF_ENDLAST = REG_BUF_END9,
  REG_BUF_LAST = REG_BUF_ENDLAST,

  REG_INST_SCRATCH_LAST = REG_BUF_LAST,

#if defined(TARGET_IA32E)
  // DWORD versions of the above G0-G29 scratch regs
  REG_INST_G0D,  ///< Scratch register used in pintools
  REG_INST_G1D,  ///< Scratch register used in pintools
  REG_INST_G2D,  ///< Scratch register used in pintools
  REG_INST_G3D,  ///< Scratch register used in pintools
  REG_INST_G4D,  ///< Scratch register used in pintools
  REG_INST_G5D,  ///< Scratch register used in pintools
  REG_INST_G6D,  ///< Scratch register used in pintools
  REG_INST_G7D,  ///< Scratch register used in pintools
  REG_INST_G8D,  ///< Scratch register used in pintools
  REG_INST_G9D,  ///< Scratch register used in pintools
  REG_INST_G10D, ///< Scratch register used in pintools
  REG_INST_G11D, ///< Scratch register used in pintools
  REG_INST_G12D, ///< Scratch register used in pintools
  REG_INST_G13D, ///< Scratch register used in pintools
  REG_INST_G14D, ///< Scratch register used in pintools
  REG_INST_G15D, ///< Scratch register used in pintools
  REG_INST_G16D, ///< Scratch register used in pintools
  REG_INST_G17D, ///< Scratch register used in pintools
  REG_INST_G18D, ///< Scratch register used in pintools
  REG_INST_G19D, ///< Scratch register used in pintools
  REG_INST_G20D, ///< Scratch register used in pintools
  REG_INST_G21D, ///< Scratch register used in pintools
  REG_INST_G22D, ///< Scratch register used in pintools
  REG_INST_G23D, ///< Scratch register used in pintools
  REG_INST_G24D, ///< Scratch register used in pintools
  REG_INST_G25D, ///< Scratch register used in pintools
  REG_INST_G26D, ///< Scratch register used in pintools
  REG_INST_G27D, ///< Scratch register used in pintools
  REG_INST_G28D, ///< Scratch register used in pintools
  REG_INST_G29D, ///< Scratch register used in pintools
  REG_TOOL_LAST = REG_INST_G29D,
#else  // end of defined(TARGET_IA32E)
  REG_TOOL_LAST = REG_BUF_LAST,
#endif // end of !(defined(TARGET_IA32E))

  REG_SPECIAL_BASE,

  // REG_X87 is a representative of the X87 fp state - it is NOT available for
  // explicit use in ANY of the Pin APIs. This register is set/get internally
  // using xsave/xrstor or fxsave/fxrstor. In order to allow proper work of
  // xsave/xrstor, the size of this register includes all the legacy xfeatures
  // and the extended header. see @ref REG_X87_SIZE.
  REG_X87 = REG_SPECIAL_BASE,

  REG_SPECIAL_LAST = REG_X87,

  REG_PIN_BASE,

  REG_PIN_SEG_GS_VAL =
      REG_PIN_BASE, // virtual reg holding actual value of GS (also known as
                    // segment selector which is the visible part of the segment
                    // register). Only used when KnobVirtualSegments is on.
  REG_PIN_SEG_FS_VAL, // virtual reg holding actual value of FS (also known as
                      // segment selector which is the visible part of the
                      // segment register). Only used when KnobVirtualSegments
                      // is on.

  REG_LAST_CONTEXT_REG = REG_PIN_SEG_FS_VAL, // Last register in the canonical
                                             // SPILL AREA Based CONTEXT

  REG_PIN_GR_BASE,

  // ia32-specific Pin gr regs
  REG_PIN_EDI = REG_PIN_GR_BASE,

#if defined(TARGET_IA32)
  REG_PIN_GDI = REG_PIN_EDI, // PIN_GDI == PIN_EDI on 32 bit, PIN_RDI on 64 bit.
#endif                       // defined(TARGET_IA32)

  REG_PIN_ESI,

#if defined(TARGET_IA32)
  REG_PIN_GSI = REG_PIN_ESI,
#endif // defined(TARGET_IA32)

  REG_PIN_EBP,

#if defined(TARGET_IA32)
  REG_PIN_GBP = REG_PIN_EBP,
#endif // defined(TARGET_IA32)

  REG_PIN_ESP,

#if defined(TARGET_IA32)
  REG_PIN_STACK_PTR = REG_PIN_ESP,
#endif // defined(TARGET_IA32)

  REG_PIN_EBX,

#if defined(TARGET_IA32)
  REG_PIN_GBX = REG_PIN_EBX,
#endif // defined(TARGET_IA32)

  REG_PIN_EDX,

#if defined(TARGET_IA32)
  REG_PIN_GDX = REG_PIN_EDX,
#endif // defined(TARGET_IA32)

  REG_PIN_ECX,

#if defined(TARGET_IA32)
  REG_PIN_GCX = REG_PIN_ECX, // PIN_GCX == PIN_ECX on 32 bit, PIN_RCX on 64 bit.
#endif                       // defined(TARGET_IA32)

  REG_PIN_EAX,

#if defined(TARGET_IA32)
  REG_PIN_GAX = REG_PIN_EAX, // PIN_GAX == PIN_EAX on 32 bit, PIN_RAX on 64 bit.
#endif                       // defined(TARGET_IA32)

  REG_PIN_AL,
  REG_PIN_AH,
  REG_PIN_AX,
  REG_PIN_CL,
  REG_PIN_CH,
  REG_PIN_CX,
  REG_PIN_DL,
  REG_PIN_DH,
  REG_PIN_DX,
  REG_PIN_BL,
  REG_PIN_BH,
  REG_PIN_BX,
  REG_PIN_BP,
  REG_PIN_SI,
  REG_PIN_DI,
  REG_PIN_SP,

#if defined(TARGET_IA32E)
  // Intel(R) 64 architecture specific pin gr regs
  REG_PIN_RDI,
  REG_PIN_GDI = REG_PIN_RDI,
  REG_PIN_RSI,
  REG_PIN_GSI = REG_PIN_RSI,
  REG_PIN_RBP,
  REG_PIN_GBP = REG_PIN_RBP,
  REG_PIN_RSP,

  REG_PIN_STACK_PTR = REG_PIN_RSP,

  REG_PIN_RBX,
  REG_PIN_GBX = REG_PIN_RBX,
  REG_PIN_RDX,
  REG_PIN_GDX = REG_PIN_RDX,
  REG_PIN_RCX,
  REG_PIN_GCX = REG_PIN_RCX,
  REG_PIN_RAX,
  REG_PIN_GAX = REG_PIN_RAX,
  REG_PIN_R8,
  REG_PIN_R9,
  REG_PIN_R10,
  REG_PIN_R11,
  REG_PIN_R12,
  REG_PIN_R13,
  REG_PIN_R14,
  REG_PIN_R15,

  REG_PIN_DIL,
  REG_PIN_SIL,
  REG_PIN_BPL,
  REG_PIN_SPL,

  REG_PIN_R8B,
  REG_PIN_R8W,
  REG_PIN_R8D,

  REG_PIN_R9B,
  REG_PIN_R9W,
  REG_PIN_R9D,

  REG_PIN_R10B,
  REG_PIN_R10W,
  REG_PIN_R10D,

  REG_PIN_R11B,
  REG_PIN_R11W,
  REG_PIN_R11D,

  REG_PIN_R12B,
  REG_PIN_R12W,
  REG_PIN_R12D,

  REG_PIN_R13B,
  REG_PIN_R13W,
  REG_PIN_R13D,

  REG_PIN_R14B,
  REG_PIN_R14W,
  REG_PIN_R14D,

  REG_PIN_R15B,
  REG_PIN_R15W,
  REG_PIN_R15D,
#endif // defined(TARGET_IA32E)

  // Every thread is assigned an index so we can implement tls
  REG_PIN_THREAD_ID,

  // ISA-independent gr regs
  REG_PIN_INDIRREG,            // virtual reg holding indirect jmp target value
  REG_PIN_IPRELADDR,           // virtual reg holding ip-rel address value
  REG_PIN_SYSENTER_RESUMEADDR, // virtual reg holding the resume address from
                               // sysenter
  REG_PIN_SYSCALL_NEXT_PC, // virtual reg holding the next PC when Pin emulates
                           // a system call
  REG_PIN_VMENTER,         // virtual reg holding the address of VmEnter
                   // actually it is the spill slot of this register that holds
                   // the address
#if defined(TARGET_IA32E)
  REG_PIN_TRACKED_TILECFG, ///< virtual reg holding latest tile configuration
                           ///< value (64 bytes). Always mapped to spill area.
                           ///< Needed to optimization purposes.
#endif

  // ISA-independent gr regs holding temporary values
  REG_PIN_T_BASE,
#ifdef TARGET_IA32E
  REG_PIN_T0 = REG_PIN_T_BASE,
  REG_PIN_T1,
  REG_PIN_T2,
  REG_PIN_T3,
  REG_PIN_T0D, // lower 32 bits of temporary register
  REG_PIN_T1D,
  REG_PIN_T2D,
  REG_PIN_T3D,
#else          // not TARGET_IA32E
  REG_PIN_T0 = REG_PIN_T_BASE,
  REG_PIN_T0D = REG_PIN_T0,
  REG_PIN_T1,
  REG_PIN_T1D = REG_PIN_T1,
  REG_PIN_T2,
  REG_PIN_T2D = REG_PIN_T2,
  REG_PIN_T3,
  REG_PIN_T3D = REG_PIN_T3,
#endif         // not TARGET_IA32E
  REG_PIN_T0W, // lower 16 bits of temporary register
  REG_PIN_T1W,
  REG_PIN_T2W,
  REG_PIN_T3W,
  REG_PIN_T0L, // lower 8 bits of temporary register
  REG_PIN_T1L,
  REG_PIN_T2L,
  REG_PIN_T3L,
  REG_PIN_T_LAST = REG_PIN_T3L,
  REG_PIN_THREAD_IDD, // REG_PIN_THREAD_ID 32 half part
  REG_TO_SPILL_LAST = REG_PIN_THREAD_IDD,
  REG_PIN_INST_COND, // for conditional instrumentation.

// Used for memory rewriting, these are not live outside the region
// but cannot use general purpose scratch registers, because they're
// used during instrumentation generation, rather than region generation.
#ifdef TARGET_IA32E
  REG_PIN_INST_T0,
  REG_PIN_INST_T1,
  REG_PIN_INST_T2,
  REG_PIN_INST_T3,
  REG_PIN_INST_T0D, // lower 32 bits of temporary register
  REG_PIN_INST_T1D,
  REG_PIN_INST_T2D,
  REG_PIN_INST_T3D,
#else               // not TARGET_IA32E
  REG_PIN_INST_T0,
  REG_PIN_INST_T0D = REG_PIN_INST_T0,
  REG_PIN_INST_T1,
  REG_PIN_INST_T1D = REG_PIN_INST_T1,
  REG_PIN_INST_T2,
  REG_PIN_INST_T2D = REG_PIN_INST_T2,
  REG_PIN_INST_T3,
  REG_PIN_INST_T3D = REG_PIN_INST_T3,
#endif              // not TARGET_IA32E
  REG_PIN_INST_T0W, // lower 16 bits of temporary register
  REG_PIN_INST_T1W,
  REG_PIN_INST_T2W,
  REG_PIN_INST_T3W,
  REG_PIN_INST_T0L, // lower 8 bits of temporary register
  REG_PIN_INST_T1L,
  REG_PIN_INST_T2L,
  REG_PIN_INST_T3L,

  // Used to preserve the predicate value around repped string ops
  REG_PIN_INST_PRESERVED_PREDICATE,

  // Used when the AC flag needs to be cleared before analysis routine
  REG_PIN_FLAGS_BEFORE_AC_CLEARING,

  // Virtual regs used by Pin inside instrumentation bridges.
  // Unlike REG_INST_BASE to REG_INST_LAST, these registers are
  // NOT visible to  Pin clients.
  REG_PIN_BRIDGE_ORIG_SP, // hold the stack ptr value before the bridge
  REG_PIN_BRIDGE_APP_IP,  // hold the application (not code cache) IP to resume
  REG_PIN_BRIDGE_SP_BEFORE_ALIGN, // hold the stack ptr value before the stack
                                  // alignment
  REG_PIN_BRIDGE_SP_BEFORE_CALL,  // hold the stack ptr value before call to
                                  // replaced function in probe mode
  REG_PIN_BRIDGE_SP_BEFORE_MARSHALLING_FRAME, // hold the stack ptr value before
                                              // allocating the marshalling
                                              // frame
  REG_PIN_BRIDGE_MARSHALLING_FRAME,        // hold the address of the marshalled
                                           // reference registers
  REG_PIN_BRIDGE_ON_STACK_CONTEXT_FRAME,   // hold the address of the on stack
                                           // context frame
  REG_PIN_BRIDGE_ON_STACK_CONTEXT_SP,      // hold the sp at which the on stack
                                           // context was pushed
  REG_PIN_BRIDGE_MULTI_MEMORYACCESS_FRAME, // hold the address of the on stack
                                           // PIN_MULTI_MEM_ACCESS_INFO frame
  REG_PIN_BRIDGE_MULTI_MEMORYACCESS_SP,    // hold the sp at which the
                                        // PIN_MULTI_MEM_ACCESS_INFO was pushed
  // hold the address of the on stack
  // MULTI_MEM_ACCESS_AND_REWRITE_EMULATION_INFO frame
  REG_PIN_MULTI_MEM_ACCESS_AND_REWRITE_EMULATION_INFO_FRAME,
  REG_PIN_OPERANDS_INFO_OP0,    // holds a pointer to a IMULTI_ELEMENT_OPERAND
                                // object for operand 0
  REG_PIN_OPERANDS_INFO_OP1,    // holds a pointer to a IMULTI_ELEMENT_OPERAND
                                // object for operand 1
  REG_PIN_OPERANDS_INFO_OP2,    // holds a pointer to a IMULTI_ELEMENT_OPERAND
                                // object for operand 2
  REG_PIN_OPERANDS_INFO_OP3,    // holds a pointer to a IMULTI_ELEMENT_OPERAND
                                // object for operand 3
  REG_PIN_OPERANDS_INFO_GEN_SP, // hold the sp at which the
                                // PIN_MEM_OPERAND_GENERATION_INFO was pushed
  REG_PIN_MEM_OPERAND_REWRITE, // holds a pointer to a ISCATTERED_MEMORY_REWRITE
                               // object for the memory operand
  REG_PIN_BRIDGE_TRANS_MEMORY_CALLBACK_FRAME, // hold the address of the on
                                              // stack PIN_MEM_TRANS_INFO frame
  REG_PIN_BRIDGE_TRANS_MEMORY_CALLBACK_SP,    // hold the sp at which the
                                              // PIN_MEM_TRANS_INFO was pushed
  REG_PIN_TRANS_MEMORY_CALLBACK_READ_ADDR,    // hold the result of read memory
                                              // address calculation
  REG_PIN_TRANS_MEMORY_CALLBACK_READ2_ADDR,   // hold the result of read2 memory
                                              // address calculation
  REG_PIN_TRANS_MEMORY_CALLBACK_WRITE_ADDR,   // hold the result of write memory
                                              // address calculation
  REG_PIN_BRIDGE_SPILL_AREA_CONTEXT_FRAME, // hold the address of the spill area
                                           // context frame
  REG_PIN_BRIDGE_SPILL_AREA_CONTEXT_SP, // hold the sp at which the spill area
                                        // context was pushed

  REG_PIN_AVX_IN_USE, // holds the value of EDX resulting from an XGETBV
                      // instruction, if both the CPU and the OS support AVX
                      // state and AVX is in use, 0 otherwise. XGETBV with ECX=1
                      // returns the logical AND of XCR0 and the current value
                      // of the XINUSE state-component bitmap

  REG_PIN_SPILLPTR, // ptr to the pin spill area
  REG_PIN_GR_LAST = REG_PIN_SPILLPTR,
  REG_PIN_X87,
  REG_PIN_MXCSR,

  // REG_PIN_FLAGS is x86-specific, but since it is not a gr, we put it out of
  // REG_PIN_GR_BASE and REG_PIN_GR_LAST

  /* these are the two registers implementing the PIN flags in pin
     REG_PIN_STATUS_FLAGS represents the OF, SF, ZF, AF, PF and CF flags.
     REG_PIN_DF_FLAG      represents the DF flag.
   */
  REG_PIN_STATUS_FLAGS,
  REG_PIN_DF_FLAG,

  /* REG_PIN_FLAGS is used only in the case when the pushf/popf sequence is used
     for flags spill/fill rather than the sahf/lahf sequence.
   */
  REG_PIN_FLAGS,

  REG_PIN_XMM_BASE,
  REG_PIN_XMM0 = REG_PIN_XMM_BASE,
  REG_PIN_XMM1,
  REG_PIN_XMM2,
  REG_PIN_XMM3,
  REG_PIN_XMM4,
  REG_PIN_XMM5,
  REG_PIN_XMM6,
  REG_PIN_XMM7,
#if defined(TARGET_IA32E)
  // additional xmm registers in the Intel(R) 64 architecture
  REG_PIN_XMM8,
  REG_PIN_XMM9,
  REG_PIN_XMM10,
  REG_PIN_XMM11,
  REG_PIN_XMM12,
  REG_PIN_XMM13,
  REG_PIN_XMM14,
  REG_PIN_XMM15,
  REG_PIN_XMM_SSE_LAST = REG_PIN_XMM15,
  REG_PIN_XMM_AVX_LAST = REG_PIN_XMM_SSE_LAST,
  REG_PIN_XMM_AVX512_HI16_FIRST,
  REG_PIN_XMM16 = REG_PIN_XMM_AVX512_HI16_FIRST,
  REG_PIN_XMM17,
  REG_PIN_XMM18,
  REG_PIN_XMM19,
  REG_PIN_XMM20,
  REG_PIN_XMM21,
  REG_PIN_XMM22,
  REG_PIN_XMM23,
  REG_PIN_XMM24,
  REG_PIN_XMM25,
  REG_PIN_XMM26,
  REG_PIN_XMM27,
  REG_PIN_XMM28,
  REG_PIN_XMM29,
  REG_PIN_XMM30,
  REG_PIN_XMM31,
  REG_PIN_XMM_AVX512_HI16_LAST = REG_PIN_XMM31,
  REG_PIN_XMM_AVX512_LAST = REG_PIN_XMM_AVX512_HI16_LAST,
  REG_PIN_XMM_LAST = REG_PIN_XMM_AVX512_LAST,
#else  // not TARGET_IA32E
  REG_PIN_XMM_SSE_LAST = REG_PIN_XMM7,
  REG_PIN_XMM_AVX_LAST = REG_PIN_XMM_SSE_LAST,
  REG_PIN_XMM_AVX512_LAST = REG_PIN_XMM_AVX_LAST,
  REG_PIN_XMM_LAST = REG_PIN_XMM_AVX512_LAST,
#endif // TARGET_IA32E

  REG_PIN_YMM_BASE,
  REG_PIN_YMM0 = REG_PIN_YMM_BASE,
  REG_PIN_YMM1,
  REG_PIN_YMM2,
  REG_PIN_YMM3,
  REG_PIN_YMM4,
  REG_PIN_YMM5,
  REG_PIN_YMM6,
  REG_PIN_YMM7,
#if defined(TARGET_IA32E)
  // additional ymm registers in the Intel(R) 64 architecture
  REG_PIN_YMM8,
  REG_PIN_YMM9,
  REG_PIN_YMM10,
  REG_PIN_YMM11,
  REG_PIN_YMM12,
  REG_PIN_YMM13,
  REG_PIN_YMM14,
  REG_PIN_YMM15,
  REG_PIN_YMM_AVX_LAST = REG_PIN_YMM15,
  REG_PIN_YMM_AVX512_HI16_FIRST,
  REG_PIN_YMM16 = REG_PIN_YMM_AVX512_HI16_FIRST,
  REG_PIN_YMM17,
  REG_PIN_YMM18,
  REG_PIN_YMM19,
  REG_PIN_YMM20,
  REG_PIN_YMM21,
  REG_PIN_YMM22,
  REG_PIN_YMM23,
  REG_PIN_YMM24,
  REG_PIN_YMM25,
  REG_PIN_YMM26,
  REG_PIN_YMM27,
  REG_PIN_YMM28,
  REG_PIN_YMM29,
  REG_PIN_YMM30,
  REG_PIN_YMM31,
  REG_PIN_YMM_AVX512_HI16_LAST = REG_PIN_YMM31,
  REG_PIN_YMM_AVX512_LAST = REG_PIN_YMM_AVX512_HI16_LAST,
  REG_PIN_YMM_LAST = REG_PIN_YMM_AVX512_LAST,
#else  // not TARGET_IA32E
  REG_PIN_YMM_AVX_LAST = REG_PIN_YMM7,
  REG_PIN_YMM_AVX512_LAST = REG_PIN_YMM_AVX_LAST,
  REG_PIN_YMM_LAST = REG_PIN_YMM_AVX512_LAST,
#endif // not TARGET_IA32E

  REG_PIN_ZMM_BASE,
  REG_PIN_ZMM0 = REG_PIN_ZMM_BASE,
  REG_PIN_ZMM1,
  REG_PIN_ZMM2,
  REG_PIN_ZMM3,
  REG_PIN_ZMM4,
  REG_PIN_ZMM5,
  REG_PIN_ZMM6,
  REG_PIN_ZMM7,
#ifndef TARGET_IA32
  REG_PIN_ZMM8,
  REG_PIN_ZMM9,
  REG_PIN_ZMM10,
  REG_PIN_ZMM11,
  REG_PIN_ZMM12,
  REG_PIN_ZMM13,
  REG_PIN_ZMM14,
  REG_PIN_ZMM15,
  REG_PIN_ZMM_AVX512_SPLIT_LAST = REG_PIN_ZMM15,
  REG_PIN_ZMM_AVX512_HI16_FIRST,
  REG_PIN_ZMM16 = REG_PIN_ZMM_AVX512_HI16_FIRST,
  REG_PIN_ZMM17,
  REG_PIN_ZMM18,
  REG_PIN_ZMM19,
  REG_PIN_ZMM20,
  REG_PIN_ZMM21,
  REG_PIN_ZMM22,
  REG_PIN_ZMM23,
  REG_PIN_ZMM24,
  REG_PIN_ZMM25,
  REG_PIN_ZMM26,
  REG_PIN_ZMM27,
  REG_PIN_ZMM28,
  REG_PIN_ZMM29,
  REG_PIN_ZMM30,
  REG_PIN_ZMM31,
  REG_PIN_ZMM_AVX512_HI16_LAST = REG_PIN_ZMM31,
  REG_PIN_ZMM_AVX512_LAST = REG_PIN_ZMM_AVX512_HI16_LAST,
  REG_PIN_ZMM_LAST = REG_PIN_ZMM_AVX512_LAST,
#else  // TARGET_IA32
  REG_PIN_ZMM_AVX512_SPLIT_LAST = REG_PIN_ZMM7,
  REG_PIN_ZMM_AVX512_LAST = REG_PIN_ZMM_AVX512_SPLIT_LAST,
  REG_PIN_ZMM_LAST = REG_PIN_ZMM_AVX512_LAST,
#endif // TARGET_IA32

  REG_PIN_K_BASE,
  REG_PIN_K0 = REG_PIN_K_BASE,
  REG_PIN_K1,
  REG_PIN_K2,
  REG_PIN_K3,
  REG_PIN_K4,
  REG_PIN_K5,
  REG_PIN_K6,
  REG_PIN_K7,
  REG_PIN_K_LAST = REG_PIN_K7,

  REG_PIN_LAST = REG_PIN_K_LAST,

  REG_LAST

} REG;

enum OPR_TYPE { _REGSTR, _IMM, _MEM, _NONE };

union OPR_VAL {
  REG reg;
  uint64_t imm;
};

struct OPR {
  OPR_TYPE type;
  OPR_VAL val;
};

struct INS {
  OPR ops[2];

  INS() {}

  INS(OPR dest, OPR src) {
    constexpr uint8_t src_idx = 1, dest_idx = 0;
    ops[src_idx] = src;
    ops[dest_idx] = dest;
  }
};

bool INS_OperandIsMemory(INS ins, uint8_t idx);
bool INS_OperandIsReg(INS ins, uint8_t idx);
bool INS_OperandIsImmediate(INS ins, uint8_t idx);
REG INS_OperandReg(INS ins, uint8_t idx);
uint64_t INS_OperandImmediate(INS ins, uint8_t idx);
void PIN_SafeCopy(void *a, void *mem_addr, uint8_t size);
std::string RTN_FindNameByAddress(ADDRINT rtn_addr);
bool is_fpu_stack_reg(REG st);
uint8_t get_fpu_stack_idx_from_st(REG st);

struct RTN {};

typedef enum {
  MAIN = 0,
  FOO = 10,
  SINUS = 11,
  COSINUS = 12,
  BAR = 20,
  EMPTY = 42
} RtnName;

struct CONTEXT {};
void PIN_GetContextRegval(CONTEXT *, REG, uint8_t *);

#endif
