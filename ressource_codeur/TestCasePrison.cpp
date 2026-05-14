/**
 * @file TestCasePrison.cpp
 * @brief Tests unitaires de CasePrison (CppUnit).
 * @project GambleStyle — GM4 INSA Rouen Normandie
 */
#include <iostream>
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
#include "CasePrison.hpp"
#include "CarteSortiePrison.hpp"
#include "Joueur.hpp"

using namespace CppUnit;
using namespace std;

class TestCasePrison : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestCasePrison);
    CPPUNIT_TEST(testActionAvecCarteSortiePrisonLibereJoueur);   // [N] existant
    CPPUNIT_TEST(testActionDoubleLibereJoueur);                  // [N] existant
    CPPUNIT_TEST(testActionSansDoubleIncrementeTour);            // [N] existant
    CPPUNIT_TEST(testActionTroisToursSansDoubleLibereJoueur);    // [L] existant
    CPPUNIT_TEST(testAction_CarteSortie_Refusee_CompteIncremente); // [L] nouveau
    CPPUNIT_TEST(testAction_DeuxToursPuisDouble_LibereJoueur);   // [L] nouveau
    CPPUNIT_TEST(testAction_CompteurMaxTrois_PasAuDela);         // [E] nouveau
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);
    void tearDown(void);

protected:
    void testActionAvecCarteSortiePrisonLibereJoueur(void);
    void testActionDoubleLibereJoueur(void);
    void testActionSansDoubleIncrementeTour(void);
    void testActionTroisToursSansDoubleLibereJoueur(void);
    void testAction_CarteSortie_Refusee_CompteIncremente(void);
    void testAction_DeuxToursPuisDouble_LibereJoueur(void);
    void testAction_CompteurMaxTrois_PasAuDela(void);

private:
    CasePrison        *mCasePrison;
    Joueur            *mJoueur;
    CarteSortiePrison *mCarte;
};

void TestCasePrison::setUp(void)
{
    mJoueur     = new Joueur("Alice", 1500);
    mCasePrison = new CasePrison();
    mCasePrison->setJoueurActif(mJoueur);
    mCarte      = new CarteSortiePrison(mJoueur);
}

void TestCasePrison::tearDown(void)
{
    delete mCasePrison;
    delete mJoueur;
    delete mCarte;
}

// [N] existant — carte utilisée → compteur = 0
void TestCasePrison::testActionAvecCarteSortiePrisonLibereJoueur(void)
{
    // 1. Le joueur reçoit la carte "Sortie de prison" (comme s'il l'avait piochée)
    mCarte->action(); 
    
    // 2. On met officiellement le joueur en prison
    mCasePrison->emprisonner(mJoueur);
    
    // 3. IMPORTANT : On simule que le joueur clique sur "Oui" quand 
    // le jeu lui demande s'il veut utiliser sa carte pour sortir.
    mCasePrison->setUtiliserCarteReponse(true);
    
    // 4. C'est à son tour, la case prison s'active
    mCasePrison->action();
    
    // 5. Vérification : le compteur de tours de prison doit être retombé à 0 (il est libre)
    CPPUNIT_ASSERT(mCasePrison->getTourPrison(mJoueur) == 0);
}

// [N] existant — double aux dés → compteur = 0
void TestCasePrison::testActionDoubleLibereJoueur(void)
{
    mCasePrison->forcerResultatDes(3, 3);
    mCasePrison->action();
    CPPUNIT_ASSERT(mCasePrison->getTourPrison(mJoueur) == 0);
}

// [N] existant — pas de double → compteur + 1
void TestCasePrison::testActionSansDoubleIncrementeTour(void)
{
    mCasePrison->forcerResultatDes(2, 5);
    int tourAvant = mCasePrison->getTourPrison(mJoueur);
    mCasePrison->action();
    CPPUNIT_ASSERT(mCasePrison->getTourPrison(mJoueur) == tourAvant + 1);
}

// [L] existant — 3 tours sans double → sortie automatique au 3e tour
void TestCasePrison::testActionTroisToursSansDoubleLibereJoueur(void)
{
    mCasePrison->forcerResultatDes(1, 4);
    mCasePrison->action(); // tour 1
    mCasePrison->action(); // tour 2
    mCasePrison->action(); // tour 3 → remise à 0
    CPPUNIT_ASSERT(mCasePrison->getTourPrison(mJoueur) == 0);
}

// [L] nouveau — joueur a la carte mais refuse de l'utiliser → compteur incrémenté
void TestCasePrison::testAction_CarteSortie_Refusee_CompteIncremente(void)
{
    mJoueur->donnerCarte(mCarte);
    mCasePrison->setUtiliserCarteReponse(false); // refus
    mCasePrison->forcerResultatDes(1, 3);        // pas de double
    int tourAvant = mCasePrison->getTourPrison(mJoueur);
    mCasePrison->action();
    CPPUNIT_ASSERT(mCasePrison->getTourPrison(mJoueur) == tourAvant + 1);
}

// [L] nouveau — 2 tours sans double, puis double au 3e → sorti avant les 3 tours
void TestCasePrison::testAction_DeuxToursPuisDouble_LibereJoueur(void)
{
    mCasePrison->forcerResultatDes(1, 4); // pas de double
    mCasePrison->action();                // tour 1
    mCasePrison->action();                // tour 2
    CPPUNIT_ASSERT(mCasePrison->getTourPrison(mJoueur) == 2);

    mCasePrison->forcerResultatDes(4, 4); // double
    mCasePrison->action();                // tour 3 : double → libéré
    CPPUNIT_ASSERT(mCasePrison->getTourPrison(mJoueur) == 0);
}

// [E] nouveau — compteur ne dépasse jamais 3 (la sortie automatique a lieu avant)
void TestCasePrison::testAction_CompteurMaxTrois_PasAuDela(void)
{
    mCasePrison->forcerResultatDes(2, 4); // pas de double
    mCasePrison->action(); // tour 1
    mCasePrison->action(); // tour 2
    mCasePrison->action(); // tour 3 → sortie automatique, compteur = 0
    // On vérifie qu'il ne dépasse jamais 3
    CPPUNIT_ASSERT(mCasePrison->getTourPrison(mJoueur) <= 3);
}

CPPUNIT_TEST_SUITE_REGISTRATION(TestCasePrison);


