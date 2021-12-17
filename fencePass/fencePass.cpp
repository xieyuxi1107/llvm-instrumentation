#include "llvm/IR/Function.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"


#include "llvm/ADT/SmallVector.h"

#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/PassManager.h"


using namespace llvm;


namespace {

int store = 0;
int fence = 1;
int flush = 2;
StringRef flush_function_name = StringRef("nova_flush_buffer");
StringRef fence_function_name = StringRef("PERSISTENT_BARRIER");

std::vector<StringRef> instrumenation_functions = {StringRef("write_fence_trace_wrapper"), StringRef("write_flush_trace_wrapper"), StringRef("write_store_trace_wrapper")};

Type* getIntPtrTy(Module& M){
  LLVMContext *C = &(M.getContext());
  int LongSize = M.getDataLayout().getPointerSizeInBits();
  Type* IntptrTy = Type::getIntNTy(*C, LongSize);
  return IntptrTy;
}

static FunctionCallee initializeFenceCallbacks(Module& M){
  LLVMContext *C = &(M.getContext());
  IRBuilder<> IRB(*C);
  Type * VoidTy = IRB.getVoidTy();
  FunctionType * func_type = FunctionType::get(VoidTy,{},false);	

  return M.getOrInsertFunction("write_fence_trace_wrapper", func_type);
}

static FunctionCallee initializeStoreCallbacks(Module& M, Type * IntptrTy){
  LLVMContext *C = &(M.getContext());
  IRBuilder<> IRB(*C);
  Type * VoidTy = IRB.getVoidTy();
  Type * IntTy = IRB.getInt32Ty();
  // Type * IntptrTy = IRB.getIntPtrTy();

  // SmallVector<Type *, 2> Args = {IntTy, IntptrTy};

  // Type * VoidTy = Type::getVoidTy(M.getContext());
  FunctionType * func_type = FunctionType::get(VoidTy,{IntptrTy, IntTy},false);	

  return M.getOrInsertFunction("write_store_trace_wrapper", func_type);
}

static FunctionCallee initializeFlushCallbacks(Module& M, Type * IntptrTy){
  LLVMContext *C = &(M.getContext());
  IRBuilder<> IRB(*C);
  Type * VoidTy = IRB.getVoidTy();
  // Type * IntTy = IRB.getInt32Ty();

  FunctionType * func_type = FunctionType::get(VoidTy,{IntptrTy},false);	

  return M.getOrInsertFunction("write_flush_trace_wrapper", func_type);
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
    // errs()<<"skipped: ";
    // errs().write_escaped(F.getName()) <<" \n";
    return false;
  }

  bool changed = false;
  Type * IntptrTy = getIntPtrTy(*F.getParent());
  FunctionCallee callback_func_store = initializeStoreCallbacks(*F.getParent(), IntptrTy); 
  FunctionCallee callback_func_flush = initializeFlushCallbacks(*F.getParent(), IntptrTy);

  for (auto &BB : F) {
    for (auto &Inst : BB) {

      // instrument store instruction
      if(Inst.getOpcode() == Instruction::Store){
        IRBuilder<> IRB(&Inst);
        Value* Addr  = (&Inst.getOperandUse(1))->get(); //?? Is it the way to do it??
        Value *AddrLong = IRB.CreatePointerCast(Addr, IntptrTy);

        DataLayout* dataLayout = new DataLayout(F.getParent());
        StoreInst* storeInst = dyn_cast<StoreInst>(&Inst);
        Value* memoryPointer = storeInst->getPointerOperand();
        PointerType* pointerType = cast<PointerType>(memoryPointer->getType());                         
        int32_t storeSize = dataLayout->getTypeStoreSize(pointerType->getPointerElementType());
        //errs()<<"STORE ADDR: "<<AddrLong<<"\n";
        //errs()<<"STORE SIZE: "<<storeSize<<"\n";

        IRB.CreateCall(callback_func_store, {AddrLong, ConstantInt::get(IRB.getInt32Ty(), storeSize)}); 
        changed = true;
        
      } // if store instr


      // if (auto CI = dyn_cast<CallInst>(&Inst)){
      //   Function* fp = CI->getCalledFunction();
      //   StringRef fname;
      //   if (fp==NULL) {
      //       Value* v=CI->getCalledOperand();
      //       Value* sv = v->stripPointerCasts();
      //       fname = sv->getName();
      //   }else{
      //     fname = fp->getName();
      //   }
      //   // errs()<<fname<<"\n";
      //   if(fname.contains(flush_function_name)){
      //     errs()<<"found flush!! ";
      //     errs()<<fname<<"\n";
      //     IRBuilder<> IRB(&Inst);  
      //     Value* Addr = (&CI->getArgOperandUse(0))->get();
      //     Value *AddrLong = IRB.CreatePointerCast(Addr, IntptrTy);
      //     Value* sz = (&CI->getArgOperandUse(1))->get();
      //     Value *szLong = IRB.CreatePointerCast(sz, IntptrTy);

      //     IRB.CreateCall(callback_func_flush, {AddrLong, szLong}); 
      //     changed = true;
      //   }  
      // }
    } 
  }
  return changed;
}


struct StoreFlushInstru : PassInfoMixin<StoreFlushInstru> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
    // errs()<<"instruction pass\n";
    if (!instrumentFunction(F))
      return PreservedAnalyses::all();
    return PreservedAnalyses::all();
  }
};

bool instrument_fence_once = false;
bool instrument_flush_once = false;
struct FenceInstru : PassInfoMixin<FenceInstru> {
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
      // instrument flush function
    // errs()<<"function pass: ";
    // errs()<<F.getName()<<"\n";
    Type * IntptrTy = getIntPtrTy(*F.getParent());
    FunctionCallee callback_func_fence = initializeFenceCallbacks(*F.getParent());
    FunctionCallee callback_func_flush = initializeFlushCallbacks(*F.getParent(), IntptrTy);

    // fence
    if(F.getName().contains(fence_function_name) && !instrument_fence_once){
      instrument_fence_once = true;
      errs()<<"found fence!! ";
      Instruction& first_instr = *(F.getEntryBlock().getFirstInsertionPt());
      IRBuilder<> IRB(&first_instr);
      errs()<<F.getName()<<" \n";
      IRB.CreateCall(callback_func_fence, {}); 
      return PreservedAnalyses::all();
    }

    // flush
    if(F.getName().contains(flush_function_name) && !instrument_flush_once){
      instrument_flush_once = true;
      errs()<<"found flush!! ";
      Instruction& first_instr = *(F.getEntryBlock().getFirstInsertionPt());
      IRBuilder<> IRB(&first_instr);
      errs()<<F.getName()<<" \n";

      Argument* Addr = F.getArg(0);
      Value *AddrLong = IRB.CreatePointerCast(Addr, IntptrTy);
      // errs()<<"first \n";
      // Argument* sz = F.getArg(1);
      // Value *szLong = IRB.CreatePointerCast(sz, IntptrTy);
      // errs()<<"second \n";
      // IRB.CreateCall(callback_func_fence, {}); 
      IRB.CreateCall(callback_func_flush, {AddrLong}); 
      // IRB.CreateCall(callback_func_flush, {AddrLong, szLong}); 
      // errs()<<"create call \n";
      return PreservedAnalyses::all();
    }
    // errs()<<"DONE\n";

    // bool changed = false;

    // if(F.getName().contains(StringRef("nova_initialize_inode_log"))){
    //   errs()<<"nova_initialize_inode_log function \n";
    // for (auto &BB : F) {
    //   for (auto &Inst : BB) {
    //     if (auto CI = dyn_cast<CallInst>(&Inst)){
    //       Function* fp = CI->getCalledFunction();
    //       StringRef fname;
    //       if (fp==NULL) {
    //           Value* v=CI->getCalledOperand();
    //           Value* sv = v->stripPointerCasts();
    //           fname = sv->getName();
    //           errs()<<fname<<" GRRRR \n";
    //       }else{
    //         fname = fp->getName();
    //         errs()<<fname<<" SUURRE \n";
    //       }
    //       // errs()<<fname<<"\n";
    //       if(fname.contains(flush_function_name)){
    //         errs()<<"found flush!! ";
    //         errs()<<fname<<"\n";
    //         IRBuilder<> IRB(&Inst);  
    //         Value* Addr = (&CI->getArgOperandUse(0))->get();
    //         Value *AddrLong = IRB.CreatePointerCast(Addr, IntptrTy);
    //         Value* sz = (&CI->getArgOperandUse(1))->get();
    //         Value *szLong = IRB.CreatePointerCast(sz, IntptrTy);

    //         IRB.CreateCall(callback_func_flush, {AddrLong, szLong}); 
    //         changed = true;
    //       }  
    //     } // if callinst
    //     } // Inst
    //   } // for BB
    // } // debug
    // if(changed){
    //   return PreservedAnalyses::all();
    // }
    return PreservedAnalyses::all();
  }
};

} // namespace

/* New PM Registration */
llvm::PassPluginLibraryInfo getInstruPluginInfo() {
  return {LLVM_PLUGIN_API_VERSION, "InstruPass", LLVM_VERSION_STRING,
          [](PassBuilder &PB) {
            PB.registerVectorizerStartEPCallback(
                [](llvm::FunctionPassManager &PM,
                   llvm::PassBuilder::OptimizationLevel Level) {
                  PM.addPass(StoreFlushInstru());
                });

            PB.registerScalarOptimizerLateEPCallback(
                [](llvm::FunctionPassManager &PM,
                   llvm::PassBuilder::OptimizationLevel Level) {
                  PM.addPass(FenceInstru());
                });
          }};
}

// #ifndef LLVM_INSTRU_LINK_INTO_TOOLS
extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return getInstruPluginInfo();
}
// #endif
