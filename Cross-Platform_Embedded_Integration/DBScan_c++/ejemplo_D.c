#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdbool.h>

#define NUM_POINTS 1000
#define NUM_POINTS_2 1000
#define NUM_POINTS_3 20


int point_in[2]={0};
int dato_ferc[NUM_POINTS_3][2]={0}; // Aquí debes inicializar los datos de entrenamiento
//int data[NUM_POINTS_3][2] = {{2, 3}, {2, 3}, {2, 2}, {2, 3}, {2, 2}, {1, 2}, {8, 7}, {8, 8}, {8, 8}, {8, 7},{18, 17}};
int arritmia[NUM_POINTS_2][2] = {0};
int data[NUM_POINTS_2][2] = {0};
float eps = 4;
int minPts = 10;
int clusters[NUM_POINTS_2][NUM_POINTS][2] = {0};
int len_clusters[NUM_POINTS_2] = {0};
int no_clusters[NUM_POINTS][2] = {0};
int len_no_clusters = 0;

float eps_2 =   18;
int minPts_2 =  5;
int clusters_2[NUM_POINTS_2][NUM_POINTS][2] = {0};
int len_clusters_2[NUM_POINTS_2] = {0};
int no_clusters_2[NUM_POINTS][2] = {0};
int len_no_clusters_2 = 0;
//int arritmia = 0;
///////////////////////////////////////
float   eps_3 =   10;
int     minPts_3 =  2;
int     clusters_3[NUM_POINTS_3][NUM_POINTS_3][2] = {0};
int     len_clusters_3[NUM_POINTS_3] = {0};
int     no_clusters_3[NUM_POINTS_3][2] = {0};
int     len_no_clusters_3 = 0;
/////////////////////////////////////////////////////////
int     aux_1_max=-2147483648;
int     aux_1_prom=2147483647;
int     aux_2_max=179181;
int     aux_2_prom=203843;
int     index_array=0;
int     i_array=0;
int     factor_x=1000;
int     long_array=1000;
int     array_aux_1[1501]={0};
int     array_aux_2[1501]={0};
bool    visited_prom[1000]={false};



float euclidean_distance(int point1[2], int point2[2]);
int get_neighbors(int data[NUM_POINTS][2], int point[2], float eps, int neighbors[NUM_POINTS][2]);
int get_neighbors_2(int data[NUM_POINTS_3][2], int point[2], float eps, int neighbors[NUM_POINTS_3][2]);
int expand_cluster(int data[NUM_POINTS][2],int neighbors[NUM_POINTS][2],float eps,bool visited[NUM_POINTS],int cluster[NUM_POINTS][2],int len_neighbors);
int expand_cluster_2(int data[NUM_POINTS_3][2],int neighbors[NUM_POINTS_3][2],float eps,bool visited[NUM_POINTS_3],int cluster[NUM_POINTS_3][2],int len_neighbors);//,int point[2],,   int minPts,  )
void dbscan(int data[NUM_POINTS][2], float eps, int minPts, int clusters[NUM_POINTS_2][NUM_POINTS][2], int len_clusters[NUM_POINTS_2], int no_clusters[NUM_POINTS][2], int *len_no_clusters, int len);
void dbscan_2(int data[NUM_POINTS_3][2], float eps, int minPts, int clusters[NUM_POINTS_3][NUM_POINTS_3][2], int len_clusters[NUM_POINTS_3], int no_clusters[NUM_POINTS_3][2], int *len_no_clusters, int len);
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

int get_neighbors_2(int data[NUM_POINTS_3][2], int point[2], float eps, int neighbors[NUM_POINTS_3][2])
{
    int other_point[2] = {0};
    int index = 0;
    int i=0;
    for (i = 0; i < NUM_POINTS_3; i++)
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

int expand_cluster_2(int data[NUM_POINTS_3][2],int neighbors[NUM_POINTS_3][2],float eps,bool visited[NUM_POINTS_3],int cluster[NUM_POINTS_3][2],int len_neighbors)//,int point[2],,   int minPts,  )
{
    int i,j;
    int len_neighbor_new=0;
    int len_neighbor_max=0;
    int new_neighbors[NUM_POINTS_3][2]={0};
    int point_aux[2];


    for(i=0;i<len_neighbors;i++)
    {
        int new_point[2] = {neighbors[i][0],neighbors[i][1]};
        len_neighbor_new = get_neighbors_2(data,new_point,eps,new_neighbors);
        if (len_neighbor_new > len_neighbor_max)
        {
            len_neighbor_max = len_neighbor_new;
            point_aux[0] = new_point[0];
            point_aux[1] = new_point[1];
        }
    }

    len_neighbor_new = get_neighbors_2(data,point_aux,eps,cluster);
    for(i=0;i<len_neighbor_new;i++)
    {
        //int point_aux2
        for(j=0;j<NUM_POINTS_3;j++)
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

void dbscan_2(int data_2[NUM_POINTS_3][2], float eps, int minPts, int clusters[NUM_POINTS_3][NUM_POINTS_3][2], int len_clusters[NUM_POINTS_3], int no_clusters[NUM_POINTS_3][2], int *len_no_clusters, int len)
{
    bool visited[NUM_POINTS_3] = {false};
    int index=0;
    int i = 0;
    int no_cluster_aux=0;
    for (i = 0; i < len; i++)
    {
        int point[2] = {data_2[i][0], data_2[i][1]};
        if (!visited[i])
        {
            //visited[i] = true;
            int neighbors[NUM_POINTS_3][2] = {0};
            //int len_neighbors = get_neighbors_2(data_2, point, eps, neighbors);
            int len_neighbors = get_neighbors_2(data_2,point, eps, neighbors);
            if (len_neighbors >= minPts)
            {
                index =  expand_cluster_2(data_2, neighbors,eps, visited, clusters[*len_no_clusters],len_neighbors);
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
    for (i = 0; i < NUM_POINTS_2; i++)
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

int find_cluster_2(int point[2], int clusters[NUM_POINTS_3][NUM_POINTS_3][2], int len_clusters[NUM_POINTS_3])
{
    int i = 0;
    int valu_out=-1;
    for (i = 0; i < NUM_POINTS_3; i++)
    {
        if (len_clusters[i] > 0)
        {
            int j = 0;
            for (j = 0; j < len_clusters[i]; j++)
            {
                valu_out=euclidean_distance(point,clusters[i][j]);
                
                if (valu_out<=5)
                {
                    //valu_out =i ;
                    return i;
                }
                
            }
        }
    }
    return -1;
    //return valu_out;
}

int main() {
    FILE *file = fopen("data.csv", "r");
    if (file == NULL) {
        printf("No se pudo abrir el archivo\n");
        return 1;
    }

   
    for (int i = 0; i < NUM_POINTS; i++)
    {
        /* code */
        fscanf(file, "%d,%d\n", &data[i][0], &data[i][1]) != EOF;
    }
    
    fclose(file);

    FILE *file2 = fopen("arritmia.csv", "r");
    if (file2 == NULL) {
        printf("No se pudo abrir el archivo\n");
        return 1;
    }

   
    for (int i = 0; i < NUM_POINTS; i++)
    {
        /* code */
        fscanf(file2, "%d,%d\n", &arritmia[i][0], &arritmia[i][1]) != EOF;
    }
    fclose(file2);

    FILE *file3 = fopen("frec.csv", "r");
        for (int i = 0; i < NUM_POINTS_3; i++)
    {
        /* code */
        fscanf(file3, "%d,%d\n", &dato_ferc[i][0], &dato_ferc[i][1]) != EOF;
    }
    fclose(file3);

    // Imprimir el contenido del array
    for (int i = 0; i < NUM_POINTS_3; i++) {
        printf("%d %d\n", dato_ferc[i][0], dato_ferc[i][1]);
    }
//////////////////////////////////////////////////////////////////////////////
    for (int i = 0; i < long_array; i++)
    {
        /* code */
        array_aux_1[i]=data[i][1];
        array_aux_2[i]=data[i][1];
    }

//////////////////////////////////////////////////////////////////////////////
    int len_aux=0, len_max=0;
    for (i_array = 0; i_array < long_array + 1; i_array++)
    {
        visited_prom[i_array]=false;
    }
    for (i_array = 0; i_array < long_array + 1; i_array++)
    {
        int aux_visited=0;
        len_aux=0; 
        if (!visited_prom[i_array])
        {
            //visited_prom[i_array]= true;               
            for (aux_visited = i_array; aux_visited < long_array + 1; aux_visited++)
            {
                if (abs(array_aux_1[i_array]-array_aux_1[aux_visited])<5)
                {
                    /* code */
                    visited_prom[aux_visited]= true;
                    len_aux++;
                }
                
            }
            
        }
        if (len_aux>len_max)
        {
            /* code */
            len_max=len_aux;
            aux_1_prom=array_aux_1[i_array];
        }
        
    }
///////////////////////////////////////////////////////////////////////////////////
    bool visited[NUM_POINTS] = {false};
    dbscan(data, eps, minPts, clusters, len_clusters, no_clusters, &len_no_clusters,NUM_POINTS);
    
    // Imprimir los resultados
    
    printf("Clusters 1:\n");
    for (int i = 0; i < NUM_POINTS; i++)
    {
        if (len_clusters[i] > 0)
        {
            //printf("Cluster %d:\n", i + 1);
            for (int j = 0; j < len_clusters[i]; j++)
            {
                printf("Cluster %i (%i, %i)\n", i, clusters[i][j][0], clusters[i][j][1]);
            }
        }
    }
    
   
    printf("No clusters 1:\n");
    for (int i = 0; i < len_no_clusters; i++)
    {
        printf(" (%i, %i)\n", no_clusters[i][0], no_clusters[i][1]);
    }
    
    
    dbscan(no_clusters, eps_2, minPts_2, clusters_2, len_clusters_2, no_clusters_2, &len_no_clusters_2,len_no_clusters);

// Imprimir los resultados
    /*
    printf("Clusters 2:\n");
    for (int i = 0; i < NUM_POINTS; i++)
    {
        if (len_clusters_2[i] > 0)
        {
            //printf("Cluster %d:\n", i + 1);
            for (int j = 0; j < len_clusters_2[i]; j++)
            {
                printf("Cluster %i (%i, %i)\n", i, clusters_2[i][j][0], clusters_2[i][j][1]);
            }
        }
    }
    
   
    printf("No clusters 2:\n");
    for (int i = 0; i < len_no_clusters_2; i++)
    {
        printf(" (%i, %i)\n", no_clusters_2[i][0], no_clusters_2[i][1]);
    }
    */
    
    dbscan_2(dato_ferc, eps_3, minPts_3, clusters_3, len_clusters_3, no_clusters_3, &len_no_clusters_3,NUM_POINTS_3);

   /*
    printf("Clusters 3:\n");
    for (int i = 0; i < NUM_POINTS_3; i++)
    {
        if (len_clusters_3[i] > 0)// se debe ver  19 y varios datos de cluster, el cluster en 0 es medio secuencial i es estable e cluster en 1 es variable 
        {
            printf("Cluster %d:\n", i + 1);
            for (int j = 0; j < len_clusters_3[i]; j++)
            {
                printf(" (%i, %i)\n", clusters_3[i][j][0], clusters_3[i][j][1]);
            }
        }
    }
  

    printf("No clusters 3:\n");
    for (int i = 0; i < len_no_clusters_3; i++)
    {
        printf("dato %i: (%i, %i)\n", i, no_clusters_3[i][0], no_clusters_3[i][1]);
    }
*/
    int point_to_search_2[2]={125,5};    
    int cluster_index = find_cluster_2(point_to_search_2, clusters_3, len_clusters_3);
        
/////////////////////////////////////////////////////////////////////////////////////////////////////
    int contador =0;
     for (int i = 0; i < NUM_POINTS; i++)
    {
        
        int point_to_search[2]={arritmia[i][0],arritmia[i][1]};
        
        
        int cluster_index = find_cluster(point_to_search, clusters, len_clusters);
        if (cluster_index != -1)
        {
            //printf("El punto (%.i, %.i) se encuentra en el cluster %d\n", point_to_search[0], point_to_search[1], cluster_index + 1);
            cluster_index=-1;
        }
        else
        {
            int cluster_index2 = find_cluster(point_to_search, clusters_2, len_clusters_2);
            if (cluster_index2 != -1)
            {
                //printf("El punto (%.i, %.i) se encuentra en el cluster %d\n", point_to_search[0], point_to_search[1], cluster_index + 1);
                cluster_index2=-1;
            }
            else
            {
                contador++;
                printf("El punto %i (%.i, %.i) no se encuentra en ningún cluster\n", contador, point_to_search[0], point_to_search[1]);
            }
            //printf("El punto (%.i, %.i) no se encuentra en ningún cluster\n", point_to_search[0], point_to_search[1]);
        }
        /* code */
    }
    
    return 0;
}
