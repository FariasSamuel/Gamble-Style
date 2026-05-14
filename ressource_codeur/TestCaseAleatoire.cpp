/**
 * @file TestCaseAleatoire.cpp
 * @brief Tests unitaires de CaseAleatoire (CppUnit).
 * @project GambleStyle — GM4 INSA Rouen Normandie
 */
#include <iostream>
#include <list>
#include <cppunit/TestCase.h>
#include <cppunit/TestFixture.h>
#include <cppunit/ui/text/TextTestRunner.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/XmlOutputter.h>
#include "CaseAleatoire.hpp"
#include "CarteDeplacement.hpp"
#include "Joueur.hpp"

using namespace CppUnit;
using namespace std;

class TestCaseAleatoire : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestCaseAleatoire);
    CPPUNIT_TEST(testActionPiochePremiereCarte);          // [N] existant
    CPPUNIT_TEST(testActionDeplacePremierCarteEnFin);     // [N] existant
    CPPUNIT_TEST(testActionAppliqueEffetCarte);           // [N] existant
    CPPUNIT_TEST(testAction_UneSeuleCarte_Circulaire);    // [L] nouveau
    CPPUNIT_TEST(testAction_ListeVide_SansPlanter);       // [E] nouveau
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);
    void tearDown(void);

protected:
    /** [N] existant — c'est la 1ère carte qui s'applique (case 5). */
    void testActionPiochePremiereCarte(void);
    /** [N] existant — après action(), la carte est en fin de file. */
    void testActionDeplacePremierCarteEnFin(void);
    /** [N] existant — 2e appel applique la 2e carte (case 10). */
    void testActionAppliqueEffetCarte(void);
    /** [L] nouveau — 1 seule carte : réutilisée indéfiniment, sans crash. */
    void testAction_UneSeuleCarte_Circulaire(void);
    /** [E] nouveau — liste vide : action() ne plante pas. */
    void testAction_ListeVide_SansPlanter(void);

private:
    CaseAleatoire *mCaseAleatoire;
    Joueur        *mJoueur;
    Carte         *mCarte1;
    Carte         *mCarte2;
};

void TestCaseAleatoire::setUp(void)
{
    mJoueur        = new Joueur("Alice", 1500);
    mCarte1        = new CarteDeplacement(mJoueur, nullptr, 5);
    mCarte2        = new CarteDeplacement(mJoueur, nullptr, 10);
    mCaseAleatoire = new CaseAleatoire();
    mCaseAleatoire->ajouterCarte(mCarte1);
    mCaseAleatoire->ajouterCarte(mCarte2);
    mCaseAleatoire->setJoueurActif(mJoueur);
}

void TestCaseAleatoire::tearDown(void)
{
    delete mCaseAleatoire;
    delete mJoueur;
    delete mCarte1;
    delete mCarte2;
}

// [N] existant
void TestCaseAleatoire::testActionPiochePremiereCarte(void)
{
    mCaseAleatoire->action();
    CPPUNIT_ASSERT(mJoueur->getCaseActuelle()->getIndex() == 5);
}

// [N] existant
void TestCaseAleatoire::testActionDeplacePremierCarteEnFin(void)
{
    mCaseAleatoire->action();
    CPPUNIT_ASSERT(mCaseAleatoire->getPremiereCarteDeFile() == mCarte2);
}

// [N] existant
void TestCaseAleatoire::testActionAppliqueEffetCarte(void)
{
    mCaseAleatoire->action(); // carte1 → case 5
    mCaseAleatoire->action(); // carte2 → case 10
    CPPUNIT_ASSERT(mJoueur->getCaseActuelle()->getIndex() == 10);
}

// [L] nouveau — 1 seule carte : 3 appels successifs sans crash
void TestCaseAleatoire::testAction_UneSeuleCarte_Circulaire(void)
{
    CaseAleatoire* caseUnique = new CaseAleatoire();
    Carte* carteUnique = new CarteDeplacement(mJoueur, nullptr, 7);
    caseUnique->ajouterCarte(carteUnique);
    caseUnique->setJoueurActif(mJoueur);

    caseUnique->action();
    CPPUNIT_ASSERT(mJoueur->getCaseActuelle()->getIndex() == 7);
    caseUnique->action();
    CPPUNIT_ASSERT(mJoueur->getCaseActuelle()->getIndex() == 7);
    caseUnique->action();
    CPPUNIT_ASSERT(mJoueur->getCaseActuelle()->getIndex() == 7);

    delete caseUnique;
    delete carteUnique;
}

// [E] nouveau — liste vide : ne plante pas (comportement défensif)
void TestCaseAleatoire::testAction_ListeVide_SansPlanter(void)
{
    CaseAleatoire* caseVide = new CaseAleatoire();
    caseVide->setJoueurActif(mJoueur);
    bool crashed = false;
    try {
        caseVide->action(); // ne doit pas lever d'exception non gérée
    } catch (...) {
        crashed = true;
    }
    CPPUNIT_ASSERT(!crashed);
    delete caseVide;
}

CPPUNIT_TEST_SUITE_REGISTRATION(TestCaseAleatoire);


