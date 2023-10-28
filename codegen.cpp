#include "codegen.hpp"
#include "vcc.h"
#include "parser.h"
#include "llvm/ADT/APSInt.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Value.h"
#include <memory>
#include <string>

using namespace llvm;

static std::unique_ptr<LLVMContext> context;
static std::unique_ptr<IRBuilder<>> builder;

Value *_codegen(Node *nd);

void init() {
  context = std::make_unique<LLVMContext>();
  builder = std::make_unique<IRBuilder<>>(*context);
  BasicBlock *BB = BasicBlock::Create(*context, "entry");
  builder->SetInsertPoint(BB);
}

Value *_integer_codegen(Node *nd) {
  return ConstantFP::get(*context, APFloat((double)nd->content.val));
};

Value *_binary_codegen(Node *nd) {
  Value *lhs_val = _codegen(nd->left);
  Value *rhs_val = _codegen(nd->right);
  switch (nd->kind) {
    case ND_ADD:
      return builder->CreateFAdd(lhs_val, rhs_val);
      break;
    case ND_SUB:
      return builder->CreateFSub(lhs_val, rhs_val);
      break;
    case ND_MUL:
      return builder->CreateFMul(lhs_val, rhs_val);
      break;
    case ND_DIV:
      return builder->CreateFDiv(lhs_val, rhs_val);
      break;
    case ND_LE:
      return builder->CreateFCmpOLE(lhs_val, rhs_val);
      break;
    case ND_LT:
      return builder->CreateFCmpOLT(lhs_val, rhs_val);
      break;
    case ND_GE:
      return builder->CreateFCmpOGE(lhs_val, rhs_val);
      break;
    case ND_GT:
      return builder->CreateFCmpOGT(lhs_val, rhs_val);
      break;
    case ND_EQ:
      return builder->CreateFCmpOEQ(lhs_val, rhs_val);
      break;
    case ND_NE:
      return builder->CreateFCmpONE(lhs_val, rhs_val);
      break;
    default:
      assert(false);
      break;
  }
  assert(false);
  return nullptr;
}

Value *_codegen(Node *nd) {
  switch (nd->kind) {
    case ND_ADD:
    case ND_SUB:
    case ND_MUL:
    case ND_DIV:
    case ND_LE:
    case ND_LT:
    case ND_GE:
    case ND_GT:
    case ND_EQ:
    case ND_NE:
      return _binary_codegen(nd);
      break;
    case ND_NUM:
      return _integer_codegen(nd);
      break;
    default:
      assert(false);
      break;
  }
  return nullptr;
}

void codegen() {
  init();
  Value * value = _codegen(root);
  value->print(errs());
  llvm::errs() << "\n";
}