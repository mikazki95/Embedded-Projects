#include <math.h>
#include <stdio.h>
#include <stdbool.h>

int get_neighbors(float data[10][2], float point[2], float eps, float neighbors[10][2]);
float euclidean_distance(float point1[2], float point2[2]);

float euclidean_distance(float point1[2], float point2[2])
{
    return sqrt(pow(point1[0] - point2[0], 2) + pow(point1[1] - point2[1], 2));
}

int get_neighbors(float data[10][2], float point[2], float eps, float neighbors[10][2])
{
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
                    neighbors[len_neighbors + i][0] = new_neighbors[i][0];
                    neighbors[len_neighbors + i][1] = new_neighbors[i][1];
                }
                len_neighbors += len_new_neighbors;
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

int main()
{
    float a[2] ={1, 2}, b[2] ={3, 4}, c=0;
    //float data[10][2]={(2, 3), (2, 3), (2, 2), (2, 3), (2, 2), (1, 2), (8, 7), (8, 8), (8, 8), (8, 7)};
    float new_neigbors[10][2]={0};
    float eps=3;
    int minPts= 2,x=0;

    float data[10][2]={{2,3},{2,3},{2,2},{2,3},{2,2},{1,2},{8,7},{8,8},{8,8},{8,8}};
    c= euclidean_distance(a,b);
    
    bool visited[10] = {false};
    float cluster[10][2] = {0};
    float point[2] = {data[0][0], data[0][1]};

    x= get_neighbors (data,point ,eps,new_neigbors);

    printf("new_neigbors: [");
     for (int i=0;i<x;i++)
     {
         printf("(%.1f,%.1f)",new_neigbors[i][0],new_neigbors[i][1]);
         if(i<x-1)
         {
             printf(",");
         }
     }
    printf("]\n");
    printf("len new_neigbors: %i",x);
    printf("]\n");
    int len_cluster = expand_cluster(data,0,point,new_neigbors,x,eps,minPts,visited,cluster);
    
     printf("Cluster: [");
     for (int i=0;i<len_cluster;i++)
     {
         printf("(%.1f,%.1f)",cluster[i][0],cluster[i][1]);
         if(i<len_cluster-1)
         {
             printf(",");
         }
     }
     printf("]\n");
     return 0;
}
