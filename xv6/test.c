// Create a new kernel thread which shares the calling proces's address space
// The new process uses stack as its user stack
// The stack should be one page in size and page-aligned
// void (*fn)(void*) is a pointer to the function that the created thread
// will run once it starts
// void *arg is the argument that will be passed to *fcn once the child runs
// void *stack is a memory address inthe parent's address space which tells the child 
// where to put its stack
int
clone(void (*fcn)(void *), void *arg, void *stack)
{
  int i, pid;
  struct proc *np;
  struct proc *curproc = myproc();

  // Make sure the stack page it's page-aligned
  if((uint)stack % PGSIZE != 0)
    return -1;

  // Make sure the stack should be one page in size
  if(curproc->sz - (uint stack) < PGSIZE
    return -1;

  // Allocate process.
  if((np = allocproc()) == 0){
    return -1;
  }
  // Is recording the size of process memory in bytes
  np->sz = curproc->sz;

  // if the process calling clone is itself a thread
  // copy its parent to the new thread
  // recording the parent process
  if(curproc->isthread == 0) {
    np->parent = curproc;
  } else {
    np->parent = proc->parent;
  }

  //np->parent = curproc;
  *np->tf = *curproc->tf;

  // Clear %eax so that fork returns 0 in the child.
  np->tf->eax = 0;

  // the child process's stack is in the address space of the original process

  // reallocate old process's page table to new process
  // that is, the clone process np and the current process curproc share
  // the same page directory 
  np->pgdir = curproc->pgdir;
  // The cloned process runs fcn as soon as it exits the trap frame
  // eip register of the trap frame should be set to the memory address of fcn
  np->tf->eip = (int)fcn;
  // set isthread to 1
  np->isthread = 1;

  // set up child thread new stack and set up registers
  np->stack = (uint)stack;
  // move esp pointer to the top of the new stack
  np->tf->esp = (uint)stack + 4096;
  // push argument
  *(uint*)(np->tf->esp) = (uint)arg;
  // push the return address
  *(uint*)(np->tf->esp -4) = 0xffffffff;
  // a word is 4 bytes long
  np->tf->esp -= 4;
  
  for(i = 0; i < NOFILE; i++)
    if(curproc->ofile[i])
      np->ofile[i] = filedup(curproc->ofile[i]);
  np->cwd = idup(curproc->cwd);

  safestrcpy(np->name, curproc->name, sizeof(curproc->name));

  pid = np->pid;

  acquire(&ptable.lock);

  np->state = RUNNABLE;

  release(&ptable.lock);

  return pid;
}