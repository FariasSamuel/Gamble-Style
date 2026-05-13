#ifndef CARTE_H
#define CARTE_H

#include <string>

class Carte {
public:
    Carte(const std::string& nom = "", const std::string& text = "");
    virtual ~Carte();

    virtual void action();

    std::string get_nom_carte() const;
    std::string get_nom_text() const;

private:
    std::string nom_carte;
    std::string text;
};

#endif // CARTE_H