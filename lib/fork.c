/* implement fork from user space */

#include <inc/string.h>
#include <inc/lib.h>

/* User-level fork with copy-on-write.
 * Create a child.
 * Lazily copy our address space and page fault handler setup to the child.
 * Then mark the child as runnable and return.
 *
 * Returns: child's envid to the parent, 0 to the child, < 0 on error.
 * It is also OK to panic on error.
 *
 * Hint:
 *   Use sys_map_region, it can perform address space copying in one call
 *   Remember to fix "thisenv" in the child process.
 */
envid_t
fork(void) {
    // LAB 9: Your code here
    envid_t envid;
    int res = 0;
    envid = sys_exofork();
    if (envid < 0) {
        panic("exofork failed\n");
    }
    if (envid == 0) { // if child
        thisenv = &envs[ENVX(sys_getenvid())];
        return 0;
    } else {
        res = sys_map_region(sys_getenvid(), 0, envid, 0, MAX_USER_ADDRESS, PROT_ALL | PROT_LAZY | PROT_COMBINE);
        if (res < 0) {
            panic("sys map region failed\n");
        }
        res = sys_env_set_pgfault_upcall(envid, thisenv->env_pgfault_upcall);
        if (res < 0) {
            panic("set page fault upcall failed\n");
        }
        //res = sys_alloc_region(envid, (void *)(USER_STACK_TOP - USER_STACK_SIZE), USER_STACK_SIZE, PROT_ALL); 
        //if (res < 0) {
        //    panic("alloc region failed\n");
        //}
        res = sys_env_set_status(envid, ENV_RUNNABLE);
        if (res < 0) {
            panic("env set status failed\n");
        }
    }
    return envid;
}

envid_t
sfork() {
    panic("sfork() is not implemented");
}
