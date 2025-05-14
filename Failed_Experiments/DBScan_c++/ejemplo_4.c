#include <math.h>
#include <stdio.h>
#include <stdbool.h>

int get_neighbors(float data[10][2], float point[2], float eps, float neighbors[10][2]);
float euclidean_distance(float point1[2], float point2[2]);
int expand_cluster(float data[10][2], int point_index, float point[2], float neighbors[10][2], int len_neighbors, float eps, int minPts, bool visited[10], float cluster[10][2]);


int expand_cluster(float data[10][2], int point_index, float point[2], float neighbors[10][2], int len_neighbors, float eps, int minPts, bool visited[10], float cluster[10][2])
{
 int index = 0;
 cluster[index][0] = point[0];
 cluster[index][1] = point[1];
 index++;
 visited[point_index] = true;
 while (len_neighbors > 0)
 {
 len_neighbors--;
 float neighbor[2] = {neighbors[len_neighbors][0], neighbors[len_neighbors][1]};
 int neighbor_index = -1;
 for (int i = 0; i < 10; i++)
 {
 if (data[i][0] == neighbor[0] && data[i][1] == neighbor[1])
 {
 neighbor_index = i;
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
 for (int i = 0; i < len_new_neighbors; i++)
 {
 bool in_cluster = false;
 for (int j = 0; j < index; j++)
 {
 if (cluster[j][0] == new_neighbors[i][0] && cluster[j][1] == new_neighbors[i][1])
 {
 in_cluster = true;
 break;
 }
 }
 if (!in_cluster)
 {
 neighbors[len_neighbors + i][0] = new_neighbors[i][0];
 neighbors[len_neighbors + i][1] = new_neighbors[i][1];
 len_neighbors++;
 }
 }
 }
 bool in_cluster = false;
 for (int i = 0; i < index; i++)
 {
 if (cluster[i][0] == neighbor[0] && cluster[i][1] == neighbor[1])
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
 return index;
}


void dbscan(float data[10][2], float eps, int minPts, float clusters[10][10][2], int len_clusters[10], float no_clusters[10][2], int *len_no_clusters)
{
    bool visited[10] = {false};
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
                int index = expand_cluster(data, i, point, neighbors, len_neighbors, eps, minPts, visited, clusters[*len_no_clusters]);
                len_clusters[*len_no_clusters] = index;
                (*len_no_clusters)++;
            }
            else
            {
                no_clusters[*len_no_clusters][0] = point[0];
                no_clusters[*len_no_clusters][1] = point[1];
                (*len_no_clusters)++;
            }
        }
    }
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


/*
int expand_cluster(float data[10][2], int point_index, float point[2], float neighbors[10][2], int len_neighbors, float eps, int minPts, bool visited[10], float cluster[10][2])
{
    int index = 0;
    cluster[index][0] = point[0];
    cluster[index][1] = point[1];
    index++;
    visited[point_index] = true;
    while (len_neighbors > 0)
    {
        len_neighbors--;
        float neighbor[2] = {neighbors[len_neighbors][0], neighbors[len_neighbors][1]};
        int neighbor_index = -1;
        for (int i = 0; i < 10; i++)
        {
            if (data[i][0] == neighbor[0] && data[i][1] == neighbor[1])
            {
                neighbor_index = i;
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
                for (int i = 0; i < len_new_neighbors; i++)
                {
                    bool in_cluster = false;
                    for (int j = 0; j < index; j++)
                    {
                        if (cluster[j][0] == new_neighbors[i][0] && cluster[j][1] == new_neighbors[i][1])
                        {
                            in_cluster = true;
                            break;
                        }
                    }
                    if (!in_cluster)
                    {
                        neighbors[len_neighbors + i][0] = new_neighbors[i][0];
                        neighbors[len_neighbors + i][1] = new_neighbors[i][1];
                        len_neighbors++;
                    }
                }
            }
            bool in_cluster = false;
            for (int i = 0; i < index; i++)
            {
                if (cluster[i][0] == neighbor[0] && cluster[i][1] == neighbor[1])
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
    return index;
}
*/

int main()
{
    //float data[10][2] = {{2, 3}, {2, 3}, {2, 2}, {2, 3}, {2, 2}, {1, 2}, {8, 7}, {8, 8}, {8, 8}, {8, 7}};
    float data[10][2] = {{2, 3}, {2, 3}, {2, 2}, {2, 3}, {2, 2}, {1, 2}, {8, 7}, {8, 8}, {8, 8}, {8, 7}};
    float eps = 3;
    int minPts = 2;
    float clusters[10][10][2] = {0};
    int len_clusters[10] = {0};
    float no_clusters[10][2] = {0};
    int len_no_clusters = 0;
    
    dbscan(data, eps, minPts, clusters, len_clusters, no_clusters, &len_no_clusters);
    
    // Imprimir los resultados
    printf("Clusters:\n");
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
    
    printf("No clusters:\n");
    for (int i = 0; i < len_no_clusters; i++)
    {
        printf(" (%.1f, %.1f)\n", no_clusters[i][0], no_clusters[i][1]);
    }
    
    return 0;
}
