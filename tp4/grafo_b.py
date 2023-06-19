from graph import Graph
from grafo_a import read_data
import numpy as np
import random
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

def bfs (visited,graph,queue,value):
    current_vertex, distance = queue.popleft()
    if current_vertex not in visited:
        visited.add(current_vertex)
        for neighbor in graph.get_neighbors(current_vertex):
            if neighbor not in visited:
                if current_vertex[:2] == 'tt':
                    queue.append((neighbor, distance+value))
                else:
                    queue.append((neighbor, distance))
    return current_vertex,distance

def calculate_distance(graph, start_vertex, target_vertex):
    visited = set()
    queue = deque([(start_vertex, 0)]) 
    while queue:
        current_vertex,distance = bfs(visited,graph,queue,1)
        if current_vertex == target_vertex:
            return distance
    return -1  

def find_longest_path(graph, start_vertex):
    visited = set()
    queue = deque([(start_vertex, 0)]) 
    longest_path = ([start_vertex], 0)
    while queue:
        current_vertex,distance = bfs(visited,graph,queue,1)
        if distance > longest_path[1]:
            longest_path = ([current_vertex], distance)
        elif distance == longest_path[1]:
            longest_path[0].append(current_vertex)
    return longest_path
    
def second_exercise(graph,actor_names_by_id):
    print ("---------EJERCICIO 2--------")
    actor1_id = 'nm0289856' 
    actor2_id = 'nm0000102'
    degree = calculate_distance(graph, actor1_id, actor2_id)
    if degree == -1:
        print("No se encontró una ruta entre los actores.")
    else:
        print(f"El grado de separación entre {actor_names_by_id[actor1_id]} y {actor_names_by_id[actor2_id]} es: {degree}")

def third_exercise(graph,actor_names_by_id):
    print ("---------EJERCICIO 3--------")
    farthest_artist,distance= find_longest_path(graph,'nm0000102')
    print("Los actores y actrices mas lejanos a Kevin Bacon son:")
    for actor in farthest_artist:
        print(f"→{actor_names_by_id[actor]}")
    print(f"a una distancia de {distance}")


def random_walks(graph, num_walks=100, walk_length=10):
    vertices = graph.get_vertices()
    centralities = {'actors': {actor: 0 for actor in vertices if actor[:2] =='nm'},
                    'movies': {movie: 0 for movie in vertices if movie[:2] =='tt'}}

    for _ in range(num_walks):
        for _ in range(walk_length):
            current_node = random.choice(vertices)
            neighbors = graph.get_neighbors(current_node)
            current_node = random.choice(neighbors)
            if current_node.startswith('nm'):
                centralities['actors'][current_node] += 1
            else:
                centralities['movies'][current_node] += 1

    # Normalizing centralities by the number of walks
    total_walks = num_walks * walk_length
    for node_type in centralities:
        for node in centralities[node_type]:
            centralities[node_type][node] /= total_walks

    return centralities

def extra_exercise (graph,actor_names_by_id,movies_by_id):
    centralities = random_walks(graph,1000,100)

    top_actor = max ( centralities['actors'].value())
    top_movie= max (centralities['movies'].value())

    print(f"El actor con mayor centralidad es {actor_names_by_id[top_actor]}")
    print(f"La película con mayor centralidad es {movies_by_id[top_movie]}")

def main():
    movies_by_id, actors_by_movie, actor_names_by_id = read_data(MOVIES_DATA_PATH, ACTORS_DATA_PATH, ACTORS_NAMES_PATH)
    graph = load_bipartite_graph(movies_by_id, actors_by_movie, actor_names_by_id)
    del actors_by_movie
    print("loading process ended succesfully ")
    
    second_exercise (graph,actor_names_by_id)
    third_exercise(graph,actor_names_by_id)
    extra_exercise(graph,actor_names_by_id,movies_by_id)

    
if __name__ == '__main__':
    main()