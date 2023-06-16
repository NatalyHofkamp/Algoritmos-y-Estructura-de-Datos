from graph import Graph
import csv
from itertools import combinations
import os
from tqdm import tqdm 
import random
import pickle
import heapq
from grafo_a import read_data,sort_clusters,get_artist,find_cluster
import time
from collections import deque

MOVIE_TITLE_TYPE = "movie"
MOVIE_COLUMNS = ["tconst", "titleType", "primaryTitle"]
PRINCIPALS_COLUMNS = ["nconst", "category"]
MOVIES_DATA_PATH = "tp4/datasets/title-basics-f.tsv"
ACTORS_DATA_PATH = "tp4/datasets/title-principals-f.tsv"
ACTORS_NAMES_PATH = "tp4/datasets/name-basics-f.tsv"

def load_bipartite_graph(movies_by_id, actors_by_movie, actor_names_by_id) -> Graph:
    """
    Loads the bipartite graph connecting movies and actors
    :param movies_by_id: the movies data by id as dict
    :param actors_by_movie: the actors data by movie
    :param actor_names_by_id: the actors names by their ids
    :return: a bipartite Graph connecting movies and actors
    """
    graph = Graph()
    print("loading data in process...")
    for movie_id in movies_by_id.keys():
        movie_title = movies_by_id[movie_id]['primaryTitle']
        graph.add_vertex(movie_id, movie_title)
        for actor_id in actors_by_movie[movie_id]:
            graph.add_vertex(actor_id, "ERROR")
            graph.add_edge(vertex1=movie_id, vertex2=actor_id)
    
    return graph
def bipartite_clustering(graph):
    print("clustering data in process...")
    visited = set()
    clusters = {}
    cluster_id = 0
    
    for vertex in graph.get_vertices():
        if vertex not in visited:
            cluster_id += 1
            cluster_graph = Graph()
            dfs_bipartite(graph, vertex, visited, clusters, cluster_id, cluster_graph)
    
    return clusters

def dfs_bipartite(graph, vertex, visited, clusters, cluster_id, cluster_graph):
    queue = deque([(vertex, None)])  # Usamos una cola en lugar de una pila para rastrear los vértices y sus padres
    while queue:
        current_vertex, parent_vertex = queue.popleft()
        if current_vertex in visited:
            continue
        
        visited.add(current_vertex)
        cluster_graph.add_vertex(current_vertex)
        
        if parent_vertex is not None:
            cluster_graph.add_edge(parent_vertex, current_vertex)  # Agregar arista al cluster
            
        neighbors = graph.get_neighbors(current_vertex)
        for neighbor in neighbors:
            if neighbor not in visited:
                queue.append((neighbor, current_vertex))  # Agregar el vértice y su padre a la cola
    
    clusters[cluster_id] = cluster_graph

# def calculate_degree_of_separation(cluster, actor1_id, actor2_id):
#lindo pero ineficiente
#     visited = set()
#     queue = deque([(actor1_id, 0)])  # Cola para el BFS
    
#     while queue:
#         current_actor, degree = queue.popleft()    
#         if current_actor == actor2_id:
#             return degree  # Se encontró la ruta más corta
#         if current_actor in visited:
#             continue
#         visited.add(current_actor)
#         neighbors = cluster.get_neighbors(current_actor)
#         for neighbor in neighbors:
#             if neighbor not in visited:
#                 queue.append((neighbor, degree + 1))
#     return -1  # No se encontró una ruta entre los actores

def second_exercise(clusters):
    actor1_id, cluster = get_artist(clusters, -1,1234)
    actor2_id, cluster = get_artist(clusters, -1,6543)
    degree = calculate_degree_of_separation(cluster, actor1_id, actor2_id)
    if degree == -1:
        print("No se encontró una ruta entre los actores.")
    else:
        print(f"El grado de separación entre {actor1_id} y {actor2_id} es: {degree}")

def calculate_degree_of_separation(cluster, actor1_id, actor2_id):
    visited_forward = set()
    visited_backward = set()
    queue_backward = deque([(actor1_id, 0)])  # Cola para el BFS desde Kevin Bacon
    queue_forward = deque([(actor2_id, 0)])  # Cola para el BFS desde el actor1
    
    
    while queue_forward and queue_backward:
        current_actor_forward, degree_forward = queue_forward.popleft()
        current_actor_backward, degree_backward = queue_backward.popleft()
        
        # Verificar si se encontró una conexión
        if current_actor_forward in visited_backward:
            return degree_forward + degree_backward
        if current_actor_backward in visited_forward:
            return degree_forward + degree_backward
        
        # Expandir los vecinos del actor actual en ambas direcciones
        if current_actor_forward not in visited_forward:
            visited_forward.add(current_actor_forward)
            neighbors_forward = cluster.get_neighbors(current_actor_forward)
            for neighbor_forward in neighbors_forward:
                if neighbor_forward not in visited_forward:
                    queue_forward.append((neighbor_forward, degree_forward + 1))
        
        if current_actor_backward not in visited_backward:
            visited_backward.add(current_actor_backward)
            neighbors_backward = cluster.get_neighbors(current_actor_backward)
            for neighbor_backward in neighbors_backward:
                if neighbor_backward not in visited_backward:
                    queue_backward.append((neighbor_backward, degree_backward + 1))
    
    return -1  # No se encontró una ruta entre los actores

def farthest_to_kevin_bacon(clusters, bacon_vertex):
    farthest_artist = ['nm0000102', 0]
    kevin_cluster = find_cluster(clusters, 'nm0000102')[1]
    #emprolijar
    vertices = kevin_cluster.get_vertices()
    print(f"En el cluster de Kevin hay {kevin_cluster.get_size()} elementos")
    progress_bar = tqdm(total=len(vertices), desc="Calculating shortest paths", unit="vertex")
    for vertex in vertices:
        progress_bar.update(1)
        if vertex != bacon_vertex:
            distance = calculate_degree_of_separation(kevin_cluster, bacon_vertex, vertex)
            if distance > farthest_artist[1]:
                farthest_artist = [vertex, distance]
    progress_bar.close()
    return farthest_artist

def third_exercise(clusters):
    farthest_artist = farthest_to_kevin_bacon(clusters,'nm0000102')
    print(f"→ El/la actor/actriz mas lejano/a a Kevin Bacon es {farthest_artist[0]} a una distancia de {farthest_artist[1]}")
    


def main():
    movies_by_id, actors_by_movie, actor_names_by_id = read_data(MOVIES_DATA_PATH, ACTORS_DATA_PATH, ACTORS_NAMES_PATH)
    graph = load_bipartite_graph(movies_by_id, actors_by_movie, actor_names_by_id)
    del movies_by_id
    del actors_by_movie
    del actor_names_by_id
    print("loading process ended succesfully ")
    clusters = bipartite_clustering(graph)
    print("clustering process ended succesfully ")
    # second_exercise (clusters)
    #el tercero necesita los clusters desordenados, porque si es una lista no puede buscar un elementos a que 
    #cluster pertenece
    third_exercise(clusters)

    
if __name__ == '__main__':
    main()