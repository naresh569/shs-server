
#pragma once

class Node {
    public:
        char data;
        Node *next;
        Node(char c);

};

Node :: Node(char c){
    data = c;
    next = NULL;
}

class Storage {
	private:
		Node *store;
		int length;
		
		Node *current;
		int index;
	public:
	    Storage();
        void write(char);
        char read();
        int available();
};

Storage :: Storage(){
    store = NULL;
    current = NULL;
    length = 0;
    index = 0;
}

void Storage :: write(char c){
    Node *nw = new Node (c);
    if(store == NULL){
        store = nw;
        length++;
        return;
    }

    Node *temp = store;
    while(temp->next != NULL){
        temp = temp->next;
    }
    temp->next = nw;
    length++;
}

int Storage :: available(){
    return length - index;
}

char Storage :: read(){
    
    if(store == NULL){
        return '\0';
    }
    
    if(current == NULL){
        current = store;
    }
    
    char result = current->data;
    current = current->next;
    index++;
    return result;
    
}

