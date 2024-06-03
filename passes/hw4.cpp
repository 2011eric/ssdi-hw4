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
// bool detect_vfncall();
bool VTableCheckerModulePass::runOnModule(Module &M) {
	outs() << __FUNCTION__ << '\n';
	// do something
  for (auto &F : M) {
    //errs() << "I saw a function called " << F.getName() << "!\n";
    for (auto &BB : F) {
      errs() << "I saw a basic block! " << BB.getName() << '\n';
      ::std::vector<unsigned> inst_history;
      for (auto &I : BB) {
        // errs() << "I saw an instruction! " << I.getOpcodeName() << '\n';
        // errs() << I.getOpcode() << '\n';
        inst_history.push_back(I.getOpcode());
        if (inst_history.size() > 4) 
          inst_history.erase(inst_history.begin());
        if (inst_history[0] == Instruction::Load 
            && inst_history[1] == Instruction::GetElementPtr
            && inst_history[2] == Instruction::Load
            && inst_history[3] == Instruction::Call) {
          errs() << "VTable call detected!\n";
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
