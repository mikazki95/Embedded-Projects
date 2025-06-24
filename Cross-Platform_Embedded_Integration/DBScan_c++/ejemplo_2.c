#include <math.h>
#include <stdio.h>
#include <stdbool.h>

int get_neighbors(float data[10][2],float point [2],float eps, float neighbors[10][2]);
float euclidean_distance(float point1[2],float point2 [2]);

int expand_cluster(float data[10][2], float point[2], float neighbors[10][2], int len_neighbors, float eps, int minPts, bool visited[10], float cluster[10][2])
{
    int index = 0;
    cluster[index][0] = point[0];
    cluster[index][1] = point[1];
    index++;
    while (len_neighbors > 0)
    {
        len_neighbors--;
        float neighbor[2] = {neighbors[len_neighbors][0], neighbors[len_neighbors][1]};
        if (!visited[len_neighbors])
        {
            visited[len_neighbors] = true;
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


float euclidean_distance(float point1[2],float point2 [2])
{
    return sqrt(pow(point1[0]-point2[0],2)+pow(point1[1]-point2[1],2));
}


int get_neighbors(float data[10][2],float point [2],float eps, float neighbors[10][2])
{
    float other_point[2]={0};
    int index=0;
    for (int i=0;i<10;i++)
    {
        other_point[0]= data[i][0];
        other_point[1]= data[i][1];
        if ((euclidean_distance(point, other_point)<= eps))
        {
            neighbors[index][0]=data[i][0];
            neighbors[index][1]=data[i][1];
            index++;
        }          
    }  
    return index;
}

int main()
{
    float a[2] ={1,2}, b[2] ={3,4},c=0;
    //float data[10][2]={(2, 3), (2, 3), (2, 2), (2, 3), (2, 2), (1, 2), (8, 7), (8, 8), (8, 8), (8, 7)};
    float   new_neigbors[10][2]={0}, cluster[10][2]={0};
    float   eps=1.5;
    int     minPts2 = 2, x=0;
    bool visited[10] = {false};

    float data[10][2]={{2,3},{2,3},{2,2},{2,3},{2,2},{1,2},{8,7},{8,8},{8,8},{8,8}};
    c= euclidean_distance(a,b);
    for (int i = 0; i < 10; i++) 
        {
            a[0] = data[i][0];
            a[1] = data[i][1];
            x=get_neighbors(data,a,eps,new_neigbors);
            printf("len de neighbors = %i",x);
            printf("\n");
            printf("punto a evaluar ( %.1f, %.1f )",a[0],a[1]);
            printf("\n");
            expand_cluster(data,a,new_neigbors,x,eps,minPts2,visited,cluster);
        }
}