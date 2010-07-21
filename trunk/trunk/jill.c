/*
* Contain artifacts to examin specific feature of compiler parser/semantic checker
*/

// check enum redefinition
enum e1 {A};
enum e1 {B}; // should report redefinition here

enum a;
enum a; // should be ok
struct a; // report redefinition here

struct b;
struct b; // should be ok
enum b; // should report redefinition here

int main()
{
    return 0;
}