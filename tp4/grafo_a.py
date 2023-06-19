from graph import Graph
import csv
from itertools import combinations
import os
from tqdm import tqdm 
import random
import pickle
import heapq

from collections import deque

import time

MOVIE_TITLE_TYPE = "movie"
MOVIE_COLUMNS = ["tconst", "titleType", "primaryTitle"]
PRINCIPALS_COLUMNS = ["nconst", "category"]
MOVIES_DATA_PATH = "tp4/datasets/title-basics-f.tsv"
ACTORS_DATA_PATH = "tp4/datasets/title-principals-f.tsv"
ACTORS_NAMES_PATH = "tp4/datasets/name-basics-f.tsv"


def read_data(movies_file, actors_file, actors_name_file):
    print("welcome")
    if os.path.exists('data.pickle'):
        print("reading data ...")
        try:
            with open('data.pickle', 'rb') as file:
                movies_by_id, actors_by_movie, actor_names_by_id = pickle.load(file)
                return movies_by_id, actors_by_movie, actor_names_by_id
        except Exception:
            pass
    movies_by_id = {}
    with open(movies_file, "r", newline="", encoding="utf-8") as file1:
        reader = csv.DictReader(file1, delimiter="\t")
        for row in reader:
            if row["titleType"] == MOVIE_TITLE_TYPE:
                movies_by_id[row['tconst']] = {'primaryTitle': row['primaryTitle']}

    actors_ids = set()
    actors_by_movie = {m: set() for m in movies_by_id.keys()}
    with open(actors_file, "r", newline="", encoding="utf-8") as file2:
        reader = csv.DictReader(file2, delimiter="\t")
        for row in reader:
            if row["tconst"] in actors_by_movie:
                actors_by_movie[row["tconst"]].update([row["nconst"]])
                actors_ids.update([row["nconst"]])

    actor_names_by_id = {}
    with open(actors_name_file, "r", newline="", encoding="utf-8") as file2:
        reader = csv.DictReader(file2, delimiter="\t")
        for row in reader:
            if row["nconst"] in actors_ids:
                actor_names_by_id[row["nconst"]] = row["primaryName"]

    with open('data.pickle', 'wb') as file:
        pickle.dump((movies_by_id, actors_by_movie, actor_names_by_id), file)

    return movies_by_id, actors_by_movie, actor_names_by_id

def load_graph(movies_by_id, actors_by_movie, actor_names_by_id) -> Graph:
    """
    Loads the graph
    :param movies_by_id: the movies data by id as dict
    :param actors_by_movie: the actors data by movie
    :param actor_names_by_id: the actors names by their ids
    :return: a Graph
    """
    graph = Graph()
    print("loading data in process....")

    for movie_id in movies_by_id.keys():
        movie_title = movies_by_id[movie_id]['primaryTitle']
        for actor1, actor2 in combinations(actors_by_movie[movie_id], 2):
            if not graph.vertex_exists(actor1):
                graph.add_vertex(actor1, actor_names_by_id.get(actor1, "ERROR"))
            if not graph.vertex_exists(actor2):
                graph.add_vertex(actor2, actor_names_by_id.get(actor2, "ERROR"))
            existing_data = set()
            if graph.edge_exists(actor1, actor2):
                existing_data = graph.get_edge_data(actor1, actor2)
            graph.add_edge(vertex1=actor1, vertex2=actor2,
                           data={movie_title} | existing_data)
    return graph
 
def sort_clusters(clusters):
    return sorted(clusters.items(), key=lambda x: x[1].get_size())

def dfs(graph, vertex, visited, clusters, cluster_id, cluster_graph):
    queue = [vertex]
    while queue:
        vertex = queue.pop()
        visited.add(vertex)
        cluster_graph.add_vertex(vertex)
        clusters[cluster_id] = cluster_graph
        for neighbor in graph.get_neighbors(vertex):
            if neighbor not in visited:
                queue.append(neighbor)
                cluster_graph.add_vertex(neighbor) 
                edge_data = graph.get_edge_data(vertex, neighbor)  # Obtener los datos de la arista del grafo original
                cluster_graph.add_edge(vertex, neighbor, edge_data)  # Agregar la arista al grafo del clustere con los datos correspondientes # Agregar la arista al grafo del clúster

def find_connected_clusters(graph):
    print("Clustering data in process...")
    visited = set()
    clusters = {}
    cluster_id = 0
    
    for vertex in graph.get_vertices():
        if vertex not in visited:
            cluster_id += 1
            cluster_graph = Graph()  # Crear un nuevo grafo para el clúster actual
            dfs(graph, vertex, visited, clusters, cluster_id, cluster_graph)
            clusters[cluster_id] = cluster_graph  # Agregar el grafo del clúster al diccionario de clusteres
    
    return cluster_id, clusters

def find_cluster(clusters, artist_id):
    for key, value in clusters.items():
        if value.vertex_exists(artist_id):
            return key, value
    raise ValueError("El artista no se encuentra en ningún clúster")
def get_artist(clusters,num):
    random_cluster = clusters[num][1]#elijo uno de los ultimos clusters pq tienen mas elementos
    random_vertex = random.choice(random_cluster.get_vertices())
    return random_vertex,clusters[num][1]

def generate_clusters(clusters):
    cluster_min1 = get_artist(clusters,-2,987)[1]
    cluster_min2 = get_artist(clusters,31500,987)[1]
    cluster_min3 = get_artist(clusters,2,987)[1]
    return [cluster_min1,cluster_min2,cluster_min3]


def average_distances(cluster):
    #distances es una lista con diccionarios para cada vertice recorrido
    distances,mean_time = get_distances_average_time(cluster,5)[0]
    avg_dist = []
    for artist in distances :
        sum_distances = sum(artist.values())
        total_vertices = len(artist)
        avg_dist.append(sum_distances / total_vertices)
    return (sum(avg_dist)/len(avg_dist)), mean_time


def dijkstra (artist_id, cluster):
    #diccionario con todas las ditancias minimas de un artista al resto del cluster
    start_time = time.time()
    distances = {artist: float('inf') for artist in cluster.get_vertices()}
    distances[artist_id] = 0
    queue = [(0, artist_id)]
    while queue:
        current_distance, current_artist = heapq.heappop(queue)
        if current_distance > distances[current_artist]:
            continue
        for neighbor in cluster.get_neighbors(current_artist):
            distance = current_distance + len(cluster.get_edge_data(current_artist, neighbor))
            if distance < distances[neighbor]:
                distances[neighbor] = distance
                heapq.heappush(queue, (distance, neighbor))
    elapsed_time = time.time() - start_time
    return distances,elapsed_time


def get_diameter (cluster):
    """ Given a cluster, the function visits all vertices
     of the graph and finds all its minimum paths and the 
     maximum of them for each vertex. Then, it returns the
     diameter of the graph and the time it would take to
     go through all the vertices to do this calculation."""
    vertices = cluster.get_vertices()
    visited_vertices = [0,0]
    start_time = time.time()
    progress_bar = tqdm(total=len(vertices), desc="Calculating shortest paths", unit="vertex")
    elapsed_time = time.time()- start_time
    while (elapsed_time < 900):
        dijkstra(vertex,cluster)
        max_distance = max(distances.values())
        progress_bar.update(1)
        if max_distance > visited_vertices[1]:
            visited_vertices[1] = max_distance
    progress_bar.close()
    average_time = elapsed_time / visited_vertices[0]
    mean = average_time * cluster.get_size()
    return visited_vertices[1],mean

def get_total_time(cluster,n):
    some_vertices = random.sample(cluster.get_vertices(), k=n)
    elapsed_time = 0
    for vertex in some_vertices:
        elapsed_time += (dijkstra(vertex,cluster)[1])
    return elapsed_time/n
    

def seventh_exercise(cluster):
    print("---------EJERCICIO 7--------")
    average_distance,mean_time = average_distances(cluster)
    print("→ Promedio de separaciones para todos los actores en general:", average_distance)
    print("→ Calcular la distancia promedio para todo el cluster llevaría:", mean_time/84600, " dias.")

def sixth_exercise(cluster):
    print ("---------EJERCICIO 6--------")
    diameter,mean= get_diameter(cluster)
    print("→En 15 minutos obtenemos un diámetro de =",diameter)
    print (f"→ Buscar el diámetro, recorriendo todos los vértices, llevaría {mean/84600} dias")

def fifth_exercise(clusters):
    print ("---------EJERCICIO 5--------")
    max_ = get_total_time(clusters[-1][1],100) [1]
    print("Aproximación de tiempo máxima →", (max_)/ 86400, "dias")

def fourth_exercise(actor_names_by_id,clusters,artist_id):
    print ("---------EJERCICIO 4--------")
    cluster_id, cluster = find_cluster(clusters, artist_id)
    distances = dijkstra (artist_id, cluster)[0]
    print(f"Camino mínimo desde {actor_names_by_id[artist_id]} a todos los demás artistas del cluster {cluster_id}:")
    for artist, distance in distances.items():
        print(f"{actor_names_by_id[artist]}: {distance} colaboraciones")

def first_exercise(amount_cl, clusters):
    print ("---------EJERCICIO 1--------")
    print("→ Cantidad de componentes conexas:", amount_cl)
    print(f"→ Segunda componente conexa más grande: {clusters[-2][0]}, con {clusters[-2][1].get_size()} elementos.")
    print(f"→ Componente conexa más pequeña: {clusters[0][0]}, con {clusters[0][1].get_size()} elementos.")


def main():
    movies_by_id, actors_by_movie, actor_names_by_id = read_data(MOVIES_DATA_PATH, ACTORS_DATA_PATH, ACTORS_NAMES_PATH)
    graph = load_graph(movies_by_id, actors_by_movie, actor_names_by_id)
    del movies_by_id
    del actors_by_movie
    print("Loading data ended successfully")
    amount_cl, clusters = find_connected_clusters(graph)
    sorted_clusters = sort_clusters(clusters)
    print("Clustering data ended successfully")
    artist_id = get_artist(sort_clusters(clusters),-3)[0]
    # first_exercise(amount_cl,sorted_clusters)
    fourth_exercise(actor_names_by_id,clusters,artist_id)
    # fifth_exercise(sorted_clusters)
    # sixth_exercise(sorted_clusters[-1][1])
    # seventh_exercise(sorted_clusters[-1][1])


if __name__=='__main__':
    main()