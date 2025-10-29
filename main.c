// includes ------------------------------------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LENGTH 50
#define MAX_DESCRIPTION_LENGTH 100
#define MAX_PREPOSITION_LENGTH 10
#define MAX_DIALOGUE_LENGTH 100
#define MAX_NPC 50
#define MAX_OBJECT 50
#define MAX_LOCATION 50
#define MAX_NEAR_LOCATIONS 5
#define MAX_STATUS 3
#define MAP_COL 10
#define MAP_ROW 25

// strucs ------------------------------------------------------------------------------------------------------------

typedef struct Object{
    char name[MAX_NAME_LENGTH];
    char description[MAX_DESCRIPTION_LENGTH];
    int value;
} Object; // Details de la structure : Object
typedef struct NPC{
    char name[MAX_NAME_LENGTH];
    char dialogue[MAX_DIALOGUE_LENGTH];
    int type;
    int health;
    int attack;
    // Pointeur vers la structure Object
    Object *object;
    Object *condition;

} NPC; // Details de la structure : NPC
typedef struct Location{
    char name[MAX_NAME_LENGTH];
    char preposition[MAX_PREPOSITION_LENGTH];
    char description[150];
    struct Location **lieux_atteignables;
    // Pointeur vers la structure Object
    Object *objectCondition;
    Object *locationInventory[MAX_OBJECT];
    // Pointeur vers la structure NPC
    NPC *npc;
} Location; // Details de la structure : Location
typedef struct Character{
    char name[MAX_NAME_LENGTH];
    int health;
    // Pointeur vers la structure Object
    Object *inventory[MAX_OBJECT];
    // Pointeur vers la structure Location
    Location *current_location;
    char map[MAP_COL][MAP_ROW];
} Character; // Details de la structure : Character

// Outils ------------------------------------------------------------------------------------------------------------

int numberVerification(void) {
    int num;
    // Tant que la saisie n'est pas un entier
    while (scanf("%d", &num) != 1) {
        printf("Attention, vous devez renseigner un nombre !\n");
        // Vide le buffer jusqu'au '\n'
        while (getchar() != '\n');
    }
    // Vide le '\n' restant
    while (getchar() != '\n');
    return num;
} // Verification de saisie

void objectExamine(const Character *player, const Location *loc, char *name) {
    // Cherche l'objet dans l’inventaire
    for (int i = 0; i < MAX_OBJECT; i++) {
        if (player->inventory[i] != NULL && strcmp(player->inventory[i]->name, name) == 0) {
            // Affiche l'objet s'il existe
            printf("%s : %s --> %d valeur/degats\n",player->inventory[i]->name,player->inventory[i]->description,player->inventory[i]->value);
            return;
            }
    }
    // Cherche l'objet dans le lieu courant
    for (int i = 0; i < MAX_OBJECT; i++) {
        if (loc->locationInventory[i] != NULL && strcmp(loc->locationInventory[i]->name,name) == 0) {
            // Affiche l'objet s'il existe
            printf("%s : %s --> %d valeur/degats\n",loc->locationInventory[i]->name,loc->locationInventory[i]->description,loc->locationInventory[i]->value);
            return;
            }
    }
    printf("Vous ne voyez pas de '%s' ici, ni dans votre inventaire.\n",name);
} // Fonction pour examiner un object

void gameSave(const char *filename, Character *player, Object* objectTable[], NPC* npcTable[], Location* locationTable[]) {
    FILE *file = fopen(filename, "w");
    if (file) {

        // Sauvegarde des objets
        fprintf(file, "OBJETS\n");
        for (int i = 0; i < MAX_OBJECT; i++) {
            if (objectTable[i] != NULL) {
                fprintf(file, "Nom: %s\n", objectTable[i]->name);
                fprintf(file, "Description: %s\n", objectTable[i]->description);
                fprintf(file, "Valeur: %d\n", objectTable[i]->value);
                fprintf(file, "\n");
            }
        }

        // Sauvegarde des PNJs
        fprintf(file, "PNJS\n");
        for (int i = 0; i < MAX_NPC; i++) {
            if (npcTable[i] != NULL) {
                fprintf(file, "Nom: %s\n", npcTable[i]->name);
                fprintf(file, "Dialogue: %s\n", npcTable[i]->dialogue);
                fprintf(file, "Type: %d\n", npcTable[i]->type);
                fprintf(file, "Attaque: %d\n", npcTable[i]->attack);
                fprintf(file, "Vie: %d\n", npcTable[i]->health);

                if (npcTable[i]->object != NULL) {
                    fprintf(file, "Objet: %s\n", npcTable[i]->object->name);
                } else {
                    fprintf(file, "Objet: NULL\n");
                }

                if (npcTable[i]->condition != NULL) {
                    fprintf(file, "Condition: %s\n", npcTable[i]->condition->name);
                } else {
                    fprintf(file, "Condition: NULL\n");
                }

                fprintf(file, "\n");
            }
        }

        // Sauvegarde des lieux
        fprintf(file, "LIEUX\n");
        for (int i = 0; locationTable[i] != NULL; i++) {
            Location *loc = locationTable[i];
            if (loc != NULL) {
                fprintf(file, "Nom: %s\n", loc->name);
                fprintf(file, "Preposition: %s\n", loc->preposition);
                fprintf(file, "Description: %s\n", loc->description);

                // Lieux atteignables
                fprintf(file, "Atteignables:");
                if (loc->lieux_atteignables != NULL) {
                    int firstReachable = 1;
                    for (int j = 0; j < MAX_NEAR_LOCATIONS; j++) {
                        if (loc->lieux_atteignables[j] != NULL) {
                            if (!firstReachable) fprintf(file, ",");
                            fprintf(file, " %s", loc->lieux_atteignables[j]->name);
                            firstReachable = 0;
                        }
                    }
                    if (firstReachable) {
                        fprintf(file, " NULL");
                    }
                } else {
                    fprintf(file, " NULL");
                }
                fprintf(file, "\n");

                // Condition d'accès
                if (loc->objectCondition != NULL) {
                    fprintf(file, "Condition: %s\n", loc->objectCondition->name);
                } else {
                    fprintf(file, "Condition: NULL\n");
                }

                // Inventaire du lieu
                fprintf(file, "Inventaire:");
                int firstItem = 1;
                for (int k = 0; k < MAX_OBJECT; k++) {
                    if (loc->locationInventory[k] != NULL) {
                        if (!firstItem) fprintf(file, ",");
                        fprintf(file, " %s", loc->locationInventory[k]->name);
                        firstItem = 0;
                    }
                }
                if (firstItem) {
                    fprintf(file, " NULL");
                }
                fprintf(file, "\n");

                // PNJ présent
                if (loc->npc != NULL) {
                    fprintf(file, "NPC: %s\n", loc->npc->name);
                } else {
                    fprintf(file, "NPC: NULL\n");
                }

                fprintf(file, "\n");
            }
        }

        // Sauvegarde du joueur
        fprintf(file, "\nJOUEUR\n");
        if (player != NULL) {
            fprintf(file, "Nom: %s\n", player->name);

            if (player->current_location != NULL) {
                fprintf(file, "LieuActuel: %s\n", player->current_location->name);

            }
            else {
                fprintf(file, "LieuActuel: NULL\n");
            }

            fprintf(file, "Inventaire:");
            int firstItem = 1;
            for (int k = 0; k < MAX_OBJECT; k++) {
                if (player->inventory[k] != NULL) {
                    if (!firstItem) fprintf(file, ",");
                    fprintf(file, " %s", player->inventory[k]->name);
                    firstItem = 0;
                }
            }
            if (firstItem) {
                fprintf(file, " NULL");
            }
            fprintf(file, "\n");
        } else {
            fprintf(file, "Nom: NULL\n");
            fprintf(file, "LieuActuel: NULL\n");
            fprintf(file, "Inventaire: NULL\n");
        }

        fclose(file);

    } else {
        printf("Erreur d'ouverture du fichier de sauvegarde\n");
    }
} // Fonction de sauvegarde






// objects ------------------------------------------------------------------------------------------------------------

Object* objectCreation(const char *name, const char *description, const int value) {
    // Allocation dynamique de la memoire
    Object *obj = malloc(sizeof(Object));

    // Si erreur d'allocation
    if (!obj) return NULL;

    // On fait strncpy pour copier les chaînes de caractère
    // On défini le dernier le dernier caractère de la chaîne par \0
    strncpy(obj->name, name, MAX_NAME_LENGTH - 1);
    obj->name[MAX_NAME_LENGTH - 1] = '\0';

    strncpy(obj->description, description, 99);
    obj->description[99] = '\0';

    obj->value = value;

    return  obj;
} // Creation d'un object (sans mode administrateur)
Object* objectByName(const char *input, Object*objectTable[MAX_OBJECT]) {
    // Parcours le tableau et retourne l'objet s'il y a une correspondance
    for (int i = 0; i < MAX_OBJECT; i++) {
        if(objectTable[i] != NULL && strcmp(input,objectTable[i]->name) ==0) {
            return objectTable[i];
        }
    }
    return NULL;
} // Fonction qui sert à retrouver un objet dans un tableau d'objets à partir de son nom
Object* objectCreationInput(Object* objectTable[]) {
    // Allocation dynamique de la memoire
    Object *obj = malloc(sizeof(Object));
    // Si erreur d'allocation
    if (!obj) return NULL;

    // Utilisation de fgets pour lire l'entree sans depasser la longueur max MAX_NAME_LENGTH
    // Remplacement du retour à la ligne (\n) par le caractere de fin de chaine

    printf("Nom de l'object : \n");
    fgets(obj->name, MAX_NAME_LENGTH, stdin);
    obj->name[strcspn(obj->name, "\n")] = '\0';

    // Contrôle des doublons
    if (objectByName(obj->name, objectTable) != NULL) {
        printf("Erreur : cet objet existe deja.\n");
        free(obj);
        return NULL;
    }

    printf("Description de l'object : \n");
    fgets(obj->description, 100, stdin);
    obj->description[strcspn(obj->description, "\n")] = '\0';

    printf("Valeur de l'object (entre 1 et 100) : \n");
    obj->value = numberVerification();

    printf("-----------------------------\n");
    return  obj;
} // Creation d'un objet en input

void objectinventoryAdd(Object *obj, Object *to[MAX_OBJECT]) {
    int i = 0;
    // Recherche un emplacement vide
    while (i < MAX_OBJECT) {
        if ( to[i] == NULL ) {
            to[i] = obj;
            i=MAX_OBJECT; // Sortie de la boucle dès qu'un emplacement est trouvé
        }
        i++;
    }
} // Ajoute un objet dans l'inventaire
void objectinventoryRemove(const Object* object, Object *to[MAX_OBJECT]) {
    int i = 0;
    //Parcours le tableau et compare chaque élément avec l'objet en paramètre
    while (i < MAX_OBJECT) {
        if (object == to[i]) {
            to[i] = NULL;
        }
        i++;
    }
} // Retire un objet de l'inventaire
void objectinventoryAddInput(Object*  objectTable[]) {
    int i = 0;
    int objectExists = 0;  // Variable pour vérifier si l'objet existe déjà

    // Crée un nouvel objet via objectCreationInput()
    Object* obj = objectCreationInput(objectTable);

    if (objectExists == 0) {
        while (i < MAX_OBJECT) {
            if (objectTable[i] == NULL )  { // Trouve la première place vide
                objectTable[i] = obj; // Ajoute l'objet au tableau
                i=MAX_OBJECT;
            }
            i++;
        }
    }
} // Ajoute un objet dans la table d'objet
void objectinventorySwitch(Object * object, Object *from[MAX_OBJECT], Object *to[MAX_OBJECT]) {

    objectinventoryRemove(object,from); // Retire un objet de l'inventaire
    objectinventoryAdd(object,to); // Ajoute un objet dans l'inventaire

} // Déplace les objets d'un inventaire à un autre en utilisant 2 autres fonctions
void objectTableDisplay(Object *objectTable[MAX_OBJECT]) {
    int i = 0;
    // Parcours la table d'objet
    while (i < MAX_OBJECT) {
        if (objectTable[i]) {
            printf("- %s\n", objectTable[i]->name); // Affiche chaque objet présent
        }
        i++;
    }
} // Affiche les objets présents dans la table d'objet

// location ------------------------------------------------------------------------------------------------------------

Location* locationCreation(const char *name, const char *description, const char *preposition, Object *objectCondition) {
    // Allocation dynamique de la memoire
    Location *lieu = malloc(sizeof(Location));

    // Si erreur d'allocation
    if (!lieu) {
        return NULL;
    }

    // On fait strncpy pour copier les chaînes de caractère
    // On défini le dernier le dernier caractère de la chaîne par \0
    strncpy(lieu->name, name, MAX_NAME_LENGTH - 1);
    lieu->name[MAX_NAME_LENGTH - 1] = '\0';

    strncpy(lieu->description, description, 149);
    lieu->description[149] = '\0';

    strncpy(lieu->preposition, preposition, MAX_PREPOSITION_LENGTH - 1);
    lieu->preposition[MAX_PREPOSITION_LENGTH - 1] = '\0';

    // Alloue un tableau de pointeurs vers des objets pour stocker les lieux voisins atteignables depuis ce lieu
    lieu->lieux_atteignables = calloc(MAX_NEAR_LOCATIONS, sizeof(Location *));
    lieu->objectCondition = objectCondition;

    for (int i = 0; i < MAX_OBJECT; i++) {
        lieu->locationInventory[i] = NULL;
    }

    lieu->npc = NULL;

    return lieu;
} // Creation d'un lieu (sans mode administrateur)
Location * locationByName(const char *input,Location*locationTable[MAX_LOCATION]) {
    // Parcours le tableau et retourne le lieu s'il y a une correspondance
    for (int i = 0; i < MAX_NEAR_LOCATIONS; i++) {
        if(locationTable[i] != NULL && strcmp(input,locationTable[i]->name) ==0) {
            return locationTable[i];
        }
    }
    return NULL;
} // Fonction qui sert à retrouver un lieu dans un tableau de location à partir de son nom
Location* locationCreationInput(Location* locationTable[MAX_LOCATION]) {
    // Allocation dynamique de la memoire
    Location *loc = malloc(sizeof(Location));
    // Si erreur d'allocation
    if (!loc) return NULL;

    // Alloue un tableau de pointeurs vers des objets pour stocker les lieux voisins atteignables depuis ce lieu
    loc->lieux_atteignables = calloc(MAX_NEAR_LOCATIONS, sizeof(Location *));
    // Initialise les champs
    loc->objectCondition = NULL;
    loc->npc = NULL;

    printf("Nom du lieu :\n");
    fgets(loc->name, MAX_NAME_LENGTH, stdin);
    loc->name[strcspn(loc->name, "\n")] = '\0';

    // Contrôle des doublons
    if (locationByName(loc->name, locationTable) != NULL) {
        printf("Erreur : ce lieu existe deja.\n");
        free(loc);
        return NULL;
    }

    printf("Preposition utilisee :\n");
    fgets(loc->preposition, MAX_PREPOSITION_LENGTH, stdin);
    loc->preposition[strcspn(loc->preposition, "\n")] = '\0';

    printf("Description du lieu :\n");
    fgets(loc->description, 150, stdin);
    loc->description[strcspn(loc->description, "\n")] = '\0';

    for (int i = 0; i < MAX_OBJECT; i++) {
        loc->locationInventory[i] = NULL;
    }

    printf("-----------------------------\n");
    return loc;
} // Creation d'un lieu (avec mode administrateur)

void locationReachable(Location *origin, Location *destination) {
    // Vérifie si la lieu de départ et la destination ne sont pas NULL
    if (origin != NULL && destination != NULL) {
        int i = 0, j = 0;

        // Parcours des lieux d'origine atteignables
        while (i < MAX_NEAR_LOCATIONS) {
            if (!origin->lieux_atteignables[i]) { // Trouve un emplacement vide
                // Parcours des lieux de destination atteignables
                while (j < MAX_NEAR_LOCATIONS) {
                    if (!destination->lieux_atteignables[j]) { // Trouve un emplacement vide
                        // Lie lieu d'origine au lieu de destination
                        destination->lieux_atteignables[j] = origin;
                        origin->lieux_atteignables[i] = destination;
                        j = i = MAX_NEAR_LOCATIONS; // Sortie de boucle
                    }
                    j++;
                }
            }
            i++;
        }

    }

} // Fonction de liaison
void locationDisplay(const Location *loc) {

    // Objects sur place
    for (int i = 0; i < MAX_OBJECT; i++) {

        if (loc->locationInventory[i]) {
            if (i==0) {
                printf("Objects presents :\n");
            }
            if (loc->locationInventory[i]) {
                printf(" - %s\n",
                   loc->locationInventory[i]->name);
            }

        }
    }

    // PNJ eventuel
    if (loc->npc) {
        printf("PNJ present :\n - %s\n",loc->npc->name);
    }

    // Lieux atteignables

    for (int i = 0; i < MAX_NEAR_LOCATIONS; i++) {

        if (loc->lieux_atteignables[i]) {
            if(i==0) {
                printf("Lieux atteignables :\n");
            }
            printf(" - %s\n",loc->lieux_atteignables[i]->name);
        }
    }
    printf("-----------------------------\n");
} // Affiche les proprietes d'un lieu
void locationAddInput(Location* locationTable[]) {
    int i = 0;
    int locationExists = 0;

    // Crée une nouvelle location via locationCreationInput()
    Location* loc = locationCreationInput(locationTable);

    if (locationExists == 0) {
        while (i < MAX_LOCATION) {
            if (locationTable[i] == NULL )  { // Trouver la première place vide
                locationTable[i] = loc; // Ajoute le lieu au tableau
                i=MAX_LOCATION;
            }
            i++;
        }
    }
} // Ajoute un nouveau lieu dans la table location
void locationRemove(const Location* location, Location*to[MAX_LOCATION]) {
    int i = 0;

    //Parcours le tableau et compare chaque élément avec le lieu en paramètre
    while (i < MAX_LOCATION) {
        if (location == to[i]) {
            to[i] = NULL;
        }
        i++;
    }
} // Retire un lieu du tableau des lieux
void locationTableDisplay(Location *locationTable[MAX_LOCATION]) {
    printf("Liste des lieux :\n");
    for (int i = 0; i < MAX_LOCATION; i++) {
        if (locationTable[i] != NULL) {
            printf("Lieu %d :%s\n", i + 1, locationTable[i]->name);
        }
    }
} // Affiche tous les lieux presents dans la table des lieux

// character ------------------------------------------------------------------------------------------------------------
void characterFight(Character* player,NPC* npc) {

    printf("%s\n",npc->dialogue);
    char input[50];
    int running = 1;
    // Boucle du combat
    while(running) {
        // Si le joueur n'a plus de points de vie, il perd le combat
        if(player->health <= 0 ) {
            printf("Vous avez perdu le combat contre %s\n",npc->name);
            printf("-----------------------------\n");
            running = 0;
        }
        else {
            // Si le NPC n'a plus de points de vie, le joueur gagne le combat et la boucle se termine
            if(npc->health <= 0) {
                printf("Vous avez battu %s\n",npc->name);
                running = 0;
                npc = NULL;
            }
            else {
                // Affiche les PVs du joueur et du NPC à chaque tour
                printf("PVs restant : %d PVs de %s : %d\n",player->health,npc->name,npc->health);
                printf("-----------------------------\n");
                printf("Choisissez un objet de votre inventaire pour vous battre :\n");

                printf("Vous avez en votre possession :\n");
                // Affiche les objets de l'inventaire et les dégats qu'ils infligent
                for (int i = 0; i < MAX_OBJECT; i++) {
                    if(player->inventory[i] != NULL) {
                        printf("- %s --> %d degat\n",player->inventory[i]->name,player->inventory[i]->value);
                    }
                }
                    // Si l'objet utilisé par le joueur existe dans l'inventaire → il inflige des dégâts au NPC
                    if (fgets(input, sizeof(input), stdin) != NULL) {
                        input[strcspn(input, "\n")] = '\0';

                        Object *object = objectByName(input, player->inventory);

                        if (object != NULL) {
                            // Si la valeur (les dégâts) de l'objet est supérieure à la santé du NPC
                            // L'objet inflige autant de dégâts que nécessaire pour tuer le NPC.
                            if(object->value > npc->health) {
                                object->value -= npc->health;
                                npc->health = 0;
                            }
                            // Si l'objet inflige moins de dégâts que la santé du NPC
                            // Le NPC subit les dégâts, et l'objet est retiré de l'inventaire
                            else if(npc->health>object->value) {
                                npc->health -= object->value;
                                objectinventoryRemove(object,player->inventory);
                            }
                            // Si l'objet inflige exactement les dégâts nécessaires pour tuer le NPC
                            // L'objet est également retiré de l'inventaire
                            else {
                                npc->health = 0;
                                objectinventoryRemove(object,player->inventory);

                            }


                        }
                    }

                    if(npc->health <= 0) {
                        printf("Vous avez battu %s\n",npc->name);
                        running = 0;
                    }
                    else {
                        // Si le NPC n'est pas encore mort → il attaque le joueur et réduit ses points de vie.
                        player->health-= npc->attack;
                        printf("%s vous a infige %d degats\n",npc->name,npc->attack);
                    }
                }
            }
    }
} // Fonction qui gère le combat entre le joueur et un npc

Character* characterCreation(const char *name) {
    // Allocation dynamique de la memoire
    Character *perso = malloc(sizeof(Character));
    // Si erreur d'allocation
    if (!perso) return NULL;

    // On fait strncpy pour copier les chaînes de caractère
    // On défini le dernier le dernier caractère de la chaîne par \0
    strncpy(perso->name, name, MAX_NAME_LENGTH - 1);
    perso->name[MAX_NAME_LENGTH - 1] = '\0';

    for (int i = 0; i < MAX_OBJECT; i++) {
        perso->inventory[i] = NULL;
    }

    return perso;
} // Creation d'un personnage (sans mode administrateur)
Character* characterCreationInput() {
    // Allocation dynamique de la memoire
    Character *character = malloc(sizeof(Character));
    // Si erreur d'allocation
    if (!character) return NULL;


    printf("Entrez le nom du personnage :\n");
    fgets(character->name, MAX_NAME_LENGTH, stdin);
    character->name[strcspn(character->name, "\n")] = '\0';

    printf("Entrez le nombre de point de vie de votre personnage :\n");
    character->health = numberVerification();

    // Mise a vide de tout les emplacement de l'inventaire
    for (int i = 0; i < MAX_OBJECT; i++) {
        character->inventory[i] = NULL;
    }
    // Initialisation de la carte du personnage
    for (int i=0;i<MAP_COL;i++) {
        for (int j=0;j<(MAP_ROW);j++) {
            character->map[i][j] = '~';
        }
    }
    character->current_location = NULL;

    printf("-----------------------------\n");
    return character;
} // Creation d'un personnage (avec mode administrateur)
void characterMoveAdmin(Character *player, Location *dest) {
    // Déplacement du joueur avec le mode admin
    player->current_location = dest;
    printf("\n-----------------------------\n");
    printf("   Vous etes %s %s.\n",
           dest->preposition, dest->name);
    printf("-----------------------------\n");
    // On peut immediatement afficher le lieu
    locationDisplay(dest);

    if(dest->npc != NULL) {
        if(dest->npc->type==1) {
            characterFight(player,dest->npc);
        }

    }
} // Gameplay de l'admin
void characterMove(Character *player, const Location *location) {

    for (int i = 0; i < MAX_NEAR_LOCATIONS; i++) {
        // Pour tous les lieux atteignables a partir de location
        Location *destination = player->current_location->lieux_atteignables[i];
        if (destination != NULL && location == destination) {

            // Si aucun objet n'est nécessaire pour entrer dans le lieu
            if (destination->objectCondition == NULL) {
                characterMoveAdmin(player, destination);
            }
            else {
                int condition = 0;

                int j = 0;
                // Si un objet est nécessaire pour entrer dans le lieu
                while(j<MAX_OBJECT) {
                    // Condition si l'objet est présent dans l'inventaire du joueur
                    if (player->inventory[j] != NULL && player->inventory[j] == destination->objectCondition) {
                        condition = 1;
                        printf("Vous devez utiliser : %s pour passer, l'utiliser ?\nNon {0} Oui {1}\n");
                        int choice = 2;
                        while (choice != 0 && choice != 1) {
                            choice = numberVerification();
                        }
                        if(choice==0) {
                            printf("Vous gardez l'objet : %s mais ne passez pas.\n",destination->objectCondition->name);
                        }
                        else {
                            printf("Vous avez utilise l'objet requis : %s\n",destination->objectCondition->name);

                            player->inventory[j] = NULL;
                            destination->objectCondition=NULL;
                            characterMoveAdmin(player, destination);
                        }
                        j = MAX_OBJECT;
                    }
                    j++;
                }

                // Condition si l'objet n'est pas présent dans l'inventaire du joueur
                if (condition == 0) {
                    printf("Vous avez besoin de : %s pour passer.\n",destination->objectCondition->name);
                }
            }
        }
    }
    }// Gameplay du joueur
void characterReachables(const Character player) {
    // Si condition de passage non definie
    if (!player.current_location->objectCondition) {
        printf("-----------------------------\nVous pouvez aller :\n");
        for (int i = 0; i < MAX_NEAR_LOCATIONS; i++) {
            Location *l = player.current_location->lieux_atteignables[i];
            if (l)
                printf(" - %s %s\n", l->preposition, l->name);
        }
    }
    // Sinon besoin d'un objet
    else {
        printf("Vous avez besoin de : %s\n", player.current_location->objectCondition->name);
    }
    printf("-----------------------------\n");
} // Affiche les lieux vers lesquels le joueur peut se déplacer
void afficherEtat(const Character *player) {
    printf("-----------------------------\n"
           "Etat du personnage\n");
    printf("Lieu actuel : %s\n", player->current_location->name);
    printf("Sante : %d pvs\n", player->health);
    printf("Inventaire : ");
    int compte = 0;
    for (int i = 0; i < MAX_OBJECT; i++) {
        if (player->inventory[i] != NULL) {
            compte++;
        }
    }
    printf("%d\n", compte);
    printf("-----------------------------\n");
} // Affiche l'état du personnage

// npc ------------------------------------------------------------------------------------------------------------

NPC* npcCreation(const char *name, const char *dialogue) {
    // Allocation dynamique de la memoire
    NPC *pnj = malloc(sizeof(NPC));
    // Si erreur d'allocation
    if (!pnj) return NULL;

    strncpy(pnj->name, name, MAX_NAME_LENGTH - 1);
    pnj->name[MAX_NAME_LENGTH - 1] = '\0';

    strncpy(pnj->dialogue, dialogue, MAX_DIALOGUE_LENGTH - 1);
    pnj->dialogue[MAX_DIALOGUE_LENGTH - 1] = '\0';

    return pnj;
} // Creation d'un npc (sans mode administrateur)
NPC* npcByName(const char *input, NPC*npcTable[MAX_NPC]) {
    // Parcours le tableau et retourne le npc s'il y a une correspondance
    for (int i = 0; i < MAX_NPC; i++) {
        if(npcTable[i] != NULL && strcmp(input,npcTable[i]->name) ==0) {
            return npcTable[i];
        }
    }
    return NULL;
} // Fonction qui sert à retrouver un npc dans une table npc à partir de son nom
NPC* npcCreationInput(NPC* npcTable[]) {
    // Allocation dynamique de la memoire
    NPC *pnj = malloc(sizeof(NPC));
    // Si erreur d'allocation
    if (!pnj) return NULL;

    printf("Entrez le nom du PNJ :\n");
    fgets(pnj->name, MAX_NAME_LENGTH, stdin);
    pnj->name[strcspn(pnj->name, "\n")] = '\0';

    // Contrôle des doublons
    if (npcByName(pnj->name, npcTable) != NULL) {
        printf("Erreur : ce pnj existe deja.\n");
        free(pnj);
        return NULL;
    }

    printf("Dialogue du PNJ :\n");
    fgets(pnj->dialogue, MAX_DIALOGUE_LENGTH, stdin);
    pnj->dialogue[strcspn(pnj->dialogue, "\n")] = '\0';

    // Affectation d'un rôle au npc
    printf("Choisissez un type de personnage :\nNeutre {0} Ennemi {1}\n");
    int choice = 2;
    while (choice != 0 && choice != 1) {
        choice = numberVerification();
    }
    pnj->type = choice;
    // Caractéristiques NPC Neutre
    if(choice==0) {
        pnj->attack = 0;
        pnj->health = 0;
    }
    else {
        // Caractéristiques NPC Ennemi
        printf("Entrez un nombre de degats :\n");
        pnj->attack = numberVerification();
        printf("Entrez le nombre de points de vie :\n");
        pnj->health = numberVerification();
    }

    pnj->condition = NULL;
    pnj->object = NULL;

    return pnj;
} // Creation d'un npc (avec mode administrateur)
void npcDisplay(NPC o) {
    printf("Nom : %s\n", o.name);
    printf("Dialogue : %s\n", o.dialogue);
} // Affiche les propriétées d'un NPC
void npcAddInput(NPC* npcTable[]) {
    int i = 0;
    int npcExists = 0;

    // Crée un nouvel pnj via npcCreationInput()
    NPC* pnj = npcCreationInput(npcTable);

    if (npcExists == 0) {
        while (i < MAX_NPC) {
            if (npcTable[i] == NULL )  { // Trouver la première place vide
                npcTable[i] = pnj; // Ajoute un pnj au tableau
                i=MAX_NPC;
            }
            i++;
        }
    }
} // Ajoute un NPC
void npcRemove(const NPC* npc, NPC *to[MAX_NPC]) {
    int i = 0;
    //Parcours le tableau et compare chaque élément avec le npc en paramètre
    while (i < MAX_NPC) {
        if (npc == to[i]) {
            to[i] = NULL;
        }
        i++;
    }
} // Retire un NPC

void npcTableDisplay(NPC *npcTable[MAX_NPC]) {
    int i = 0;
    while (i < MAX_NPC) {
        if (npcTable[i]) {
            printf("- %s\n", npcTable[i]->name);

        }
        i++;
    }
} // Affiche tous les npc presents dans la table des npc

void gameLoad(const char *filename, Character *player, Object* objectTable[], NPC* npcTable[], Location* locationTable[]) {

    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Erreur d'ouverture du fichier de sauvegarde\n");
        return;
    }

    char line[256];
    enum Section { NONE, OBJETS, PNJS, LIEUX, JOUEUR } section = NONE; // Définit la section actuelle

    while (fgets(line, sizeof(line), file)) {
        // Détection de la section
        if (strncmp(line, "OBJETS", 6) == 0) {
            section = OBJETS;
            continue; // Passage a la ligne suivante
        } else if (strncmp(line, "PNJS", 4) == 0) {
            section = PNJS;
            continue;
        } else if (strncmp(line, "LIEUX", 5) == 0) {
            section = LIEUX;
            continue;
        } else if (strncmp(line, "JOUEUR", 6) == 0) {
            section = JOUEUR;
            continue;
        }

        // Lecture selon la section
        if (section == OBJETS) {
            char name[MAX_NAME_LENGTH], description[MAX_DESCRIPTION_LENGTH];
            int value;
            char valuechar[MAX_NAME_LENGTH];

            if (sscanf(line, "Nom: %[^\n]", name) == 1) {
                fgets(line, sizeof(line), file);
                sscanf(line, "Description: %[^\n]", description);

                fgets(line, sizeof(line), file);
                line[strcspn(line, "\n")] = '\0';
                sscanf(line, "Valeur: %d", &value);

                // Création et ajout de l'objet
                Object *obj = malloc(sizeof(Object));
                strncpy(obj->name, name, MAX_NAME_LENGTH);
                strncpy(obj->description, description, MAX_DESCRIPTION_LENGTH);
                obj->value = value;



                objectinventoryAdd(obj,objectTable);
            }
        }
        // Lecture des PNJs
        else if (section == PNJS) {
            char name[MAX_NAME_LENGTH], dialogue[MAX_DIALOGUE_LENGTH], objectName[MAX_NAME_LENGTH], conditionName[MAX_NAME_LENGTH];
            int type, attack, health;

            if (sscanf(line, "Nom: %[^\n]", name) == 1) {
                NPC *npc = malloc(sizeof(NPC));
                strncpy(npc->name, name, MAX_NAME_LENGTH);

                fgets(line, sizeof(line), file);
                sscanf(line, "Dialogue: %[^\n]", npc->dialogue);

                fgets(line, sizeof(line), file);
                sscanf(line, "Type: %d", &npc->type);

                fgets(line, sizeof(line), file);
                sscanf(line, "Attaque: %d", &npc->attack);

                fgets(line, sizeof(line), file);
                sscanf(line, "Vie: %d", &npc->health);

                fgets(line, sizeof(line), file);
                sscanf(line, "Objet: %[^\n]", objectName);

                if (strcmp(objectName, "NULL") != 0) {
                    npc->object = objectByName(objectName, objectTable);
                } else {
                    npc->object = NULL;
                }


                fgets(line, sizeof(line), file);
                sscanf(line, "Condition: %[^\n]", conditionName);

                if (strcmp(conditionName, "NULL") != 0) {
                    npc->condition = objectByName(conditionName, objectTable);
                } else {
                    npc->condition = NULL;
                }
                // Ajout du PNJ dans la table
                for (int i = 0; i < MAX_NPC; i++) {
                    if (npcTable[i] == NULL) {
                        npcTable[i] = npc;
                        break;
                    }
                }
            }
        }
        // Lecture des lieux
        else if (section == LIEUX) {

            char name[MAX_NAME_LENGTH], preposition[MAX_PREPOSITION_LENGTH], description[150];
            char reachableList[512], conditionName[MAX_NAME_LENGTH], inventoryList[512], npcName[MAX_NAME_LENGTH];

            if (sscanf(line, "Nom: %[^\n]", name) == 1) {
                Location *loc = malloc(sizeof(Location));
                strncpy(loc->name, name, MAX_NAME_LENGTH);

                fgets(line, sizeof(line), file);
                sscanf(line, "Preposition: %[^\n]", loc->preposition);

                fgets(line, sizeof(line), file);
                sscanf(line, "Description: %[^\n]", loc->description);

                // Lieux atteignables
                fgets(line, sizeof(line), file);
                sscanf(line, "Atteignables: %[^\n]", reachableList);

                loc->lieux_atteignables = calloc(MAX_NEAR_LOCATIONS, sizeof(Location *));

                if (strcmp(reachableList, " NULL") != 0) {
                    char *token = strtok(reachableList, ",");
                    int idx = 0;
                    while (token != NULL && idx < MAX_NEAR_LOCATIONS) {
                        while (*token == ' ') token++; // skip space
                        Location *dest = locationByName(token, locationTable);
                        if (dest != NULL) {
                            locationReachable(loc, dest);
                        }
                        token = strtok(NULL, ",");
                        idx++;
                    }
                }


                // Condition
                fgets(line, sizeof(line), file);
                sscanf(line, "Condition: %[^\n]", conditionName);
                if (strcmp(conditionName, "NULL") != 0) {
                    loc->objectCondition = objectByName(conditionName, objectTable);
                } else {
                    loc->objectCondition = NULL;
                }

                // Inventaire
                fgets(line, sizeof(line), file);
                sscanf(line, "Inventaire: %[^\n]", inventoryList);

                for (int i = 0; i < MAX_OBJECT; i++) loc->locationInventory[i] = NULL;

                if (strcmp(inventoryList, " NULL") != 0) {
                    char *token = strtok(inventoryList, ",");
                    int idx = 0;
                    while (token != NULL && idx < MAX_OBJECT) {
                        while (*token == ' ') token++; // skip space
                        loc->locationInventory[idx] = objectByName(token, objectTable);
                        token = strtok(NULL, ",");
                        idx++;
                    }
                }

                // PNJ
                fgets(line, sizeof(line), file);
                sscanf(line, "NPC: %[^\n]", npcName);
                if (strcmp(npcName, "NULL") != 0) {
                    loc->npc = npcByName(npcName, npcTable);
                } else {
                    loc->npc = NULL;
                }

                for (int i = 0; i < MAX_LOCATION; i++) {
                    if (locationTable[i] == NULL) {
                        locationTable[i] = loc;
                        break;
                    }
                }
            }
        }
        // Lecture des informations du joueur
        else if (section == JOUEUR) {

            if (strncmp(line, "Nom:", 4) == 0) {
                sscanf(line, "Nom: %[^\n]", player->name);
            }
            else if (strncmp(line, "LieuActuel:", 11) == 0) {
                char locName[MAX_NAME_LENGTH];
                sscanf(line, "LieuActuel: %[^\n]", locName);
                if (strcmp(locName, "NULL") != 0) {
                    player->current_location = locationByName(locName, locationTable);
                } else {
                    player->current_location = NULL;
                }
            }
            else if (strncmp(line, "Inventaire:", 11) == 0) {
                char inventoryList[512];
                sscanf(line, "Inventaire: %[^\n]", inventoryList);

                for (int i = 0; i < MAX_OBJECT; i++) player->inventory[i] = NULL;

                if (strcmp(inventoryList, " NULL") != 0) {
                    char *token = strtok(inventoryList, ",");
                    int idx = 0;
                    while (token != NULL && idx < MAX_OBJECT) {
                        while (*token == ' ') token++;
                        player->inventory[idx] = objectByName(token, objectTable);
                        token = strtok(NULL, ",");
                        idx++;
                    }
                }
            }
        }
    }

    fclose(file);
} // Charge la partie depuis le fichier de sauvegarde

void gameLaunch(Character player) {
    char commande[150];
    char action[50];
    char input0[50];
    char input1[50];

    int running = 1;
    for (int i = 0; i<100;i++) {
        printf("\n");
    }

    printf("-----------------------------\n");
    printf(" BIENVENUE A LA CITE OUBLIEE\n");
    printf("-----------------------------\n\n");

    if (player.current_location != NULL) {
        characterMoveAdmin(&player, player.current_location); // On affiche l'emplacement de depart

        while (running) {

            if(player.health <= 0) {
                printf("Vous avez perdu!\n");
                running=0;
                return;
            }
            printf("Que voulez-vous faire ?\n(tapez 'aide' pour les options)\n");

            if (fgets(commande, sizeof(commande), stdin) != NULL) {
                commande[strcspn(commande, "\n")] = '\0';

                if (sscanf(commande, "%s %s", action, input0) == 2) {
                    // Action : aller
                    if (strcmp(action, "aller") == 0) {
                        if(strcmp(input0,"?")==0) {
                            characterReachables(player);
                        }
                        else {
                            characterMove( &player, locationByName(input0,player.current_location->lieux_atteignables));
                        }

                    }
                    // Commande : echanger
                    else if (strcmp(action, "echanger") == 0) {

                        if(strcmp(input0,player.current_location->npc->name)==0) {
                            printf("%s\n",player.current_location->npc->dialogue);

                            NPC *npc = player.current_location->npc;
                            // Echange d'objet avec pnj
                            if (npc->object != NULL) {
                                if(npc->condition == NULL) {
                                    objectinventoryAdd(npc->object,player.inventory);
                                    printf("%s vous a donne : %s\n",npc->name,npc->object->name);
                                    npc->object = NULL;

                                }
                                else if (npc->condition != NULL) {
                                    printf("%s vous demande : %s pour : %s\n",npc->name,npc->condition->name,npc->object->name);

                                    Object *search = objectByName(npc->condition->name,player.inventory);

                                    if (search != NULL) {
                                        printf("Vous avez : %s, lui echanger ?\nNon {0} Oui {1}\n",npc->condition->name);
                                        int choice = 2;
                                        while (choice != 0 && choice != 1) {
                                            choice = numberVerification();
                                        }

                                        if (choice == 0) {
                                            printf("Vous refusez l'echange.\n");
                                        }
                                        else {
                                            objectinventoryAdd(npc->object,player.inventory);
                                            printf("%s vous a donne : %s\n",npc->name,npc->object->name);
                                            npc->object = NULL;
                                            objectinventoryRemove(player.current_location->npc->condition,player.inventory);
                                        }

                                    }

                                    else {
                                        printf("Vous n'avez pas :%s\n",player.current_location->npc->condition->name);
                                    }
                                }

                            }
                            else {
                                printf("%s n'a rien a te donner / echanger\n",npc->name);
                            }
                        }
                        else {
                            printf("Personne ne s'appelle %s cela ici\n",input0);
                        }

                    }


                    // Commande : examiner
                    else if (strcmp(action, "examiner") == 0) {
                        objectExamine(&player, player.current_location, input0);
                    }
                    // Commande : prendre
                    else if (strcmp(action, "prendre") == 0) {
                        Object *obj = objectByName(input0, player.current_location->locationInventory);
                        if (obj) {
                            objectinventorySwitch(obj, player.current_location->locationInventory, player.inventory);
                            printf("%s est dans votre inventaire\n", obj->name);
                        } else {
                            printf("Objet non trouve ici.\n");
                        }
                    }
                    // Commande : lacher
                    else if (strcmp(action, "lacher") == 0) {
                        Object *obj = objectByName(input0, player.inventory);
                        if (obj) {
                            objectinventorySwitch(obj, player.inventory, player.current_location->locationInventory);
                            printf("Vous avez lache : %s\n", obj->name);
                        } else {
                            printf("Vous n'avez pas cet objet.\n");
                        }
                    }
                    // Commande : parler
                    else if (strcmp(action, "parler") == 0) {
                        NPC *npc = player.current_location->npc;
                        if (npc != NULL && strcmp(npc->name, input0) == 0) {
                            printf("%s dit : \" %s \"\n", npc->name, npc->dialogue);
                        } else {
                            printf("Il n'y a pas de personnage nomme %s ici.\n", input0);
                        }
                    }

                    else {
                        printf("Commande inconnue : %s\n", action);
                    }
                }
                // Commande : inventaire
                else if (strcmp(commande, "inventaire") == 0) {
                    objectTableDisplay(player.inventory);
                }
                // Commande : etat
                else if (strcmp(commande, "etat") == 0) {
                    afficherEtat(&player);
                }
                // Commande : carte
                else if (strcmp(commande,"carte") == 0) {
                    printf("\n");
                    for(int i = 0; i < MAP_COL; i++) {
                        for(int j = 0; j < MAP_ROW; j++) {
                            printf("%c",player.map[i][j]);
                        }
                        printf("\n");
                    }
                    printf("\n");
                }

                // Commande : aide
                else if (strcmp(commande, "aide") == 0) {
                    printf("\nCommandes disponibles :\n");
                    printf(" - aller <lieu>\n");
                    printf(" - examiner <objet>\n");
                    printf(" - prendre <objet>\n");
                    printf(" - lacher <objet>\n");
                    printf(" - echanger <npc>\n");
                    printf(" - etat (affiche votre etat actuel)\n");
                    printf(" - carte (affiche la carte)\n");
                    printf(" - quitter (sort du jeu)\n");
                }
                // Commande : quitter
                else if (strcmp(commande, "quitter") == 0) {
                    printf("Merci d'avoir joue !\n");
                    running = 0;
                }

                else {
                    printf("Commande inconnue. Tapez 'aide pour voir les options.\n");
                }
            }
        }
    }
    else {
        printf("%s est tombe dans le vide !",player.name);

    }
} // Fonction qui prend en entrée le personnage et qui paramètre son gameplay

int main() {

    printf("-----------------------------\n");
    printf(" BIENVENUE A LA CITE OUBLIEE\n");
    printf("-----------------------------\n\n");


    char commande[250];
    char action[50];
    char input0[50];
    char input1[50];
    char input2[50];
    char input3[50];




        // Creation du joueur
        Character *player = characterCreationInput();
        if (!player) return EXIT_FAILURE; // Si erreur d'allocation

        // Creation de la liste de creation des objets (inventaire creatif) et mise a NULL
        Object* objectTable[MAX_OBJECT];
        for (int i = 0; i < MAX_OBJECT; i++) {
            objectTable[i] = NULL;
        }

        // Creation de la liste de creation des locations (inventaire creatif) et mise a NULL
        Location* locationTable [MAX_LOCATION];
        for (int i = 0; i < MAX_LOCATION; i++) {
            locationTable[i] = NULL;
        }

        // Creation de la liste de creation des npc (inventaire creatif) et mise a NULL
        NPC* npcTable [MAX_NPC];
        for (int i = 0; i < MAX_NPC; i++) {
            npcTable[i] = NULL;
        }

        // Boucle de jeux admin
        int running = 1;
        while (running) {

            printf("Entrez une commande :\n");

            // Input de l'utilisateur
            if (fgets(commande, sizeof(commande), stdin) != NULL) {
                commande[strcspn(commande, "\n")] = '\0';  // Supprime le \n

                // Commande : quitter
                if (strcmp(commande, "quitter") == 0) {
                    printf("Au revoir !\n");
                    running = 0;
                }

                // Commande : inventaire
                else if (strcmp(commande, "inventaire") == 0) {
                    objectTableDisplay(player->inventory);
                }

                // Commande : jouer
                else if (strcmp(commande,"jouer") == 0) {
                    running=0;
                    gameLaunch(*player);
                }

                // Commande : aide
                else if (strcmp(commande,"aide") == 0) {

                    printf("Voici les commandes disponibles :\n\n");

                    // Commandes simples (1 mot)
                    printf("quitter : quitter le jeu\n");
                    printf("inventaire : afficher votre inventaire\n");
                    printf("jouer : lancer le jeu\n");
                    printf("aide : afficher cette aide\n");
                    printf("carte : afficher la carte actuelle\n");

                    // Commandes à 2 mots
                    printf("aller <lieu> : deplacer le joueur vers un lieu accessible\n");
                    printf("admin.aller <lieu> : se teleporter a un lieu (admin)\n");
                    printf("prendre <objet> : prendre un objet present sur le lieu actuel\n");
                    printf("lacher <objet> : deposer un objet de l’inventaire dans le lieu actuel\n");

                    printf("objet + : creer un objet\n");
                    printf("objet afficher : afficher tous les objets\n");

                    printf("lieu + : creer un lieu\n");
                    printf("lieu afficher : afficher tous les lieux\n");

                    printf("npc + : creer un personnage (NPC)\n");
                    printf("npc afficher : afficher tous les NPCs\n");

                    // Commandes à 3 mots
                    printf("inventaire + <objet> : ajouter un objet a l’inventaire\n");
                    printf("inventaire - <objet> : retirer un objet de l’inventaire\n");

                    printf("lieu - <lieu> : supprimer un lieu\n");
                    printf("objet - <objet> : supprimer un objet\n");
                    printf("npc - <npc> : supprimer un NPC\n");

                    // Commandes à 4 mots
                    printf("lieu lier <lieu1> <lieu2> : relier deux lieux entre eux\n");
                    printf("carte - <x> <y> : supprimer une case de la carte\n");

                    // Commandes à 5 mots
                    printf("carte + <x> <y> <caractere> : modifier une case de la carte\n");

                    printf("lieu objet + <objet> <lieu> : ajouter un objet a un lieu\n");
                    printf("lieu objet - <objet> <lieu> : retirer un objet d’un lieu\n");

                    printf("lieu npc + <npc> <lieu> : affecter un NPC a un lieu\n");
                    printf("lieu npc - <npc> <lieu> : retirer le NPC d’un lieu\n");

                    printf("lieu condition + <objet> <lieu> : definir un objet comme condition d'acces\n");
                    printf("lieu condition - <objet> <lieu> : retirer la condition d’acces\n");

                    printf("npc objet + <objet> <npc> : donner un objet a un NPC\n");
                    printf("npc objet - <objet> <npc> : retirer un objet d’un NPC\n");

                    printf("npc condition + <objet> <npc> : definir une condition d’interaction pour un NPC\n");
                    printf("npc condition - <objet> <npc> : retirer la condition d’un NPC\n");

                    printf("\nUtilisez ces commandes pour gerer les objets, lieux, personnages et la carte.\n");
                }

                // Commande : carte
                else if (strcmp(commande,"carte") == 0) {
                    printf("\n");
                    for(int i = 0; i < MAP_COL; i++) {
                        for(int j = 0; j < MAP_ROW; j++) {
                            printf("%c",player->map[i][j]);
                        }
                        printf("\n");
                    }
                    printf("\n");
                }


                else if (sscanf(commande, "%s %s %s %s %s", action, input0, input1, input2, input3) == 5) {

                    // Ajout lieu sur la carte : carte + {x} {y} {initiale}
                    if (strcmp(action, "carte") == 0) {
                        if(strcmp(input0,"+") == 0) {
                            const int row = atoi(input1);
                            const int col = atoi(input2);

                            if(row >=0 && row < MAP_ROW) {
                                if(col >= 0 && col < MAP_COL) {

                                    player->map[col][row] = input3[0];


                                    printf("Carte modifiee avec succes,\nEntrez la commande carte pour appercevoir le changement.\n\n");
                                }
                                else {
                                    printf("Colonne hors de portee\n");
                                }

                            }
                            else {
                                printf("Ligne hors de portee\n");
                            }
                        }
                        else {
                            printf("Erreur de commande : carte");
                        }

                    }
                    // lieu objet { + || - } {objet} {lieu}
                    // lieu npc { + || - } {npc} {lieu}
                    // lieu condition { + || - } {objet} {lieu}
                    else if(strcmp(action, "lieu") == 0) {
                        Location *location = locationByName(input3, locationTable);
                        if (location != NULL) {
                            if(strcmp(input0,"objet") == 0){
                                Object *object = objectByName(input2, objectTable);

                                if (object != NULL) {

                                    if(strcmp(input1,"+") == 0) {
                                        objectinventoryAdd(object, location->locationInventory);
                                        printf("%s --> %s\n", object->name, location->name);

                                    }
                                    else if(strcmp(input1,"-") == 0) {
                                        objectinventoryRemove(object, location->locationInventory);
                                        printf("%s -/> %s\n", object->name, location->name);
                                    }
                                    else {
                                        printf("Erreur de signe\n");
                                    }

                                }

                                else {
                                    printf("Objet non trouve\n");
                                }
                            }
                            // lieu npc { + || - } {npc} {lieu}
                            else if(strcmp(input0,"npc") == 0) {
                                NPC *npc = npcByName(input2, npcTable);
                                if (npc != NULL) {
                                    if(strcmp(input1,"+") == 0) {
                                        location->npc = npc;
                                        printf("%s --> %s\n", npc->name, location->name);
                                    }
                                    else if(strcmp(input1,"-") == 0) {
                                        location->npc = NULL;
                                        printf("%s -/> %s\n", npc->name, location->name);
                                    }
                                    else {
                                        printf("Erreur de signe\n");
                                    }

                                }
                                else {
                                    printf("NPC non trouve\n");
                                }
                            }
                            //lieu condition { + || - } {objet} {lieu}
                            else if(strcmp(input0,"condition") == 0) {
                                Object *object = objectByName(input2, objectTable);

                                if (object != NULL) {
                                    if(strcmp(input1,"+") == 0) {
                                        location->objectCondition = object;
                                        printf("%s --> %s\n",object->name,location->name);
                                    }
                                    else if(strcmp(input1,"-") == 0) {
                                        location->objectCondition = NULL;
                                        printf("%s -/> %s\n",object->name,location->name);
                                    }
                                    else {
                                        printf("Erreur de signe\n");
                                    }

                                }
                                else {
                                    printf("Objet non trouve\n");
                                }

                            }
                            else {
                                printf("Erreur commande : lieu\n");
                            }
                        }
                        else {
                            printf("Lieu non trouve\n");
                        }
                    }
                    // npc objet { + || - } {objet} {npc}
                    // npc condition { + || - } {objet} {npc}
                    else if (strcmp(action, "npc") == 0) {
                        NPC *npc = npcByName(input3,npcTable);

                        if (npc != NULL) {

                            Object *object = objectByName(input2,objectTable);

                            if (object != NULL) {
                                if(strcmp(input0,"objet") == 0) {
                                    if(strcmp(input1,"+") == 0) {
                                        npc->object = object;
                                        printf("%s --> %s\n",npc->object->name,npc->name);
                                    }
                                    else if(strcmp(input1,"-") == 0) {
                                        printf("%s -/> %s\n",npc->object->name,npc->name);
                                        npc->object = NULL;
                                    }
                                    else {
                                        printf("Erreur de signe\n");
                                    }

                                }

                                else if(strcmp(input0,"condition") == 0) {
                                    if(strcmp(input1,"+") == 0) {
                                        npc->condition = object;
                                        printf("%s --> %s\n",npc->condition->name,npc->name);
                                    }
                                    else if(strcmp(input1,"-") == 0) {
                                        printf("%s -/> %s\n",npc->condition->name,npc->name);
                                        npc->condition = NULL;
                                    }
                                    else {
                                        printf("Erreur de signe\n");
                                    }

                                }
                                else {
                                    printf("Erreur de commande : npc\n");
                                }
                            }

                        }
                        else {
                            printf("NPC non trouve\n");
                        }
                    }
                    else {
                        printf("Commande inconcomplète ou non reconnue\n");
                    }
                }

                // Modifier carte : carte - {x} {y}
                else if (sscanf(commande, "%s %s %s %s", action, input0, input1, input2) == 4) {

                    if (strcmp(action, "carte") == 0) {
                        if(strcmp(input0,"-") == 0) {
                            const int row = atoi(input1);
                            const int col = atoi(input2);

                            if(row >=0 && row < MAP_ROW) {
                                if(col >= 0 && col < MAP_COL) {

                                    player->map[col][row] = '~';

                                    printf("Carte modifiee avec succes,\nEntrez la commande carte pour appercevoir le changement.\n\n");
                                }
                                else {
                                    printf("Colonne hors de portee\n");
                                }

                            }
                            else {
                                printf("Ligne hors de portee\n");
                            }
                        }
                        else {
                            printf("Erreur : impossible de modifier la carte\n");
                        }
                    }

                    // lieu lier {lieu1} {lieu2}
                    else if (strcmp(action, "lieu") == 0) {

                        if(strcmp(input0,"lier") == 0) {
                            Location *location0 = locationByName(input1,locationTable);
                            Location *location1 = locationByName(input2,locationTable);

                            if (location0 != NULL && location1 != NULL && location0 != location1) {
                                locationReachable(location0,location1);
                                printf("%s <--> %s\n",location0->name,location1->name);
                            }
                            else {
                                printf("Lieu(x) non trouve(s) ou egaux\n");
                            }
                        }

                        else {
                            printf("Erreur de commande : lieu\n");
                        }
                    }

                    else {
                        printf("Commande non reconnue\n");
                    }


                }

                // inventaire { + || - } {objet}
                else if (sscanf(commande, "%s %s %s", action, input0, input1) == 3) {

                    if (strcmp(action, "inventaire") == 0) {
                        Object *object = objectByName(input1, objectTable);

                        if (object != NULL) {
                            if (strcmp(input0, "+") == 0) {
                                objectinventoryAdd(object, player->inventory);
                                printf("%s --> inventaire\n", object->name);
                            }

                            else if(strcmp(input0, "-") == 0){
                                objectinventoryRemove(object, player->inventory);
                                printf("%s -/> inventaire\n", object->name);
                            }

                            else {
                                printf("Erreur de signe'\n");
                            }
                        }
                        else {
                            printf("Objet non trouve, faites 'objet +'");
                        }
                    }
                    // supprimer lieu : lieu - {nom_du_lieu}
                    else if (strcmp(action, "lieu") == 0) {
                        if(strcmp(input0,"-") == 0) {
                            Location * lieu = locationByName(input1,locationTable);
                            if (lieu != NULL) {
                                printf("%s supprime\n",lieu->name);
                                locationRemove(lieu,locationTable);
                            }
                            else {
                                printf("Lieu non trouve\n");
                            }
                        }
                        else {
                            printf("Erreur de signe'\n");
                        }

                    }
                    //supprimer objet : objet - {nom_de_l'objet}
                    else if (strcmp(action, "objet") == 0) {
                        if(strcmp(input0,"-") == 0) {
                            Object * object = objectByName(input1,objectTable);

                            if (object != NULL) {
                                printf("%s supprime\n",object->name);
                                objectinventoryRemove(object,objectTable);
                            }
                            else {
                                printf("Objet non trouve\n");
                            }
                        }
                        else {
                            printf("Erreur de signe'\n");
                        }

                    }
                    //supprimer npc : npc - {nom_du_npc}
                    else if (strcmp(action, "npc") == 0) {
                        if(strcmp(input0,"-") == 0) {
                            NPC * npc = npcByName(input1,npcTable);

                            if (npc != NULL) {
                                printf("%s supprime\n",npc->name);
                                npcRemove(npc,npcTable);
                            }
                            else {
                                printf("NPC non trouve\n");
                            }
                        }
                        else {
                            printf("Erreur de signe'\n");
                        }

                    }
                    else {
                        printf("Erreur de commande : npc\n");
                    }
                }

                // Commande à 2 mots
                else if (sscanf(commande, "%s %s", action, input0) == 2) {
                    // Commande : lacher {objet}
                    if (strcmp(action, "lacher") == 0) {
                        Object *object = objectByName(input0, objectTable);
                        printf("Tu laches : %s\n", input0);
                        objectinventorySwitch(object, player->inventory, player->current_location->locationInventory);
                    }
                    // Commande : prendre {objet}
                    else if (strcmp(action, "prendre") == 0) {
                        Object *object = objectByName(input0, objectTable);
                        printf("Tu prends : %s\n", input0);
                        objectinventorySwitch(object, player->current_location->locationInventory, player->inventory);
                    }
                    // Commande : aller {lieu}
                    else if (strcmp(action, "aller") == 0) {
                        characterMove(player, locationByName(input0, locationTable));
                    }
                    // se téléporter : admin.aller {lieu}
                    else if (strcmp(action, "admin.aller") == 0) {
                        characterMoveAdmin(player, locationByName(input0, locationTable));
                    }
                    // ajouter objet : objet +
                    // afficher objet : object afficher
                    else if (strcmp(action, "objet") == 0) {
                        if (strcmp(input0, "+") == 0) {
                            objectinventoryAddInput(objectTable);
                        }
                        else if (strcmp(input0, "afficher") == 0) {
                            objectTableDisplay(objectTable);
                        }
                        else {
                            printf("Erreur de commande : objet\n");
                        }
                    }
                    // ajouter lieu : lieu +
                    // afficher lieu : lieu afficher
                    else if (strcmp(action, "lieu") == 0) {
                        if (strcmp(input0, "+") == 0) {
                            locationAddInput(locationTable);
                        }
                        else if (strcmp(input0, "afficher") == 0) {
                            locationTableDisplay(locationTable);
                        }
                        else {
                            printf("Erreur de commande : lieu\n");
                        }
                    }
                    // ajouter npc : npc +
                    // afficher npc : npc afficher
                    else if (strcmp(action, "npc") == 0) {
                        if (strcmp(input0, "+") == 0) {
                            npcAddInput(npcTable);
                        }
                        else if (strcmp(input0, "afficher") == 0) {
                            npcTableDisplay(npcTable);
                        }
                        else {
                            printf("Erreur de commande : npc\n");
                        }
                    }

                    // Commande : sauvegarder
                    else if (strcmp(action, "sauvegarder") == 0) {
                        gameSave(input0, player, objectTable, npcTable, locationTable);
                    }
                    // Commande : charger
                    else if (strcmp(action, "charger") == 0) {
                        gameLoad(input0, player, objectTable, npcTable, locationTable);
                    }

                    else {
                        printf("Commande non reconnue : %s\n", action); // Commande a 2 mots non reconnue
                    }
                }
                else {
                    printf("Commande incomplete ou non reconnue.\n");// Commande incomplete ou inconnue
                }
            }
            else {
                printf("Commande incomplete ou non reconnue.\n");// Commande incomplete ou inconnue
            }

        }

        for (int i = 0; i < MAX_NPC; i++) {
            free(npcTable[i]);
        }
        for (int i = 0; i < MAX_OBJECT; i++) {
            free(objectTable[i]);
        }
        for (int i = 0; i < MAX_LOCATION; i++) {
            free(locationTable[i]);
        }

        free(player);


    return 0;
}