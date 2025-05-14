#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#define NUM_POINTS 30
#define NUM_POINTS_2 30


int point_in[2]={0};
int data[NUM_POINTS][2]={0}; // Aquí debes inicializar los datos de entrenamiento
float eps = 2;
int minPts = 10;
int clusters[NUM_POINTS_2][NUM_POINTS][2] = {0};
int len_clusters[NUM_POINTS_2] = {0};
int no_clusters[NUM_POINTS][2] = {0};
int len_no_clusters = 0;

float eps_2 = 8;
int minPts_2 = 5;
int clusters_2[NUM_POINTS_2][NUM_POINTS][2] = {0};
int len_clusters_2[NUM_POINTS_2] = {0};
int no_clusters_2[NUM_POINTS][2] = {0};
int len_no_clusters_2 = 0;
int arritmia = 0;

bool flag_trained = false;      // indicar tiempo de entrenamiento par ala red
bool flag_dato_rdy = false; 

int get_neighbors(float data[10][2], float point[2], float eps, float neighbors[10][2]);
float euclidean_distance(float point1[2], float point2[2]);
int expand_cluster(float data[10][2], int point_index, float point[2], float neighbors[10][2], int len_neighbors, float eps, int minPts, bool visited[10], float cluster[10][2]);
void dbscan(float data[10][2], float eps, int minPts, float clusters[10][10][2], int len_clusters[10], float no_clusters[10][2], int *len_no_clusters);
int find_cluster(float point[2], float clusters[10][10][2], int len_clusters[10]);

int expand_cluster(float data[10][2], int point_index, float point[2], float neighbors[10][2], int len_neighbors, float eps, int minPts, bool visited[10], float cluster[10][2])
{
    int index = 0, index2=0;
    int visitados_aux[10];
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
        for (int j = 0; j < 10; j++)
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
            float new_neighbors[10][2] = {0};
            int len_new_neighbors = get_neighbors(data, neighbor, eps, new_neighbors);
            if (len_new_neighbors >= minPts)
            {
                for (int j = 0; j < len_new_neighbors; j++)
                {
                    bool in_cluster = false;
                    for (int k = 0; k < index; k++)
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
            for (int j = 0; j < index; j++)
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
    for (int l = 0; l < index; l++)
    {
        /* code */
        for (int m = 0; m < 10; m++)
        {
            if (fabs(data[m][0] - cluster[l][0]) < 1e-6 && fabs(data[m][1] - cluster[l][1]) < 1e-6)
            {
                visited[m] = true;
                //break;
            }
            
            /* code */
        }
        
    }
    return index;
}

void dbscan(float data[10][2], float eps, int minPts, float clusters[10][10][2], int len_clusters[10], float no_clusters[10][2], int *len_no_clusters)
{
    bool visited[10] = {false};
    int index=0;
    for (int i = 0; i < 10; i++)
    {
        float point[2] = {data[i][0], data[i][1]};
        if (!visited[i])
        {
            visited[i] = true;
            float neighbors[10][2] = {0};
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

float euclidean_distance(float point1[2], float point2[2])
{
    return sqrt(pow(point1[0] - point2[0], 2) + pow(point1[1] - point2[1], 2));
}

int get_neighbors(float data[10][2], float point[2], float eps, float neighbors[10][2])
{
    //printf("point (%0.1f, %0.1f )\n",point[0],point[1]);
    float other_point[2] = {0};
    int index = 0;
    for (int i = 0; i < 10; i++)
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

int find_cluster(float point[2], float clusters[10][10][2], int len_clusters[10])
{
    for (int i = 0; i < 10; i++)
    {
        if (len_clusters[i] > 0)
        {
            for (int j = 0; j < len_clusters[i]; j++)
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

int main()
{
    //float data[NUM_POINTS][2] = {{2, 3}, {2, 3}, {2, 2}, {2, 3}, {2, 2}, {1, 2}, {8, 7}, {8, 8}, {8, 8}, {8, 7}};
    float data[10][2] = {{2, 3}, {2, 3}, {2, 2}, {2, 3}, {2, 2}, {1, 2}, {8, 7}, {8, 8}, {8, 8}, {8, 7}};
    float eps = 3;
    int minPts = 2;
    float clusters[10][10][2] = {0};
    int len_clusters[10] = {0};
    float no_clusters[10][2] = {0};
    int len_no_clusters = 0;

    float eps_2 = 3;
    int minPts_2 = 2;
    float clusters_2[10][10][2] = {0};
    int len_clusters_2[10] = {0};
    float no_clusters_2[10][2] = {0};
    int len_no_clusters_2 = 0;
    
    dbscan(data, eps, minPts, clusters, len_clusters, no_clusters, &len_no_clusters);
    
    // Imprimir los resultados
    printf("Clusters 1:\n");
    for (int i = 0; i < 10; i++)
    {
        if (len_clusters[i] > 0)
        {
            printf("Cluster %d:\n", i + 1);
            for (int j = 0; j < len_clusters[i]; j++)
            {
                printf(" (%.1f, %.1f)\n", clusters[i][j][0], clusters[i][j][1]);
            }
        }
    }
    
    printf("No clusters 1:\n");
    for (int i = 0; i < len_no_clusters; i++)
    {
        printf(" (%.1f, %.1f)\n", no_clusters[i][0], no_clusters[i][1]);
    }
    
    dbscan(no_clusters, eps_2, minPts_2, clusters_2, len_clusters_2, no_clusters_2, &len_no_clusters_2);

    printf("Clusters 2:\n");
    for (int i = 0; i < 10; i++)
    {
        if (len_clusters_2[i] > 0)
        {
            printf("Cluster %d:\n", i + 1);
            for (int j = 0; j < len_clusters_2[i]; j++)
            {
                printf(" (%.1f, %.1f)\n", clusters_2[i][j][0], clusters_2[i][j][1]);
            }
        }
    }
    
    printf("No clusters 2:\n");
    for (int i = 0; i < len_no_clusters_2; i++)
    {
        printf(" (%.1f, %.1f)\n", no_clusters_2[i][0], no_clusters_2[i][1]);
    }
/////////////////////////////////////////////////////////////////////////////////////////////////////
    float point_to_search[2] = {8, 7}; // Punto a buscar
    
    while (true)
    {
        char input[10];
        printf("Ingrese el punto a buscar (x y) o 'x' para volver a entrenar: ");
        scanf("%s", input);
        if (input[0] == 'x')
        {
            printf("Ingrese los nuevos datos de entrenamiento:\n");
            for (int i = 0; i < 10; i++)
            {
                printf("Punto %d (x y): ", i + 1);
                scanf("%f %f", &data[i][0], &data[i][1]);
            }
             dbscan(data, eps, minPts, clusters, len_clusters, no_clusters, &len_no_clusters);
             dbscan(no_clusters, eps_2, minPts_2, clusters_2, len_clusters_2, no_clusters_2, &len_no_clusters_2);
        }
        else
        {
            //float point_to_search[2];
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
    }
    
    return 0;
}
