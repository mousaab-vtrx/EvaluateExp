#ifndef DS_STATUS_H
#define DS_STATUS_H

/* Codes de retour communs à toutes les structures de données du chapitre. */
typedef enum {
    DS_OK = 0,   /* opération réussie                         */
    DS_EMPTY,    /* structure vide                            */
    DS_NOMEM,    /* échec d'allocation (malloc/realloc)       */
    DS_SYNTAX,   /* expression mal formée                     */
    DS_DIVZERO,  /* division par zéro                         */
    DS_BADARG    /* argument invalide                         */
} DsStatus;

/* Modifier le type des éléments à cet emplacement unique. */
typedef int Item;

#endif /* DS_STATUS_H */
