#include <math.h>
#include <stdio.h>
#include <stdbool.h>

#define NUM_POINTS      200
#define NUM_POINTS_2    2000

float euclidean_distance(float point1[2], float point2[2])
{
    return sqrt(pow(point1[0] - point2[0], 2) + pow(point1[1] - point2[1], 2));
}

int get_neighbors(float data[NUM_POINTS][2], float point[2], float eps, float neighbors[NUM_POINTS][2])
{
    float other_point[2] = {0};
    int index = 0;
    int i=0;
    for (i = 0; i < NUM_POINTS; i++)
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

int expand_cluster(float data[NUM_POINTS][2], int point_index, float point[2], float neighbors[NUM_POINTS][2], int len_neighbors, float eps, int minPts, bool visited[NUM_POINTS], float cluster[NUM_POINTS][2])
{
    int index = 0;
    cluster[index][0] = point[0];
    cluster[index][1] = point[1];
    index++;
    visited[point_index] = true;
    int i = 0;
    while (i < len_neighbors)
    {
        float neighbor[2] = {neighbors[i][0], neighbors[i][1]};
        i++;
        int neighbor_index = -1;
        int j = 0;
        for ( j = 0; j < NUM_POINTS; j++)
        {
            if (fabs(data[j][0] - neighbor[0]) < 1e-6 && fabs(data[j][1] - neighbor[1]) < 1e-6)
            {
                neighbor_index = j;
                break;
            }
        }
        if (neighbor_index != -1 && !visited[neighbor_index])
        {
            visited[neighbor_index] = true;
            float new_neighbors[NUM_POINTS][2] = {0};
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
                        if (fabs(cluster[k][0] - new_neighbors[j][0]) < 1e-6 && fabs(cluster[k][1] - new_neighbors[j][1]) < 1e-6)
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
                if (fabs(cluster[j][0] - neighbor[0]) < 1e-6 && fabs(cluster[j][1] - neighbor[1]) < 1e-6)
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
        for (m = 0; m < NUM_POINTS; m++)
        {
            if (fabs(data[m][0] - cluster[l][0]) < 1e-6 && fabs(data[m][1] - cluster[l][1]) < 1e-6)
            {
                visited[m] = true;
            }
        }
    }
    return index;
}


void dbscan(float data[NUM_POINTS][2], float eps, int minPts, float clusters[NUM_POINTS][NUM_POINTS][2], int len_clusters[NUM_POINTS], float no_clusters[NUM_POINTS][2], int *len_no_clusters)
{
    bool visited[NUM_POINTS] = {false};
    int index=0;
    int i = 0;
    for (i = 0; i < NUM_POINTS; i++)
    {
        float point[2] = {data[i][0], data[i][1]};
        if (!visited[i])
        {
            visited[i] = true;
            float neighbors[NUM_POINTS][2] = {0};
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


int find_cluster(float point[2], float clusters[NUM_POINTS][NUM_POINTS][2], int len_clusters[NUM_POINTS])
{
    int i = 0;
    for (i = 0; i < NUM_POINTS; i++)
    {
        if (len_clusters[i] > 0)
        {
            int j = 0;
            for (j = 0; j < len_clusters[i]; j++)
            {
                if (fabs(clusters[i][j][0] - point[0]) < 1e-6 && fabs(clusters[i][j][1] - point[1]) < 1e-6)
                {
                    return i;
                }
            }
        }
    }
    return -1;
}

/**
 * main.c
 */
int main(void)
{
        float data[NUM_POINTS][2]; // Aquí debes inicializar los datos de entrenamiento
        float eps = 2;
        int minPts = 10;
        float clusters[NUM_POINTS][NUM_POINTS][2] = {0};
        int len_clusters[NUM_POINTS] = {0};
        float no_clusters[NUM_POINTS_2][2] = {0};
        int len_no_clusters = 0;

        float eps_2 = 8;
        int minPts_2 = 5;
        float clusters_2[NUM_POINTS][NUM_POINTS][2] = {0};
        int len_clusters_2[NUM_POINTS] = {0};
        float no_clusters_2[NUM_POINTS_2][2] = {0};
        int len_no_clusters_2 = 0;

        bool trained = false;

        //dbscan(data, eps, minPts, clusters, len_clusters, no_clusters, &len_no_clusters);
        //dbscan(no_clusters, eps_2, minPts_2, clusters_2, len_clusters_2, no_clusters_2, &len_no_clusters_2);
        //trained = true;

        while (true)
        {
            char input[10];
            printf("Ingrese el punto a buscar (x y) o 'x' para volver a entrenar: ");
            scanf("%s", input);
            if (input[0] == 'x')
            {
                int i = 0;
                printf("Ingrese los nuevos datos de entrenamiento:\n");
                for (i = 0; i < NUM_POINTS_2; i++)
                {
                    printf("Punto %d (x y): ", i + 1);
                    scanf("%f %f", &data[i][0], &data[i][1]);
                }
                dbscan(data, eps, minPts, clusters, len_clusters, no_clusters, &len_no_clusters);
                dbscan(no_clusters, eps_2, minPts_2, clusters_2, len_clusters_2, no_clusters_2, &len_no_clusters_2);
                trained = true;
            }
            else if (trained)
            {
                float point_to_search[2];
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
