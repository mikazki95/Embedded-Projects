#include <math.h>
#include <stdio.h>
#include <stdbool.h>
#define NUM_POINTS 11
#define NUM_POINTS_2 11


int point_in[2]={0};
//int data[NUM_POINTS][2]={0}; // Aquí debes inicializar los datos de entrenamiento
int data[NUM_POINTS_2][2] = {{2, 3}, {2, 3}, {2, 2}, {2, 3}, {2, 2}, {1, 2}, {8, 7}, {8, 8}, {8, 8}, {8, 7},{18, 17}};
float eps = 2;
int minPts = 2;//10;
int clusters[NUM_POINTS_2][NUM_POINTS][2] = {0};
int len_clusters[NUM_POINTS_2] = {0};
int no_clusters[NUM_POINTS][2] = {0};
int len_no_clusters = 0;

float eps_2 = 1;//8;
int minPts_2 = 3;//5;
int clusters_2[NUM_POINTS_2][NUM_POINTS][2] = {0};
int len_clusters_2[NUM_POINTS_2] = {0};
int no_clusters_2[NUM_POINTS][2] = {0};
int len_no_clusters_2 = 0;
int arritmia = 0;

bool flag_trained = false;      // indicar tiempo de entrenamiento par ala red
bool flag_dato_rdy = false; 

float euclidean_distance(int point1[2], int point2[2]);
int get_neighbors(int data[NUM_POINTS][2], int point[2], float eps, int neighbors[NUM_POINTS][2]);
int expand_cluster(int data[NUM_POINTS][2],int neighbors[NUM_POINTS][2],float eps,bool visited[NUM_POINTS],int cluster[NUM_POINTS][2],int len_neighbors);
void dbscan(int data[NUM_POINTS][2], float eps, int minPts, int clusters[NUM_POINTS_2][NUM_POINTS][2], int len_clusters[NUM_POINTS_2], int no_clusters[NUM_POINTS][2], int *len_no_clusters, int len);
int find_cluster(int point[2], int clusters[NUM_POINTS_2][NUM_POINTS][2], int len_clusters[NUM_POINTS_2]);


float euclidean_distance(int point1[2], int point2[2])
{
    return sqrt(pow(point1[0] - point2[0], 2) + pow(point1[1] - point2[1], 2));
}

int get_neighbors(int data[NUM_POINTS][2], int point[2], float eps, int neighbors[NUM_POINTS][2])
{
    int other_point[2] = {0};
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

int expand_cluster(int data[NUM_POINTS][2],int neighbors[NUM_POINTS][2],float eps,bool visited[NUM_POINTS],int cluster[NUM_POINTS][2],int len_neighbors)//,int point[2],,   int minPts,  )
{
    int i,j;
    int len_neighbor_new=0;
    int len_neighbor_max=0;
    int new_neighbors[NUM_POINTS][2]={0};
    int point_aux[2];


    for(i=0;i<len_neighbors;i++)
    {
        int new_point[2] = {neighbors[i][0],neighbors[i][1]};
        len_neighbor_new = get_neighbors(data,new_point,eps,new_neighbors);
        if (len_neighbor_new > len_neighbor_max)
        {
            len_neighbor_max = len_neighbor_new;
            point_aux[0] = new_point[0];
            point_aux[1] = new_point[1];
        }
    }

    len_neighbor_new = get_neighbors(data,point_aux,eps,cluster);
    for(i=0;i<len_neighbor_new;i++)
    {
        //int point_aux2
        for(j=0;j<NUM_POINTS;j++)
        {
            if ((cluster[i][0]==data[j][0])&&(cluster[i][1]==data[j][1])&&(!visited[j]))
            {
                visited[j] = true;
            }
        }


    }
    return len_neighbor_new;
}

void dbscan(int data[NUM_POINTS][2], float eps, int minPts, int clusters[NUM_POINTS_2][NUM_POINTS][2], int len_clusters[NUM_POINTS_2], int no_clusters[NUM_POINTS][2], int *len_no_clusters, int len)
{
    bool visited[NUM_POINTS] = {false};
    int index=0;
    int i = 0;
    int no_cluster_aux=0;
    for (i = 0; i < len; i++)
    {
        int point[2] = {data[i][0], data[i][1]};
        if (!visited[i])
        {
            //visited[i] = true;
            int neighbors[NUM_POINTS][2] = {0};
            int len_neighbors = get_neighbors(data, point, eps, neighbors);
            if (len_neighbors >= minPts)
            {
                index =  expand_cluster(data, neighbors,eps, visited, clusters[*len_no_clusters],len_neighbors);
                //index = expand_cluster(data, i, point, neighbors, len_neighbors, eps, minPts, visited, clusters[*len_no_clusters]);
                len_clusters[*len_no_clusters] = index;
                (*len_no_clusters)++;
            }
            else
            {
                /*no_clusters[*len_no_clusters-index][0] = point[0];
                no_clusters[*len_no_clusters-index][1] = point[1];
                (*len_no_clusters)++;*/
                no_clusters[no_cluster_aux][0] = point[0];
                no_clusters[no_cluster_aux][1] = point[1];
                (no_cluster_aux)++;
            }
        }
    }
    //*len_no_clusters=*len_no_clusters-index;
    *len_no_clusters=no_cluster_aux;
}


int find_cluster(int point[2], int clusters[NUM_POINTS_2][NUM_POINTS][2], int len_clusters[NUM_POINTS_2])
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
    //float data[NUM_POINTS][2] = {{2, 3}, {2, 3}, {2, 2}, {2, 3}, {2, 2}, {1, 2}, {8, 7}, {8, 8}, {8, 8}, {8, 7}};
    //float data[10][2] = {{2, 3}, {2, 3}, {2, 2}, {2, 3}, {2, 2}, {1, 2}, {8, 7}, {8, 8}, {8, 8}, {8, 7}};
    bool visited[NUM_POINTS] = {false};
    dbscan(data, eps, minPts, clusters, len_clusters, no_clusters, &len_no_clusters,NUM_POINTS);
    
    // Imprimir los resultados
    printf("Clusters 1:\n");
    for (int i = 0; i < NUM_POINTS; i++)
    {
        if (len_clusters[i] > 0)
        {
            printf("Cluster %d:\n", i + 1);
            for (int j = 0; j < len_clusters[i]; j++)
            {
                printf(" (%i, %i)\n", clusters[i][j][0], clusters[i][j][1]);
            }
        }
    }
    
    printf("No clusters 1:\n");
    for (int i = 0; i < len_no_clusters; i++)
    {
        printf(" (%i, %i)\n", no_clusters[i][0], no_clusters[i][1]);
    }
    
    dbscan(no_clusters, eps_2, minPts_2, clusters_2, len_clusters_2, no_clusters_2, &len_no_clusters_2,len_no_clusters);

    printf("Clusters 2:\n");
    for (int i = 0; i < NUM_POINTS; i++)
    {
        if (len_clusters_2[i] > 0)
        {
            printf("Cluster %d:\n", i + 1);
            for (int j = 0; j < len_clusters_2[i]; j++)
            {
                printf(" (%i, %i)\n", clusters_2[i][j][0], clusters_2[i][j][1]);
            }
        }
    }
    
    printf("No clusters 2:\n");
    for (int i = 0; i < len_no_clusters_2; i++)
    {
        printf(" (%i, %i)\n", no_clusters_2[i][0], no_clusters_2[i][1]);
    }
/////////////////////////////////////////////////////////////////////////////////////////////////////
    int point_to_search[2] = {8, 7}; // Punto a buscar
    
    while (true)
    {
        char input[NUM_POINTS];
        printf("Ingrese el punto a buscar (x y) o 'x' para volver a entrenar: ");
        scanf("%s", input);
        if (input[0] == 'x')
        {
            bool visited[NUM_POINTS] = {false};
            printf("Ingrese los nuevos datos de entrenamiento:\n");
            for (int i = 0; i < NUM_POINTS; i++)
            {
                printf("Punto %d (x y): ", i + 1);
                scanf("%i %i", &data[i][0], &data[i][1]);
            }
             dbscan(data, eps, minPts, clusters, len_clusters, no_clusters, &len_no_clusters,NUM_POINTS);
             dbscan(no_clusters, eps_2, minPts_2, clusters_2, len_clusters_2, no_clusters_2, &len_no_clusters_2,len_no_clusters);
        }
        else
        {
            //float point_to_search[2];
            point_to_search[0] = atoi(input);
            scanf("%i", &point_to_search[1]);
            int cluster_index = find_cluster(point_to_search, clusters, len_clusters);
            if (cluster_index != -1)
            {
                printf("El punto (%.i, %.i) se encuentra en el cluster %d\n", point_to_search[0], point_to_search[1], cluster_index + 1);
            }
            else
            {
                printf("El punto (%.i, %.i) no se encuentra en ningún cluster\n", point_to_search[0], point_to_search[1]);
            }
        }
    }
    
    return 0;
}
