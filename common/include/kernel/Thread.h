/**
 * @file Thread.h
 */

#ifndef _THREAD_H_
#define _THREAD_H_

#include "types.h"
#include "fs/FileSystemInfo.h"

#define STACK_CANARY (0xDEADDEAD)

enum ThreadState {Running, Sleeping, ToBeDestroyed, Worker};

class Thread;
class ArchThreadInfo;
class Loader;
class Terminal;
class Mutex;
class FsWorkingDirectory;

extern Thread* currentThread;

extern Thread* currentThread;

/**
 * @class Thread
 * thread base class
 */
class Thread
{
    friend class Scheduler;
  public:

    static const char* threadStatePrintable[4];

    /**
     * Constructor
     * @return Thread instance
     */
    Thread(const char* name);

    /**
     * Constructor with FsWorkingDirectory given
     * @param working_dir working directory informations for the new Thread
     * @param name Thread's name
     * @return Thread instance
     */
    Thread ( FileSystemInfo* working_dir, const char* name );

    /**
     * Destructor
     */
    virtual ~Thread();

    /**
     * Marks the thread to be deleted by the scheduler.
     * DO Not use new / delete in this Method, as it sometimes called from an Interrupt Handler with Interrupts disabled
     */
    void kill();

    /**
     * runs whatever the user wants it to run;
     */
    virtual void Run() =0;

    ArchThreadInfo *kernel_arch_thread_info_;
    ArchThreadInfo *user_arch_thread_info_;
    uint32 stack_[2048];

    uint32 switch_to_userspace_;

    /**
     * Returns the stack's start pointer.
     * @return the pointer
     */
    pointer getStackStartPointer();

    Loader *loader_;

    ThreadState state_;

    /**
     * Returns the thread's name.
     * @return the name
     */
    const char *getName()
    {
      if ( name_ )
        return name_;
      else
        return "<UNNAMED THREAD>";
    }

    size_t getTID()
    {
      return tid_;
    }

    /**
     * Returns thread's current terminal
     * @return
     */
    Terminal *getTerminal();

    /**
     * Sets the thread's terminal
     * @param my_term the new terminal
     */
    void setTerminal ( Terminal *my_term );

    /**
     * getting the informations about the working Directory of this
     * Thread
     * @return the thread's FsWorkingDirectory
     */
    FileSystemInfo* getWorkingDirInfo(void);

    /**
     * sets the working directory informations of the this Thread
     * @param working_dir the new working directory informations
     */
    void setWorkingDirInfo(FileSystemInfo* working_dir);

    /**
     * prints a backtrace (i.e. the call stack) to the
     * debug output.
     * @param use_stored_thread_info determines whether to use the stored or the current thread registers
     */
    void printBacktrace();
    void printBacktrace(bool use_stored_registers);

    void printUserBacktrace();

    /**
     * Adds a new job to the thread
     */
    void addJob();

    /**
     * Marks a job as completed
     */
    void jobDone();

    /**
     * Should be called by worker threads instead of Scheduler::yield()
     */
    void waitForNextJob();

    /**
     * Are there open jobs?
     * @return true if jobs_scheduled_ > jobs_done_
     */
    virtual bool hasWork();

    /**
     * Tells the scheduler if this thread is ready for scheduling
     * @return true if ready for scheduling
     */
    bool schedulable();

    /**
     * debugging information for mutex deadlocks
     */
    Mutex* sleeping_on_mutex_;
  private:

    /**
     * Copy Constructor (not implemented)
     * @param src the object to copy
     * @return the new object
     */
    Thread ( Thread const &src );

    /**
     * Operator = using Copy Constructor (not implemented)
     * @param src the object to copy
     * @return the new object
     */
    Thread &operator= ( Thread const &src );

    size_t num_jiffies_;
    size_t tid_;

    Terminal *my_terminal_;

  protected:
    FileSystemInfo* working_dir_;

    const char *name_;
    uint64 jobs_scheduled_;
    uint64 jobs_done_;

};

#endif
