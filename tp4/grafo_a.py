from graph import Graph
import csv
from itertools import combinations
import os
from tqdm import tqdm 
import random
import pickle
import heapq
import numpy as np
from collections import deque

import time

MOVIE_TITLE_TYPE = "movie"
MOVIE_COLUMNS = ["tconst", "titleType", "primaryTitle"]
PRINCIPALS_COLUMNS = ["nconst", "category"]
MOVIES_DATA_PATH = "tp4/datasets/title-basics-f.tsv"
ACTORS_DATA_PATH = "tp4/datasets/title-principals-f.tsv"
ACTORS_NAMES_PATH = "tp4/datasets/name-basics-f.tsv"


def read_data(movies_file, actors_file, actors_name_file):
    if os.path.exists('data.pickle'):
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
    """
    Sorts the clusters based on their sizes in ascending order.
    """
    return sorted(clusters.items(), key=lambda x: x[1].get_size())

def clustering (graph):
    """
    Performs clustering on a graph to identify disjoint clusters.
    """
    visited = set()
    clusters = {}
    cluster_id = 0
    
    for vertex in graph.get_vertices():
        if vertex not in visited:
            cluster_id += 1
            cluster_graph = Graph() 
            dfs(graph, vertex, visited, clusters, cluster_id, cluster_graph)
            clusters[cluster_id] = cluster_graph 
    
    return cluster_id, clusters

def dfs(graph, vertex, visited, clusters, cluster_id, cluster_graph):
    """
    Performs a depth-first search (DFS) traversal on 
    a graph starting from a given vertex.
    """
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
                edge_data = graph.get_edge_data(vertex, neighbor)  
                cluster_graph.add_edge(vertex, neighbor, edge_data) 

def find_cluster(clusters, artist_id):
    """
    Finds the cluster in which the specified artist is present.
    """
    for key, value in clusters.items():
        if value.vertex_exists(artist_id):
            return key, value
    raise ValueError("El artista no se encuentra en ningún clúster")

def get_artist(cluster):
    """
    Retrieves a random artist from the given cluster.
    """
    return random.choice(cluster.get_vertices())

def dijkstra (artist_id, cluster):
    """
    Calculates the shortest distances from a
    given artist_id to all other vertices in
    the cluster using Dijkstra's algorithm.
    """
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
    return distances

def average_distances(cluster):
    """
    Calculates the average distances within
    a cluster using a random sample of actors.
    """
    actors = random.sample(cluster.get_vertices(), k=5)
    total_distances = []
    actor_distances = {}

    for actor in actors:
        distances = bfs(cluster, actor)[1]
        total_distances.extend(list(distances.values()))
        actor_distances[actor] = distances

    average_all = np.mean(total_distances)
    average_actors = {actor: np.mean(list(distances.values())) for actor, distances in actor_distances.items()}

    return average_actors, average_all

def bfs(cluster, start_vertex):
    """
    Performs a breadth-first search (BFS) 
    traversal in a cluster or graph starting
    from a given start_vertex.

    """
    visited = set()
    queue = deque([(start_vertex, 0)])
    max_distance = 0
    distances = {start_vertex: 0}
    while queue:
        current_vertex, distance = queue.popleft()
        if current_vertex not in visited:
            visited.add(current_vertex)
            max_distance = max(max_distance, distance)

            for neighbor in cluster.get_neighbors(current_vertex):
                if neighbor not in visited:
                    queue.append((neighbor, distance + 1))
                    distances[neighbor] = distance + 1

    return max_distance,distances

def get_diameter(cluster, time_limit=100):
    """
    Calculates the diameter and mean time 
    of the cluster using a breadth-first
    search (BFS) approach.
     """
    diameter, elapsed_time, i = 0, 0, 0
    start_time = time.time()
    vertices = cluster.get_vertices()
    while (elapsed_time < time_limit) and (i < len(vertices)):
        distance = bfs(cluster, vertices[i])[0]
        diameter = max(diameter, distance)
        i += 1
        elapsed_time = time.time() - start_time

    mean_time = (elapsed_time / i) * len(vertices)
    return diameter, mean_time, i

def get_distances(cluster,n):
    """
    Calculates the average time taken to
    calculate shortest distances using
    Dijkstra's algorithm for a given number
    of random vertices.

    """
    vertices = cluster.get_vertices()
    start_time = time.time()
    for _ in range(n): 
        vertex = random.choice(vertices)
        dijkstra(vertex, cluster)
    return (time.time() - start_time)/ n 


def mean_time(clusters, graph):
    """
    Calculates the mean time based
    on the average distances computed
    using Dijkstra's algorithm for 
    different clusters.

    """
    max_time = get_distances(clusters[-1][1],10)
    intermediate_time = get_distances(clusters[-2][1],10)
    min_time = get_distances(clusters[2][1],2)
    return ((max_time + intermediate_time + min_time) / 3) * graph.get_size()

def seventh_exercise(cluster,actor_names_by_id):
    print("---------EJERCICIO 7--------")
    average_actors, average_all = average_distances(cluster)
    for actor in average_actors:
        print(f"→ Promedio de separaciones para {actor_names_by_id[actor]}:", average_actors[actor])
    print("→ Estimación del promedio de separaciones para todos los actores:", average_all)

def sixth_exercise(cluster):
    print ("---------EJERCICIO 6--------")
    diameter,mean,visited= get_diameter(cluster,900)
    print("→En 15 minutos estimamos un diámetro de =",diameter)
    print (f"→ Buscar el diámetro, recorriendo todos los vértices, llevaría {mean/84600} dias")

def fifth_exercise(clusters,graph):
    print ("---------EJERCICIO 5--------")
    mean_ = mean_time(clusters,graph)
    print("Aproximación de tiempo máxima →", (mean_)/ 84600, "dias")

def fourth_exercise(actor_names_by_id,clusters,artist_id):
    print ("---------EJERCICIO 4--------")
    cluster_id, cluster = find_cluster(clusters, artist_id)
    distances = dijkstra (artist_id, cluster)
    print(f"Camino mínimo desde {actor_names_by_id[artist_id]} a todos los demás artistas del cluster {cluster_id}:")
    for artist, distance in distances.items():
        print(f"{actor_names_by_id[artist]}: {distance} películas")

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

    amount_cl, clusters = clustering (graph)
    sorted_clusters = sort_clusters(clusters)
    artist_id = get_artist(sort_clusters(clusters)[-3][1])

    first_exercise(amount_cl,sorted_clusters)
    fourth_exercise(actor_names_by_id,clusters,artist_id)
    fifth_exercise(sorted_clusters,graph)
    sixth_exercise(sorted_clusters[-1][1])
    seventh_exercise(sorted_clusters[-1][1],actor_names_by_id)


if __name__=='__main__':
    main()