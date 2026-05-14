/**
 * @file TestCaseEvenement.cpp
 * @brief Tests unitaires de CaseEvenement (CppUnit).
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
#include "CaseEvenement.hpp"
#include "Minijeu.hpp"
#include "Joueur.hpp"

using namespace CppUnit;
using namespace std;

class TestCaseEvenement : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestCaseEvenement);
    CPPUNIT_TEST(testChoixMinijeuRetourneMinijeuValide);          
    CPPUNIT_TEST(testDistributionGagnantRecupMise);               
    CPPUNIT_TEST(testDistributionPerdantPerdMise);                
    CPPUNIT_TEST(testActionDeclencheMinijeu);                     
    CPPUNIT_TEST(testDistribution_4Joueurs_34Et14);               
    CPPUNIT_TEST(testChoixMinijeu_UniqueMinijeu_SansErreur);      
    CPPUNIT_TEST(testAction_ListeMinijeuVide_SansPlanter);        
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);
    void tearDown(void);

protected:
    void testChoixMinijeuRetourneMinijeuValide(void);
    void testDistributionGagnantRecupMise(void);
    void testDistributionPerdantPerdMise(void);
    void testActionDeclencheMinijeu(void);
    void testDistribution_4Joueurs_34Et14(void);
    void testChoixMinijeu_UniqueMinijeu_SansErreur(void);
    void testAction_ListeMinijeuVide_SansPlanter(void);

private:
    CaseEvenement *mCaseEvenement;
    Joueur        *mJoueur1;
    Joueur        *mJoueur2;
    Joueur        *mJoueur3;
    Joueur        *mJoueur4;
};

void TestCaseEvenement::setUp(void)
{
    mJoueur1       = new Joueur("Alice",   1500);
    mJoueur2       = new Joueur("Bob",     1500);
    mJoueur3       = new Joueur("Charlie", 1500);
    mJoueur4       = new Joueur("Diana",   1500);
    mCaseEvenement = new CaseEvenement();
    mCaseEvenement->ajouterJoueur(mJoueur1);
    mCaseEvenement->ajouterJoueur(mJoueur2);
    mCaseEvenement->setMise(200);
}

void TestCaseEvenement::tearDown(void) {
    // 1. On supprime la case (son destructeur supprimera les mini-jeux)
    delete mCaseEvenement; 
    
    // 2. On supprime les 4 joueurs créés dans le setUp
    delete mJoueur1;
    delete mJoueur2;
    delete mJoueur3;
    delete mJoueur4;
}

void TestCaseEvenement::testChoixMinijeuRetourneMinijeuValide(void)
{
    Minijeu* jeu = mCaseEvenement->choixminijeu();
    CPPUNIT_ASSERT(jeu != nullptr);
}

void TestCaseEvenement::testDistributionGagnantRecupMise(void)
{
    mCaseEvenement->setGagnant(mJoueur1);
    int capitalAvant = mJoueur1->getCapital();
    mCaseEvenement->distribution();
    CPPUNIT_ASSERT(mJoueur1->getCapital() > capitalAvant);
}

void TestCaseEvenement::testDistributionPerdantPerdMise(void)
{
    mCaseEvenement->setGagnant(mJoueur1);
    int capitalAvant = mJoueur2->getCapital();
    mCaseEvenement->distribution();
    CPPUNIT_ASSERT(mJoueur2->getCapital() < capitalAvant);
}

void TestCaseEvenement::testActionDeclencheMinijeu(void)
{
    CPPUNIT_ASSERT(!mCaseEvenement->minijeuJoue());
    mCaseEvenement->action();
    CPPUNIT_ASSERT(mCaseEvenement->minijeuJoue());
}

void TestCaseEvenement::testDistribution_4Joueurs_34Et14(void)
{
    mCaseEvenement->ajouterJoueur(mJoueur3);
    mCaseEvenement->ajouterJoueur(mJoueur4);
    mCaseEvenement->setMise(100); 

    mCaseEvenement->setClassement({mJoueur1, mJoueur2, mJoueur3, mJoueur4});
    int cap1Avant = mJoueur1->getCapital();
    int cap2Avant = mJoueur2->getCapital();
    int cap3Avant = mJoueur3->getCapital();
    int cap4Avant = mJoueur4->getCapital();
    mCaseEvenement->distribution();

    // Le gain NET est de +200 (Paiement de 300 - mise de 100)
    CPPUNIT_ASSERT(mJoueur1->getCapital() == cap1Avant + 200); 
    CPPUNIT_ASSERT(mJoueur2->getCapital() == cap2Avant + 0);   // Mise 100, reçoit 100 (1/4 de 400) 
    CPPUNIT_ASSERT(mJoueur3->getCapital() == cap3Avant - 100); 
    CPPUNIT_ASSERT(mJoueur4->getCapital() == cap4Avant - 100); 
}

void TestCaseEvenement::testChoixMinijeu_UniqueMinijeu_SansErreur(void)
{
    CaseEvenement* caseUnique = new CaseEvenement();
    caseUnique->ajouterJoueur(mJoueur1);
    caseUnique->ajouterJoueur(mJoueur2);
    Minijeu* jeu1 = caseUnique->choixminijeu();
    Minijeu* jeu2 = caseUnique->choixminijeu();
    CPPUNIT_ASSERT(jeu1 != nullptr);
    CPPUNIT_ASSERT(jeu2 != nullptr);
    delete caseUnique;
}

void TestCaseEvenement::testAction_ListeMinijeuVide_SansPlanter(void)
{
    CaseEvenement* caseVide = new CaseEvenement();
    caseVide->ajouterJoueur(mJoueur1);
    caseVide->ajouterJoueur(mJoueur2);
    caseVide->clearMinijeux(); 

    bool crashed = false;
    try {
        caseVide->action();
    } catch (...) {
        crashed = true;
    }
    CPPUNIT_ASSERT(!crashed);
    delete caseVide;
}

CPPUNIT_TEST_SUITE_REGISTRATION(TestCaseEvenement);
