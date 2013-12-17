  #include <iostream>
  #include <string>
  using std::cout; using std::endl; using std::string;

  class C {
    string name;
  public:
    C(string name) : name(name) { cout << name << " is constructed" << endl; }
    ~C() { cout << name << " is destructed" << endl; }
  };

  int main() {
    C a("a");
    {
      C b("b");
      {
        C c("c");
      }    // c is out of scope, and de-allocated
    }    // b is out of scope, and de-allocated
    return 0;
  }    // a is out of scope, and de-allocated

