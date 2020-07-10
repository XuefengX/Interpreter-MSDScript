#include "gcable.hpp"
#include "cont.hpp"
#include "env.hpp"
#include "pointer.hpp"
#include "step.hpp"

static std::size_t heap_size = (8 * 1024 * 1024);
static void *to_space = nullptr, *from_space = nullptr;
static std::size_t allocation_offset = 0;
static bool out_of_memory = false;
static const std::size_t safety_margin = 256;

void resize_heap();
void swap_ptrs(void **a, void **b);

GCable::GCable() {
    forwarded = nullptr;
}

void *GCable::operator new(std::size_t size) {
    if (to_space == nullptr) {
        to_space = malloc(heap_size);
        from_space = malloc(heap_size);
    }

    if (allocation_offset + size > heap_size) {
        //        std::cout << "hhh" << std::endl;
        out_of_memory = true;
        to_space = malloc(heap_size);
        allocation_offset = 0;
    }

    void *new_obj = (char *)to_space + allocation_offset;
    allocation_offset += size;
    return new_obj;
}

void GCable::operator delete(void *p) {
}

void GCable::check_collect() {
    if (out_of_memory == true) {
        throw std::runtime_error("out of memory");
    }

    if (allocation_offset + safety_margin >= heap_size) {
        GCable::collect();
    }

    if (allocation_offset + safety_margin >= heap_size) {
        resize_heap();
    }
}

void resize_heap() {
    heap_size *= 2;
    free(from_space);
    from_space = malloc(heap_size);

    GCable::collect();

    free(from_space);
    from_space = malloc(heap_size);
}

void swap_ptrs(void **a, void **b) {
    void *temp = *a;
    *a = *b;
    *b = temp;
}

void GCable::collect() {
    swap_ptrs(&to_space, &from_space);
    allocation_offset = 0;

    update_variables();

    trace_objects();
}

void GCable::update_variables() {
    UPDATE(Step::cont);
    if (Step::mode == Step::interp_mode) {
        UPDATE(Step::expr);
        UPDATE(Step::env);
    } else
        UPDATE(Step::val);

    UPDATE(Cont::done);
    UPDATE(Env::emptyenv);
}

void GCable::trace_objects() {
    std::size_t trace_offset = 0;

    while (trace_offset < allocation_offset) {
        GCable *obj = (GCable *)((char *)to_space + trace_offset);
        obj->trace();
        trace_offset += obj->size();
    }
}

void GCable::update(GCable **addr_to_update) {
    GCable *obj = *addr_to_update;

    if (obj->forwarded == nullptr) {
        std::size_t size = obj->size();
        void *new_obj = (char *)to_space + allocation_offset;
        allocation_offset += size;

        memcpy(new_obj, (void *)obj, size);
        obj->forwarded = (GCable *)new_obj;
    }

    *addr_to_update = obj->forwarded;
}
