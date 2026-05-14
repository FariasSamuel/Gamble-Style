/**
 * @file TestCasePropriete.cpp
 * @brief Tests unitaires de CasePropriete (CppUnit).
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
#include "CasePropriete.hpp"
#include "Joueur.hpp"

using namespace CppUnit;
using namespace std;

class TestCasePropriete : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestCasePropriete);
    CPPUNIT_TEST(testActionSansProprietaireCaseAchetable);       // [N] existant
    CPPUNIT_TEST(testActionAvecProprietaireLanceMinijeu);        // [N] existant
    CPPUNIT_TEST(testRepartitionGagnantRecupGamble);             // [N] existant
    CPPUNIT_TEST(testRepartitionPerdantPerdGamble);              // [N] existant
    CPPUNIT_TEST(testGambleMontantPositif);                      // [N] existant
    CPPUNIT_TEST(testAction_ProprietaireEtVisiteurIdentiques);   // [L] nouveau
    CPPUNIT_TEST(testAction_AchatApresHypotheque);               // [L] nouveau
    CPPUNIT_TEST(testGamble_MiseHorsLimites_BorneeMax);          // [E] nouveau
    CPPUNIT_TEST(testGamble_MiseSousMinimum_BorneeMin);          // [E] nouveau
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);
    void tearDown(void);

protected:
    void testActionSansProprietaireCaseAchetable(void);
    void testActionAvecProprietaireLanceMinijeu(void);
    void testRepartitionGagnantRecupGamble(void);
    void testRepartitionPerdantPerdGamble(void);
    void testGambleMontantPositif(void);
    void testAction_ProprietaireEtVisiteurIdentiques(void);
    void testAction_AchatApresHypotheque(void);
    void testGamble_MiseHorsLimites_BorneeMax(void);
    void testGamble_MiseSousMinimum_BorneeMin(void);

private:
    CasePropriete *mCase;
    Joueur        *mProprietaire;
    Joueur        *mVisiteur;
};

void TestCasePropriete::setUp(void)
{
    mProprietaire = new Joueur("Alice", 1500);
    mVisiteur     = new Joueur("Bob",   1500);
    mCase         = new CasePropriete("Avenue Mozart", 300);
    mCase->setJoueurActif(mVisiteur);
}

void TestCasePropriete::tearDown(void)
{
    delete mCase;
    delete mProprietaire;
    delete mVisiteur;
}

// [N] existant — sans propriétaire, la case signale qu'elle est achetable
void TestCasePropriete::testActionSansProprietaireCaseAchetable(void)
{
    CPPUNIT_ASSERT(mCase->getProprietaire() == nullptr);
    mCase->action();
    CPPUNIT_ASSERT(mCase->achatPropose());
}

// [N] existant — avec propriétaire, le mini-jeu est marqué comme lancé
void TestCasePropriete::testActionAvecProprietaireLanceMinijeu(void)
{
    mCase->setProprietaire(mProprietaire);
    mCase->setGamble(100);
    mCase->action();
    CPPUNIT_ASSERT(mCase->minijeuLance());
}

// [N] existant — le gagnant récupère le montant du gamble
void TestCasePropriete::testRepartitionGagnantRecupGamble(void)
{
    mCase->setProprietaire(mProprietaire);
    mCase->setGamble(200);
    mCase->setGagnant(mProprietaire);
    int capitalAvant = mProprietaire->getCapital();
    mCase->repartition();
    CPPUNIT_ASSERT(mProprietaire->getCapital() == capitalAvant + 200);
}

// [N] existant — le perdant perd le montant du gamble
void TestCasePropriete::testRepartitionPerdantPerdGamble(void)
{
    mCase->setProprietaire(mProprietaire);
    mCase->setGamble(200);
    mCase->setGagnant(mProprietaire);
    int capitalAvant = mVisiteur->getCapital();
    mCase->repartition();
    CPPUNIT_ASSERT(mVisiteur->getCapital() == capitalAvant - 200);
}

// [N] existant — gamble() produit une mise > 0
void TestCasePropriete::testGambleMontantPositif(void)
{
    mCase->setProprietaire(mProprietaire);
    mCase->setGamblePredefini(150);
    mCase->gamble();
    CPPUNIT_ASSERT(mCase->getGamble() > 0);
}

// [L] nouveau — visiteur == propriétaire : aucun mini-jeu, capital inchangé
void TestCasePropriete::testAction_ProprietaireEtVisiteurIdentiques(void)
{
    mCase->setProprietaire(mProprietaire);
    mCase->setJoueurActif(mProprietaire); // même joueur
    int capitalAvant = mProprietaire->getCapital();
    mCase->action();
    CPPUNIT_ASSERT(!mCase->minijeuLance());
    CPPUNIT_ASSERT(mProprietaire->getCapital() == capitalAvant);
}

// [L] nouveau — case hypothéquée puis rachetée → redevient achetable
void TestCasePropriete::testAction_AchatApresHypotheque(void) {
    mProprietaire->acheter(mCase);
    mProprietaire->hypotequer(mCase);
    
    // Une case hypothéquée appartient toujours à son propriétaire, 
    // elle n'est donc pas achetable par un visiteur !
    CPPUNIT_ASSERT(!mCase->isAchetable()); 
}

// [E] nouveau — mise proposée > mise_max → bornée à mise_max
void TestCasePropriete::testGamble_MiseHorsLimites_BorneeMax(void)
{
    mCase->setProprietaire(mProprietaire);
    mCase->setMiseLimites(50, 200);     // min=50, max=200
    mCase->setGamblePredefini(500);     // dépasse max
    mCase->gamble();
    CPPUNIT_ASSERT(mCase->getGamble() <= 200);
}

// [E] nouveau — mise proposée < mise_min → bornée à mise_min
void TestCasePropriete::testGamble_MiseSousMinimum_BorneeMin(void)
{
    mCase->setProprietaire(mProprietaire);
    mCase->setMiseLimites(50, 200);     // min=50, max=200
    mCase->setGamblePredefini(10);      // sous le min
    mCase->gamble();
    CPPUNIT_ASSERT(mCase->getGamble() >= 50);
}

CPPUNIT_TEST_SUITE_REGISTRATION(TestCasePropriete);


