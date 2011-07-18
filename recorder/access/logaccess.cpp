/* Author: Junfeng Yang (junfeng@cs.columbia.edu) */
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>

#include "util.h"
#include "common/instr/instrutil.h"
#include "recorder/instr/loggable.h"
#include "llvm/Function.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/CFG.h"
#include "llvm/Support/CallSite.h"
#include "logaccess.h"

//#define _DEBUG_RECORDER

#ifdef _DEBUG_RECORDER
#  define dprintf(fmt...) fprintf(stderr, fmt)
#else
#  define dprintf(fmt...)
#endif

using namespace std;
using namespace llvm;

namespace tern {

RawLog::RawLog() : _fd(-1), _num(0), _buf(NULL) {}

RawLog::RawLog(const char* filename) {
  struct stat st;
  long pagesz;

  _fd = open(filename, O_RDONLY);  assert(_fd >= 0);
  fstat(_fd, &st);
  assert(st.st_size % RECORD_SIZE == 0
         && "file size is not multiple of record size!");
  _num = st.st_size / RECORD_SIZE;
  pagesz = getpagesize();
  _mapsz = ((st.st_size+pagesz-1)&~(pagesz-1));
  _buf = (char*)mmap(0, _mapsz, PROT_READ, MAP_SHARED, _fd, 0);

  if(_buf == MAP_FAILED) {
    perror("mmap");
    abort();
  }
}

RawLog::~RawLog() {
  if(_buf)
    munmap(_buf, _mapsz);
  if(_fd >= 0)
    close(_fd);
}

RawLog::iterator RawLog::begin() {
  return iterator(this, 0);
}

RawLog::iterator RawLog::end() {
  return iterator(this, _num);
}

RawLog::reverse_iterator RawLog::rbegin() {
  return reverse_iterator(end());
}

RawLog::reverse_iterator RawLog::rend() {
  return reverse_iterator(begin());
}


void InstLog::append(const RawLog::iterator &ri) {
  ExecutedInstID id;
  id.inst = ri.getIndex();
  id.isLogged = 1;
  instLog.push_back(id);

#ifdef _DEBUG_RECORDER
  printExecutedInst(errs(), id, true) << "\n";
#endif
}

void InstLog::append(Instruction *I) {
  ExecutedInstID id;
  unsigned insid = getIDManager()->getInstructionID(I);

if(insid == INVALID_INSID) {
  errs() << *I << "\n";
  insid = getIDManager()->getInstructionID(I);
}

  assert(insid != INVALID_INSID && "instruction has no valid id!");
  // TODO assert IDM->size() not too large once and for all
  id.inst = insid;
  id.isLogged = 0;
  instLog.push_back(id);

#ifdef _DEBUG_RECORDER
  printExecutedInst(errs(), id, true) << "\n";
#endif
}

raw_ostream &InstLog::printExecutedInst(raw_ostream &o,
                       ExecutedInstID id, bool details) {
  unsigned insid;
  if(id.isLogged) {
    o << "L idx=" << id.inst;
    raw_iterator ri(rawLog, id.inst);
    o << " " << *ri;
    insid = ri->insid;
  } else {
    o << "P ins=" << id.inst;
    insid = id.inst;
  }

  if(details) {
    Instruction *I = getIDManager()->getInstruction(insid);
    if(I)
      o << *I;
  }
  return o;
}

InstLog::func_map         InstLog::funcsEscape;
InstLog::func_map         InstLog::funcsCallLogged;
InstLog::reverse_func_map InstLog::funcsIDMap;

unsigned InstLog::getFuncID(Function *F) {
  func_map::iterator fi = funcsCallLogged.find(F);
  if(fi == funcsCallLogged.end())
    return (unsigned)-1;
  return fi->second;
}

Function *InstLog::getFunc(unsigned funcid) {
  reverse_func_map::iterator fi = funcsIDMap.find(funcid);
  if(fi == funcsIDMap.end())
    return NULL;
  return fi->second;
}

void InstLog::setFuncMap(const char *file, const Module& M) {
  ifstream f(file);
  assert(f && "can't open func map file for read!");

  unsigned funcid;
  unsigned escape;
  string funcname;
  for(;;) {
    f >> funcid;   if(!f) break;
    f >> escape;   if(!f) break;
    f >> funcname; if(!f) break;

    assert(funcid > Intrinsic::num_intrinsics
           && "invalid func id in .funcs file");
    assert((escape==0||escape==1)
           && "invalid escape value id in .funcs file");
    assert(!funcname.empty()
           && "invalid func name in .funcs file");

    Function *F = M.getFunction(funcname);
    assert(F && "can't find a function logged in .funcs file in the module!");
    assert(funcsCallLogged.find(F) == funcsCallLogged.end()
           && "redundant function names in .funcs file!");

    funcsCallLogged[F] = funcid;
    funcsIDMap[funcid] = F;
    if(escape)
      funcsEscape[F] = escape;
  }
}

/// this call should be the call that changed the execution from curBB to
/// nxtBB
bool InstLogBuilder::nextInstFromCall() {
  Function *F;
  BasicBlock *nxtBB;
  const CallSite cs(cur_ii);

  F = cs.getCalledFunction();
  if(F && !funcBodyLogged(F)) {
    ++ cur_ii;
    return false;
  }

  // sanity: nxtBB is the entry of the called function
  nxtBB = nxt_ii->getParent();
  if(F == NULL) // indirect call
    F = nxtBB->getParent();
  assert(nxtBB == &F->getEntryBlock()
         && "no log record from a function entry block!");
  assert(nxtBB->getParent() == F
         && "call target is different than logged!");

  // return address
  BasicBlock::iterator ret_ii = cur_ii;
  ++ ret_ii;
  callStack.push(ret_ii);

  // move cur_ii
  cur_ii = nxtBB->begin();

  return true;
}

bool InstLogBuilder::nextInstFromReturn() {
  // TODO sanity: the previous instruction of ii is a call to this function
  assert(!callStack.empty() && "return and call mismatch!");
  cur_ii = callStack.top();
  callStack.pop();

  return true;
}

bool InstLogBuilder::nextInstFromJmp() {
  BasicBlock *nxtBB = nxt_ii->getParent();

  // sanity: nxtBB is one of the successors of curBB
  bool is_succ = false;
  BasicBlock *BB = cur_ii->getParent();
  for(succ_iterator si=succ_begin(BB); si!=succ_end(BB); ++si) {
    if(*si == nxtBB) {
      is_succ = true;
      break;
    }
  }
#ifdef _DEBUG_RECORDER
  if(!is_succ)
    dumpIterators();
#endif
  assert(is_succ && "successor BB of a branch is not logged!");
  cur_ii = nxtBB->begin();

  return true;
}

// can control-transfer more than once.  consider the example below:
//
// bb:
//    %1 = call foo();
//    br i1 %1, label %bb1, label bb2;
//
// suppose current instruction is the ret instruction from foo(), then
// we'll have to process br as well since there's no instruction logged
// between the two instructions shown above
//
void InstLogBuilder::getInbetweenInsts() {
  unsigned cur_insid = cur_ri->getInsid();
  cur_ii = getIDManager()->getInstruction(cur_insid);
  unsigned nxt_insid = nxt_ri->getInsid();
  nxt_ii = getIDManager()->getInstruction(nxt_insid);

  if(instLogged(cur_ii) != LogBBMarker) {
    assert(cur_ri->numRecForInst() == 1
           && "multiple records for one instruction must be specially handled!");
    instLog->append(cur_ri);
    ++ cur_ii;
  }

  int transfers = 0;
  while(cur_ii != nxt_ii) {
    instLog->append(cur_ii);

    // sanity: can only do one of these control transfer inst
    switch(cur_ii->getOpcode()) {
    case Instruction::Call:
    case Instruction::Invoke:
      transfers += nextInstFromCall();
      break;
    case Instruction::Ret:
      transfers += nextInstFromReturn();
      break;
    case Instruction::Unwind:
    case Instruction::IndirectBr:
    case Instruction::Switch:
    case Instruction::Br:
      transfers += nextInstFromJmp();
      break;
    case Instruction::Unreachable:
      assert(0 && "InstLogBuilder should never reach an UnrechableInst!");
    default:
      ++ cur_ii;
      break;
    }
  }

#if 0
if(transfers > 1)
  dumpIterators();
#endif

}

void InstLogBuilder::getInst() {
  // sanity
  unsigned insid = cur_ri->getInsid();
  BasicBlock::iterator ii = getIDManager()->getInstruction(insid);
  for(RawLog::iterator ri=cur_ri; ri!=nxt_ri; ++ri)
    assert(ri->insid == insid
           && "records are supposedly for the same instruction, "\
           "but have different instruction IDs!");

  if(cur_ri->type == CallRecTy) { // just append one inst to inst log
    instLog->append(cur_ri);
  } else { // a sync op; append all records from this op
    for(int i=0; i<cur_ri->numRecForInst(); ++i)
      instLog->append(cur_ri);
  }
}

void InstLogBuilder::getInstPrefix() {
  // TODO
}

void InstLogBuilder::getInstSuffix() {
  // TODO
}

InstLog *InstLogBuilder::create(const char *file) {
  RawLog *rawLog = new RawLog(file);
  return create(rawLog);
}


InstLog *InstLogBuilder::create(RawLog *log) {

  instLog = new InstLog(log);

  cur_ri = log->begin(); // thread_begin();
  nxt_ri =  cur_ri + 1; // first logged inst of the prog

  getInstPrefix(); // thread_begin() to first logged inst of the program

  bool calledPthreadExit = true;
  RawLog::iterator end_ri = log->end();
  -- end_ri;  // thread_end();
  if(!end_ri->validInsid()) {
    calledPthreadExit = false;
    -- end_ri; // last logged inst before ret from thread func (or main())
  }

  int nrec;
  while(nxt_ri != end_ri) {
    cur_ri = nxt_ri;
    nrec = cur_ri->numRecForInst();
    nxt_ri = cur_ri + nrec;

    if(nrec > 1)
      ;
    else
      getInbetweenInsts();
  }

  if(calledPthreadExit)
    instLog->append(end_ri); // append thread_end
  else
    getInstSuffix(); // last logged inst to ret from thread func (or main())

  return instLog;
}

void InstLogBuilder::dumpIterators() {
  BasicBlock *BB = cur_ii->getParent();
  BasicBlock *nxtBB = nxt_ii->getParent();
  errs() << "distance between raw iterators = " << nxt_ri - cur_ri << "\n";
  errs() << *cur_ri  << "\n";
  errs() << *nxt_ri  << "\n";
  errs() << "cur I"  << *cur_ii << "\n"
         << "nxt I"  << *nxt_ii << "\n";
  errs() << "cur BB" << *BB
         << "nxt BB" << *nxtBB;
}

static const char* recNames[] = {
  "marker",
  "load",
  "store",
  "call",
  "arg",
  "ret",
  "sync",
};

static raw_ostream& printInsidRec(raw_ostream& o, const InsidRec& rec) {
  assert(rec.type <= LastRecTy && "invalid log record type!");
  if(rec.insid == tern::INVALID_INSID_IN_REC)
    o << "ins=n/a";
  else
    o << "ins= " << rec.insid;
  return o << " " << recNames[rec.type];
}

raw_ostream& operator<<(raw_ostream& o, const InsidRec& rec) {
  switch(rec.type) {
  case InsidRecTy:
    printInsidRec(o, rec);
    break;
  case LoadRecTy:
    o << (const LoadRec&)rec;
    break;
  case StoreRecTy:
    o << (const StoreRec&)rec;
    break;
  case CallRecTy:
    o << (const CallRec&)rec;
    break;
  case ExtraArgsRecTy:
    o << (const ExtraArgsRec&)rec;
    break;
  case ReturnRecTy:
    o << (const ReturnRec&)rec;
    break;
  case SyncRecTy:
    o << (const SyncRec&)rec;
    break;
  }
  return o;
}

raw_ostream &operator<<(raw_ostream &o, const LoadRec &rec) {
  return printInsidRec(o, (const InsidRec&)rec)
    << " " << rec.addr << " = " << rec.data;
}
raw_ostream &operator<<(raw_ostream &o, const StoreRec &rec) {
  return printInsidRec(o, (const InsidRec&)rec)
    << " " << rec.addr << ", " << rec.data;
}
raw_ostream &operator<<(raw_ostream &o, const CallRec &rec) {
  Function *F = InstLog::getFunc(rec.funcid);
  printInsidRec(o, (const InsidRec&)rec);
  if(F)
    o << " " << F->getName();
  else
    o << " func" << rec.funcid;
  return o << "(" << rec.narg << " args)";
}
raw_ostream &operator<<(raw_ostream &o, const ExtraArgsRec &rec) {
  return printInsidRec(o, (const InsidRec&)rec) << "(...)";
}
raw_ostream &operator<<(raw_ostream &o, const ReturnRec &rec) {
  Function *F = InstLog::getFunc(rec.funcid);
  printInsidRec(o, (const InsidRec&)rec);
  if(F)
    o << " " << F->getName();
  else
    o << " func" << rec.funcid;
  return o << "() = " << rec.data;
}
raw_ostream &operator<<(raw_ostream &o, const SyncRec &rec) {
  printInsidRec(o, (const InsidRec&)rec)
    << " " << syncfunc::getName(rec.sync) << "()";
  if(NumRecordsForSync(rec.sync) == 2)
     o << (rec.after?"after":"before");
  return o << " turn = " << rec.turn;
}

} // namespace tern
