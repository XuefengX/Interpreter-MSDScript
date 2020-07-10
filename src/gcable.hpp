#ifndef GCable_hpp
#define GCable_hpp

#include <iostream>
#include <stdio.h>

class GCable {
public:
    GCable *forwarded;
    GCable();

    virtual int size() = 0;
    virtual void trace() = 0;

    static void update(GCable **addr_to_update);

    void *operator new(std::size_t size);
    void operator delete(void *p);

    static void check_collect();
    static void collect();

    static void update_variables();
    static void trace_objects();
};

#endif /* GCable_hpp */
