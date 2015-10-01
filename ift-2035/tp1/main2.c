#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { false, true } bool;

struct digit {
    int val;
    struct digit *next;
};

struct num {
    bool pos;
    int len;
    struct digit *first;
};

struct charbuff {
    int cap;
    int len;
    char* buff;
};

struct stack {
    int cap;
    int len;
    void** items;
};

/* convertit une valeur numérique [0-9] en son caractère */
char val_to_car(char);
/* convertit un caractère ['0'-'9'] en sa valeur numérique */
char car_to_val(char);
/* détermine si le caractère donné est un chiffre */
bool is_digit(char);
/* détermine si le caractère donné est une lettre */
bool is_letter(char);
/* détermine si le caractère donné est un espace */
bool is_whitespace(char);

/* créer un nombre à partir d'une string */
struct num* num_from_str(const char*);
/* créer un clone complet du nombre */
struct num* num_clone(const struct num*);
/* formatter le nombre en string */
char* num_to_str(const struct num*);
/* libérer la mémoire utilisée par un nombre */
void num_free(struct num*);

/* créer un  nouveau charbuff avec la capacitée donnée */
struct charbuff* charbuff_new(int cap);
/* ajouter un caractère et redimensionner au besoin */
bool charbuff_push(struct charbuff*, char);
/* disposer du texte contenu, mais sauvegarder l'espace mémoire  */
void charbuff_clear(struct charbuff*);
/* libérer la mémoire utilisé */
void charbuff_free(struct charbuff*);

/* créer un nouvea stack avec la capacité donnée */
struct stack* stack_new(int cap);
/* ajouter un élément au tas */
bool stack_push(struct stack*, void*);
/* obtenir l'élément au dessus du tas */
void* stack_pop(struct stack*);
/* libérer la mémoire utilisée par la tas */
void stack_free(struct stack*);

/* implémentation */

inline char val_to_car(char val) {
    return val + '0';
}

inline char car_to_val(char car) {
    return car - '0';
}

inline bool is_digit(char car) {
    return car >= '0' && car <= '9';
}

inline bool is_letter(char car) {
    return car >= 'a' && car <= 'z';
}

inline bool is_whitespace(char car) {
    return car == ' ' || car == '\t';
}

struct num* num_from_str(const char *text) {
    int len, i, lo;
    struct num *n;
    struct digit *p, *d;

    len = strlen(text);
    p = NULL;

    n = malloc(sizeof(struct num));
    if (n == NULL) return NULL;

    /* si le nombre débute par '-', il est négatif */
    n->pos = text[0] != '-';
    n->len = 0;
    /* si il y a un signe '-', on arrête de copier avant 1 caractère plus tôt */
    lo = !n->pos;

    for (i = len - 1; i >= lo; i--) {
        d = malloc(sizeof(struct digit));
        /* si on arrive pas à allouer, on détruit tout ce qui a été alloué auparavant */
        if (d == NULL) {
            num_free(n);
            return NULL;
        }

        d->val = car_to_val(text[i]);
        n->len++;
        if (p == NULL)
            n->first = p = d;
        else
            p->next = d;
        p = d;
    }

    return n;
}

struct num* num_clone(const struct num *n) {
    struct num *c;
    struct digit *d, *prev, *new;

    c = malloc(sizeof(struct num));
    if (c == NULL) return NULL;

    c->pos = n->pos;
    c->len = n->len;

    d = n->first;
    prev = NULL;
    while (d != NULL) {
        new = malloc(sizeof(struct digit));
        if (new == NULL) {
            num_free(c);
            return NULL;
        }

        new->val = d->val;
        /* si c'est la première itération, on attache new à c->first */
        if (prev == NULL)
            c->first = new;
        /* sinon on attache new, au précédent */
        else
            prev->next = new;

        prev = new;
        d = d->next;
    }

    return c;
}

char* num_to_str(const struct num *n) {
    int len, i;
    char *text;
    struct digit *d;

    len = n->len + 1;   // \0
    if (!n->pos) len++; // négatif

    text = malloc(sizeof(char) * len);
    if (text == NULL) return NULL;

    i = len - 2;
    d = n->first;
    while (d != NULL) {
        text[i--] = val_to_car(d->val);
        d = d->next;
    }

    text[len - 1] = '\0';
    if (!n->pos) {
        text[0] = '-';
    }

    return text;
}

void num_free(struct num *n) {
    struct digit *d, *t;

    d = n->first;
    while (d != NULL) {
        t = d->next;
        free(d);
        d = t;
    }

    free(n);
}

struct charbuff* charbuff_new(int cap) {
    struct charbuff *cb;

    cb = malloc(sizeof(struct charbuff));
    if (cb == NULL) return NULL;

    cb->cap = cap;
    cb->len = 0;

    cb->buff = malloc(sizeof(char) * cap + 1);
    if (cb->buff == NULL) {
        free(cb);
        return NULL;
    }

    return cb;
}

bool charbuff_push(struct charbuff *cb, char c) {
    int newcap;
    char* buff;

    if (cb->len >= cb->cap) {
        newcap = cb->cap * 2;
        buff = realloc(cb->buff, sizeof(char) * newcap + 1);
        if (buff == NULL) {
            charbuff_free(cb);
            return false;
        }
        cb->cap = newcap;
        cb->buff = buff;
    }

    cb->buff[cb->len++] = c;
    cb->buff[cb->len] = '\0';

    return true;
}

void charbuff_clear(struct charbuff *cb) {
    cb->buff[0] = '\0';
    cb->len = 0;
}

void charbuff_free(struct charbuff *cb) {
    free(cb->buff);
    free(cb);
}

struct stack* stack_new(int cap) {
    struct stack *s;

    s = malloc(sizeof(struct stack));
    if (s == NULL) return NULL;

    s->cap = cap;
    s->len = 0;

    s->items = malloc(sizeof(void*) * cap);
    if (s->items == NULL) {
        free(s);
        return NULL;
    }

    return s;
}

bool stack_push(struct stack *s, void *item) {
    int newcap;
    void** items;

    if (s->len >= s->cap) {
        newcap = s->cap * 2;
        items = realloc(s->items, newcap);
        if (items == NULL) return false;
        s->cap = newcap;
        s->items = items;
    }

    s->items[s->len++] = item;
    return true;
}

void* stack_pop(struct stack *s) {
    if (s->len > 0) {
        return s->items[--s->len];
    }
    return NULL;
}

void stack_free(struct stack *s) {
    free(s->items);
    free(s);
}

enum ast_kind {
    AST_KIND_ASSIGN,
    AST_KIND_USE,
    AST_KIND_NUMBER,
    AST_KIND_OPERATOR,
};

enum ast_oper_kind {
    AST_OPER_KIND_ADD,
    AST_OPER_KIND_SUB,
    AST_OPER_KIND_MUL,
};

struct ast_node {
    enum ast_kind kind;
    union {
        /* assignation de variable */
        struct {
            char var;
            struct ast_node* val;
        } assign;
        /* utilisation de variable */
        struct {
            char var;
        } use;
        /* nombre */
        struct {
            char* val;
        } num;
        /* opérateur */
        struct {
            enum ast_oper_kind kind;
            struct ast_node *op1;
            struct ast_node *op2;
        } oper;
    };
};

struct ast_node* ast_node_assign(char var, struct ast_node *val) {
    struct ast_node *node;

    node = malloc(sizeof(struct ast_node));
    node->kind = AST_KIND_ASSIGN;
    node->assign.var = var;
    node->assign.val = val;

    return node;
}

struct ast_node* ast_node_use(char var) {
    struct ast_node *node;

    node = malloc(sizeof(struct ast_node));
    node->kind = AST_KIND_USE;
    node->assign.var = var;

    return node;
}

struct ast_node* ast_node_num(const char* source, int len) {
    struct ast_node *node;
    char* val;

    val = malloc(sizeof(char) * len + 1);
    strcpy(val, source);

    node = malloc(sizeof(struct ast_node));
    node->kind = AST_KIND_NUMBER;
    node->num.val = val;

    return node;
}

struct ast_node* ast_node_oper(enum ast_oper_kind kind, struct ast_node* op1, struct ast_node* op2) {
    struct ast_node *node;

    node = malloc(sizeof(struct ast_node));
    node->kind = AST_KIND_OPERATOR;
    node->oper.kind = kind;
    node->oper.op1 = op1;
    node->oper.op2 = op2;

    return node;
}

struct ast_node* ast_parse(const char* text) {
    int i, len;
    char car, var;
    struct ast_node *node, *op1, *op2;
    struct stack *nodes;
    struct charbuff *cb;

    len = strlen(text);
    nodes = stack_new(2);
    cb = charbuff_new(16);

    for (i = 0; i < len; i++) {
        car = text[i];
        if (is_whitespace(car)) {
            printf("whitespace\n");
            continue;
        }
        if (is_digit(car)) {
            while (is_digit(text[i]))
                charbuff_push(cb, text[i++]);
            printf("pushing number: %s\n", cb->buff);
            node = ast_node_num(cb->buff, cb->len);
            stack_push(nodes, node);
        } else if (car == '+') {
            op1 = stack_pop(nodes);
            op2 = stack_pop(nodes);
            // TODO: error if op1 or op2 null
            printf("pushing add\n");
            node = ast_node_oper(AST_OPER_KIND_ADD, op1, op2);
            stack_push(nodes, node);
        } else if (car == '-') {
            op1 = stack_pop(nodes);
            op2 = stack_pop(nodes);
            // TODO: error if op1 or op2 null
            printf("pushing sub\n");
            node = ast_node_oper(AST_OPER_KIND_SUB, op1, op2);
            stack_push(nodes, node);
        } else if (car == '*') {
            op1 = stack_pop(nodes);
            op2 = stack_pop(nodes);
            // TODO: error if op1 or op2 null
            printf("pushing mul\n");
            node = ast_node_oper(AST_OPER_KIND_MUL, op1, op2);
            stack_push(nodes, node);
        } else if (car == '=') {
            var = text[++i];
            if (is_letter(var)) {
                node = stack_pop(nodes);
                node = ast_node_assign(var, node);
                stack_push(nodes, node);
                printf("pushing assign of %c\n", var);
            }
            // TODO: invalid variable name
        } else if (is_letter(car)) {
            node = ast_node_use(car);
            stack_push(nodes, node);
            printf("pushing variable %c\n", car);
        }

        charbuff_clear(cb);
    }

    charbuff_free(cb);
    return stack_pop(nodes);
}

struct inter {
    int vars[26];
};

int inter_eval(struct inter *vm, struct ast_node *node) {
    int val;

    switch (node->kind) {
        case AST_KIND_ASSIGN:
            printf("in assign\n");
            val = inter_eval(vm, node->assign.val);
            vm->vars[node->assign.var - 'a'] = val;
            return val;
        case AST_KIND_USE:
            printf("in use\n");
            return vm->vars[node->use.var - 'a'];
        case AST_KIND_NUMBER:
            printf("in number\n");
            int val = atoi(node->num.val);
            return val;
        case AST_KIND_OPERATOR:
            printf("in oper\n");
            switch (node->oper.kind) {
                // TODO: use big num
                case AST_OPER_KIND_ADD:
                    return inter_eval(vm, node->oper.op1) + inter_eval(vm, node->oper.op2);
                case AST_OPER_KIND_SUB:
                    return inter_eval(vm, node->oper.op2) - inter_eval(vm, node->oper.op1);
                case AST_OPER_KIND_MUL:
                    return inter_eval(vm, node->oper.op1) * inter_eval(vm, node->oper.op2);
            }
    }

    return 0;
}

void inter_print_vars(struct inter *vm) {
    int i;
    for (i = 0; i < 26; i++) {
        printf("%c = %d\n", i + 'a', vm->vars[i]);
    }
}

int main(int argc, char **argv) {
    int val, c;
    struct inter vm;
    struct ast_node* node;
    struct charbuff* cb;

    memset(&vm, 0, sizeof(struct inter));
    cb = charbuff_new(20);

    printf("> ");
    while ((c = getchar()) != EOF) {
        if (c == '\r') continue;
        if (c == '\n') {
            node = ast_parse(cb->buff);
            val = inter_eval(&vm, ast_parse(cb->buff));
            printf("%d\n", val);
            charbuff_clear(cb);
            printf("> ");
        } else {
            charbuff_push(cb, c);
        }
    }
    printf("\nAu revoir!\n");

    charbuff_free(cb);
    return 0;
}
