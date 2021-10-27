#include "llvm/ADT/SmallVector.h"

#include "llvm/IR/DerivedTypes.h"
#include "llvm/Pass.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

int store = 0;
int fence = 1;
int flush = 2;
StringRef flush_function_name = StringRef("flush_pm");

std::vector<StringRef> instrumenation_functions = {StringRef("print_trace"), StringRef("print_trace_addr")};

Type* getIntPtrTy(Module& M){
  LLVMContext *C = &(M.getContext());
  int LongSize = M.getDataLayout().getPointerSizeInBits();
  Type* IntptrTy = Type::getIntNTy(*C, LongSize);
  return IntptrTy;
}

static FunctionCallee initializeCallbacks(Module& M){
  LLVMContext *C = &(M.getContext());
  IRBuilder<> IRB(*C);
  Type * VoidTy = IRB.getVoidTy();
  Type * IntTy = IRB.getInt32Ty();
  FunctionType * func_type = FunctionType::get(VoidTy,{IntTy},false);	

  return M.getOrInsertFunction("print_trace", func_type);
}

static FunctionCallee initializeStoreCallbacks(Module& M, Type * IntptrTy){
  LLVMContext *C = &(M.getContext());
  IRBuilder<> IRB(*C);
  Type * VoidTy = IRB.getVoidTy();
  Type * IntTy = IRB.getInt32Ty();
  // Type * IntptrTy = IRB.getIntPtrTy();

  // SmallVector<Type *, 2> Args = {IntTy, IntptrTy};

  // Type * VoidTy = Type::getVoidTy(M.getContext());
  FunctionType * func_type = FunctionType::get(VoidTy,{IntTy, IntptrTy},false);	

  return M.getOrInsertFunction("print_trace_addr", func_type);
}

bool instrumented_function(Function &F){
  for(auto instrumenation_function : instrumenation_functions){
    if (F.getName().equals(instrumenation_function)){
      return true;
    }
  }
  return false;
}

bool instrumentFunction(Function &F){
  if (instrumented_function(F)){
    return false;
  }

  bool changed = false;
  Type * IntptrTy = getIntPtrTy(*F.getParent());
  FunctionCallee callback_func_store = initializeStoreCallbacks(*F.getParent(), IntptrTy); // TODO: create instrument function
  FunctionCallee callback_func = initializeCallbacks(*F.getParent());

  errs().write_escaped(F.getName()) <<" \n";

  // instrument flush function
  if (F.getName().contains(flush_function_name)){
    // errs()<<"found flush \n";
    Instruction& first_instr = *(F.getEntryBlock().getFirstInsertionPt());
    IRBuilder<> IRB(&first_instr);
    IRB.CreateCall(callback_func, {ConstantInt::get(IRB.getInt32Ty(), flush)}); 
    changed = true;
    return changed;
  }
  
  for (auto &BB : F) {
    for (auto &Inst : BB) {
      // instrument fence instruction
      if(Inst.getOpcode() == Instruction::Fence){
        IRBuilder<> IRB(&Inst);
        IRB.CreateCall(callback_func, {ConstantInt::get(IRB.getInt32Ty(), fence)}); 

        changed = true;
      } // if fence instr

      // instrument store instruction
      if(Inst.getOpcode() == Instruction::Store){
        IRBuilder<> IRB(&Inst);
        Value* Addr  = (&Inst.getOperandUse(1))->get(); //?? Is it the way to do it??
        Value *AddrLong = IRB.CreatePointerCast(Addr, IntptrTy);
        IRB.CreateCall(callback_func_store, {ConstantInt::get(IRB.getInt32Ty(), store), AddrLong}); 
        changed = true;
        
      } // if store instr
    }
  }
  return changed;
}

namespace {
struct FencePass : public FunctionPass {
  static char ID;
  FencePass() : FunctionPass(ID) {}

  bool runOnFunction(Function &F) override {
    return instrumentFunction(F);
  }
}; // end of struct Fence
}  // end of anonymous namespace

char FencePass::ID = 0;
static RegisterPass<FencePass> X("fencePass", "Fence Pass",
                             false /* Only looks at CFG */,
                             false /* Analysis Pass */);
