#ifndef CASE_H
#define CASE_H
#include <string>

class Case {
    //attribut
    private: const int num_case;  // private ou protected ?? 
            
    //methodes
    public: Case(int num_case);
            int get_num_case();
            virtual void action();
};


#endif