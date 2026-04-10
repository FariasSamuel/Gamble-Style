#ifndef CARTE_H
#define CARTE_H
#include <string>
// #include "Joueur.hpp"
#include "Case.hpp"


class Carte {
    //attribut
<<<<<<< HEAD
    protected: const std::string nom_carte;
             const std::string text;
            
    //methodes
    public: Carte(std::string nom_carte, std::string text);
            virtual void action();
=======
    private: const std::string nom_carte;
             const std::string text;
            
    //methodes
    public: Carte(string nom_carte, string text);
            std::string get_nom_carte();
            std::string get_nom_text();
            void action();
>>>>>>> b2acfafbc336c37558b23a8a560eaef08bd2cf57
};


#endif