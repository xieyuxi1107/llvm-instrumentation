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

static FunctionCallee initializeCallbacks(Module& M){
  LLVMContext *C = &(M.getContext());
  IRBuilder<> IRB(*C);
  Type * VoidTy = IRB.getVoidTy();
  Type * IntTy = IRB.getInt32Ty();

  // Type * VoidTy = Type::getVoidTy(M.getContext());
  FunctionType * func_type = FunctionType::get(VoidTy,{IntTy},false);	

  return M.getOrInsertFunction("print_trace", func_type);
}

bool instrumentFunction(Function &F){
  bool changed = false;
  FunctionCallee callback_func = initializeCallbacks(*F.getParent()); // TODO: create instrument function
  
  // instrument flush function
  if (F.getName().contains(flush_function_name)){
    errs()<<"found flush \n";
    Instruction& first_instr = *(F.getEntryBlock().getFirstInsertionPt());
    IRBuilder<> IRB(&first_instr);
    IRB.CreateCall(callback_func, ConstantInt::get(IRB.getInt32Ty(), flush)); 
    changed = true;
    return changed;
  }
  
  for (auto &BB : F) {
    for (auto &Inst : BB) {
      // instrument fence instruction
      if(Inst.getOpcode() == Instruction::Fence){
        IRBuilder<> IRB(&Inst);
        IRB.CreateCall(callback_func, ConstantInt::get(IRB.getInt32Ty(), fence)); 

        // Instruction* newInst = CallInst::Create(callback_func, "", &Inst);
        changed = true;
        // errs() << "xieyuxi: instrumented ";
        // errs().write_escaped(F.getName()) <<" ";
        // errs() << BB.getName() << "\n ";
      } // if fence instr
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
