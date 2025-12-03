#ifndef ENGINE_H
#define ENGINE_H

#include <iostream>   
#include <vector>     
#include "BST.h"      
#include "Record.h"
#include <algorithm>
//add header files as needed

using namespace std;

// Converts a string to lowercase (used for case-insensitive searches)
static inline string toLower(string s) {
    for (char &c : s) c = (char)tolower((unsigned char)c);
    return s;
}

// ================== Index Engine ==================
// Acts like a small "database engine" that manages records and two BST indexes:
// 1) idIndex: maps student_id → record index (unique key)
// 2) lastIndex: maps lowercase(last_name) → list of record indices (non-unique key)
struct Engine {
    vector<Record> heap;                  // the main data store (simulates a heap file)
    BST<int, int> idIndex;                // index by student ID
    BST<string, vector<int>> lastIndex;   // index by last name (can have duplicates)

    // Inserts a new record and updates both indexes.
    // Returns the record ID (RID) in the heap.
    int insertRecord(const Record &recIn) {
        heap.push_back(recIn);

        int recordId = heap.size() - 1;

        vector<int>* vec = lastIndex.find(toLower(recIn.last));
        if(vec) vec -> push_back(recordId);
        else lastIndex.insert(toLower(recIn.last), vector<int>{recordId});

        // idIndex.insert(recIn.id, recordId); // key, value
        // lastIndex.insert(recIn.last, recordId);

        return recordId;
    }

    // Deletes a record logically (marks as deleted and updates indexes)
    // Returns true if deletion succeeded.
    bool deleteById(int id) {
        idIndex.resetMetrics(); // reset counters
        int* pointer = idIndex.find(id);
        if(!pointer) return false;

        int recordId = *pointer;
        Record& rec = heap[recordId]; // get record
        if(rec.deleted) return false;

        rec.deleted = true; // delete otherwise

        idIndex.erase(id);

        // -----------------------
        vector<int>* vec = lastIndex.find(rec.last);
        if (vec) {
            vec->erase(
                std::remove(vec->begin(), vec->end(), recordId), 
                vec->end()
            );

            if(vec -> empty()) lastIndex.erase(rec.last);
        }

        return true;
    }

    // Finds a record by student ID.
    // Returns a pointer to the record, or nullptr if not found.
    // Outputs the number of comparisons made in the search.
    const Record *findById(int id, int &cmpOut) {
        idIndex.resetMetrics();

        int* pointer = idIndex.find(id);
        cmpOut = idIndex.comparisons;

        if (!pointer) return nullptr;

        // int recordId = *pointer;
        // if (heap[recordId].deleted) return nullptr;

        return &heap[*pointer]; 
    }

    // Returns all records with ID in the range [lo, hi].
    // Also reports the number of key comparisons performed.
    vector<const Record *> rangeById(int lo, int hi, int &cmpOut) {
        vector<const Record*> results;

        idIndex.resetMetrics();
        idIndex.rangeApply(
            lo, 
            hi, 
            [&](const int& key, const int& rid) {
                if (!heap[rid].deleted) results.push_back(&heap[rid]);
            }
        );

        cmpOut = idIndex.comparisons;
        return results;
    }

    // Returns all records whose last name begins with a given prefix.
    // Case-insensitive using lowercase comparison.
    vector<const Record *> prefixByLast(const string &prefix, int &cmpOut) {
        //TODO
    }
};

#endif
