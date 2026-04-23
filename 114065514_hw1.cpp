#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <set>
#include <map>
#include <iomanip>

void ReadFile(std::string filename, std::vector<std::vector<int> >& input);
void WriteFile(std::string filename, std::map<std::set<int>, int>& output_map, int transaction_num);

struct Node {
    int item;
    int count;
    Node* next;
    Node* parent;
    std::unordered_map<int, Node*> children;
    std::map<std::set<int>, int> set_count;
    // std::unordered_map<std::string, int> set_count;

    Node(int data) : item(data), count(1), next(nullptr), parent(nullptr) {}
};

struct Tree{
    Node* root;
    std::unordered_map <int, Node*> header_table;
    std::vector<int> freq_items;
    std::unordered_map<int, int> freq;
    bool singlePath;
    // std::map<std::set<int>, int> result;

    Tree() : root(new Node(-1)), singlePath(true) {}
    void Build(std::vector<std::vector<int> >& input, int min_sup);
    void InsertAPath(std::vector<int> transaction, int path_count);
    void BuildConditionalTree(Node* root, Node* head, int min_sup);
    // void FP_Growth(int min_sup, std::ofstream& file, int transaction_num);
    void FP_Growth(std::vector<int>& base, int min_sup, std::ofstream& file, int trxs_size);
    void GenerateFrequentItemSet(Tree* tree, Node* root, int min_sup, std::ofstream& file, int transaction_num);
    void Traverse(Node* node);
};

int main(int argc, char** argv) {
    // argv = {min_sup, input_file, output_file}
    double fmin_sup = atof(argv[1]);
    std::string input_file = argv[2];
    std::string output_file = argv[3];

    // read file
    std::vector<std::vector<int> > input;
    Tree tree;
    ReadFile(input_file, input);
    int transaction_num = input.size();

    int min_sup = fmin_sup * input.size();
    tree.Build(input, min_sup);
    // open the output file
    std::ofstream file(output_file);
    if (!file) {
        std::cerr << "error\n";
        return 0;
    }
    // void FP_Growth(std::vector<int>& base, int min_sup, std::ofstream& file, int trxs_size, std::ostringstream& base_oss);
    std::vector<int> base;
    tree.FP_Growth(base, min_sup, file, transaction_num);
    // tree.Traverse(tree.root);

    // WriteFile(output_file, tree.result, transaction_num);
    return 0;
}

void Tree::Build(std::vector<std::vector<int> >& input, int min_sup) {
    // compute frequency
    for(auto transaction: input){
        for(auto item: transaction){
            if(freq.find(item) == freq.end()) {
                freq[item] = 1;
            } else {
                freq[item]++;
            }
        }
    }
    // find frequent items
    for(auto item: freq){
        if(item.second >= min_sup){
            freq_items.emplace_back(item.first);
        }
    }
    // sort frequent items by frequency
    std::sort(freq_items.begin(), freq_items.end(), [&](int a, int b){
        //if freq[a] == freq[b], sort by item
        if(freq[a] == freq[b]){
            return a < b;
        } else {
            return freq[a] > freq[b];
        }
    });
    // print freq_items
    // for(auto item: freq_items){
    //     std::cout << item << " " << freq[item] << "\n";
    // }
    // std::cout << "\n\n";

    for(auto transaction: input){
        std::vector<int> ordered_transaction;
        for(auto item: transaction){
            if(freq[item] >= min_sup){
                ordered_transaction.emplace_back(item);
            }
        }
        std::sort(ordered_transaction.begin(), ordered_transaction.end(), [&](int a, int b){
            return freq[a] > freq[b];
        });
        //print ordered_transaction
        // for(auto item: ordered_transaction){
        //     std::cout << item << " ";
        // }
        // std::cout << "\n";
        // insert ordered_transaction to tree
        InsertAPath(ordered_transaction, 1);
    }
    // std::cout << "\n\n";

}

void Tree::InsertAPath(const std::vector<int> transaction, int path_count = 1){
    Node* cur = root;
    for(auto item: transaction){
        auto child = cur->children.find(item);
        if(child != cur->children.end()){
            child->second->parent = cur;
            cur = child->second;
            cur->count+= path_count;
        }else{
            singlePath = false;
            Node* new_child = new Node(item);
            cur->children[item] = new_child;
            new_child->parent = cur;
            new_child->count = path_count;
            cur = new_child;

            if(header_table.find(item) == header_table.end()){
                header_table[item] = new_child;
            } else{
                Node* header = header_table[item];
                while(header->next != nullptr){
                    header = header->next;
                }
                header->next = new_child;
            }
        }
    }
}

void Tree::BuildConditionalTree(Node* preroot, Node* head, int min_sup){
    // compute frequency
    // std::cout << "head: " << head->item << "\n";
    for(Node* cur = head; cur != nullptr; cur = cur->next){
        // freq[cur->item] = cur->count;
        Node* cur2 = cur->parent;
        while(cur2->parent != nullptr){
            if(freq.find(cur2->item) == freq.end()){
                freq[cur2->item] = cur->count;
            } else{
                freq[cur2->item] += cur->count;
            }
            cur2 = cur2->parent;
        }
    }
    // print freq
    // std::cout << "freq\n";
    for(auto item: freq){
        // std::cout << item.first << " " << item.second << "\n";
        if(item.second >= min_sup){
            freq_items.emplace_back(item.first);
        }
    }

    std::sort(freq_items.begin(), freq_items.end(), [&](int a, int b){
        if(freq[a] == freq[b]){
            return a < b;
        } else {
            return freq[a] > freq[b];
        }
    });
    // print freq_items
    // for(auto item: freq_items){
    //     std::cout << item << " " << freq[item] << "\n";
    // }
    // std::cout << "\n\n";

    for(Node* leaf = head; leaf != nullptr; leaf = leaf->next){
        // get path
        std::vector<int> transaction;
        for(Node* parent = leaf->parent; parent->parent != nullptr; parent = parent->parent){
            // exclude leaf (base item)
            if(freq[parent->item] >= min_sup){
                transaction.emplace_back(parent->item);
            }
        }
        std::reverse(transaction.begin(), transaction.end());
        // print transaction
        // for(auto item: transaction){
        //     std::cout << item << " ";
        // }
        // std::cout << "\n";
        InsertAPath(transaction, leaf->count);
    }
}

// void Tree::FP_Growth(int min_sup, std::ofstream& file, int transaction_num){
//     if(header_table.empty()){
//         return;
//     }
//     std::set<std::set<int> > freq_item_set;
//     for(auto item: header_table){
//         // std::cout << "base item: " << item.first << "\n";
//         Tree conditional_tree;
//         conditional_tree.BuildConditionalTree(root, item.second, min_sup);
//         // conditional_tree.Traverse(conditional_tree.root);

//         conditional_tree.GenerateFrequentItemSet(this, conditional_tree.root, min_sup, file, transaction_num);
//         for(auto item_set: conditional_tree.root->set_count){
//             if(item_set.second >= min_sup){
//                 // result.insert(item_set.first);
//                 // result[item_set.first] = item_set.second;

//                 // write to file
//                 file << *item_set.first.begin();
//                 for(auto it = ++item_set.first.begin(); it != item_set.first.end(); ++it){
//                     file << "," << *it;
//                 }
//                 file << ":" << std::fixed << std::setprecision(4) << (double)item_set.second / transaction_num << "\n";
//             }
//         }
//     }
// }



void Tree::FP_Growth(std::vector<int>& base, int min_sup, std::ofstream& file, int trxs_size){
    for(int i = freq_items.size()-1; i>=0; i--){
        int base_item = freq_items[i];
        base.emplace_back(base_item);

        file << base[0];
        // file.flush();
        for(int j = 1; j < base.size(); j++){
            file << "," << base[j];
        }
        file << ":" << std::fixed << std::setprecision(4) << (double)freq[base_item]/trxs_size << "\n";
        file.flush();

        // build conditional tree
        Tree conditional_tree;
        conditional_tree.BuildConditionalTree(root, header_table[base_item], min_sup);
        if(!conditional_tree.header_table.empty()){
            conditional_tree.FP_Growth(base, min_sup, file, trxs_size);
        }
        base.pop_back();
    }
    
}


void Tree::GenerateFrequentItemSet(Tree* tree, Node* root, int min_sup, std::ofstream& file, int transaction_num){
    //print
    // std::cout << "root: " << root->item << "\n";
    // std::cout << "size: " << freq_item_sets.size() << "\n";

    if(root->children.empty()){
        // leaf
        if(root->item == -1){
            return;
        }
        root->set_count[{root->item}] = root->count;
        return;
        // std::string item_set = std::to_string(root->item);
        // file << item_set << ":" << std::fixed << std::setprecision(4) << (double)root->count / transaction_num << "\n";
        // return;
    }
    for (auto child : root->children) {
        GenerateFrequentItemSet(tree, child.second, min_sup, file, transaction_num);

        for (auto item_set : child.second->set_count) {
            root->set_count[item_set.first] += item_set.second;
            std::set<int> new_set = item_set.first;
            if (root->item != -1) {
                new_set.insert(root->item);
                root->set_count[new_set] += item_set.second;
            }
        }
    }
}



void Tree::Traverse(Node* node){
    if(node->children.empty()){
        std::cout << "leaf\n";
        return;
    }
    for(auto child: node->children){
        std::cout << child.first << " count: " << child.second->count << "\n";
        Traverse(child.second);
    }
}

void ReadFile(std::string filename, std::vector<std::vector<int> >& input) {
    std::ifstream file(filename);
    if (!file) {
        std::cerr << "error\n";
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::vector<int> row;
        std::stringstream ss(line);
        std::string number;
        while (getline(ss, number, ',')) {
            if (!number.empty()) {
                row.emplace_back(stoi(number));
            }
        }
        input.emplace_back(row);
    }

    //print
    // for (int i = 0; i < input.size(); i++) {
    //     for (int j = 0; j < input[i].size(); j++) {
    //         std::cout << input[i][j] << " ";
    //     }
    //     std::cout << "\n";
    // }

    file.close();
    return;
}

void WriteFile(std::string filename, std::map<std::set<int>, int>& output, int transaction_num){
    std::ofstream file(filename);
    if (!file) {
        std::cerr << "error\n";
        return;
    }
    for(auto item_set: output){
        for(auto item: item_set.first){
            file << item;
            if(item != *item_set.first.rbegin()){
                file << ",";
            }
        }
        // write count
        file << ":" << std::fixed << std::setprecision(4) << (double)item_set.second / transaction_num << "\n";
    }
}
