#include "garbage.h"

/*start of func defs*/
Garbage *new_garbage(){
    Garbage *gr = malloc(sizeof(Garbage));
    gr->stack = new_stack();
    return gr;
}
unsigned int collect(const Garbage *gr){
    if(gr != NULL){
        while(!is_stack_empty(gr->stack)){
            GarbageEntry *ge = (GarbageEntry*) pop(gr->stack);
            if(ge->object != NULL){
                switch(ge->type){
                    case GrNode:
                        free_node((Node*)ge->object);
                        break;
                    case GrCString:
                        free_cstring((CString*)ge->object);
                        break;
                    case GrStack:
                        free_stack((Stack*)ge->object);
                        break;
                    case GrTokensList:
                        free_tokens_list((TokensList*)ge->object);
                        break;
                    case GrSplitsList:
                        free_splits_list((SplitsList*)ge->object);
                        break;
                    case GrParsingReport:
                        free_report((ParserReport*)ge->object);
                        break;
                    default:
                        free(ge->object);
                }
                free(ge);
            }
        }
    }
}
void dispose(Garbage *gr){
    free_stack(gr->stack);
    free(gr);
}
void collect_and_dispose(Garbage *gr){
    collect(gr);
    dispose(gr);
}
GarbageEntry *new_gr_entry(void* obj, GrType t){
    GarbageEntry* ge = malloc(sizeof(GarbageEntry));
    ge->object = obj;
    ge->type = t;
    return ge;
}
void* w(const Garbage *gr, void* ptr, GrType type){
    if(gr != NULL)
        push(gr->stack, new_gr_entry(ptr, type));
    return ptr;
}
/*end of func defs*/