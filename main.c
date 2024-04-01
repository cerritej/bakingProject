#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define TOT_RECIPES 5
#define TOT_INGREDIENTS 9

typedef struct {
    sem_t pantry;
    sem_t refrigerator[2];
    sem_t mixer;
    sem_t bowl;
    sem_t spoon;
    sem_t oven;
} Kitchen;

typedef struct {
    int id;
    Kitchen* kitchen;
    int ramsied; // Flag to indicate if the baker is 'Ramsied'
} Baker;

Kitchen* kitchen;

// Global variable to track the 'Ramsied' baker
int ramsied_baker_id = -1;

void* baker_thread(void* arg) {
    Baker* baker = (Baker*)arg;

    // Recipes
    char* recipes[TOT_RECIPES] = { "Cookies", "Pancakes", "Homemade pizza dough", "Soft Pretzels", "Cinnamon rolls" };

    // Ingredients
    char* ingredients[TOT_RECIPES][TOT_INGREDIENTS] = {
        {"Flour", "Sugar", "Milk", "Butter", NULL, NULL, NULL, NULL, NULL},
        {"Flour", "Sugar", "Baking soda", "Salt", "Egg", "Milk", "Butter", NULL, NULL},
        {"Yeast", "Sugar", "Salt", NULL, NULL, NULL, NULL, NULL, NULL},
        {"Flour", "Sugar", "Salt", "Yeast", "Baking Soda", "Egg", NULL, NULL, NULL},
        {"Flour", "Sugar", "Salt", "Butter", "Eggs", "Cinnamon", NULL, NULL, NULL}
    };

    // Real-time output (for demonstration purposes)
    printf("Baker_%d starting...\n", baker->id);

    // Randomly select one baker to be 'Ramsied'
    if (baker->id == ramsied_baker_id) {
        // Announce that the baker has been Ramsied
        printf("Baker_%d has been Ramsied! They may drop all ingredients for a recipe at some point.\n", baker->id);

        // Randomly select a recipe to be 'Ramsied'
        int ramsied_recipe_index = rand() % TOT_RECIPES;

        // Simulate baking each recipe
        for (int i = 0; i < TOT_RECIPES; i++) {
            printf("Baker_%d starting %s...\n", baker->id, recipes[i]);

            // Simulate acquiring ingredients
            for (int j = 0; ingredients[i][j] != NULL; j++) {
                printf("Baker_%d acquiring %s...\n", baker->id, ingredients[i][j]);
                usleep(rand() % 1000000); // Simulate acquiring time
            }

            // Simulate mixing
            printf("Baker_%d mixing...\n", baker->id);
            usleep(rand() % 1000000); // Simulate mixing time

            // Simulate baking
            printf("Baker_%d baking...\n", baker->id);
            usleep(rand() % 1000000); // Simulate baking time

            // Announce completion of the recipe
            printf("Baker_%d finished %s!\n", baker->id, recipes[i]);

            // Check if the current recipe is the randomly selected Ramsied recipe
            if (i == ramsied_recipe_index) {
                // Release all semaphores held by the Ramsied baker
                sem_post(&baker->kitchen->pantry);
                sem_post(&baker->kitchen->refrigerator[0]);
                sem_post(&baker->kitchen->refrigerator[1]);
                sem_post(&baker->kitchen->mixer);
                sem_post(&baker->kitchen->bowl);
                sem_post(&baker->kitchen->spoon);
                sem_post(&baker->kitchen->oven);

                // Announce that the baker has dropped all ingredients for the Ramsied recipe
                printf("Baker_%d has dropped all their ingredients for recipe %s due to being Ramsied.\n", baker->id, recipes[i]);
            }
        }
    }
    else {
        // Simulate baking each recipe for non-Ramsied bakers
        for (int i = 0; i < TOT_RECIPES; i++) {
            printf("Baker_%d starting %s...\n", baker->id, recipes[i]);

            // Simulate acquiring ingredients
            for (int j = 0; ingredients[i][j] != NULL; j++) {
                printf("Baker_%d acquiring %s...\n", baker->id, ingredients[i][j]);
                usleep(rand() % 1000000); // Simulate acquiring time
            }

            // Simulate mixing
            printf("Baker_%d mixing...\n", baker->id);
            usleep(rand() % 1000000); // Simulate mixing time

            // Simulate baking
            printf("Baker_%d baking...\n", baker->id);
            usleep(rand() % 1000000); // Simulate baking time

            // Announce completion of the recipe
            printf("Baker_%d finished %s!\n", baker->id, recipes[i]);
        }
    }

    pthread_exit(NULL);
}

void initialize_kitchen() {
    sem_init(&kitchen->pantry, 0, 1);
    sem_init(&kitchen->refrigerator[0], 0, 1);
    sem_init(&kitchen->refrigerator[1], 0, 1);
    sem_init(&kitchen->mixer, 0, 2);
    sem_init(&kitchen->bowl, 0, 3);
    sem_init(&kitchen->spoon, 0, 5);
    sem_init(&kitchen->oven, 0, 1);
}

int main() {
    srand(time(NULL));

    int num_bakers;
    printf("Enter the number of bakers: ");
    scanf("%d", &num_bakers);

    // Create shared memory for the kitchen
    kitchen = (Kitchen*)malloc(sizeof(Kitchen));

    // Initialize the kitchen resources
    initialize_kitchen();

    // Randomly select one baker to be 'Ramsied'
    ramsied_baker_id = rand() % num_bakers + 1;

    // Create baker threads
    pthread_t bakers[num_bakers];
    Baker baker_args[num_bakers];
    for (int i = 0; i < num_bakers; i++) {
        baker_args[i].id = i + 1;
        baker_args[i].kitchen = kitchen;
        pthread_create(&bakers[i], NULL, baker_thread, (void*)&baker_args[i]);
    }

    // Join baker threads
    for (int i = 0; i < num_bakers; i++) {
        pthread_join(bakers[i], NULL);
    }

    // Clean up resources
    free(kitchen);

    return 0;
}
