#include <vector>
#include <stdexcept>
using namespace std;

const int maxColision = 3;
const float maxFillFactor = 0.8;

template<typename TK, typename TV>
struct ChainHashNode {
    TK key;
    TV value;
    size_t hashcode;
    ChainHashNode* next;

    ChainHashNode(TK k, TV v, size_t h) : key(k), value(v), hashcode(h), next(nullptr) {}
};

template<typename TK, typename TV>
class ChainHashListIterator {
private:
    ChainHashNode<TK, TV>* current;
public:
    ChainHashListIterator(ChainHashNode<TK, TV>* node) : current(node) {}

    ChainHashNode<TK, TV>& operator*() { return *current; }
    ChainHashNode<TK, TV>* operator->() { return current; }

    ChainHashListIterator& operator++() {
        if (current) current = current->next;
        return *this;
    }

    bool operator!=(const ChainHashListIterator& other) const {
        return current != other.current;
    }
};

template<typename TK, typename TV>
class ChainHash {
private:
    typedef ChainHashNode<TK, TV> Node;
    typedef ChainHashListIterator<TK, TV> Iterator;

    Node** array;  
    int nsize; 
    int capacity; 
    int *bucket_sizes; 
    int usedBuckets; 

public:
    ChainHash(int initialCapacity = 10){
        this->capacity = initialCapacity; 
        this->array = new Node*[capacity]();  
        this->bucket_sizes = new int[capacity]();
        this->nsize = 0;
        this->usedBuckets = 0;
    }

    TV get(TK key){
        size_t hashcode = getHashCode(key);
        size_t index = hashcode % capacity;
        
        Node* current = this->array[index];
        while(current != nullptr){
            if(current->key == key) return current->value;
            current = current->next;
        }
        throw std::out_of_range("Key no encontrado");
    }
    
    int size(){ return this->nsize; }    

    int bucket_count(){ return this->capacity; }

    int bucket_size(int index) { 
        if(index < 0 || index >= this->capacity) throw std::out_of_range("Indice de bucket invalido");
        return this->bucket_sizes[index]; 
    }   
    
    void set(TK key, TV value){
        size_t hashcode = getHashCode(key);
        size_t index = hashcode % capacity;
        Node* current = array[index];
		
        while(current){
            if(current->key == key){
                current->value = value;
                return;
            }
            current = current->next;
        }

        Node* newNode = new Node(key, value, hashcode);
        newNode->next = array[index];
        array[index] = newNode;

        nsize++;
        if(bucket_sizes[index] == 0) usedBuckets++;
        bucket_sizes[index]++;

        if(bucket_sizes[index] > maxColision || fillFactor() > maxFillFactor){
            rehashing();
        }
    }

    bool remove(TK key){
        size_t hashcode = getHashCode(key);
        size_t index = hashcode % capacity;
        Node* current = array[index];
        Node* prev = nullptr;

        while(current){
            if(current->key == key){
                if(prev) prev->next = current->next;
                else array[index] = current->next;

                delete current;
                nsize--;
                bucket_sizes[index]--;
                if(bucket_sizes[index] == 0) usedBuckets--;
                return true;
            }
            prev = current;
            current = current->next;
        }
        return false;
    }

    bool contains(TK key){
        size_t hashcode = getHashCode(key);
        size_t index = hashcode % capacity;
        Node* current = array[index];
        while(current){
            if(current->key == key) return true;
            current = current->next;
        }
        return false;
    }

    Iterator begin(int index){
        if(index < 0 || index >= capacity) throw std::out_of_range("Indice invalido");
        return Iterator(array[index]);
    }

    Iterator end(int index){
        return Iterator(nullptr);
    }

private:
    double fillFactor(){
        return (double)this->usedBuckets / (double)this->capacity;
    }   

    size_t getHashCode(TK key){
        std::hash<TK> ptr_hash;
        return ptr_hash(key);
    }

    void rehashing(){
        int oldCapacity = capacity;
        capacity *= 2;

        Node** oldArray = array;
        int* oldBucketSizes = bucket_sizes;

        array = new Node*[capacity]();
        bucket_sizes = new int[capacity]();
        nsize = 0;
        usedBuckets = 0;

        for(int i = 0; i < oldCapacity; i++){
            Node* current = oldArray[i];
            while(current){
                set(current->key, current->value);
                Node* toDelete = current;
                current = current->next;
                delete toDelete;
            }
        }

        delete[] oldArray;
        delete[] oldBucketSizes;
    }

public:
    ~ChainHash(){
        for(int i = 0; i < capacity; i++){
            Node* current = array[i];
            while(current){
                Node* toDelete = current;
                current = current->next;
                delete toDelete;
            }
        }
        delete[] array;
        delete[] bucket_sizes;
    }
};
