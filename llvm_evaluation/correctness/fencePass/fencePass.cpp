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

int store = 0;
int fence = 1;
int flush = 2;
StringRef flush_function_name = StringRef("nvdimm_flush");
// StringRef fence_function_name = StringRef("PERSISTENT_BARRIER");

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
    errs()<<"skipped: ";
    errs().write_escaped(F.getName()) <<" \n";
    return false;
  }

  bool changed = false;
  Type * IntptrTy = getIntPtrTy(*F.getParent());
  FunctionCallee callback_func_store = initializeStoreCallbacks(*F.getParent(), IntptrTy); // TODO: create instrument function
  FunctionCallee callback_func_fence = initializeFenceCallbacks(*F.getParent());
  FunctionCallee callback_func_flush = initializeFlushCallbacks(*F.getParent(), IntptrTy);

  errs().write_escaped(F.getName()) <<" \n";

  // // instrument flush function
  // if (F.getName().contains(flush_function_name)){
  //   errs()<<"found flush \n";
  //   Instruction& first_instr = *(F.getEntryBlock().getFirstInsertionPt());
  //   IRBuilder<> IRB(&first_instr);
  //   IRB.CreateCall(callback_func, {ConstantInt::get(IRB.getInt32Ty(), flush)}); 
  //   changed = true;
  //   return changed;
  // }
  
  for (auto &BB : F) {
    // flush
    // if (F.getName().contains(flush_function_name)){
    //   Instruction& first_instr = *(F.getEntryBlock().getFirstInsertionPt());
    //   IRBuilder<> IRB(&first_instr);
    //   errs()<<"Found Flush:";
    //   errs().write_escaped(F.getName()) <<" \n";
    //   // Value* Addr = (&CI->getArgOperandUse(0))->get();
    //   Argument* Addr = F.getArg(0);
    //   Value *AddrLong = IRB.CreatePointerCast(Addr, IntptrTy);

    //   IRB.CreateCall(callback_func_flush, {AddrLong, ConstantInt::get(IRB.getInt32Ty(), 0)}); 
    //   changed = true;
    // }

    for (auto &Inst : BB) {
      // instrument fence instruction
      if(Inst.getOpcode() == Instruction::Fence){
       IRBuilder<> IRB(&Inst);
       IRB.CreateCall(callback_func_fence, {}); 

        changed = true;
      } // if fence instr

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

      if (auto CI = dyn_cast<CallInst>(&Inst)){
      Function* fp = CI->getCalledFunction();
      StringRef fname;
      if (fp==NULL) {
          Value* v=CI->getCalledOperand();
          Value* sv = v->stripPointerCasts();
          fname = sv->getName();
      }else{
        fname = fp->getName();
      }
      errs()<<"Called function:";
      errs()<<fname<<"\n";
      if(fname.contains(flush_function_name)){
        IRBuilder<> IRB(&Inst);  
        Value* Addr = (&CI->getArgOperandUse(0))->get();
        Value *AddrLong = IRB.CreatePointerCast(Addr, IntptrTy);

        IRB.CreateCall(callback_func_flush, {AddrLong}); 
        changed = true;
      }
    }
    }
  }
  return changed;
}


// struct StoreFenceInstru : PassInfoMixin<StoreFenceInstru> {
//   PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
//     if (!instrumentFunction(F))
//       return PreservedAnalyses::all();
//     return PreservedAnalyses::none();
//   }
// };


// struct FlushInstru : PassInfoMixin<FlushInstru> {
//   PreservedAnalyses run(Function &F, FunctionAnalysisManager &) {
//       // instrument flush function
//     Type * IntptrTy = getIntPtrTy(*F.getParent());
//     FunctionCallee callback_func_flush = initializeFlushCallbacks(*F.getParent(), IntptrTy);
//     if (F.getName().contains(flush_function_name)){
//       errs()<<"found flush \n";

//       Instruction& first_instr = *(F.getEntryBlock().getFirstInsertionPt());
//       IRBuilder<> IRB(&first_instr);

//       // get flush arguments
//       // Argument* args = F.getArg();
//       Argument* Addr = F.getArg(0); //?? Is it the way to do it??
//       // Attribute attri = Addr->getAttribute(Attribute::AttrKind::None);
//       // uint32_t val = attri.getValueAsInt();
//       Value *AddrLong = IRB.CreatePointerCast(Addr, IntptrTy);


//       // DataLayout* dataLayout = new DataLayout(F.getParent());
//       // StoreInst* storeInst = dyn_cast<StoreInst>(&Inst);
//       // Value* memoryPointer = storeInst->getPointerOperand();
//       // PointerType* pointerType = cast<PointerType>(Addr->getType());                         
//       // int32_t storeSize = dataLayout->getTypeStoreSize(pointerType->getPointerElementType());

//       // auto* ci = dyn_cast<ConstantInt>(F.getArg(0));
//       // const APInt& Addr1 = ci->getValue();
//       // Module& M = *F.getParent();
//       // LLVMContext& C = M.getContext();
//       // Constant* constValue = ConstantInt::get(C, Addr1);
//       // errs() << ci->getValue() << "\n";
//       // errs() << *(F.getArg(0)) << "\n";

//       IRB.CreateCall(callback_func_flush, {AddrLong, ConstantInt::get(IRB.getInt32Ty(), 0)}); 
//       return PreservedAnalyses::all();
//     }
//     return PreservedAnalyses::none();
//   }
// };

// /* New PM Registration */
// llvm::PassPluginLibraryInfo getInstruPluginInfo() {
//   return {LLVM_PLUGIN_API_VERSION, "InstruPass", LLVM_VERSION_STRING,
//           [](PassBuilder &PB) {
//             PB.registerVectorizerStartEPCallback(
//                 [](llvm::FunctionPassManager &PM,
//                    llvm::PassBuilder::OptimizationLevel Level) {
//                   PM.addPass(StoreFenceInstru());
//                 });

//             PB.registerScalarOptimizerLateEPCallback(
//                 [](llvm::FunctionPassManager &PM,
//                    llvm::PassBuilder::OptimizationLevel Level) {
//                   PM.addPass(FlushInstru());
//                 });
//           }};
// }

// // #ifndef LLVM_INSTRU_LINK_INTO_TOOLS
// extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
// llvmGetPassPluginInfo() {
//   return getInstruPluginInfo();
// }
// // #endif
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
