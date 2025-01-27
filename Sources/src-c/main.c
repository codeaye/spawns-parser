#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.c"

int main()
{
    FILE *spawn_file = fopen("../../data/Spawns.json", "r");
    fseek(spawn_file, 0, SEEK_END);
    long fsize = ftell(spawn_file);
    fseek(spawn_file, 0, SEEK_SET);

    char *spawn_data = malloc(fsize + 1);
    fread(spawn_data, fsize, 1, spawn_file);
    fclose(spawn_file);

    cJSON *spawn_json = cJSON_Parse(spawn_data);
    int spawn_count = cJSON_GetArraySize(spawn_json);
    printf("%d\n", spawn_count);

    cJSON *parsed_spawns = cJSON_CreateObject();
    int parsed_spawn_count = 0;
    for (int i = 0; i < spawn_count; i++)
    {
        cJSON *scene = cJSON_GetArrayItem(spawn_json, i);
        cJSON *spawns = cJSON_GetObjectItem(scene, "spawns");
        int spawn_count = cJSON_GetArraySize(spawns);
        for (int j = 0; j < spawn_count; j++)
        {
            cJSON *spawn = cJSON_GetArrayItem(spawns, j);
            cJSON *monsterId = cJSON_GetObjectItem(spawn, "monsterId");
            char *monsterId_str = malloc(sizeof(char) * 10);
            sprintf(monsterId_str, "%d", monsterId->valueint);
            cJSON *parsed_spawn = cJSON_GetObjectItem(parsed_spawns, monsterId_str);
            if (parsed_spawn == NULL)
            {
                cJSON_AddItemToObject(parsed_spawns, monsterId_str, cJSON_CreateArray());
                parsed_spawn = cJSON_GetObjectItem(parsed_spawns, monsterId_str);
            }
            cJSON *pos = cJSON_GetObjectItem(spawn, "pos");
            cJSON *x = cJSON_GetObjectItem(pos, "x");
            cJSON *y = cJSON_GetObjectItem(pos, "y");
            cJSON *z = cJSON_GetObjectItem(pos, "z");
            cJSON *parsed_spawn_pos = cJSON_CreateObject();
            cJSON_AddItemToObject(parsed_spawn_pos, "x", cJSON_CreateNumber(x->valuedouble));
            cJSON_AddItemToObject(parsed_spawn_pos, "y", cJSON_CreateNumber(y->valuedouble));
            cJSON_AddItemToObject(parsed_spawn_pos, "z", cJSON_CreateNumber(z->valuedouble));
            cJSON_AddItemToArray(parsed_spawn, parsed_spawn_pos);
            parsed_spawn_count++;
        }
    }

    FILE *parsed_spawn_file = fopen("../../parsed/parsedSpawns.json", "w");
    char *parsed_spawn_data = cJSON_Print(parsed_spawns);
    fwrite(parsed_spawn_data, strlen(parsed_spawn_data), 1, parsed_spawn_file);
    fclose(parsed_spawn_file);
}