#include "garbage.h"

/*start of func defs*/
// allocate a garbage object
// a garbage object is a list of objects to be freed
Garbage *new_garbage(){
    Garbage *gr = malloc(sizeof(Garbage));
    if (gr == NULL) return NULL;
    gr->stack = new_stack();
    return gr;
}
// free the list of objects in the garbage
void collect(const Garbage *gr){
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
// free the garbage object
void dispose(Garbage *gr){
    free_stack(gr->stack);
    free(gr);
}
void collect_and_dispose(Garbage *gr){
    collect(gr);
    dispose(gr);
}
// allocate a garbage placeholder (obj, type)
GarbageEntry *new_gr_entry(void* obj, GrType t){
    GarbageEntry* ge = malloc(sizeof(GarbageEntry));
    if (ge == NULL) return NULL;
    ge->object = obj;
    ge->type = t;
    return ge;
}
// add object *ptr to the garbage gr
void* w(const Garbage *gr, void* ptr, GrType type){
    if(gr != NULL)
        push(gr->stack, new_gr_entry(ptr, type));
    return ptr;
}
/*end of func defs*/