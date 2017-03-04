/* Parser.hpp */
#include <string>

using namespace std;

class Attribute{
public:
    string name;
    string type;
    bool isPrimaryKey;
    
    Attribute(){
        name = "";
        type = "";
        isPrimaryKey = false;
    }
};

class Parser{
    void Parse(Scanner scanner)
    bool Create_Table_Query(Scanner scanner);
    bool Insert_Query(Scanner scanner);
};


