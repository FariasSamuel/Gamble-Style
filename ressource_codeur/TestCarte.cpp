/**
 * @file TestCarte.cpp
 * @brief Tests unitaires de la hiérarchie Carte (CppUnit).
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
#include "Carte.hpp"
#include "CarteAnniversaire.hpp"
#include "CarteDeplacement.hpp"
#include "CarteSortiePrison.hpp"
#include "CarteGain.hpp"
#include "CarteAmende.hpp"
#include "CarteImpotMinijeux.hpp"
#include "CarteDoubleGain.hpp"
#include "Joueur.hpp"
#include "Plateau.hpp"

using namespace CppUnit;
using namespace std;

class TestCarte : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestCarte);

    CPPUNIT_TEST(testActionCarteAnniversaireTransfertVersJoueur); 
    CPPUNIT_TEST(testActionCarteAnniversaire_SeulJoueur);         
    CPPUNIT_TEST(testActionCarteAnniversaire_MontantExact);       
    CPPUNIT_TEST(testActionCarteDeplacementChangeLaCase);         
    CPPUNIT_TEST(testActionCarteDeplacement_CaseDepartCreditee);  
    CPPUNIT_TEST(testActionCarteSortiePrisonDonneCarte);          
    CPPUNIT_TEST(testActionCarteSortiePrison_PasDeDoubleAttribution); 
    CPPUNIT_TEST(testActionCarteGainCrediteCapital);              
    CPPUNIT_TEST(testActionCarteGain_MontantExact);               
    CPPUNIT_TEST(testActionCarteAmendeDebiteCapital);             
    CPPUNIT_TEST(testActionCarteAmende_DeclencheBanqueroute);     
    CPPUNIT_TEST(testActionCarteImpot_ProportionnelCases);        
    CPPUNIT_TEST(testActionCarteImpot_AucunePropriete_SansEffet); 
    CPPUNIT_TEST(testActionCarteDoubleGain_AjouteeEnMain);        

    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);
    void tearDown(void);

protected:
    void testActionCarteAnniversaireTransfertVersJoueur(void);
    void testActionCarteAnniversaire_SeulJoueur(void);
    void testActionCarteAnniversaire_MontantExact(void);
    void testActionCarteDeplacementChangeLaCase(void);
    void testActionCarteDeplacement_CaseDepartCreditee(void);
    void testActionCarteSortiePrisonDonneCarte(void);
    void testActionCarteSortiePrison_PasDeDoubleAttribution(void);
    void testActionCarteGainCrediteCapital(void);
    void testActionCarteGain_MontantExact(void);
    void testActionCarteAmendeDebiteCapital(void);
    void testActionCarteAmende_DeclencheBanqueroute(void);
    void testActionCarteImpot_ProportionnelCases(void);
    void testActionCarteImpot_AucunePropriete_SansEffet(void);
    void testActionCarteDoubleGain_AjouteeEnMain(void);

private:
    Plateau           *mPlateau;
    Joueur            *mTitulaire;
    Joueur            *mAutreJoueur;
    CarteAnniversaire *mCarteAnniversaire;
    CarteDeplacement  *mCarteDeplacement;
    CarteSortiePrison *mCarteSortiePrison;
    CarteGain         *mCarteGain;
    CarteAmende       *mCarteAmende;
    CarteImpotMinijeux *mCarteImpot;
    CarteDoubleGain   *mCarteDoubleGain;
};

void TestCarte::setUp(void)
{
    mPlateau          = new Plateau();
    mTitulaire        = new Joueur("Alice", 1500);
    mAutreJoueur      = new Joueur("Bob",   1500);
    mPlateau->ajouterJoueur(mTitulaire);
    mPlateau->ajouterJoueur(mAutreJoueur);

    mCarteAnniversaire = new CarteAnniversaire(mTitulaire, mPlateau, 50);
   mCarteDeplacement = new CarteDeplacement(mTitulaire, nullptr, 10);
    mCarteSortiePrison = new CarteSortiePrison(mTitulaire);
    mCarteGain         = new CarteGain(mTitulaire, mPlateau, 100);
    mCarteAmende       = new CarteAmende(mTitulaire, mPlateau, 80);
    mCarteImpot        = new CarteImpotMinijeux(mTitulaire, mPlateau, 25);
    mCarteDoubleGain   = new CarteDoubleGain(mTitulaire);
}

void TestCarte::tearDown(void)
{
    delete mCarteAnniversaire;
    delete mCarteDeplacement;
    delete mCarteSortiePrison;
    delete mCarteGain;
    delete mCarteAmende;
    delete mCarteImpot;
    delete mCarteDoubleGain;
    delete mPlateau;
    
    // <-- AJOUT : On détruit les joueurs qui ne sont plus détruits par le plateau
    delete mTitulaire;   
    delete mAutreJoueur; 
}

void TestCarte::testActionCarteAnniversaireTransfertVersJoueur(void) {
    int capitalTitulaireAvant   = mTitulaire->getCapital();
    int capitalAutreJoueurAvant = mAutreJoueur->getCapital();
    mCarteAnniversaire->action();
    CPPUNIT_ASSERT(mTitulaire->getCapital()   == capitalTitulaireAvant   + 50);
    CPPUNIT_ASSERT(mAutreJoueur->getCapital() == capitalAutreJoueurAvant - 50);
}

void TestCarte::testActionCarteAnniversaire_SeulJoueur(void) {
    Plateau* p2 = new Plateau();
    Joueur* seul = new Joueur("Seul", 1000);
    p2->ajouterJoueur(seul);
    CarteAnniversaire carte(seul, p2, 50);
    int capitalAvant = seul->getCapital();
    carte.action();
    CPPUNIT_ASSERT(seul->getCapital() == capitalAvant); 
    delete p2;
    delete seul;
}

void TestCarte::testActionCarteAnniversaire_MontantExact(void) {
    int capitalAvant = mTitulaire->getCapital();
    mCarteAnniversaire->action();
    CPPUNIT_ASSERT(mTitulaire->getCapital() == capitalAvant + 50);
}

void TestCarte::testActionCarteDeplacementChangeLaCase(void) {
    mCarteDeplacement->action();
    // CORRECTION ICI : On teste la position numérique, pas le pointeur de case
    CPPUNIT_ASSERT(mTitulaire->getPosition() == 10);
}

void TestCarte::testActionCarteDeplacement_CaseDepartCreditee(void) {
    CarteDeplacement carteDepart(mTitulaire, mPlateau, 0); 
    int capitalAvant = mTitulaire->getCapital();
    carteDepart.action();
    CPPUNIT_ASSERT(mTitulaire->getCapital() >= capitalAvant);
}

void TestCarte::testActionCarteSortiePrisonDonneCarte(void) {
    CPPUNIT_ASSERT(!mTitulaire->possedeCarte(CarteSortiePrison::ID));
    mCarteSortiePrison->action();
    CPPUNIT_ASSERT(mTitulaire->possedeCarte(CarteSortiePrison::ID));
}

void TestCarte::testActionCarteSortiePrison_PasDeDoubleAttribution(void) {
    mCarteSortiePrison->action();
    int nbCartesApres1 = mTitulaire->getNbCartes();
    mCarteSortiePrison->action();
    CPPUNIT_ASSERT(mTitulaire->getNbCartes() == nbCartesApres1);
}

void TestCarte::testActionCarteGainCrediteCapital(void) {
    int capitalAvant = mTitulaire->getCapital();
    mCarteGain->action();
    CPPUNIT_ASSERT(mTitulaire->getCapital() > capitalAvant);
}

void TestCarte::testActionCarteGain_MontantExact(void) {
    int capitalAvant = mTitulaire->getCapital();
    mCarteGain->action();
    CPPUNIT_ASSERT(mTitulaire->getCapital() == capitalAvant + 100);
}

void TestCarte::testActionCarteAmendeDebiteCapital(void) {
    int capitalAvant = mTitulaire->getCapital();
    mCarteAmende->action();
    CPPUNIT_ASSERT(mTitulaire->getCapital() < capitalAvant);
}

void TestCarte::testActionCarteAmende_DeclencheBanqueroute(void) {
    mTitulaire->setCapital(50);
    mTitulaire->clearProprietes();
    CarteAmende amendeForte(mTitulaire, mPlateau, 9999);
    amendeForte.action();
    CPPUNIT_ASSERT(mTitulaire->conditionfinanciere() != Condition::RICHE);
}

void TestCarte::testActionCarteImpot_ProportionnelCases(void) {
    mTitulaire->setNbProprietesMock(3);
    int capitalAvant = mTitulaire->getCapital();
    mCarteImpot->action();
    CPPUNIT_ASSERT(mTitulaire->getCapital() == capitalAvant - 75);
}

void TestCarte::testActionCarteImpot_AucunePropriete_SansEffet(void) {
    mTitulaire->clearProprietes();
    int capitalAvant = mTitulaire->getCapital();
    mCarteImpot->action();
    CPPUNIT_ASSERT(mTitulaire->getCapital() == capitalAvant);
}

void TestCarte::testActionCarteDoubleGain_AjouteeEnMain(void) {
    int nbCarteAvant = mTitulaire->getNbCartes();
    mCarteDoubleGain->action();
    CPPUNIT_ASSERT(mTitulaire->getNbCartes() == nbCarteAvant + 1);
}

CPPUNIT_TEST_SUITE_REGISTRATION(TestCarte);
