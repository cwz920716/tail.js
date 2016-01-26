#include <iostream>
#include <fstream>

using namespace std;

int main(void) {
  ofstream out("dot_wrap_out.dot");
  ifstream in("/tmp/pedg.dot");
  out << "strict digraph EDG {\n";
  out << "graph [ splines = false ]\n";
  string line;
  while (getline(in, line)) {
    out << line << endl;
  }
  out << "}\n";
  out.close();
}
