/**
 * @file TestPlateau.cpp
 * @brief Tests unitaires de la classe Plateau (CppUnit).
 *
 * Couverture : donnercapital(), enlevercapital(), transfertargent(),
 * ordre(), update(), gamelooping(), fin().
 *
 * @project GambleStyle — GM4 INSA Rouen Normandie
 */

#include <iostream>
#include <string>
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
#include "Plateau.hpp"
#include "Joueur.hpp"

using namespace CppUnit;
using namespace std;

class TestPlateau : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestPlateau);

    // ── donnercapital() ─────────────────────────────────────────
    CPPUNIT_TEST(testDonnerCapitalAugmenteCapitalJoueur);        
    CPPUNIT_TEST(testDonnerCapital_MontantNul_SansEffet);        

    // ── enlevercapital() ────────────────────────────────────────
    CPPUNIT_TEST(testEnleverCapitalDiminueCapitalJoueur);        
    CPPUNIT_TEST(testEnleverCapitalDeclencheBanqueroute);        
    CPPUNIT_TEST(testEnleverCapital_JoueurPasseEnFaillite);      

    // ── transfertargent() ───────────────────────────────────────
    CPPUNIT_TEST(testTransfertArgentDebitEtCredit);              
    CPPUNIT_TEST(testTransfertArgent_CapitalExactementVide);     

    // ── ordre() ─────────────────────────────────────────────────
    CPPUNIT_TEST(testOrdreJoueursTrieParDe);                     
    CPPUNIT_TEST(testOrdreJoueurs3JoueursTrieParDe);             

    // ── update() ────────────────────────────────────────────────
    CPPUNIT_TEST(testUpdateExecuteTousLesTours);                  

    // ── gamelooping() ───────────────────────────────────────────
    CPPUNIT_TEST(testGameloopingArreteSurConditionFin);          

    // ── fin() ───────────────────────────────────────────────────
    CPPUNIT_TEST(testFinRetourneGagnantParCapitalPlusBiens);      
    CPPUNIT_TEST(testFin_UnSeulJoueurActif);                     

    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);
    void tearDown(void);

protected:
    void testDonnerCapitalAugmenteCapitalJoueur(void);
    void testDonnerCapital_MontantNul_SansEffet(void);
    void testEnleverCapitalDiminueCapitalJoueur(void);
    void testEnleverCapitalDeclencheBanqueroute(void);
    void testEnleverCapital_JoueurPasseEnFaillite(void);
    void testTransfertArgentDebitEtCredit(void);
    void testTransfertArgent_CapitalExactementVide(void);
    void testOrdreJoueursTrieParDe(void);
    void testOrdreJoueurs3JoueursTrieParDe(void);
    void testUpdateExecuteTousLesTours(void);
    void testGameloopingArreteSurConditionFin(void);
    void testFinRetourneGagnantParCapitalPlusBiens(void);
    void testFin_UnSeulJoueurActif(void);

private:
    Plateau *mPlateau;
    Joueur  *mJoueur1;
    Joueur  *mJoueur2;
    Joueur  *mJoueur3;
};

void TestPlateau::setUp(void)
{
    mPlateau  = new Plateau();
    mJoueur1  = new Joueur("Alice",   1500);
    mJoueur2  = new Joueur("Bob",      500);
    
    // Le plateau prend la responsabilité de ces joueurs
    mPlateau->ajouterJoueur(mJoueur1);
    mPlateau->ajouterJoueur(mJoueur2);
}

void TestPlateau::tearDown(void)
{
    // C'est la SEULE ligne nécessaire. 
    // mPlateau va détruire mJoueur1, mJoueur2 et mJoueur3 (s'il a été ajouté).
    delete mPlateau; 
    delete mJoueur1;
    delete mJoueur2;
}

// ── Implémentations ──────────────────────────────────────────────────────────

void TestPlateau::testDonnerCapitalAugmenteCapitalJoueur(void) {
    int capitalAvant = mJoueur1->getCapital();
    mPlateau->donnercapital(mJoueur1, 200);
    CPPUNIT_ASSERT(mJoueur1->getCapital() == capitalAvant + 200);
}

void TestPlateau::testDonnerCapital_MontantNul_SansEffet(void) {
    int capitalAvant = mJoueur1->getCapital();
    mPlateau->donnercapital(mJoueur1, 0);
    CPPUNIT_ASSERT(mJoueur1->getCapital() == capitalAvant);
}

void TestPlateau::testEnleverCapitalDiminueCapitalJoueur(void) {
    int capitalAvant = mJoueur1->getCapital();
    mPlateau->enlevercapital(mJoueur1, 300);
    CPPUNIT_ASSERT(mJoueur1->getCapital() == capitalAvant - 300);
}

void TestPlateau::testEnleverCapitalDeclencheBanqueroute(void) {
    mPlateau->enlevercapital(mJoueur2, 9999);
    CPPUNIT_ASSERT(mJoueur2->conditionfinanciere() != Condition::RICHE);
}

void TestPlateau::testEnleverCapital_JoueurPasseEnFaillite(void) {
    mJoueur2->clearProprietes(); 
    mPlateau->enlevercapital(mJoueur2, 9999);
    CPPUNIT_ASSERT(mJoueur2->conditionfinanciere() == Condition::FAILLITE);
}

void TestPlateau::testTransfertArgentDebitEtCredit(void) {
    int cap1Avant = mJoueur1->getCapital();
    int cap2Avant = mJoueur2->getCapital();
    mPlateau->transfertargent(mJoueur1, mJoueur2, 100);
    CPPUNIT_ASSERT(mJoueur1->getCapital() == cap1Avant - 100);
    CPPUNIT_ASSERT(mJoueur2->getCapital() == cap2Avant + 100);
}

void TestPlateau::testTransfertArgent_CapitalExactementVide(void) {
    int capitalJ2 = mJoueur2->getCapital(); 
    mPlateau->transfertargent(mJoueur2, mJoueur1, capitalJ2);
    CPPUNIT_ASSERT(mJoueur2->getCapital() == 0);
    CPPUNIT_ASSERT(mJoueur2->conditionfinanciere() == Condition::RICHE); 
}

void TestPlateau::testOrdreJoueursTrieParDe(void) {
    mPlateau->ordre();
    CPPUNIT_ASSERT(mPlateau->getJoueur(0)->getDernierLancer()
                >= mPlateau->getJoueur(1)->getDernierLancer());
}
void TestPlateau::testOrdreJoueurs3JoueursTrieParDe(void) {
    Joueur* mJoueur3 = new Joueur("Charlie", 1000); // Création locale
    mPlateau->ajouterJoueur(mJoueur3); // Le plateau en devient propriétaire
    
    mJoueur1->setDernierLancer(8);   
    mJoueur2->setDernierLancer(11);  
    mJoueur3->setDernierLancer(5);   
    
    mPlateau->trierParDernierLancer();
    
    CPPUNIT_ASSERT(mPlateau->getJoueur(0) == mJoueur2); 
    CPPUNIT_ASSERT(mPlateau->getJoueur(1) == mJoueur1); 
    CPPUNIT_ASSERT(mPlateau->getJoueur(2) == mJoueur3);
    
    delete mJoueur3; // <-- AJOUT : On détruit manuellement le joueur
}


void TestPlateau::testUpdateExecuteTousLesTours(void) {
    mJoueur1->resetCompteurTours();
    mJoueur2->resetCompteurTours();
    mPlateau->update();
    CPPUNIT_ASSERT(mJoueur1->getCompteurTours() == 1);
    CPPUNIT_ASSERT(mJoueur2->getCompteurTours() == 1);
}

void TestPlateau::testGameloopingArreteSurConditionFin(void) {
    mJoueur2->setCapital(-9999);
    mJoueur2->clearProprietes();
    mJoueur2->misebanqueroute(); 
    mPlateau->retirerJoueursEnFaillite();
    mPlateau->gamelooping();
    CPPUNIT_ASSERT(mPlateau->finDePartie() == true);
}

void TestPlateau::testFinRetourneGagnantParCapitalPlusBiens(void) {
    Joueur* gagnant = mPlateau->fin();
    CPPUNIT_ASSERT(gagnant == mJoueur1);
}

void TestPlateau::testFin_UnSeulJoueurActif(void) {
    mJoueur2->setCapital(-9999);
    mJoueur2->clearProprietes();
    mJoueur2->misebanqueroute();
    mPlateau->retirerJoueursEnFaillite();
    Joueur* gagnant = mPlateau->fin();
    CPPUNIT_ASSERT(gagnant == mJoueur1);
}

CPPUNIT_TEST_SUITE_REGISTRATION(TestPlateau);
