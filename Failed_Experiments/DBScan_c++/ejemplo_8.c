#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <math.h>
#include <stdbool.h>
//   constantes para tarea 2 //
#define NUM_POINTS          (30U)     // tamaño de cluster = NUM_POINTS_2 / Minimo de puntos mínimo
#define NUM_POINTS_2        (30U)    // tamaño de array

//************** variables para tarea 2
int     contador_tarea_2 = 0;

int point_in[2]={0};
int data[NUM_POINTS_2][2]={0}; // Aquí debes inicializar los datos de entrenamiento
float eps = 2;
int minPts = 10;
int clusters[NUM_POINTS][NUM_POINTS_2][2] = {0};
int len_clusters[NUM_POINTS] = {0};
int no_clusters[NUM_POINTS_2][2] = {0};
int len_no_clusters = 0;

float eps_2 = 8;
int minPts_2 = 5;
int clusters_2[NUM_POINTS][NUM_POINTS_2][2] = {0};
int len_clusters_2[NUM_POINTS] = {0};
int no_clusters_2[NUM_POINTS_2][2] = {0};
int len_no_clusters_2 = 0;
int arritmia = 0;

bool flag_trained = false;      // indicar tiempo de entrenamiento par ala red
bool flag_dato_rdy = false;     // indica dato valido de entrada para la red


// funciones para red
float euclidean_distance(int point1[2], int point2[2]);
int get_neighbors(int data[NUM_POINTS_2][2], int point[2], float eps, int neighbors[NUM_POINTS_2][2]);
int expand_cluster(int data[NUM_POINTS_2][2], int point_index, int point[2], int neighbors[NUM_POINTS_2][2], int len_neighbors, float eps, int minPts, bool visited[NUM_POINTS_2], int cluster[NUM_POINTS_2][2]);
void dbscan(int data[NUM_POINTS_2][2], float eps, int minPts, int clusters[NUM_POINTS][NUM_POINTS_2][2], int len_clusters[NUM_POINTS_2], int no_clusters[NUM_POINTS_2][2], int *len_no_clusters);
int find_cluster(int point[2], int clusters[NUM_POINTS][NUM_POINTS_2][2], int len_clusters[NUM_POINTS_2]);

/*  Funciones principales      */


float euclidean_distance(int point1[2], int point2[2])
{
    return sqrt(pow(point1[0] - point2[0], 2) + pow(point1[1] - point2[1], 2));
}

int get_neighbors(int data[NUM_POINTS_2][2], int point[2], float eps, int neighbors[NUM_POINTS_2][2])
{
    int other_point[2] = {0};
    int index = 0;
    int i=0;
    for (i = 0; i < NUM_POINTS_2; i++)
    {
        other_point[0] = data[i][0];
        other_point[1] = data[i][1];
        if ((euclidean_distance(point, other_point) <= eps))
        {
            neighbors[index][0] = data[i][0];
            neighbors[index][1] = data[i][1];
            index++;
        }
    }
    return index;
}

int expand_cluster(int data[NUM_POINTS_2][2], int point_index, int point[2], int neighbors[NUM_POINTS_2][2], int len_neighbors, float eps, int minPts, bool visited[NUM_POINTS_2], int cluster[NUM_POINTS_2][2])
{
    int index = 0;
    cluster[index][0] = point[0];
    cluster[index][1] = point[1];
    index++;
    visited[point_index] = true;
    int i = 0;
    while (i < len_neighbors)
    {
        int neighbor[2] = {neighbors[i][0], neighbors[i][1]};
        i++;
        int neighbor_index = -1;
        int j = 0;
        for ( j = 0; j < NUM_POINTS_2; j++)
        {
            if (abs(data[j][0] - neighbor[0]) < 2 && abs(data[j][1] - neighbor[1]) < 2)
            {
                neighbor_index = j;
                break;
            }
        }
        if (neighbor_index != -1 && !visited[neighbor_index])
        {
            visited[neighbor_index] = true;
            int new_neighbors[NUM_POINTS_2][2] = {0};
            int len_new_neighbors = get_neighbors(data, neighbor, eps, new_neighbors);
            if (len_new_neighbors >= minPts)
            {
                int j =0;       /////////////////////twnwr cuidado con volver a poner j como variable
                for (j = 0; j < len_new_neighbors; j++)
                {
                    bool in_cluster = false;
                    int k=0;
                    for (k = 0; k < index; k++)
                    {
                        if (abs(cluster[k][0] - new_neighbors[j][0]) < 2 && abs(cluster[k][1] - new_neighbors[j][1]) < 2)
                        {
                            in_cluster = true;
                            break;
                        }
                    }
                    if (!in_cluster)
                    {
                        neighbors[len_neighbors][0] = new_neighbors[j][0];
                        neighbors[len_neighbors][1] = new_neighbors[j][1];
                        len_neighbors++;
                    }
                }
            }
            bool in_cluster = false;
            int j =0;           /////////////////////twnwr cuidado con volver a poner j como variable
            for (j = 0; j < index; j++)
            {
                if (abs(cluster[j][0] - neighbor[0]) < 2 && abs(cluster[j][1] - neighbor[1]) < 2)
                {
                    in_cluster = true;
                    break;
                }
            }
            if (!in_cluster)
            {
                cluster[index][0] = neighbor[0];
                cluster[index][1] = neighbor[1];
                index++;
            }
        }
    }
    int l=0;
    for (l = 0; l < index; l++)
    {
        int m =0;
        for (m = 0; m < NUM_POINTS_2; m++)
        {
            if (abs(data[m][0] - cluster[l][0]) < 2 && abs(data[m][1] - cluster[l][1]) < 2)
            {
                visited[m] = true;
            }
        }
    }
    return index;
}

void dbscan(int data[NUM_POINTS_2][2], float eps, int minPts, int clusters[NUM_POINTS][NUM_POINTS_2][2], int len_clusters[NUM_POINTS_2], int no_clusters[NUM_POINTS_2][2], int *len_no_clusters)
{
    bool visited[NUM_POINTS_2] = {false};
    int index=0;
    int i = 0;
    for (i = 0; i < NUM_POINTS_2; i++)
    {
        int point[2] = {data[i][0], data[i][1]};
        if (!visited[i])
        {
            visited[i] = true;
            int neighbors[NUM_POINTS_2][2] = {0};
            int len_neighbors = get_neighbors(data, point, eps, neighbors);
            if (len_neighbors >= minPts)
            {
                index = expand_cluster(data, i, point, neighbors, len_neighbors, eps, minPts, visited, clusters[*len_no_clusters]);
                len_clusters[*len_no_clusters] = index;
                (*len_no_clusters)++;
            }
            else
            {
                no_clusters[*len_no_clusters-index][0] = point[0];
                no_clusters[*len_no_clusters-index][1] = point[1];
                (*len_no_clusters)++;
            }
        }
    }
    *len_no_clusters=*len_no_clusters-index;
}


int find_cluster(int point[2], int clusters[NUM_POINTS][NUM_POINTS_2][2], int len_clusters[NUM_POINTS_2])
{
    int i = 0;
    for (i = 0; i < NUM_POINTS; i++)
    {
        if (len_clusters[i] > 0)
        {
            int j = 0;
            for (j = 0; j < len_clusters[i]; j++)
            {
                if (abs(clusters[i][j][0] - point[0]) < 2 && abs(clusters[i][j][1] - point[1]) < 2)
                {
                    return i;
                }
            }
        }
    }
    return -1;
}


int main()
{
    

    while (true)
    {
        char input[10];
        printf("Ingrese el punto a buscar (x y) o 'x' para volver a entrenar: ");
        scanf("%s", input);
        if (input[0] == 'x')
        {
            printf("Ingrese los nuevos datos de entrenamiento:\n");
            for (int i = 0; i < NUM_POINTS; i++)
            {
                printf("Punto %d (x y): ", i + 1);
                scanf("%f %f", &data[i][0], &data[i][1]);
            }
            dbscan(data, eps, minPts, clusters, len_clusters, no_clusters, &len_no_clusters);
            dbscan(no_clusters, eps_2, minPts_2, clusters_2, len_clusters_2, no_clusters_2, &len_no_clusters_2);
            flag_trained = true;
        }
        else if (flag_trained)
        {
            int point_to_search[2];
            point_to_search[0] = atof(input);
            scanf("%f", &point_to_search[1]);
            int cluster_index = find_cluster(point_to_search, clusters, len_clusters);
            if (cluster_index != -1)
            {
                printf("El punto (%.1f, %.1f) se encuentra en el cluster %d\n", point_to_search[0], point_to_search[1], cluster_index + 1);
            }
            else
            {
                printf("El punto (%.1f, %.1f) no se encuentra en ningún cluster\n", point_to_search[0], point_to_search[1]);
            }
        }
        else
        {
            printf("Debe entrenar el modelo antes de buscar un punto\n");
        }
    }

    return 0;
}
