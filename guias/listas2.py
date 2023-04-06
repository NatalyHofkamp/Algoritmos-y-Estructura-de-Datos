import operator as ops
import sys
from dataclasses import dataclass
from typing import Optional, TypeVar


@dataclass
class Node:
    _Self = TypeVar('_Self', bound='A[T]')
    value: int
    next_node: Optional[_Self] = None
    
@dataclass
class List:
    head: Optional[Node] = None
    size: int = 0
    
    def __getitem__(self, idx):
        curr = self.head
        for _ in range(idx):
            curr = curr.next_node
        return curr.value
    
    def __len__(self):
        return self.size
    
    def __eq__(self, other):
        if len(self) != len(other):
            return False
        for i in range(len(self)):
            if self[i] != other[i]:
                return False
        return True
    
    def __repr__(self):
        values = []
        curr = self.head
        while curr != None:
            values.append(str(curr.value))
            curr = curr.next_node
        return " ".join(values)
    
    def __str__(self):
        return repr(self)


def append_to_list(l: List, value: int):
    new_node = Node(value=value, next_node=None)
    
    if l.size == 0:
        l.head = new_node
    else:
        curr = l.head
        while curr.next_node != None:
            curr = curr.next_node
        curr.next_node = new_node
        
    l.size += 1

def reverse_list(l: List):
    HeadPointer= List.head
    while(HeadPointer):
        HeadPointer.next_node.next_node=HeadPointer
    List.head=HeadPointer;
    return

if __name__ == "__main__":
    l = List()
    args = input()
    for arg in args.split(" "):
        append_to_list(l, int(arg))
    
    reverse_list(l)
    print(l)
