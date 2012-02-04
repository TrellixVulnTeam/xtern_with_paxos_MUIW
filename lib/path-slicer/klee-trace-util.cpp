#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>

#include "klee-trace-util.h"
#include "util.h"
#include "dyn-instrs.h"
using namespace tern;

using namespace klee;

using namespace llvm;

KleeTraceUtil::KleeTraceUtil() {
  kmodule = NULL;
}

KleeTraceUtil::~KleeTraceUtil() {

}

void KleeTraceUtil::initIdMap(Module &M) {
  idAssigner = new IDAssigner();
  PassManager *pm = new PassManager;
  Util::addTargetDataToPM(&M, pm);
  pm->add(idAssigner);
  pm->run(M);
}

void KleeTraceUtil::initKModule(KModule *kmodule) {
  this->kmodule = kmodule;
}

void KleeTraceUtil::load(const char *tracePath, DynInstrVector *trace) {
  //NOP.
}

void KleeTraceUtil::store(const char *tracePath) {
  //NOP.
}

void KleeTraceUtil::record(DynInstrVector *trace, void *instr, void *state, void *f) {
  record(trace, (KInstruction *)instr, (ExecutionState *)state, (Function *)f);
}

void KleeTraceUtil::record(DynInstrVector *trace, KInstruction *kInstr,
  ExecutionState *state, Function *f) {
  Instruction *instr = kInstr->inst;
  // Ignore an instruction if it is not from the original module.
  if (idAssigner->getInstructionID(instr) == U_NEG1)
    return;

  // Real recording.
  if (Util::isPHI(instr)) {
    recordPHI(trace, kInstr, state);
  } else if (Util::isBr(instr)) {
    recordBr(trace, kInstr, state);
  }  else if (Util::isRet(instr)) {
    recordRet(trace, kInstr, state);
  }  else if (Util::isCall(instr)) {
    recordCall(trace, kInstr, state, f);
  } else if (Util::isMem(instr)) {
    if (Util::isLoad(instr))
      recordLoad(trace, kInstr, state);
    else if (Util::isStore(instr))
      recordStore(trace, kInstr, state);
  } else {
    recordNonMem(trace, kInstr, state);    
  }
}

void KleeTraceUtil::recordPHI(DynInstrVector *trace, klee::KInstruction *kInstr,
  klee::ExecutionState *state) {
  DynPHIInstr *phi = new DynPHIInstr;
  phi->setIndex(trace->size());
  phi->setTid(0);
  phi->setOrigInstrId(idAssigner->getInstructionID(kInstr->inst));
  trace->push_back(phi);
}

void KleeTraceUtil::recordBr(DynInstrVector *trace, klee::KInstruction *kInstr,
  klee::ExecutionState *state) {
  /*errs() << "KleeTraceUtil::recordBr: " << idAssigner->getInstructionID(kInstr->inst)
    << ": " << *(kInstr->inst) << "\n";*/
  DynBrInstr *br = new DynBrInstr;
  br->setIndex(trace->size());
  br->setTid(0);
  br->setOrigInstrId(idAssigner->getInstructionID(kInstr->inst));
  trace->push_back(br);
}

void KleeTraceUtil::recordRet(DynInstrVector *trace, klee::KInstruction *kInstr,
  klee::ExecutionState *state) {
  DynRetInstr *ret = new DynRetInstr;
  ret->setIndex(trace->size());
  ret->setTid(0);
  ret->setOrigInstrId(idAssigner->getInstructionID(kInstr->inst));
  trace->push_back(ret);
}

void KleeTraceUtil::recordCall(DynInstrVector *trace, klee::KInstruction *kInstr,
  klee::ExecutionState *state, Function *f) {
  DynCallInstr *call = new DynCallInstr;
  call->setIndex(trace->size());
  call->setTid(0);
  call->setOrigInstrId(idAssigner->getInstructionID(kInstr->inst));
  call->setCalledFunc(f);
  trace->push_back(call);
}

void KleeTraceUtil::recordNonMem(DynInstrVector *trace, KInstruction *kInstr,
  ExecutionState *state) {
  DynInstr *instr = new DynInstr;
  instr->setIndex(trace->size());
  instr->setTid(0);
  instr->setOrigInstrId(idAssigner->getInstructionID(kInstr->inst));
  trace->push_back(instr);  
}

void KleeTraceUtil::recordLoad(DynInstrVector *trace, KInstruction *kInstr,
  ExecutionState *state) {
  DynMemInstr *load = new DynMemInstr;
  load->setIndex(trace->size());
  load->setTid(0);
  load->setOrigInstrId(idAssigner->getInstructionID(kInstr->inst));
  load->setConAddr(0);// TBD: add sym/concrete hybrid execution and get concrete mem addr.
  load->setSymAddr(eval(kInstr, 0, *state).value);
  trace->push_back(load);    
}

void KleeTraceUtil::recordStore(DynInstrVector *trace, KInstruction *kInstr,
  ExecutionState *state) {
  DynMemInstr *store = new DynMemInstr;
  store->setIndex(trace->size());
  store->setTid(0);
  store->setOrigInstrId(idAssigner->getInstructionID(kInstr->inst));
  store->setConAddr(0);// TBD: add sym/concrete hybrid execution and get concrete mem addr.
  store->setSymAddr(eval(kInstr, 1, *state).value);
  trace->push_back(store);      
}

/* Borrowed from the Executor.cpp in klee. */
const Cell& KleeTraceUtil::eval(KInstruction *ki, unsigned index, 
                           ExecutionState &state) const {
  assert(index < ki->inst->getNumOperands());
  int vnumber = ki->operands[index];

  assert(vnumber != -1 &&
         "Invalid operand to eval(), not a value or constant!");

  // Determine if this is a constant or not.
  if (vnumber < 0) {
    unsigned index = -vnumber - 2;
    return kmodule->constantTable[index];
  } else {
    unsigned index = vnumber;
    StackFrame &sf = state.stack.back();
    return sf.locals[index];
  }
}

void KleeTraceUtil::preProcess(DynInstrVector *trace) {
  // For each path, must clear ctx mgr.
  ctxMgr->clear();
  
  for (size_t i = 0; i < trace->size(); i++) {
    DynInstr *dynInstr = trace->at(i);
    Instruction *instr = idAssigner->getInstruction(dynInstr->getOrigInstrId());
    
    // (1) For each dynamic instruction, setup calling context.
    ctxMgr->setCallStack(dynInstr);

    // (2) For each dynamic phi instruction, setup incoming index.
    if (Util::isPHI(instr)) {
      DynInstr *prevInstr = NULL;
      for (int j = i - 1; j >= 0; j--) {
        prevInstr = trace->at(i);
        if (prevInstr->getTid() == dynInstr->getTid())
          break;
      }
      assert(prevInstr);
      BasicBlock *bb = Util::getBasicBlock(idAssigner->getInstruction(prevInstr->getOrigInstrId()));
      PHINode *phi = dyn_cast<PHINode>(instr);
      int idx = phi->getBasicBlockIndex(bb);
      ((DynPHIInstr *)dynInstr)->setIncomingIndex(idx);
    }
    
    // (3) For each dynamic ret instruction, setup its dynamic call instruction.
    if (Util::isRet(instr)) {
      DynRetInstr *ret = (DynRetInstr *)dynInstr;
      DynCallInstr *call = ctxMgr->getCallOfRet(ret);
      if (!call)
        assert(ret->getCallingCtx()->size() == 0);
      else 
        ret->setDynCallInstr(call);
    }
    
    // (4) For each dynamic pthread_create(), setup its child thread id.
    if (Util::isCall(instr)) {
      DynCallInstr *call = (DynCallInstr *)dynInstr;
      if (Util::isThreadCreate(call))
        ((DynSpawnThreadInstr *)call)->setChildTid(call->getTid()+1);
      /* Must make sure Gang's thread model in KLEE still make this rule hold:
      a child's tid is the parent's +1. */
    }

    // (5) Update per-tid callstack. This must happen after (3), which calls getCallOfRet().
    if (Util::isCall(instr) || Util::isRet(instr))
      ctxMgr->updateCallStack(dynInstr);

    // Debug print.
    fprintf(stderr, "\n\nINDEX " SZ ", THREAD-ID: %d, INSTR-ID: %d, OP: %s\n",
      dynInstr->getIndex(), dynInstr->getTid(), dynInstr->getOrigInstrId(),
      instr->getOpcodeName());
    ctxMgr->printCallStack(dynInstr);
    fprintf(stderr, "\n\n");
  }
}

void KleeTraceUtil::postProcess(DynInstrVector *trace) {
  for (size_t i = 0; i < trace->size(); i++)
    delete trace->at(i);
  trace->clear();
}

