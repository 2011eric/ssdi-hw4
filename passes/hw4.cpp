#include "llvm/Analysis/LoopInfo.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/DebugInfoMetadata.h"
#include "llvm/IR/DebugLoc.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include <vector>

using namespace llvm;
using namespace std;

namespace {
struct VTableCheckerModulePass
    : public PassInfoMixin<VTableCheckerModulePass> {
private:
  bool runOnModule(Module &M);

public:
  PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM) {
    runOnModule(M);
    return PreservedAnalyses::all();
  }
};
} // namespace llvm
bool VTableCheckerModulePass::runOnModule(Module &M) {
	outs() << __FUNCTION__ << '\n';
	// do something
  for (auto &F : M) {
    LLVMContext &C = F.getContext();

    for (auto &BB : F) {
      // errs() << "I saw a basic block! " << BB.getName() << '\n';
      ::std::vector<unsigned> inst_history;
      GetElementPtrInst *gep;
      LoadInst *load1, *load2;
      load1 = load2 = nullptr;
      for (auto &I : BB) {
        inst_history.push_back(I.getOpcode());
        if (inst_history.size() > 4) 
          inst_history.erase(inst_history.begin());
        if (I.getOpcode() == Instruction::GetElementPtr) {
          gep = dyn_cast<GetElementPtrInst>(&I);
        }
        if (I.getOpcode() == Instruction::Load) {
          if (!load1){
            load1 = dyn_cast<LoadInst>(&I);
          } else {
            if (!load2){
              load2 = dyn_cast<LoadInst>(&I);
            }else {
              load1 = load2;
              load2 = dyn_cast<LoadInst>(&I);
            }
          }
        }
        if (inst_history[0] == Instruction::Load 
            && inst_history[1] == Instruction::GetElementPtr
            && inst_history[2] == Instruction::Load
            && inst_history[3] == Instruction::Call) {
          errs() << "VTable call detected!\n";
          IRBuilder<> builder(load1->getNextNode());
          Value* vtable_addr = gep->getPointerOperand();
          ::std::vector<Type*> paramTypes = {vtable_addr->getType()};
          Type *retType = Type::getVoidTy(C);
          FunctionType *traceType = FunctionType::get(retType, paramTypes, true);
          FunctionCallee traceFunc = F.getParent()->getOrInsertFunction("trace", traceType);
          ::std::vector<Value*> args = {vtable_addr};
          builder.CreateCall(traceFunc, args);
        }
      }
    }
  }
  return true;
}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
  return {.APIVersion = LLVM_PLUGIN_API_VERSION,
          .PluginName = "VTable checker pass",
          .PluginVersion = "v0.1",
          .RegisterPassBuilderCallbacks =
              [](PassBuilder &PB) {
                PB.registerPipelineStartEPCallback(
                    [](ModulePassManager &MPM, OptimizationLevel Level) {
                      MPM.addPass(VTableCheckerModulePass());
                    });
              }

  };
}
