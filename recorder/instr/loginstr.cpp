#include "util.h"
#include "loggable.h"
#include "loginstr.h"
#include "llvm/LLVMContext.h"
#include "llvm/Support/TypeBuilder.h"
#include "llvm/Support/IRBuilder.h"
#include "llvm/Support/IRBuilder.h"

using namespace llvm;
using namespace std;

namespace tern {

char LogInstr::ID = 0;

void LogInstr::getAnalysisUsage(AnalysisUsage &AU) const {
  AU.setPreservesCFG();
  ModulePass::getAnalysisUsage(AU);
}

bool LogInstr::runOnModule(Module &M) {

  const FunctionType *functype;

  targetData = getAnalysisIfAvailable<TargetData>();
  context = &getGlobalContext();

  addrType = Type::getInt8PtrTy(*context);
  dataType = Type::getInt64Ty(*context);

  getEscapeFuncs(M);
  markLoggableCallees(M);

  functype = TypeBuilder<void (types::i<32>), false>::get(*context);
  logInsid = dyn_cast<Value>(M.getOrInsertFunction("tern_log_insid", functype));
  functype = TypeBuilder<void (types::i<32>, types::i<8>*,
                               types::i<64>), false>::get(*context);
  logLoad  = dyn_cast<Value>(M.getOrInsertFunction
                             ("tern_log_load", functype));
  logStore = dyn_cast<Value>(M.getOrInsertFunction
                             ("tern_log_store", functype));
  functype = TypeBuilder<void (types::i<32>, types::i<32>, types::i<32>,
                               types::i<8>*, ...), false>::get(*context);
  logCall = dyn_cast<Value>(M.getOrInsertFunction("tern_log_call", functype));
  functype = TypeBuilder<void (types::i<32>, types::i<32>, types::i<32>,
                               types::i<8>*, types::i<64>), false>::get(*context);
  logRet = dyn_cast<Value>(M.getOrInsertFunction("tern_log_ret", functype));

  forallfunc(M, fi) {
    // instr function @fi if it is loggable
    if(LoggableFunc(fi))
      instrFunc(*fi);
  }

  return true;
}

void LogInstr::getEscapeFuncs(Module &M) {
  unsigned funcid = Intrinsic::num_intrinsics;
  forallfunc(M, fi) {
    if(!LoggableCallee(fi))
      continue;
    for(Value::use_iterator ui=fi->use_begin(), E=fi->use_end();
        ui != E; ++ui) {
      if(isa<CallInst>(*ui) || isa<InvokeInst>(*ui)) { // use is call
        CallSite cs(dyn_cast<Instruction>(*ui));
        for(CallSite::arg_iterator ai=cs.arg_begin(); ai!=cs.arg_end(); ++ai)
          if(*ai == fi) {// use is argument of a call ==> escape
            escape_funcs[fi] = ++funcid;
          }
      } else { // any other use ==> escape
        escape_funcs[fi] = ++funcid;
      }
    }
  }
}

/// create function tern_all_loggable_callees() and mark loggable callees
void LogInstr::markLoggableCallees(Module &M) {
  const char *markall_name = "tern_all_loggable_callees";
  const FunctionType *functype = TypeBuilder<void (void), false>::get(*context);

  Function *markall = dyn_cast<Function>
    (M.getOrInsertFunction(markall_name, functype));
  BasicBlock *entry = BasicBlock::Create(*context, "entry", markall);

  const char *markone_name = "tern_loggable_callee";
  functype = TypeBuilder<void (types::i<8>*, types::i<32>),
    false>::get(*context);
  Function *markone = dyn_cast<Function>
    (M.getOrInsertFunction(markone_name, functype));

  IRBuilder<> builder(entry);
  forall(escape_map_t, fi, escape_funcs) {
    Value *args[2];
    args[0] = builder.CreateBitCast(fi->first, addrType, "tern_funcast");
    args[1] = ConstantInt::get(Type::getInt32Ty(*context), fi->second);
    builder.CreateCall2(markone, args[0], args[1]);
  }

  BasicBlock *ret = BasicBlock::Create(*context, "ret", markall);
  builder.CreateBr(ret);
  builder.SetInsertPoint(ret);
  builder.CreateRetVoid();
}

void LogInstr::instrFunc(Function &F) {
  forall(Function, BB, F) {
    BasicBlock::iterator cur, prv;
    for(cur=BB->begin(); cur!=BB->end();) {
      prv = cur;
      cur ++;
      if(!LoggableInstruction(prv))
        continue;
      // mark instruction loggable
      SetLoggable(*context, prv);
      switch(prv->getOpcode()) {
      case Instruction::Load:
        instrLoad(dyn_cast<LoadInst>(prv));
        break;
      case Instruction::Store:
        instrStore(dyn_cast<StoreInst>(prv));
        break;
      case Instruction::Call:
      case Instruction::Invoke:
        instrCall(prv);
        break;
      default: // no other ins loggable, log FirstNonPHI
        instrFirstNonPHI(prv);
      }
    }
  }
}

Value *LogInstr::castIfNecessary(Value *srcval, const Type *dst,
                                 Instruction *insert) {
  const Type *src = srcval->getType();
  if(src == dst) return srcval;
  if(src->isPointerTy())
    return CastInst::CreatePointerCast(srcval, dst, "tern_ptrcast", insert);
  return CastInst::CreateZExtOrBitCast(srcval, dst, "tern_valcast", insert);
}

void LogInstr::instrLoadStore(Value *insid, Value *addr,
                              Value *data, Instruction *ins, bool isload) {
  BasicBlock::iterator ii = ins;
  Instruction *insert_ins = ++ii;  // can't be NULL; every BB has a terminator

  Value *logFunc = (isload? logLoad : logStore);

  addr = castIfNecessary(addr, addrType, insert_ins);
  data = castIfNecessary(data, dataType, insert_ins);

  vector<Value*> args;
  args.push_back(insid);
  args.push_back(addr);
  args.push_back(data);
  CallInst::Create(logFunc, args.begin(), args.end(), "", insert_ins);
}

void LogInstr::instrLoad(LoadInst *load) {
  Value *insid = GetInsID(load); assert(insid);
  Value *addr = load->getPointerOperand();
  Value *data = load;
  instrLoadStore(insid, addr, data, load, true);
}

void LogInstr::instrStore(StoreInst *store) {
  Value *insid = GetInsID(store); assert(insid);
  Value *addr = store->getPointerOperand();
  Value *data = store->getOperand(0);
  instrLoadStore(insid, addr, data, store, false);
}

void LogInstr::instrFirstNonPHI(Instruction *ins) {
  assert(ins == ins->getParent()->getFirstNonPHI());
  Value *insid = GetInsID(ins); assert(insid);

  vector<Value*> args;
  args.push_back(insid);
  CallInst::Create(logInsid, args.begin(), args.end(), "", ins);
}

// FIXME: InvokeInst may throw an exception, which we currently don't log
void LogInstr::instrCall(Instruction *call) {
  CallSite cs(call);
  Value *insid = GetInsID(call); assert(insid);
  unsigned narg = cs.arg_end() - cs.arg_begin();
  Value *nargval = ConstantInt::get(Type::getInt32Ty(*context), narg);

  Value *func = cs.getCalledValue();
  func = castIfNecessary(func, addrType, call);

  Value *indir;
  indir = (cs.getCalledFunction() ?
           ConstantInt::get(Type::getInt32Ty(*context), 0)
           : ConstantInt::get(Type::getInt32Ty(*context), 1));

  // log call
  vector<Value*> args;
  args.push_back(indir);
  args.push_back(insid);
  args.push_back(nargval);
  args.push_back(func);
  for(CallSite::arg_iterator ai=cs.arg_begin(); ai!=cs.arg_end(); ++ai) {
    Value *arg = ai->get();
    arg = castIfNecessary(arg, dataType, call);
    args.push_back(arg);
  }
  CallInst::Create(logCall, args.begin(), args.end(), "", call);

  // log return
  BasicBlock::iterator ii = call;
  Instruction *next_ins = ++ii;  // can't be NULL; every BB has a terminator
  Value *data = call;
  const Type *voidType = Type::getVoidTy(*context);
  if(data->getType() != voidType)
    data = castIfNecessary(data, dataType, next_ins);
  else
    data = Constant::getNullValue(dataType);
  args.clear();
  args.push_back(indir);
  args.push_back(insid);
  args.push_back(nargval);
  args.push_back(func);
  args.push_back(data);
  CallInst::Create(logRet, args.begin(), args.end(), "", next_ins);
}

} // namespace tern

namespace {

static RegisterPass<tern::LogInstr>
X("loginstr", "instrumentation for recoding", false, false);

}
