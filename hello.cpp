#include <cstdlib>
#include <cstdio>
#include <cstring>

typedef void (*fn_t)(void);

void attacker_func(){
    printf("Hello from attacker!\n");
}
class Foo {
    public:
        virtual void v(){
            printf("Hello from base class Foo virtual function!\n");
        }
};

class Bar: public Foo{
public:
        virtual void v(){
            printf("Hello from child class Bar virtual function!\n");
        }
};

class FooBar: public Foo{
public:
        virtual void v(){
            printf("Hello from WRONG virtual function!\n");
        }
};

int main(){
    Bar bar1;
    // FooBar foobar;
    // Foo *fooptr;
    Bar *barptr = &bar1;
    long long attacker_vtable[1] = {(long long)attacker_func};

    // // barasptr = (long long*)&bar2;
    // // vtable = (long long*)barasptr[0];
    // // vfn = (long long*)vtable[0];
    // // printf("Bar2 ptr:%lx\n", barasptr);
    // // printf("%lx\n", vtable);
    // // printf("%lx\n", vfn);
    // // bar2.v();
    
    // long long *foobarasptr = (long long*)&foobar;
    // long long *badvtable = (long long*)foobarasptr[0];
    // long long *badvfn = (long long*)badvtable[0];
    // printf("%lx\n", badvtable);
    // printf("%lx\n", badvfn);
    // // fn_t fn = (fn_t)badvfn;
    // // fn();
    // // fn();
    // // fn();
    printf("call to normal function: \n");
    barptr->v();
    long long *barasptr = (long long*)barptr;
    barasptr[0] = (long long)attacker_vtable;
    // long long *vtable = (long long*)barasptr[0];
    // long long *vfn = (long long*)vtable[0];
    // printf("Bar1 ptr:%lx\n", barasptr);
    // printf("vtable: %lx\n", vtable);
    // printf("vfn: %lx\n", vfn);
    // // printf("Vtable address is %lx\n", barasptr[0]);
    printf("call to hijacked function: \n");
    barptr->v();

    // // barasptr = (long long*)&bar2;
    // vtable = (long long*)barasptr[0];
    // vfn = (long long*)vtable[0];
    // printf("Bar2 ptr:%lx\n", barasptr);
    // printf("vtable: %lx\n", vtable);
    // printf("vfn: %lx\n", vfn);
    return 0;
}