from graph import Graph
import csv
from itertools import combinations
import os
from tqdm import tqdm 
import random
import pickle
import heapq
from grafo_a import read_data
import time

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
    actor_vertices = {}

    for movie_id in movies_by_id.keys():
        movie_title = movies_by_id[movie_id]['primaryTitle']
        movie_vertex = graph.add_vertex(movie_id, movie_title)

        for actor_id in actors_by_movie[movie_id]:
            if actor_id not in actor_vertices:
                actor_name = actor_names_by_id.get(actor_id, "ERROR")
                actor_vertex = graph.add_vertex(actor_id, actor_name)
                actor_vertices[actor_id] = actor_vertex
            else:
                actor_vertex = actor_vertices[actor_id]
            
            graph.add_edge(movie_vertex, actor_vertex)

    return graph
  

def main():
    movies_by_id, actors_by_movie, actor_names_by_id = read_data(MOVIES_DATA_PATH, ACTORS_DATA_PATH, ACTORS_NAMES_PATH)
    graph = load_bipartite_graph(movies_by_id, actors_by_movie, actor_names_by_id)
    del movies_by_id
    del actors_by_movie
    del actor_names_by_id
    print("cargó el grafo")
    graph.print_graph()

if __name__ == '__main__':
    main()