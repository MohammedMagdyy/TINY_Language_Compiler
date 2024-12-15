#include <iostream>
#include <fstream>
#include "Scanner.h"
#include "Parser.h"
#include "TreeNode.h"

using namespace std;

//This Function deleted after made GUI
void display_tree(const shared_ptr<TreeNode> &node, const string &prefix = "", bool isLast = true) {
    if (!node) return;

    // Determine connector style for ASCII
    string connector = isLast ? "\\-- " : "|-- ";

    // Display node name and value
    cout << prefix << connector << node->name;
    if (!node->value.empty()) {
        cout << " (" << node->value << ")";
    }
    cout << endl;

    // Prepare prefix for child nodes
    string childPrefix = prefix + (isLast ? "    " : "|   ");

    // Recursively display children
    for (size_t i = 0; i < node->children.size(); ++i) {
        bool lastChild = (i == node->children.size() - 1);
        display_tree(node->children[i], childPrefix, lastChild);
    }

    // Recursively display siblings
    for (size_t i = 0; i < node->siblings.size(); ++i) {
        display_tree(node->siblings[i], prefix, (i == node->siblings.size() - 1));
    }
}

void print_tree_details(const shared_ptr<TreeNode>& node) {
    if (!node) return;

    // Print the current node's name and value
    cout << "Node: " << node->name;
    if (!node->value.empty()) {
        cout << ", Value: " << node->value;
    }
    cout << endl;

    // Print children details
    if (!node->children.empty()) {
        cout << "  Children of " << node->name << ":" << endl;
        for (const auto& child : node->children) {
            cout << "    - " << child->name;
            if (!child->value.empty()) {
                cout << " (Value: " << child->value << ")";
            }
            cout << endl;
        }
    }

    // Print sibling details
    if (!node->siblings.empty()) {
        cout << "  Siblings of " << node->name << ":" << endl;
        for (const auto& sibling : node->siblings) {
            cout << "    - " << sibling->name;
            if (!sibling->value.empty()) {
                cout << " (Value: " << sibling->value << ")";
            }
            cout << endl;
        }
    }

    cout << endl << "-------------------------------------" << endl;

    // Recursively print children
    for (const auto& child : node->children) {
        print_tree_details(child);
    }

    // Recursively print siblings
    for (const auto& sibling : node->siblings) {
        print_tree_details(sibling);
    }
}


int main() {
    string inputFile = R"(D:\Mohanad.Khh\ASU Senior 1 - Fall '23\Design Of Compilers\Input_File.txt)";
    string outputFile = R"(D:\Mohanad.Khh\ASU Senior 1 - Fall '23\Design Of Compilers\Output_File.txt)";


    ifstream inFile(inputFile);
    ofstream outFile(outputFile);

    try {
        if (!inFile.is_open()) {
            throw runtime_error("Error: Could not open input file.");
        }

        if (!outFile.is_open()) {
            throw runtime_error("Error: Could not open output file.");
        }

        int lineCount = 1;
        string line;
        vector<Token> outputTokens;

        // Read input file line by line
        while (getline(inFile, line)) {
            vector<Token> tokens = tokenize(line, lineCount);
            outputTokens.insert(outputTokens.end(), tokens.begin(), tokens.end());
            lineCount++;
        }

        // Write tokens to the output file
        for (const auto &token: outputTokens) {
            outFile << token.value << "," << token.type << endl;
        }

        // Parse tokens
        Parser parser(outputTokens);
        auto syntaxTree = parser.parse();

        // Display the syntax tree
        print_tree_details(syntaxTree);
        cout << endl << endl;
        display_tree(syntaxTree);

    } catch (const runtime_error &e) {
        // Handle any runtime error by printing the message
        cerr << e.what() << endl;
        return 1; // Indicating failure
    }

    // Close files
    inFile.close();
    outFile.close();

    return 0; // Exit successfully
}