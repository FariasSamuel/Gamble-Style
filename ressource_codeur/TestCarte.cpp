/**
 * @file TestCarte.cpp
 * @brief Tests unitaires de la hiérarchie Carte (CppUnit).
 *
 * Couverture : CarteAnniversaire, CarteDeplacement, CarteSortiePrison,
 *              CarteGain, CarteAmende, CarteImpotMinijeux, CarteDoubleGain.
 *
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

    // ── CarteAnniversaire ────────────────────────────────────────
    CPPUNIT_TEST(testActionCarteAnniversaireTransfertVersJoueur); // [N] existant
    CPPUNIT_TEST(testActionCarteAnniversaire_SeulJoueur);         // [L] nouveau
    CPPUNIT_TEST(testActionCarteAnniversaire_MontantExact);       // [N] nouveau

    // ── CarteDeplacement ────────────────────────────────────────
    CPPUNIT_TEST(testActionCarteDeplacementChangeLaCase);         // [N] existant
    CPPUNIT_TEST(testActionCarteDeplacement_CaseDepartCreditee);  // [L] nouveau

    // ── CarteSortiePrison ───────────────────────────────────────
    CPPUNIT_TEST(testActionCarteSortiePrisonDonneCarte);          // [N] existant
    CPPUNIT_TEST(testActionCarteSortiePrison_PasDeDoubleAttribution); // [E] nouveau

    // ── CarteGain ───────────────────────────────────────────────
    CPPUNIT_TEST(testActionCarteGainCrediteCapital);              // [N] nouveau
    CPPUNIT_TEST(testActionCarteGain_MontantExact);               // [N] nouveau

    // ── CarteAmende ─────────────────────────────────────────────
    CPPUNIT_TEST(testActionCarteAmendeDebiteCapital);             // [N] nouveau
    CPPUNIT_TEST(testActionCarteAmende_DeclencheBanqueroute);     // [E] nouveau

    // ── CarteImpotMinijeux ──────────────────────────────────────
    CPPUNIT_TEST(testActionCarteImpot_ProportionnelCases);        // [N] nouveau
    CPPUNIT_TEST(testActionCarteImpot_AucunePropriete_SansEffet); // [L] nouveau

    // ── CarteDoubleGain ─────────────────────────────────────────
    CPPUNIT_TEST(testActionCarteDoubleGain_AjouteeEnMain);        // [N] nouveau

    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);
    void tearDown(void);

protected:
    // CarteAnniversaire
    void testActionCarteAnniversaireTransfertVersJoueur(void);
    void testActionCarteAnniversaire_SeulJoueur(void);
    void testActionCarteAnniversaire_MontantExact(void);
    // CarteDeplacement
    void testActionCarteDeplacementChangeLaCase(void);
    void testActionCarteDeplacement_CaseDepartCreditee(void);
    // CarteSortiePrison
    void testActionCarteSortiePrisonDonneCarte(void);
    void testActionCarteSortiePrison_PasDeDoubleAttribution(void);
    // CarteGain
    void testActionCarteGainCrediteCapital(void);
    void testActionCarteGain_MontantExact(void);
    // CarteAmende
    void testActionCarteAmendeDebiteCapital(void);
    void testActionCarteAmende_DeclencheBanqueroute(void);
    // CarteImpotMinijeux
    void testActionCarteImpot_ProportionnelCases(void);
    void testActionCarteImpot_AucunePropriete_SansEffet(void);
    // CarteDoubleGain
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
    mCarteDeplacement  = new CarteDeplacement(mTitulaire, mPlateau, 10);
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
    delete mTitulaire;
    delete mAutreJoueur;
}

// ── CarteAnniversaire ────────────────────────────────────────────────────────

// [N] existant — chaque autre joueur verse 50 au titulaire
void TestCarte::testActionCarteAnniversaireTransfertVersJoueur(void)
{
    int capitalTitulaireAvant   = mTitulaire->getCapital();
    int capitalAutreJoueurAvant = mAutreJoueur->getCapital();
    int montant = 50;
    mCarteAnniversaire->action();
    CPPUNIT_ASSERT(mTitulaire->getCapital()   == capitalTitulaireAvant   + montant);
    CPPUNIT_ASSERT(mAutreJoueur->getCapital() == capitalAutreJoueurAvant - montant);
}

// [L] nouveau — seul joueur dans la partie : aucun versement possible
void TestCarte::testActionCarteAnniversaire_SeulJoueur(void)
{
    Plateau* p2 = new Plateau();
    Joueur*  seul = new Joueur("Seul", 1000);
    p2->ajouterJoueur(seul);
    CarteAnniversaire carte(seul, p2, 50);
    int capitalAvant = seul->getCapital();
    carte.action();
    CPPUNIT_ASSERT(seul->getCapital() == capitalAvant); // aucun versement
    delete p2;
    delete seul;
}

// [N] nouveau — le montant total reçu = montant × nombre d'autres joueurs
void TestCarte::testActionCarteAnniversaire_MontantExact(void)
{
    int capitalAvant = mTitulaire->getCapital();
    mCarteAnniversaire->action(); // 1 autre joueur × 50
    CPPUNIT_ASSERT(mTitulaire->getCapital() == capitalAvant + 50);
}

// ── CarteDeplacement ──────────────────────────────────────────────────────────

// [N] existant — le joueur atterrit sur la case indiquée (index 10)
void TestCarte::testActionCarteDeplacementChangeLaCase(void)
{
    mCarteDeplacement->action();
    CPPUNIT_ASSERT(mTitulaire->getCaseActuelle()->getIndex() == 10);
}

// [L] nouveau — déplacement vers la case départ crédite la prime de passage
void TestCarte::testActionCarteDeplacement_CaseDepartCreditee(void)
{
    CarteDeplacement carteDepart(mTitulaire, mPlateau, 0); // case départ = index 0
    int capitalAvant = mTitulaire->getCapital();
    carteDepart.action();
    // Atterrir sur la case départ doit créditer le montant fixe (200)
    CPPUNIT_ASSERT(mTitulaire->getCapital() >= capitalAvant);
}

// ── CarteSortiePrison ─────────────────────────────────────────────────────────

// [N] existant — la carte est bien dans l'inventaire après action()
void TestCarte::testActionCarteSortiePrisonDonneCarte(void)
{
    CPPUNIT_ASSERT(!mTitulaire->possedeCarte(CarteSortiePrison::ID));
    mCarteSortiePrison->action();
    CPPUNIT_ASSERT(mTitulaire->possedeCarte(CarteSortiePrison::ID));
}

// [E] nouveau — appeler action() deux fois ne donne pas deux cartes
void TestCarte::testActionCarteSortiePrison_PasDeDoubleAttribution(void)
{
    mCarteSortiePrison->action();
    int nbCartesApres1 = mTitulaire->getNbCartes();
    mCarteSortiePrison->action(); // 2e appel
    // La carte ne doit pas être dupliquée
    CPPUNIT_ASSERT(mTitulaire->getNbCartes() == nbCartesApres1);
}

// ── CarteGain ─────────────────────────────────────────────────────────────────

// [N] nouveau — le capital augmente
void TestCarte::testActionCarteGainCrediteCapital(void)
{
    int capitalAvant = mTitulaire->getCapital();
    mCarteGain->action();
    CPPUNIT_ASSERT(mTitulaire->getCapital() > capitalAvant);
}

// [N] nouveau — le capital augmente exactement de 100
void TestCarte::testActionCarteGain_MontantExact(void)
{
    int capitalAvant = mTitulaire->getCapital();
    mCarteGain->action();
    CPPUNIT_ASSERT(mTitulaire->getCapital() == capitalAvant + 100);
}

// ── CarteAmende ───────────────────────────────────────────────────────────────

// [N] nouveau — le capital diminue
void TestCarte::testActionCarteAmendeDebiteCapital(void)
{
    int capitalAvant = mTitulaire->getCapital();
    mCarteAmende->action();
    CPPUNIT_ASSERT(mTitulaire->getCapital() < capitalAvant);
}

// [E] nouveau — amende > capital → déclenche banqueroute
void TestCarte::testActionCarteAmende_DeclencheBanqueroute(void)
{
    mTitulaire->setCapital(50); // capital faible
    mTitulaire->clearProprietes();
    CarteAmende amendeForte(mTitulaire, mPlateau, 9999);
    amendeForte.action();
    CPPUNIT_ASSERT(mTitulaire->conditionfinanciere() != Condition::RICHE);
}

// ── CarteImpotMinijeux ────────────────────────────────────────────────────────

// [N] nouveau — impôt = 25 × 3 cases possédées = 75
void TestCarte::testActionCarteImpot_ProportionnelCases(void)
{
    // Simuler 3 propriétés
    mTitulaire->setNbProprietesMock(3);
    int capitalAvant = mTitulaire->getCapital();
    mCarteImpot->action();
    CPPUNIT_ASSERT(mTitulaire->getCapital() == capitalAvant - 75);
}

// [L] nouveau — 0 case possédée → capital inchangé
void TestCarte::testActionCarteImpot_AucunePropriete_SansEffet(void)
{
    mTitulaire->clearProprietes();
    int capitalAvant = mTitulaire->getCapital();
    mCarteImpot->action();
    CPPUNIT_ASSERT(mTitulaire->getCapital() == capitalAvant);
}

// ── CarteDoubleGain ───────────────────────────────────────────────────────────

// [N] nouveau — la carte est ajoutée à l'inventaire du joueur
void TestCarte::testActionCarteDoubleGain_AjouteeEnMain(void)
{
    int nbCarteAvant = mTitulaire->getNbCartes();
    mCarteDoubleGain->action();
    CPPUNIT_ASSERT(mTitulaire->getNbCartes() == nbCarteAvant + 1);
}

CPPUNIT_TEST_SUITE_REGISTRATION(TestCarte);


