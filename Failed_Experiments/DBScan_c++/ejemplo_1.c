#include <math.h>
#include <stdio.h>

int get_neighbors(float data[10][2],float point [2],float eps, float neighbors[10][2]);
float euclidean_distance(float point1[2],float point2 [2]);

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
    float   new_neigbors[10][2]={0};
    float   eps=1.5;
    int     minPts2 = 2, x=0;

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

            printf("data:[");
            for (int i = 0; i < x; i++) 
            {
                printf("(");
                for (int j = 0; j < 2; j++) 
                {
                    printf(" %.1f  ",new_neigbors[i][j]);
                    
                }  
                printf(")");      
            }
            printf("]");
            printf("\n");
        }
    
    printf("llamado a distance: %f r",c);
}