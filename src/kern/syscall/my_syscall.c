#include <types.h>
#include <kern/unistd.h>
#include <kern/errno.h> // errors
#include <lib.h>
#include <copyinout.h>
#include <syscall.h>
#include <current.h>
#include <addrspace.h>
#include <proc.h>
#include <my_syscall.h>

int sys_read(int fd, userptr_t buf, size_t size, int *val){

    size_t i; char c; int result;
    
    // controlli di errore
    // if (fd != 1 && fd != 2) { return EBADF; }
    // if (buf == NULL) { return EFAULT; }
    if (fd != STDIN_FILENO){
        kprintf("sys_read only to stdin\n");
        return EBADF;
    }

    // ciclo per la lettura
    for(i=0;i<size;i++){
        // leggi carattere
        c = getch();
        kprintf("Hai premuto: %d\n", (int)c); // DEBUG
        // funzione di sicurezza per scrivere nella memoria del programma utente
        result = copyout(&c, buf + i, 1);

        // controllo per vedere se la memoria utente è valida
        if (result){
            return result;
        }

        // quando premi invio, interrompi la lettura
        if (c == '\r' || c == '\n') {
            i++; 
            break;
        }
    }

    // valore di ritorno per il trapframe
    *val = (int)i;

    return 0;

}

int sys_write(int fd, userptr_t buf, size_t size, int *val){

    size_t i; char c; int result;
    
    // controlli di errore
    // if (fd != 1 && fd != 2) { return EBADF; }
    // if (buf == NULL) { return EFAULT; }
    if (fd != STDOUT_FILENO && fd != STDERR_FILENO){
        kprintf("sys_write only to stdout\n");
        return EBADF;
    }
    // ciclo per la lettura
    for(i=0;i<size;i++){
        // funzione copyin per entrare nel kernel space
        result = copyin(buf + i, &c, 1);

        // controllo per vedere se la memoria utente è valida
        if (result){
            return result;
        }

        // stampa carattere
        putch(c);
    }

    // valore di ritorno per il trapframe
    *val = (int)i;

    return 0;

}

int sys_exit(int exitcode){
    // il kernel deve liberare la memoria fisica e le tabelle delle pagine
    // associate al processo che sta chiudendo, quindi metto in delle struct
    // - prendo l'adress space del processo
    // - e il processo corrente
    struct addrspace *as;

    (void)exitcode;

    // recuperi adress space del processo
    as = proc_getas();

    if (as != NULL){
        // distruggi adress space associato al processo
        as_destroy(as);
        // pulisci il puntatore del processo 
        proc_setas(NULL);
    }
    // distruggi il thread per terminare l'esecuzione
    thread_exit();

    return 0;
}