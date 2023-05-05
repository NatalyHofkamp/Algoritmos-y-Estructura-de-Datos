# #%%
# def es_palindromo(palabra):
#     pila=list(palabra)
#     for letra in pila:
#         if(letra!=pila.pop()):
#             return print(f"{palabra} no es palindromo");
#     return print(f"{palabra} es palindromo");

# es_palindromo("neuquen");
# es_palindromo("mendoza");

#%% Implementar un programa que lea una secuencia de letras hasta llegar a un
# punto e imprima los caracteres ingresados en orden inverso.

# def inv (palabra,stop):
#     pila=[]
#     for i in range(stop):
#         pila.append(palabra[i])
#     while(len(pila)>0):
#         print(pila.pop())

# inv("mañanita",3)
# inv("casita",5)

# %% que verifica si los elementos de la pila están ordenados en orden decreciente

# def stack_sorted_descending(pila):
#     for elemento in pila:
#         if (elemento<pila.pop()):
#             return print("no es decendiente!")
#     return print("es decendiente :)")

# stack_sorted_descending([1,2,3,4,5,6,7])
# stack_sorted_descending([7,6,5,4,3,2,1])

#%%