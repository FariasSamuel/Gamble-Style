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
    CPPUNIT_TEST(testActionPiochePremiereCarte);          
    CPPUNIT_TEST(testActionDeplacePremierCarteEnFin);     
    CPPUNIT_TEST(testActionAppliqueEffetCarte);           
    CPPUNIT_TEST(testAction_UneSeuleCarte_Circulaire);    
    CPPUNIT_TEST(testAction_ListeVide_SansPlanter);       
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);
    void tearDown(void);

protected:
    void testActionPiochePremiereCarte(void);
    void testActionDeplacePremierCarteEnFin(void);
    void testActionAppliqueEffetCarte(void);
    void testAction_UneSeuleCarte_Circulaire(void);
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
    // CORRECTION ICI : On ne supprime que la case et le joueur.
    // La case supprime ses cartes toute seule (évite le Segfault).
    delete mCaseAleatoire;
    delete mJoueur;
}

void TestCaseAleatoire::testActionPiochePremiereCarte(void)
{
    mCaseAleatoire->action();
    CPPUNIT_ASSERT(mJoueur->getPosition() == 5);
}

void TestCaseAleatoire::testActionDeplacePremierCarteEnFin(void)
{
    mCaseAleatoire->action();
    CPPUNIT_ASSERT(mCaseAleatoire->getPremiereCarteDeFile() == mCarte2);
}

void TestCaseAleatoire::testActionAppliqueEffetCarte(void)
{
    mCaseAleatoire->action(); 
    mCaseAleatoire->action(); 
    CPPUNIT_ASSERT(mJoueur->getPosition() == 10);
}

void TestCaseAleatoire::testAction_UneSeuleCarte_Circulaire(void)
{
    CaseAleatoire* caseUnique = new CaseAleatoire();
    Carte* carteUnique = new CarteDeplacement(mJoueur, nullptr, 7);
    caseUnique->ajouterCarte(carteUnique);
    caseUnique->setJoueurActif(mJoueur);

    caseUnique->action();
    CPPUNIT_ASSERT(mJoueur->getPosition() == 7);
    caseUnique->action();
    CPPUNIT_ASSERT(mJoueur->getPosition() == 7);
    
    delete caseUnique;
}

void TestCaseAleatoire::testAction_ListeVide_SansPlanter(void)
{
    CaseAleatoire* caseVide = new CaseAleatoire();
    caseVide->setJoueurActif(mJoueur);
    bool crashed = false;
    try {
        caseVide->action(); 
    } catch (...) {
        crashed = true;
    }
    CPPUNIT_ASSERT(!crashed);
    delete caseVide;
}

CPPUNIT_TEST_SUITE_REGISTRATION(TestCaseAleatoire);
