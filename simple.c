#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SIZE 25

typedef struct
{
    char items[SIZE];
    int top;
} char_stack;

typedef struct
{
    long items[SIZE];
    int top;
} int_stack;

void init_char_stack(char_stack *s)
{
    s->top = -1;
}

void init_int_stack(int_stack *s)
{
    s->top = -1;
}

int is_char_empty(char_stack *s)
{
    return s->top == -1;
}

int is_int_empty(int_stack *s)
{
    return s->top == -1;
}

int is_char_full(char_stack *s)
{
    return s->top == SIZE - 1;
}

int is_int_full(int_stack *s)
{
    return s->top == SIZE - 1;
}

char char_top_of_stack(char_stack *s)
{
    return is_char_empty(s) ? '\0' : s->items[s->top];
}

long int_top_of_stack(int_stack *s)
{
    return is_int_empty(s) ? '\0' : s->items[s->top];
}

void push_char(char_stack *s, char c)
{
    if (is_char_full(s))
    {
        printf("ERROR 1 : Stack Overflow - stack is full!\n");
        exit(1);
    }

    s->items[++(s->top)] = c;
}

void push_int(int_stack *s, int i)
{
    if (is_int_full(s))
    {
        printf("ERROR 1 : Stack Overflow - stack is full!\n");
        exit(1);
    }

    s->items[++(s->top)] = i;
}

char pop_char(char_stack *s)
{
    if (is_char_empty(s))
    {
        printf("ERROR 2 : Stack Underflow - stack is empty!\n");
        exit(2);
    }
    char temp = s->items[s->top--];
    return temp;
}

long pop_int(int_stack *s)
{
    if (is_int_empty(s))
    {
        printf("ERROR 2 : Stack Underflow - stack is empty!\n");
        exit(2);
    }
    long temp = s->items[s->top--];
    return temp;
}

int priority(char c)
{
    if (c == '^')
        return 3;

    if (c == '*' || c == '/' || c == '%')
        return 2;

    if (c == '+' || c == '-')
        return 1;

    return 0;
}

int power(int num1, int num2)
{
    int result = 1;
    for (int i = 0; i < num2; i++)
    {
        result *= num1;
    }
    return result;
}

long calculate(long num1, long num2, char op)
{
    switch (op)
    {
    case '+':
        return num1 + num2;
    case '-':
        return num1 - num2;
    case '*':
        return num1 * num2;
    case '/':
        if (num2 == 0)
        {
            printf("ERROR 4 : Division by zero\n");
            exit(4);
        }
        return num1 / num2;
    case '%':
        return num1 % num2;
    case '^':
        return power(num1, num2);
    default:
        break;
    }

    printf("ERROR 3 : Invalid operators \"%c\"\n", op);
    exit(3);
}

int main()
{
    char varNames[SIZE][SIZE];
    int varValues[SIZE];
    int varCount = 0;

    printf("Welcome to the Simple Language :\n");

    do
    {
        char buf[32], output[32], *marker = buf;
        int i = 0;
        char_stack operators;
        int_stack operand;
        init_char_stack(&operators);
        init_int_stack(&operand);
        printf(">> ");
        fgets(buf, 32, stdin);

        if (strncmp(buf, "let ", 4) == 0)
        {
            char varName[SIZE];
            int value;
            if (sscanf(buf, "let %s = %d", varName, &value) == 2)
            {
                strcpy(varNames[varCount], varName);
                varValues[varCount] = value;
                varCount++;
                continue;
            }
            else
            {
                printf("ERROR: Invalid variable declaration\n");
                continue;
            }
        }

        if (strcmp(buf, "exit\n") == 0)
        {
            printf("Exiting...\n");
            break;
        }

        while (*marker != '\0')
        {
            while (*marker == ' ')
                marker++;
            if (*marker >= '0' && *marker <= '9')
            {
                long num = 0;
                while (*marker >= '0' && *marker <= '9')
                {
                    num = num * 10 + (*marker - '0');
                    output[i++] = *marker;
                    marker++;
                }
                push_int(&operand, num);
            }
            else if (*marker == '(')
            {
                push_char(&operators, *marker);
                marker++;
            }
            else if (*marker == '+' || *marker == '-' || *marker == '*' || *marker == '/' ||
                     *marker == '%' || *marker == '^')
            {
                while (!is_char_empty(&operators) && char_top_of_stack(&operators) != '(' &&
                       priority(*marker) <= priority(char_top_of_stack(&operators)))
                {
                    long num2 = pop_int(&operand);
                    long num1 = pop_int(&operand);
                    long result = calculate(num1, num2, output[i++] = pop_char(&operators));
                    push_int(&operand, result);
                }
                push_char(&operators, *marker);
                marker++;
            }
            else if (*marker == ')')
            {
                while (char_top_of_stack(&operators) != '(')
                {
                    long num2 = pop_int(&operand);
                    long num1 = pop_int(&operand);
                    long result = calculate(num1, num2, output[i++] = pop_char(&operators));
                    push_int(&operand, result);
                }
                pop_char(&operators);
                marker++;
            }
            else if (isalpha(*marker))
            {
                char variable[SIZE];
                int j = 0;
                while (isalnum(*marker))
                {
                    variable[j++] = *marker;
                    marker++;
                }
                variable[j] = '\0';
                int found = 0;
                for (int k = 0; k < varCount; k++)
                {
                    if (strcmp(varNames[k], variable) == 0)
                    {
                        push_int(&operand, varValues[k]);
                        found = 1;
                        break;
                    }
                }
                if (!found)
                {
                    printf("ERROR: Undefined variable '%s'\n", variable);
                    exit(5);
                }
            }
            else
                marker++;
        }

        while (!is_char_empty(&operators))
        {
            long num2 = pop_int(&operand);
            long num1 = pop_int(&operand);
            long result = calculate(num1, num2, output[i++] = pop_char(&operators));
            push_int(&operand, result);
        }
        output[i] = '\0';

        printf(">> result is : %li\n", int_top_of_stack(&operand));
    } while (1);

    return 0;
}
