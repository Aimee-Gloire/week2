#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <openssl/sha.h>

typedef struct Block {
    int index;
    char data[256];
    char prev_hash[65];
    char hash[65];
    struct Block *next;
} Block;

/* -------- FUNCTION PROTOTYPES -------- */
void calculate_hash(Block *b, char output[65]);
Block* create_genesis_block();
void add_block(Block *head, const char *data);
int is_chain_valid(Block *head);

/* -------- HASH FUNCTION (TASK 1) -------- */
void calculate_hash(Block *b, char output[65]) {

    char input[512];
    sprintf(input, "%d%s%s", b->index, b->data, b->prev_hash);

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)input, strlen(input), hash);

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        sprintf(output + (i * 2), "%02x", hash[i]);

    output[64] = '\0';
}

/* -------- GENESIS BLOCK (TASK 1) -------- */
Block* create_genesis_block() {

    Block *genesis = (Block*)malloc(sizeof(Block));

    genesis->index = 0;
    strcpy(genesis->data, "Genesis Block");
    strcpy(genesis->prev_hash, "0");

    calculate_hash(genesis, genesis->hash);

    genesis->next = NULL;
    return genesis;
}

/* -------- ADD BLOCK (TASK 2) -------- */
void add_block(Block *head, const char *data) {

    Block *temp = head;
    while (temp->next != NULL)
        temp = temp->next;

    Block *newBlock = (Block*)malloc(sizeof(Block));

    newBlock->index = temp->index + 1;
    strcpy(newBlock->data, data);
    strcpy(newBlock->prev_hash, temp->hash);

    calculate_hash(newBlock, newBlock->hash);

    newBlock->next = NULL;
    temp->next = newBlock;
}

/* -------- IMMUTABILITY AUDITOR (TASK 3) -------- */
int is_chain_valid(Block *head) {

    Block *current = head;
    Block *previous = NULL;

    while (current != NULL) {

        char recalculated_hash[65];
        calculate_hash(current, recalculated_hash);

        if (strcmp(current->hash, recalculated_hash) != 0) {
            printf("❌ Block %d has invalid hash!\n", current->index);
            return 0;
        }

        if (previous != NULL) {
            if (strcmp(current->prev_hash, previous->hash) != 0) {
                printf("❌ Block %d previous hash mismatch!\n", current->index);
                return 0;
            }
        }

        previous = current;
        current = current->next;
    }

    return 1;
}

/* -------- MAIN (TAMPER DEMO) -------- */
int main() {

    Block *chain = create_genesis_block();

    add_block(chain, "Alice pays Bob 10");
    add_block(chain, "Bob pays Carol 5");

    printf("\nChecking chain before tampering...\n");
    if (is_chain_valid(chain))
        printf("✅ Chain is valid\n");

    printf("\n⚠️ Tampering with Block 1...\n");
    strcpy(chain->next->data, "Alice pays Bob 1000");

    printf("\nChecking chain after tampering...\n");
    if (!is_chain_valid(chain))
        printf("🚨 Chain is broken!\n");

    return 0;
}
