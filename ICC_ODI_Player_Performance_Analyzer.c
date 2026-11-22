#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEAM_COUNT 10
#define PLAYERS_PER_TEAM 50
#define ROLE_COUNT 3
#define INITIAL_PLAYERS 20

struct Player{
    int playerId;
    char name[51];
    char teamName[51];
    int role;
    int totalRuns;
    float battingAverage;
    float strikeRate;
    int wickets;
    float economyRate;
    float performanceIndex;
    struct Player *next;
    struct Player *roleNext;
};

struct Team{
    int teamId;
    char name[51];
    int totalPlayers;
    float averageBattingStrikeRate;
    struct Player *playerHead;
    struct Player *roleHead[ROLE_COUNT];
    int roleCount[ROLE_COUNT];
};

struct HeapNode{
    struct Player *player;
    int teamIndex;
};

struct Team *teams[TEAM_COUNT];

char teamNames[TEAM_COUNT][51] = {
    "India",
    "Australia",
    "England",
    "Pakistan",
    "New Zealand",
    "South Africa",
    "Bangladesh",
    "Sri Lanka",
    "Afghanistan",
    "West Indies"
};

char *roleNames[ROLE_COUNT] = {
    "Batsman",
    "Bowler",
    "All-Rounder"
};

float calculatePerformanceIndex(struct Player *player){
    if(player->role == 1){
        return (player->battingAverage * player->strikeRate) / 100.0f;
    }
    else if(player->role == 2){
        return (player->wickets * 2) + (100 - player->economyRate);
    }
    else{
        return ((player->battingAverage * player->strikeRate) / 100.0f) + (player->wickets * 2);
    }
}

void updateTeamStatistics(struct Team *team){
    struct Player *current = team->playerHead;
    float totalStrikeRate = 0;
    int count = 0;
    while(current != NULL){
        if(current->role == 1 || current->role == 3){
            totalStrikeRate += current->strikeRate;
            count++;
        }
        current = current->next;
    }
    if(count > 0){
        team->averageBattingStrikeRate = totalStrikeRate / count;
    }
    else{
        team->averageBattingStrikeRate = 0;
    }
}

int comparePlayers(struct Player *a, struct Player *b){
    if(a->performanceIndex > b->performanceIndex){
        return 1;
    }
    if(a->performanceIndex < b->performanceIndex){
        return 0;
    }
    return a->playerId < b->playerId;
}

void insertPlayer(struct Team *team, struct Player *player){
    player->next = team->playerHead;
    team->playerHead = player;
    int index = player->role - 1;
    struct Player **current = &team->roleHead[index];
    while(*current != NULL && comparePlayers(*current, player)){
        current = &((*current)->roleNext);
    }
    player->roleNext = *current;
    *current = player;
    team->totalPlayers++;
    team->roleCount[index]++;
    updateTeamStatistics(team);
}

void initializeTeams(){
    for(int i = 0; i < TEAM_COUNT; i++){
        teams[i] = malloc(sizeof(struct Team));
        if(teams[i] == NULL){
            printf("Memory allocation failed.\n");
            exit(1);
        }
        teams[i]->teamId = i + 1;
        strcpy(teams[i]->name, teamNames[i]);
        teams[i]->totalPlayers = 0;
        teams[i]->averageBattingStrikeRate = 0;
        teams[i]->playerHead = NULL;
        for(int j = 0; j < ROLE_COUNT; j++){
            teams[i]->roleHead[j] = NULL;
            teams[i]->roleCount[j] = 0;
        }
    }
}

void initializePlayers(){
    for(int i = 0; i < TEAM_COUNT; i++){
        for(int j = 0; j < INITIAL_PLAYERS; j++){
            struct Player *player = malloc(sizeof(struct Player));
            if(player == NULL){
                printf("Memory allocation failed.\n");
                exit(1);
            }
            player->playerId = (i * INITIAL_PLAYERS) + j + 1;
            snprintf(player->name, sizeof(player->name), "%s_Player_%02d", teamNames[i], j + 1);
            strcpy(player->teamName, teamNames[i]);
            player->role = (j % 3) + 1;
            player->totalRuns = 500 + (j * 120) + (i * 50);
            player->battingAverage = 15.0f + (j * 1.8f) + (i * 0.5f);
            player->strikeRate = 65.0f + (j * 2.5f) + (i * 0.7f);
            player->wickets = 5 + (j * 4) + i;
            player->economyRate = 3.5f + (j % 5) * 0.3f;
            player->performanceIndex = calculatePerformanceIndex(player);
            player->next = NULL;
            player->roleNext = NULL;
            insertPlayer(teams[i], player);
        }
    }
}

struct Team *searchTeam(int teamId){
    int low = 0;
    int high = TEAM_COUNT - 1;
    while(low <= high){
        int mid = low + (high - low) / 2;
        if(teams[mid]->teamId == teamId){
            return teams[mid];
        }
        else if(teams[mid]->teamId < teamId){
            low = mid + 1;
        }
        else{
            high = mid - 1;
        }
    }
    return NULL;
}

int playerIdExists(int playerId){
    for(int i = 0; i < TEAM_COUNT; i++){
        struct Player *current = teams[i]->playerHead;
        while(current != NULL){
            if(current->playerId == playerId){
                return 1;
            }
            current = current->next;
        }
    }
    return 0;
}

int readInteger(const char *message, int min, int max){
    int value;
    char buffer[100];
    while(1){
        printf("%s", message);
        if(fgets(buffer, sizeof(buffer), stdin) == NULL){
            exit(1);
        }
        if(sscanf(buffer, "%d", &value) == 1 && value >= min && value <= max){
            return value;
        }
        printf("Enter a valid number between %d and %d.\n", min, max);
    }
}

float readFloat(const char *message, float min, float max){
    float value;
    char buffer[100];
    while(1){
        printf("%s", message);
        if(fgets(buffer, sizeof(buffer), stdin) == NULL){
            exit(1);
        }
        if(sscanf(buffer, "%f", &value) == 1 && value >= min && value <= max){
            return value;
        }
        printf("Enter a valid number between %.2f and %.2f.\n", min, max);
    }
}

void readName(char *name, int size){
    while(1){
        printf("Name: ");
        if(fgets(name, size, stdin) == NULL){
            exit(1);
        }
        name[strcspn(name, "\n")] = '\0';
        if(strlen(name) > 0){
            return;
        }
        printf("Name cannot be empty.\n");
    }
}

void displayPlayer(struct Player *player, int showTeam){
    if(showTeam){
        printf("%-5d %-25s %-17s %-13s %-7d %-7.1f %-7.1f %-6d %-6.1f %-10.2f\n",
               player->playerId, player->name, player->teamName, roleNames[player->role - 1],
               player->totalRuns, player->battingAverage, player->strikeRate, player->wickets,
               player->economyRate, player->performanceIndex);
    }
    else{
        printf("%-5d %-25s %-13s %-7d %-7.1f %-7.1f %-6d %-6.1f %-10.2f\n",
               player->playerId, player->name, roleNames[player->role - 1],
               player->totalRuns, player->battingAverage, player->strikeRate, player->wickets,
               player->economyRate, player->performanceIndex);
    }
}

void displayHeader(int showTeam){
    if(showTeam){
        printf("%-5s %-25s %-17s %-13s %-7s %-7s %-7s %-6s %-6s %-10s\n",
               "ID", "Name", "Team", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");
    }
    else{
        printf("%-5s %-25s %-13s %-7s %-7s %-7s %-6s %-6s %-10s\n",
               "ID", "Name", "Role", "Runs", "Avg", "SR", "Wkts", "ER", "Perf.Index");
    }
}

void addPlayer(){
    int teamId = readInteger("Enter Team ID to add player: ", 1, 1000);
    struct Team *team = searchTeam(teamId);
    if(team == NULL){
        printf("Team not found.\n");
        return;
    }
    if(team->totalPlayers >= PLAYERS_PER_TEAM){
        printf("Team has reached its maximum player limit.\n");
        return;
    }
    struct Player *player = malloc(sizeof(struct Player));
    if(player == NULL){
        printf("Memory allocation failed.\n");
        return;
    }
    printf("Enter Player Details:\n");
    player->playerId = readInteger("Player ID: ", 1, 1000);
    if(playerIdExists(player->playerId)){
        printf("Player ID already exists.\n");
        free(player);
        return;
    }
    readName(player->name, sizeof(player->name));
    player->role = readInteger("Role (1-Batsman, 2-Bowler, 3-All-rounder): ", 1, 3);
    player->totalRuns = readInteger("Total Runs: ", 0, 100000);
    player->battingAverage = readFloat("Batting Average: ", 0, 1000);
    player->strikeRate = readFloat("Strike Rate: ", 0, 1000);
    player->wickets = readInteger("Wickets: ", 0, 10000);
    player->economyRate = readFloat("Economy Rate: ", 0, 100);
    strcpy(player->teamName, team->name);
    player->performanceIndex = calculatePerformanceIndex(player);
    player->next = NULL;
    player->roleNext = NULL;
    insertPlayer(team, player);
    printf("Player added successfully to Team %s!\n", team->name);
    printf("Performance Index: %.2f\n", player->performanceIndex);
}

void displayTeamPlayers(){
    int teamId = readInteger("Enter Team ID: ", 1, 1000);
    struct Team *team = searchTeam(teamId);
    if(team == NULL){
        printf("Team not found.\n");
        return;
    }
    printf("\nPlayers of Team %s:\n", team->name);
    displayHeader(0);
    struct Player *current = team->playerHead;
    while(current != NULL){
        displayPlayer(current, 0);
        current = current->next;
    }
    printf("Total Players: %d\n", team->totalPlayers);
    printf("Average Batting Strike Rate: %.2f\n", team->averageBattingStrikeRate);
}

void mergeTeams(struct Team *arr[], struct Team *temp[], int left, int mid, int right){
    int i = left;
    int j = mid + 1;
    int k = left;
    while(i <= mid && j <= right){
        if(arr[i]->averageBattingStrikeRate >= arr[j]->averageBattingStrikeRate){
            temp[k++] = arr[i++];
        }
        else{
            temp[k++] = arr[j++];
        }
    }
    while(i <= mid){
        temp[k++] = arr[i++];
    }
    while(j <= right){
        temp[k++] = arr[j++];
    }
    for(i = left; i <= right; i++){
        arr[i] = temp[i];
    }
}

void mergeSortTeams(struct Team *arr[], struct Team *temp[], int left, int right){
    if(left < right){
        int mid = left + (right - left) / 2;
        mergeSortTeams(arr, temp, left, mid);
        mergeSortTeams(arr, temp, mid + 1, right);
        mergeTeams(arr, temp, left, mid, right);
    }
}

void displaySortedTeams(){
    struct Team *sorted[TEAM_COUNT];
    struct Team *temp[TEAM_COUNT];
    for(int i = 0; i < TEAM_COUNT; i++){
        sorted[i] = teams[i];
    }
    mergeSortTeams(sorted, temp, 0, TEAM_COUNT - 1);
    printf("\nTeams Sorted by Average Batting Strike Rate\n");
    printf("%-5s %-20s %-15s %-15s\n", "ID", "Team Name", "Avg Bat SR", "Total Players");
    for(int i = 0; i < TEAM_COUNT; i++){
        printf("%-5d %-20s %-15.2f %-15d\n",
               sorted[i]->teamId, sorted[i]->name,
               sorted[i]->averageBattingStrikeRate, sorted[i]->totalPlayers);
    }
}

void displayTopKPlayers(){
    int teamId = readInteger("Enter Team ID: ", 1, 1000);
    struct Team *team = searchTeam(teamId);
    if(team == NULL){
        printf("Team not found.\n");
        return;
    }
    int role = readInteger("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ", 1, 3);
    int count = team->roleCount[role - 1];
    if(count == 0){
        printf("No players found for this role.\n");
        return;
    }
    int k = readInteger("Enter number of players (K): ", 1, count);
    printf("\nTop %d %s of Team %s:\n", k, roleNames[role - 1], team->name);
    displayHeader(0);
    struct Player *current = team->roleHead[role - 1];
    for(int i = 0; i < k && current != NULL; i++){
        displayPlayer(current, 0);
        current = current->roleNext;
    }
}

void swapHeapNodes(struct HeapNode *a, struct HeapNode *b){
    struct HeapNode temp = *a;
    *a = *b;
    *b = temp;
}

void heapifyUp(struct HeapNode heap[], int index){
    while(index > 0){
        int parent = (index - 1) / 2;
        if(heap[parent].player->performanceIndex >= heap[index].player->performanceIndex){
            break;
        }
        swapHeapNodes(&heap[parent], &heap[index]);
        index = parent;
    }
}

void heapifyDown(struct HeapNode heap[], int size, int index){
    while(1){
        int largest = index;
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        if(left < size && heap[left].player->performanceIndex > heap[largest].player->performanceIndex){
            largest = left;
        }
        if(right < size && heap[right].player->performanceIndex > heap[largest].player->performanceIndex){
            largest = right;
        }
        if(largest == index){
            break;
        }
        swapHeapNodes(&heap[index], &heap[largest]);
        index = largest;
    }
}

void insertHeap(struct HeapNode heap[], int *size, struct Player *player, int teamIndex){
    if(player == NULL){
        return;
    }
    heap[*size].player = player;
    heap[*size].teamIndex = teamIndex;
    heapifyUp(heap, *size);
    (*size)++;
}

struct HeapNode extractMax(struct HeapNode heap[], int *size){
    struct HeapNode result = heap[0];
    heap[0] = heap[*size - 1];
    (*size)--;
    if(*size > 0){
        heapifyDown(heap, *size, 0);
    }
    return result;
}

void displayAllPlayersByRole(){
    int role = readInteger("Enter Role (1-Batsman, 2-Bowler, 3-All-rounder): ", 1, 3);
    struct HeapNode heap[TEAM_COUNT];
    int heapSize = 0;
    int total = 0;
    for(int i = 0; i < TEAM_COUNT; i++){
        struct Player *head = teams[i]->roleHead[role - 1];
        if(head != NULL){
            insertHeap(heap, &heapSize, head, i);
        }
        total += teams[i]->roleCount[role - 1];
    }
    if(total == 0){
        printf("No players found for this role.\n");
        return;
    }
    printf("\nAll %s Players Across All Teams:\n", roleNames[role - 1]);
    displayHeader(1);
    while(heapSize > 0){
        struct HeapNode node = extractMax(heap, &heapSize);
        displayPlayer(node.player, 1);
        struct Player *next = node.player->roleNext;
        if(next != NULL){
            insertHeap(heap, &heapSize, next, node.teamIndex);
        }
    }
    printf("Total Players Displayed: %d\n", total);
}

void displayMenu(){
    printf("          ICC ODI PLAYER PERFORMANCE ANALYZER\n");
    printf("1. Add Player to Team\n");
    printf("2. Display Players of a Specific Team\n");
    printf("3. Display Teams by Average Batting Strike Rate\n");
    printf("4. Display Top K Players of a Specific Team by Role\n");
    printf("5. Display All Players of a Specific Role Across All Teams\n");
    printf("6. Exit\n");
}

void freeMemory(){
    for(int i = 0; i < TEAM_COUNT; i++){
        struct Player *current = teams[i]->playerHead;
        while(current != NULL){
            struct Player *temp = current;
            current = current->next;
            free(temp);
        }
        free(teams[i]);
    }
}

int main(){
    initializeTeams();
    initializePlayers();
    int choice;
    printf("ICC ODI Player Performance Analyzer Initialized Successfully!\n");
    printf("Sample Data Loaded: %d Teams, %d Players\n", TEAM_COUNT, TEAM_COUNT * INITIAL_PLAYERS);
    while(1){
        displayMenu();
        choice = readInteger("Enter your choice: ", 1, 6);
        switch(choice){
            case 1:{
                addPlayer();
                break;
            }
            case 2:{
                displayTeamPlayers();
                break;
            }
            case 3:{
                displaySortedTeams();
                break;
            }
            case 4:{
                displayTopKPlayers();
                break;
            }
            case 5:{
                displayAllPlayersByRole();
                break;
            }
            case 6:{
                freeMemory();
                printf("Exiting ICC ODI Player Performance Analyzer. Goodbye!\n");
                return 0;
            }
        }
    }
}
