from graph import Graph
import csv
from itertools import combinations
import os
from tqdm import tqdm 
import random
import pickle
import heapq
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
        print("leyendo data")
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
    print("cargando data")

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

def dfs(graph, vertex, visited, components, component_id, component_graph):
    queue = [vertex]
    while queue:
        vertex = queue.pop()
        visited.add(vertex)
        component_graph.add_vertex(vertex)
        components[component_id] = component_graph
        for neighbor in graph.get_neighbors(vertex):
            if neighbor not in visited:
                queue.append(neighbor)
                component_graph.add_vertex(neighbor) 
                edge_data = graph.get_edge_data(vertex, neighbor)  # Obtener los datos de la arista del grafo original
                component_graph.add_edge(vertex, neighbor, edge_data)  # Agregar la arista al grafo del componente con los datos correspondientes # Agregar la arista al grafo del clúster

def find_connected_components(graph):
    visited = set()
    components = {}
    component_id = 0
    
    for vertex in graph.get_vertices():
        if vertex not in visited:
            component_id += 1
            component_graph = Graph()  # Crear un nuevo grafo para el clúster actual
            dfs(graph, vertex, visited, components, component_id, component_graph)
            components[component_id] = component_graph  # Agregar el grafo del clúster al diccionario de componentes
    
    return component_id, components

def first_exercise(amount_cl, clusters):
    sorted_components = sort_clusters(clusters)
    print("→ Cantidad de componentes conexas:", amount_cl)
    print(f"→ Segunda componente conexa más grande: {sorted_components[-2][0]}, con {sorted_components[-2][1].get_size()} elementos.")
    print(f"→ Componente más pequeña: {sorted_components[0][0]}, con {sorted_components[0][1].get_size()} elementos.")

def find_cluster(components, artist_id):
    for key, value in components.items():
        if value.vertex_exists(artist_id):
            return key, value
    raise ValueError("El artista no se encuentra en ningún clúster")

def find_distances(artist_id, cluster):
    distances = {artist: float('inf') for artist in cluster.get_vertices()}
    distances[artist_id] = 0
    queue = [(0, artist_id)]
    while queue:
        current_distance, current_artist = heapq.heappop(queue)
        if current_distance > distances[current_artist]:
            continue
        for neighbor in cluster.get_neighbors(current_artist):
            edge_data = len(cluster.get_edge_data(current_artist, neighbor))
            distance = current_distance + edge_data
            if distance < distances[neighbor]:
                distances[neighbor] = distance
                heapq.heappush(queue, (distance, neighbor))
    return distances

def get_artist(clusters,num):
    sorted_clusters = sort_clusters(clusters)
    random_cluster = sorted_clusters[num][1]#elijo uno de los ultimos clusters pq tienen mas elementos
    random_vertex = random.choice(random_cluster.get_vertices())
    return random_vertex,sorted_clusters[num][1]

def fourth_exercise(artist_id,clusters):
    cluster_id, cluster = find_cluster(clusters, artist_id)
    distances = find_distances(artist_id, cluster)
    print(f"Camino mínimo desde {artist_id} a todos los demás artistas del cluster {cluster_id}:")
    for artist, distance in distances.items():
        print(f"{artist}: {distance} colaboraciones")


def generate_clusters(clusters):
    cluster_min1 = get_artist(clusters,-2)[1]
    cluster_min2 = get_artist(clusters,31500)[1]
    cluster_min3 = get_artist(clusters,2)[1]
    return [cluster_min1,cluster_min2,cluster_min3]
    
def get_cluster_mean(cluster,n):
    shortest_paths = {}
    vertices = random.sample(cluster.get_vertices(), k=n)
    progress_bar = tqdm(total=len(vertices), desc="Calculating shortest paths", unit="vertex")
    start_time = time.time()
    for vertex in vertices:
        shortest_paths[vertex] = find_distances(vertex, cluster) #dijkstra
        progress_bar.update(1)
    elapsed_time = time.time() - start_time
    progress_bar.close()
    average_time = elapsed_time / len(vertices)
    l = cluster.get_size()
    mean = average_time*l
    print(f"Promedio del cluster con {l}  elementos →{mean}")
    return mean

def max_mean(clusters):
    #promedio tomando el promedio del cluster mas grande
    return get_cluster_mean(get_artist(clusters,-1)[1],2)

def min_mean(clusters):
    #promedio tomando el segundo cluster mas grande
    return get_cluster_mean(get_artist(clusters,-2)[1],2)

def intermediate_mean (clusters):
    #Promedio tomando tres clusters
    some_clusters = generate_clusters(clusters)
    mean = 0 
    for cluster in some_clusters:
        mean += get_cluster_mean(cluster,2)
    return (((mean / len(some_clusters)) * (len(clusters) - 1)) + max_mean(clusters))/2

def fifth_exercise(clusters):
    max_ =max_mean(clusters)
    min_ = min_mean(clusters)
    intermediate_ = intermediate_mean(clusters)
    print ("Aproximación de tiempo máxima→",(max_*len(clusters))/3600,"horas")
    print ("Aproximación de tiempo intermedia →",(intermediate_*len(clusters))/3600,"horas")
    print ("Aproximación de tiempo mínima→",(min_*len(clusters))/3600,"horas")
    return print ("Promedio de las aproximaciones→",(max_+min_+intermediate_)/3,"horas")


def main():
    movies_by_id, actors_by_movie, actor_names_by_id = read_data(MOVIES_DATA_PATH, ACTORS_DATA_PATH, ACTORS_NAMES_PATH)
    graph = load_graph(movies_by_id, actors_by_movie, actor_names_by_id)
    del movies_by_id
    del actors_by_movie
    del actor_names_by_id
    print("cargó el grafo")
    amount_cl, clusters = find_connected_components(graph)
    print("terminó el clustering")
    # first_exercise(amount_cl,clusters)
    # artist_id,cluster_min = get_artist(clusters)
    # fourth_exercise(artist_id,clusters)
    fifth_exercise( clusters)


if __name__=='__main__':
    main()