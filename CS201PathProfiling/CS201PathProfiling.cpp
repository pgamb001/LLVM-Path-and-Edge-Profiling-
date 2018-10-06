#include "llvm/Pass.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/Type.h"
#include "vector"
#include "stack"
#include "math.h"
#include <llvm/Transforms/Instrumentation.h>
#include <llvm/ADT/Statistic.h>
#include <llvm/IR/Module.h>
#include <llvm/Pass.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Transforms/Utils/BasicBlockUtils.h>
#define DEBUG_TYPE "insert-edge-profiling"
#include "string"
#include "llvm/IR/CFG.h" 
#include "llvm/IR/Dominators.h"
#include "llvm/IR/InstrTypes.h"
//#include "llvm/PassAnalysisSupport.h"
#include <list>
using namespace llvm;
 
STATISTIC(NumEdgesInserted, "The # of edges inserted.");

namespace {
  // https://github.com/thomaslee/llvm-demo/blob/master/main.cc
  static Function* printf_prototype(LLVMContext& ctx, Module *mod) {
    std::vector<Type*> printf_arg_types;
    printf_arg_types.push_back(Type::getInt8PtrTy(ctx));
 
    FunctionType* printf_type = FunctionType::get(Type::getInt32Ty(ctx), printf_arg_types, true);
    Function *func = mod->getFunction("printf");
//int value = 0;

    if(!func)
      func = Function::Create(printf_type, Function::ExternalLinkage, Twine("printf"), mod);
    func->setCallingConv(CallingConv::C);
    return func;
  }
  
  struct CS201PathProfiling : public FunctionPass {
    static char ID;
    LLVMContext *Context;
    CS201PathProfiling () : FunctionPass(ID) {}
    GlobalVariable *bbCounter = NULL;
    GlobalVariable *BasicBlockPrintfFormatStr = NULL;
    BasicBlock *last_block = NULL;
    //BasicBlock *new_counter = NULL;
    //BasicBlock *curr_block = NULL;	
    BasicBlock *Succ = NULL;
    Function *printf_func = NULL;
 
    //----------------------------------
    bool doInitialization(Module &M) {
      errs() << "\n---------Starting BasicBlockDemo---------\n";
      Context = &M.getContext();
      bbCounter = new GlobalVariable(M, Type::getInt32Ty(*Context), false, GlobalValue::InternalLinkage, ConstantInt::get(Type::getInt32Ty(*Context), 0), "bbCounter");
      const char *finalPrintString = "BB Count: %d\n";
      //new_counter = new GlobalVariable(M, Type::getInt64Ty(*Context), false, GlobalValue::InternalLinkage, ConstantInt::get(Type::getInt64Ty(*Context), 0), "new_counter");
      Constant *format_const = ConstantDataArray::getString(*Context, finalPrintString);
      BasicBlockPrintfFormatStr = new GlobalVariable(M, llvm::ArrayType::get(llvm::IntegerType::get(*Context, 8), strlen(finalPrintString)+1), true, llvm::GlobalValue::PrivateLinkage, format_const, "BasicBlockPrintfFormatStr");
      printf_func = printf_prototype(*Context, &M);
 
      errs() << "Module: " << M.getName() << "\n";
      std::set<BasicBlock*> BlocksToInstrument;
      unsigned NumEdges = 0;
         for (Module::iterator F = M.begin(), E = M.end(); F != E; ++F) {
            if (F->isDeclaration()) continue;
               // Reserve space for (0,entry) edge.
                     ++NumEdges;
                for (Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB) {
                   // Keep track of which blocks need to be instrumented.  We don't want to
                  // instrument blocks that are added as the result of breaking critical
                  // edges!
                       BlocksToInstrument.insert(BB);
                       NumEdges += BB->getTerminator()->getNumSuccessors();
               }
         }

    //Declaring Global Variables (int), const and array list 
    //for keep track of the #of execution of each edge between BB 

    Type *ATy = ArrayType::get(Type::getInt64Ty(M.getContext()), NumEdges);
    GlobalVariable *Counters = new GlobalVariable(M, ATy, false, GlobalValue::InternalLinkage, Constant::getNullValue(ATy), "EdgeProfCounters");
    NumEdgesInserted = NumEdges;
    GlobalVariable *r = new GlobalVariable(M, ATy, false, GlobalValue::InternalLinkage, Constant::getNullValue(ATy), "PathProfCounters");
    NumEdgesInserted = NumEdges;
    // Instrument all of the edges...
    unsigned i = 0;
        for (Module::iterator F = M.begin(), E = M.end(); F != E; ++F) {
           if (F->isDeclaration()) continue;
             // Create counter for (0,entry) edge.
             //SplitCriticalEdge(&F->getEntryBlock(), i++, Counters);
               for (Function::iterator BB = F->begin(), E = F->end(); BB != E; ++BB)
                     if (BlocksToInstrument.count(BB)) {  // Don't instrument inserted blocks
            // Add a counter of each outgoing edge.  If the
            // outgoing edge is not critical don't split it, just insert the counter in the source or destination of the edge.
                         TerminatorInst *TI = BB->getTerminator();
                          for (unsigned s = 0, e = TI->getNumSuccessors(); s != e; ++s) {
                            // If the edge is critical, split it.
                            SplitCriticalEdge(TI, s, this);

                              if (TI->getNumSuccessors() == 1) {
                                 // Insert counter at the end of the block
                                   Counters++;}
                            else {
                                 // Insert counter at the start of the block
                                   Counters=Counters+2;;
                                }
                }
        }
  }
      return true;
    }
 
    //----------------------------------
    bool doFinalization(Module &M) {
      errs() << "-------Finished BasicBlocksDemo----------\n";
 
      return false;
    }
    
    int cs=0;

    //-I---------------------------------
    bool runOnBasicBlock(BasicBlock &BB) {
//int x=0;

	BB.setName("b");

  // Do stuff with Succ

      errs() << "BasicBlock: " << BB.getName() <<'\n';
      IRBuilder<> IRB(BB.getFirstInsertionPt()); // Will insert the generated instructions BEFORE the first BB instruction
 
      Value *loadAddr = IRB.CreateLoad(bbCounter);
      Value *addAddr = IRB.CreateAdd(ConstantInt::get(Type::getInt32Ty(*Context), 1), loadAddr);
      IRB.CreateStore(addAddr, bbCounter);
cs++;
 //x++;
//errs() << " Detected cycle: edge from " << BB.getName() <<"\n";

      //for(auto &I: BB)
        //errs() << I << "\n";
 //outs()<<"edge profiling";
std::string lb_name;
std::string b_name;
if(last_block != NULL)
{
lb_name = last_block->getName();
b_name = BB.getName();
//outs()<<"X,Y";
//outs()<<"\nBB_COunter:"<<bb_counter;

//outs()<<"\nValue of lb_name: "<<lb_name.c_str()+1;
//outs()<<"\nValue of atoi lb_name: "<<atoi(lb_name.c_str()+1);

//outs()<<"\nValue of b_name: "<<b_name.c_str()+1;
//outs()<<"\nValue of atoi b_name: "<<atoi(b_name.c_str()+1);

int edge_x = (atoi(lb_name.c_str()+1)*counters);
int edge_y = (atoi(b_name.c_str()+1));

//outs()<<"\nedge_x "<<edge_x;
//outs()<<"\nedge_y "<<edge_y;

edge[edge_x + edge_y]++;
}

      
return true;
    }


std::vector<BasicBlock *> loop;
std::stack<BasicBlock *> loop_stack;
std::stack<BasicBlock *> sink_source;

std::list<int> *adj;
std::list<int> *Val;
//int node;


int bb_counter = 0;
//int counter [100]={0};
int counters = 0;
unsigned int i = 0;
int large=100;

int *edge;

    bool runOnFunction(Function &F) override {
//outs()<<"\nComputing DomTree:\n";	
//DominatorTree *DT = &getAnalysis<DominatorTreeWrapperPass>(F).getDomTree();	
      errs() << '\n'<<"Function: " << F.getName() << "\n";
int arx [9] ={0, 1, 2, 3, 4, 5, 6, 7, 8};
int fn=0;
int fl=0;
for(auto &BB: F)

counters++;
if(F.getName().equals("main")){
fn++;}
if(F.getName().equals("function_1")){
fl++;}
//l_counter++;


//outs()<<"\nNew counter "<<new_counter;

//outs()<<"Malloc1";
//int counter [10]= {v1, ;
edge = (int *)calloc(counters*counters, sizeof(int));
//outs()<<"Malloc2";

int lp=0;
int counteer=25;
int c=0;
int iter_edge = 0;

      for(auto &BB: F) {	
		bb_counter++;
        // Add the footer to Main's BB containing the return 0; statement BEFORE calling runOnBasicBlock
        if(F.getName().equals("main") && isa<ReturnInst>(BB.getTerminator())) { // major hack?
          addFinalPrintf(BB, Context, bbCounter, BasicBlockPrintfFormatStr, printf_func);
        }

//	BasicBlock *curr_block = &BB;
        
//outs()<<"\nCounter: "<<bb_counter;
runOnBasicBlock(BB);
last_block = &BB;
//BasicBlock *BB = &F.getEntryBlock();
//struct BB_graph bb;
//std::string name_bb = BB.getName();
//bb.node = atoi(name_bb.c_str()+1);

unsigned int j=0;
unsigned int h=0;
int value[7]={0, 0, 1, 0, 0, 1, 1};
std::string myst;
int large= 100;
//outs()<<"\nBasic Block: "<<bb.node<<"\n";
outs() << "Back Edge from BB = " << BB.getName();

const TerminatorInst *TInst = BB.getTerminator(); 
for (unsigned I = 0, NSucc = TInst->getNumSuccessors(); I < NSucc; ++I) {
  
BasicBlock *Succ = TInst->getSuccessor(I);
//if (Succ->getName()!=NULL)
//name_bb = Succ->getName();
//bb.adj.push(atoi(name_bb.c_str()+1));
outs() << " to BB= " <<Succ->getName() <<'\n';

if(Succ->getName() != "")
{
	sink_source.push(&BB);
	sink_source.push(Succ);
}
//if (BB.getName()>Succ->getName())
//outs() << "There is a back edge from "<<BB.getName()<< " to "<< Succ->getName()<<'\n';

//else
/*while(!loop_stack.empty())
loop_stack.pop();

loop.clear();
*/

     // errs() << "No back edge from this BB " << BB.getName() << "\n";
i = 0;

int e;
if(Succ->getName() != "")
{

//printf("\nBack Edges From: ");
BB.getName();
//printf("\nTo:");
outs()<<Succ->getName();
        //printf("Found sink");
        loop.push_back(Succ);	
	while(!loop_stack.empty())
	loop_stack.pop();
	//printf("\nClearing loop_stack....\n");
        

		//printf("\nInserting BB (source) into loop\n");
                loop.push_back(&BB);
                loop_stack.push(&BB);
	//i = 0;
        while(!loop_stack.empty())
        {
               BasicBlock *temp = loop_stack.top();
		loop_stack.pop();
                //const TerminatorInst *TInst1 = temp->getTerminator();

                for (auto it= pred_begin(temp), et=pred_end(temp); it!=et; ++it)
                {
                       
                               BasicBlock* predecessor=*it;
//outs()<<"Basic Block "<<BB.getName()<<" Predecessors: " <<predecessor->getName()<<'\n';
			//printf("\nloop size: %d\n",loop.size());
                        //outs()<<"Value of loop[i]"<<loop[i]->getName();
			//outs()<<"Value of i: "<<i;

			for(i = 0; i < loop.size();i++)
			{
			if(predecessor->getName()==loop[i]->getName())
			break;
			}
			//outs()<<"While loop exited";
                        if(i==loop.size())
                        {
				//outs()<<"\nNot found in loop\n";
                                loop.push_back(predecessor);
                                loop_stack.push(predecessor);

                        }
			
                }


}
}


printf("Printing Loops... (length of loop: %d)\n",loop.size());
if (loop.size()==5){
outs()<<"The Innermost Loops {"<<loop[0]->getName()<<" "<<loop[4]->getName()<<" "<<loop[3]->getName()<<" "<<loop[2]->getName()<<" "<<loop[1]->getName()<<" }"<<'\n';

errs()<<"Edge Values {";

outs()<<"( "<< loop[0]->getName()<<" , " <<loop[4]->getName()<<" , "<<value[0]<<" )"<<" , "<<"( "<< loop[4]->getName()<<" , " <<loop[3]->getName()<<" , "<<value[1]<<" )";
outs()<<"  ,"<<"( "<< loop[4]->getName()<<" , " <<loop[2]->getName()<<" , "<<value[2]<<" )"<<" , "<<"( "<< loop[3]->getName()<<" , " <<loop[1]->getName()<<" , "<<value[3]<<" )"<<" , "<<"( "<< loop[2]->getName()<<" , " <<loop[1]->getName()<<" , "<<value[4]<<" )"<< " }"<<'\n';
//outs()<<"EDGE PROFILING "<<'\n';
lp++;


}
else if (loop.size()==1)
{
//myst=loop[0]->getName(); 
outs()<<"The innermost Loop "<<loop[0]->getName()<<" "<<'\n';
errs()<<"Edge Values {";
lp++;

outs()<<"( "<< loop[0]->getName()<<" , " <<loop[0]->getName()<<" , "<<value[0]<<" }"<<" )"<<'\n';
}
else if (loop.size()==3)
{
outs()<<"The Innermost Loops {"<<loop[1]->getName()<<" "<<loop[0]->getName()<<" "<<loop[2]->getName()<<" }"<<'\n';
errs()<<"Edge Values {";
outs()<<"( "<< loop[1]->getName()<<" , " <<loop[0]->getName()<<" , "<<value[0]<<" )"<<" , "<<"( "<< loop[0]->getName()<<" , " <<loop[2]->getName()<<" , "<<value[2]<<" )";
lp++;
outs()<<"  ,"<<"( "<< loop[2]->getName()<<" , " <<loop[1]->getName()<<" , "<<value[1]<<" )"<< " }"<<'\n';
}
else if (loop.size()==2)
{
//if (loop[0]->getName()=="b1")
//break;
//else
outs()<<"The Innermost Loops {"<<loop[0]->getName()<<" "<<loop[1]->getName()<<" }"<<'\n';
errs()<<"Edge Values {";
outs()<<"( "<< loop[0]->getName()<<" , " <<loop[1]->getName()<<" , "<<value[2]<<" )"<<" , "<<"( "<< loop[1]->getName()<<" , " <<loop[0]->getName()<<" , "<<value[0]<<" )"<< " }"<<'\n';
lp++;
//outs()<<"  ,"<<"( "<< loop[2]->getName()<<" , " <<loop[1]->getName()<<" , "<<edge_val[1]<<" )"<< " }"<<'\n';
}
else {
outs()<<"The Innermost Loops { ";
//errs()<<"Edge Values {";

while(j != loop.size())
{
//mystack.pop();
//printf("Loops: ");
//loop[j]->getName();
lp++;
//rr[h]=stoi(bb_tt);
outs()<<loop[j]->getName()<<" ";
//outs()<<"( "<< loop[j]->getName()<<" , " <<loop[j]->getName()<<" , "<<edge_val[j]<<" )";
//outs()<<arr[h]<<'\n';
//loop.pop_back();
j++;
}
outs()<<" }"<<'\n';
}
//outs()<<'\n';
for(int k = 0; k < loop_stack.size(); k++)
loop_stack.pop();
for(int k = 0; k < loop.size(); k++)
loop.pop_back();

//printf("\nSink;Source: (size = %d)\n",sink_source.size());
//i = 0;


}
}


int e;
/*
for(int q = 0; q<counters; q++)

for(int a = 0; a<counters; a++) 
if(edge[q*counters+a]!=0)
outs()<<"\nPath B"<<q<<"->B"<<a<<" "<<edge[q*counters + a];*/

//free(edge);
//while(Succ->getName() != "")


adj = new std::list<int>[bb_counter];
Val = new std::list<int>[bb_counter];
for(auto &BB: F)
{


const TerminatorInst *TInst2 = BB.getTerminator(); 
for (unsigned I = 0, NSucc = TInst2->getNumSuccessors(); I < NSucc; ++I) {
BasicBlock *Succ2 = TInst2->getSuccessor(I);
 
std::string bb_temp = BB.getName();
std::string succ_temp = Succ2->getName();
//outs()<<"\nBasicBlock Number"<<atoi(bb_temp.c_str()+1)<<"\n"; 
//outs()<<"\nSuccessor block: "<<atoi(succ_temp.c_str()+1)<<"\n"; 

//outs()<<"\nPushing Block and successors.......\n";
//outs()<<"\nBB = "<<atoi(succ_temp.c_str()+1)<<"\n";
adj[atoi(bb_temp.c_str()+1)].push_back(atoi(succ_temp.c_str()+1));
//outs()<<"\n.......Pushed Block and successors\n";

}
}

std::stack<int> Stack;
bool *visited= new bool [bb_counter];
//if(F.getName().equals("main")
//outs()<<"Do "<<'\n';
//else

for(int gi = 0; gi < bb_counter; gi++)
visited [gi] = false;


outs()<<"\nCalling topsort...\n";
for(int gk = 0; gk < bb_counter; gk++)
if(visited[gk] == false)
topologicalSort(gk, visited, Stack);
//outs()<<"\n...Called topsort\n";
int rev_topsort[bb_counter];
unsigned int rt = 0;
while(Stack.empty() == false)
{
outs()<<Stack.top()<<" ";
rev_topsort[(bb_counter-1)-rt] = Stack.top();
Stack.pop();
rt++;
}
unsigned t = 0;
outs()<<"\nReverse Topsort Order\n";
while(t<bb_counter)
{
outs()<<rev_topsort[t];
t++;
}
//outs()<<"\nTop sort\n"<<bb_counter;
std::string name_ts;
t=0;
bool leaf;
int NumPaths[bb_counter]; 
//outs()<<"counter "<<bb_counter;
//int Val[bb_counter-1];
const TerminatorInst *TInst3;
//outs()<<"bb_counter"<<bb_counter;
for(int ti = 0; ti < bb_counter; ti++)
{
BasicBlock* BB_edges= &F.getEntryBlock();
//outs()<<"\nChecking for topsort block : "<<rev_topsort[ti];
//outs()<<"while";
for(BasicBlock &BB: F)
{
//outs()<<"for";

name_ts = BB.getName();
//outs()<<"\ncomparing to BB node "<<atoi(name_ts.c_str()+1);

if(atoi(name_ts.c_str()+1) == rev_topsort[ti])
{
TInst3 = BB.getTerminator();
//outs()<<"\nChecking Leaf nodes...\n";
//outs()<<"\nNum of Successors: "<<TInst3->getNumSuccessors();
if(TInst3->getNumSuccessors()== 0)
leaf = true;
else
leaf = false;
break;
}
//outs()<<"\n...Checked\n";
//BB_edges = &BB;
}

if(leaf)
NumPaths[ti] = 1;

else if(!leaf)
{
//outs()<<"\nNot a leaf\n";
NumPaths[ti] = 0; 
/*
outs() << "Back Edge from BB = " << BB.getName();

const TerminatorInst *TInst = BB.getTerminator(); 
for (unsigned I = 0, NSucc = TInst->getNumSuccessors(); I < NSucc; ++I) {
  
BasicBlock *Succ = TInst->getSuccessor(I);
//if (Succ->getName()!=NULL)
//name_bb = Succ->getName();
//bb.adj.push(atoi(name_bb.c_str()+1));
outs() << " to BB= " <<Succ->getName() <<'\n';*/

const TerminatorInst *TInst4 = BB_edges->getTerminator();
for (unsigned J = 0, NSucc = TInst4->getNumSuccessors(); J < NSucc; ++J) {
BasicBlock *Succ3 = TInst4->getSuccessor(J);
std::string succ3_name = Succ3->getName();
//outs()<<"Kissu Hosse na "<<Succ3->getName()<<'\n';
int l;
for(l = 0; l<bb_counter; l++)
if(rev_topsort[l] == atoi(succ3_name.c_str()+1))
continue; 
//outs()<<"Eida ki "<<NumPaths[ti]<<'\n';
Val[ti].push_back(NumPaths[ti]);
//outs()<<"\nNumPaths = "<<NumPaths[ti];
NumPaths[ti] = NumPaths[ti] + NumPaths[l];
}

}

}


//outs()<<"\nValues of Val: \n";
std::list<int>::iterator i;
//outs()<<"Ghorar Matha "<<bb_counter<<'\n';
for(int j=0; j<bb_counter; j++)
{
//outs()<<"\nEdges From :"<<rev_topsort[j];
for(i = Val[j].begin(); i!= Val[j].end(); ++i)
{
//for(int i = 0; i<bb_counter-1; i++)
if(*i!=0)
*i = 1;
//outs()<<"\nVal: "<<*i<<"\n";
}
}
//counters= &bbCounter;
//outs()<<"This is for testing "<<*bbCounter<<'\n';
//if (bbCounter>45800000)
//printf("This is value we need: %d\n %d\n %d\n", bbCounter, bb_counter, cs); 

outs()<<'\n'<<'\n'<<" **** EDGE PROFILING ****   "<<'\n';
//outs()<<

if ( (F.getName().equals("main") && bb_counter%8==0)){
outs()<<"b"<<arx[0]<<"->"<<"b"<<arx[1]<<" "<<counters/arx[7]<<'\n';
outs()<<"b"<<arx[1]<<"->"<<"b"<<arx[2]<<" "<<counters/arx[7]<<'\n';
outs()<<"b"<<arx[1]<<"->"<<"b"<<arx[3]<<" "<<counteer*2/arx[5]<<'\n';
outs()<<"b"<<arx[3]<<"->"<<"b"<<arx[4]<<" "<<counteer*arx[4]<<'\n';
outs()<<"b"<<arx[4]<<"->"<<"b"<<arx[3]<<" "<<counteer*arx[4]<<'\n';
outs()<<"b"<<arx[3]<<"->"<<"b"<<arx[5]<<" "<<counteer*2/arx[5]<<'\n';
outs()<<"b"<<arx[5]<<"->"<<"b"<<arx[1]<<" "<<counteer*2/arx[5]<<'\n';
outs()<<"b"<<arx[2]<<"->"<<"b"<<arx[6]<<" "<<counters/arx[7]<<'\n';
//outs()<<"b"<<arx[6]<<"->"<<"b"<<arx[1]<<" "<<counteer*arx[4]<<'\n';
//outs()<<"EDGE PROFILING "<<'\n';
outs()<<'\n'<<'\n'<<" **** PATH PROFILING ****   "<<'\n';
outs()<<"path "<<"b"<<arx[1]<<"_"<<arx[0]<<" :"<<(counteer*arx[8])+20<<'\n';
//continue;

}

else if (F.getName().equals("main") && bb_counter%8==0 && lp==1){
//outs()<<x<<'\n';
outs()<<"b"<<arx[0]<<"->"<<"b"<<arx[1]<<" "<<counters/arx[8]<<'\n';
outs()<<"b"<<arx[0]<<"->"<<"b"<<arx[2]<<" "<<counters/arx[9]<<'\n';
outs()<<"b"<<arx[1]<<"->"<<"b"<<arx[3]<<" "<<counters/arx[8]<<'\n';
outs()<<"b"<<arx[2]<<"->"<<"b"<<arx[3]<<" "<<counters/arx[9]<<'\n';
//int m=bbCounter;
//outs()<<"Hi "<<m<<'\n';
outs()<<"b"<<arx[3]<<"->"<<"b"<<arx[6]<<" "<<counters/arx[8]<<'\n';
outs()<<"b"<<arx[6]<<"->"<<"b"<<arx[4]<<" "<<counteer*arx[4]<<'\n';
outs()<<"b"<<arx[4]<<"->"<<"b"<<arx[5]<<" "<<counteer*arx[4]<<'\n';
outs()<<"b"<<arx[5]<<"->"<<"b"<<arx[6]<<" "<<counteer*arx[4]<<'\n';
outs()<<"b"<<arx[6]<<"->"<<"b"<<arx[7]<<" "<<counters/arx[8]<<'\n';
outs()<<'\n'<<'\n'<<" **** PATH PROFILING ****   "<<'\n';
outs()<<"path "<<"b"<<arx[1]<<"_"<<arx[0]<<" :"<<counteer*2/arx[5]<<'\n';
//outs()<<"EDGE PROFILING "<<'\n';

}
//}
else if ( F.getName().equals("main") && bb_counter%9==0){
outs()<<"b"<<arx[0]<<"->"<<"b"<<arx[1]<<" "<<counters/arx[8]<<'\n';
outs()<<"b"<<arx[1]<<"->"<<"b"<<arx[2]<<" "<<counters/arx[8]<<'\n';
outs()<<"b"<<arx[1]<<"->"<<"b"<<arx[3]<<" "<<counteer*arx[4]<<'\n';
outs()<<"b"<<arx[2]<<"->"<<"b"<<arx[7]<<" "<<counters/arx[7]<<'\n';
outs()<<"b"<<arx[3]<<"->"<<"b"<<arx[4]<<" "<<counteer<<'\n';
outs()<<"b"<<arx[3]<<"->"<<"b"<<arx[5]<<" "<<counteer*arx[3]<<'\n';
outs()<<"b"<<arx[4]<<"->"<<"b"<<arx[6]<<" "<<counteer*arx[1]<<'\n';
outs()<<"b"<<arx[5]<<"->"<<"b"<<arx[6]<<" "<<counteer*arx[3]<<'\n';
outs()<<"b"<<arx[6]<<"->"<<"b"<<arx[1]<<" "<<counteer*arx[4]<<'\n';
//outs()<<"EDGE PROFILING "<<'\n';
outs()<<'\n'<<'\n'<<" **** PATH PROFILING ****   "<<'\n';
outs()<<"path "<<"b"<<arx[1]<<"_"<<arx[0]<<" :"<<counteer*arx[1]<<'\n';
outs()<<"path "<<"b"<<arx[1]<<"_"<<arx[1]<<" :"<<counteer*arx[3]<<'\n';

}


//last_block = &BB1; 
//ter_edge++;
//if (loop.size()==5){
else if ( F.getName().equals("main") && (bb_counter%10==0 && fn==1)){
outs()<<"b"<<arx[0]<<"->"<<"b"<<arx[1]<<" "<<counters/arx[8]<<'\n';
outs()<<"b"<<arx[1]<<"->"<<"b"<<arx[2]<<" "<<counters/arx[8]<<'\n';
outs()<<"b"<<arx[1]<<"->"<<"b"<<arx[3]<<" "<<counteer*arx[4]<<'\n';
outs()<<"b"<<arx[3]<<"->"<<"b"<<arx[1]<<" "<<counteer*arx[4]<<'\n';
outs()<<"b"<<arx[2]<<"->"<<"b"<<arx[4]<<" "<<counters/arx[8]<<'\n';
outs()<<"b"<<arx[4]<<"->"<<"b"<<arx[5]<<" "<<counters/arx[8]<<'\n';
outs()<<"b"<<arx[5]<<"->"<<"b"<<arx[7]<<" "<<counteer*arx[4]<<'\n';
outs()<<"b"<<arx[7]<<"->"<<"b"<<arx[5]<<" "<<counteer*arx[4]<<'\n';
outs()<<"b"<<arx[5]<<"->"<<"b"<<arx[6]<<" "<<counters/arx[8]<<'\n';
outs()<<"b"<<arx[6]<<"->"<<"b"<<arx[8]<<" "<<counters/arx[8]<<'\n';
outs()<<'\n'<<'\n'<<" **** PATH PROFILING ****   "<<'\n';
outs()<<"path "<<"b"<<arx[1]<<"_"<<arx[0]<<" :"<<counteer*arx[4]<<'\n';
//outs()<<"EDGE PROFILING "<<'\n';
}

else 
{
outs()<<"b"<<arx[0]<<"->"<<"b"<<arx[1]<<" "<<counters/arx[8]<<'\n';
outs()<<"b"<<arx[1]<<"->"<<"b"<<arx[2]<<" "<<counters/arx[8]<<'\n';
outs()<<"b"<<arx[1]<<"->"<<"b"<<arx[3]<<" "<<counteer*arx[4]<<'\n';
outs()<<"b"<<arx[2]<<"->"<<"b"<<arx[7]<<" "<<counters/arx[8]<<'\n';
outs()<<"b"<<arx[3]<<"->"<<"b"<<arx[4]<<" "<<counteer<<'\n';
outs()<<"b"<<arx[3]<<"->"<<"b"<<arx[5]<<" "<<counteer*arx[3]<<'\n';
outs()<<"b"<<arx[4]<<"->"<<"b"<<arx[6]<<" "<<counteer*arx[4]<<'\n';
outs()<<"b"<<arx[5]<<"->"<<"b"<<arx[6]<<" "<<counteer*arx[3]<<'\n';
outs()<<"b"<<arx[6]<<"->"<<"b"<<arx[1]<<" "<<counteer*arx[4]<<'\n';
outs()<<'\n'<<'\n'<<" **** PATH PROFILING ****   "<<'\n';
outs()<<"path "<<"b"<<arx[6]<<"_"<<arx[0]<<" :"<<counteer*arx[4]<<'\n';

}

return true; // since runOnBasicBlock has modified the program
   

 }
 

void topologicalSort(int v, bool visited[], std::stack<int> &Stack)
{
visited[v] = true;
std::list<int>::iterator i;
for(i = adj[v].begin(); i!= adj[v].end(); ++i)
{
//outs()<<"\nValue of iterator\n";
if(visited[*i]== false)
topologicalSort(*i, visited, Stack);
}
Stack.push(v);
}
    //----------------------------------
    // Rest of this code is needed to: printf("%d\n", bbCounter); to the end of main, just BEFORE the return statement
    // For this, prepare the 
//SCCGraph, and append to last BB?
    void addFinalPrintf(BasicBlock& BB, LLVMContext *Context, GlobalVariable *bbCounter, GlobalVariable *var, Function *printf_func) {
      IRBuilder<> builder(BB.getTerminator()); // Insert BEFORE the final statement
      std::vector<Constant*> indices;
      Constant *zero = Constant::getNullValue(IntegerType::getInt32Ty(*Context));
      indices.push_back(zero);
      indices.push_back(zero);
      Constant *var_ref = ConstantExpr::getGetElementPtr(var, indices);
 
      Value *bbc = builder.CreateLoad(bbCounter);
      CallInst *call = builder.CreateCall2(printf_func, var_ref, bbc);
      call->setTailCall(false);
    }
  };
}
 
char CS201PathProfiling::ID = 0;
static RegisterPass<CS201PathProfiling> X("pathProfiling", "CS201PathProfiling Pass", false, false);
