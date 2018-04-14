#ifndef TFHE_TFHEGLOBALCONTEXT_H
#define TFHE_TFHEGLOBALCONTEXT_H


#include <set>
#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <condition_variable>
#include "tfhe_thread_context.h"
#include "context_object_creator.h"
#include "../allocator/allocator.h"

class TfheGlobalContext {
    /** @brief next available thread context id */
    static uint64_t nextThreadId;
    /** @brief map of all available thread contexts, indexed by thread context id */
    std::map<uint64_t, TfheThreadContext> threadContexts;
    /** @brief vector of thread context objects factories (same indexes as the actual objects in the thread contexts) */
    std::vector<std::unique_ptr<ContextObjectCreator>> contextObjectCreators;
    /** @brief map between a (human readable) string identifier for a thread object and its actual index */
    std::map<std::string, int64_t> contextObjectIndex;
    /** @brief global allocator */
    Allocator globalAllocator;

    // + something to synchronize the global context
    mutable std::mutex _lock;
    mutable std::condition_variable _cv;

public:
    /** @brief default constructor */
    TfheGlobalContext();

    /** @brief default destructor, releases all thread contexts */
    ~TfheGlobalContext();

    /**
     * @brief creates a new thread context, and populate each of its objects using the factories
     * @note This function is synchronized with the global context's lock
     * @return the created thread context
     */
    TfheThreadContext *newThreadContext();

    /**
     * @brief destroys a threadContext and all its objects
     * @note This function is synchronized with the global context's lock
     * @param threadContext the threadContext to destroy
     */
    void deleteThreadContext(TfheThreadContext *threadContext);

    /**
     * @brief Sets or replace all context objects in all threadContexts with the one from this factory
     * @note This function is synchronized with the global context's lock
     * @param objectCreator the factory to create all the thread objects
     * @return the index in the object vector
     */
    int64_t setContextObject(const std::string &objectId, std::unique_ptr<ContextObjectCreator> &&objectCreator);

    /**
     * @brief finds the index of the context object identified by its title
     * @note This function is synchronized with the global context's lock
     * @return the index in the object vector, or -1 if not found
     */
    int64_t findContextObjectIndex(const std::string &objectId) const;

    PREVENT_COPY_ALLOW_STACK(TfheGlobalContext);
};

extern TfheGlobalContext tfheGlobalContext;
extern Allocator *&tfheGlobalAllocator;

#endif //TFHE_TFHEGLOBALCONTEXT_H
