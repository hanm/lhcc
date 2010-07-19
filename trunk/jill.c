/*
* Contain artifacts to examin specific feature of compiler parser/semantic checker
*/

// check enum redefinition
enum e1 {A};
enum e1 {B}; // should report redefinition here

int main()
{
    return 0;
}