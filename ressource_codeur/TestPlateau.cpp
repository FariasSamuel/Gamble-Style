/**
 * @file TestPlateau.cpp
 * @brief Tests unitaires de la classe Plateau (CppUnit).
 *
 * Couverture : donnercapital(), enlevercapital(), transfertargent(),
 *              ordre(), update(), gamelooping(), fin().
 *
 * Sources des tests :
 *   [N/L/E-existant]  = présent dans le TestPlateau.cpp d'origine
 *   [N/L/E-specs]     = issu du Cahier des Spécifications Techniques (PDF)
 *   [N/L/E-nouveau]   = ajouté pour compléter la couverture
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
    CPPUNIT_TEST(testDonnerCapitalAugmenteCapitalJoueur);        // [N] existant + specs PDF
    CPPUNIT_TEST(testDonnerCapital_MontantNul_SansEffet);        // [L] nouveau

    // ── enlevercapital() ────────────────────────────────────────
    CPPUNIT_TEST(testEnleverCapitalDiminueCapitalJoueur);        // [N] existant + specs PDF
    CPPUNIT_TEST(testEnleverCapitalDeclencheBanqueroute);        // [E] existant
    CPPUNIT_TEST(testEnleverCapital_JoueurPasseEnFaillite);      // [E] nouveau

    // ── transfertargent() ───────────────────────────────────────
    CPPUNIT_TEST(testTransfertArgentDebitEtCredit);              // [N] existant + specs PDF
    CPPUNIT_TEST(testTransfertArgent_CapitalExactementVide);     // [L] nouveau

    // ── ordre() ─────────────────────────────────────────────────
    CPPUNIT_TEST(testOrdreJoueursTrieParDe);                     // [N] existant
    CPPUNIT_TEST(testOrdreJoueurs3JoueursTrieParDe);             // [N] specs PDF (A=8,B=11,C=5)

    // ── update() ────────────────────────────────────────────────
    CPPUNIT_TEST(testUpdateExecuteTousLesTours);                  // [N] specs PDF

    // ── gamelooping() ───────────────────────────────────────────
    CPPUNIT_TEST(testGameloopingArreteSurConditionFin);          // [N] specs PDF

    // ── fin() ───────────────────────────────────────────────────
    CPPUNIT_TEST(testFinRetourneGagnantParCapitalPlusBiens);      // [N] existant
    CPPUNIT_TEST(testFin_UnSeulJoueurActif);                     // [L] nouveau

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

// ── setUp / tearDown ─────────────────────────────────────────────────────────

void TestPlateau::setUp(void)
{
    mPlateau  = new Plateau();
    mJoueur1  = new Joueur("Alice",   1500);
    mJoueur2  = new Joueur("Bob",      500);
    mJoueur3  = new Joueur("Charlie", 1000);
    mPlateau->ajouterJoueur(mJoueur1);
    mPlateau->ajouterJoueur(mJoueur2);
}

void TestPlateau::tearDown(void)
{
    delete mPlateau;
    delete mJoueur1;
    delete mJoueur2;
    delete mJoueur3;
}

// ── donnercapital() ──────────────────────────────────────────────────────────

// [N] existant + specs PDF §1 : "donner_capital(Joueur:200, 100) → 300"
void TestPlateau::testDonnerCapitalAugmenteCapitalJoueur(void)
{
    int capitalAvant = mJoueur1->getCapital();
    mPlateau->donnercapital(mJoueur1, 200);
    CPPUNIT_ASSERT(mJoueur1->getCapital() == capitalAvant + 200);
}

// [L] nouveau — montant nul : capital ne doit pas changer
void TestPlateau::testDonnerCapital_MontantNul_SansEffet(void)
{
    int capitalAvant = mJoueur1->getCapital();
    mPlateau->donnercapital(mJoueur1, 0);
    CPPUNIT_ASSERT(mJoueur1->getCapital() == capitalAvant);
}

// ── enlevercapital() ─────────────────────────────────────────────────────────

// [N] existant + specs PDF §1 : "enlever_capital(Joueur:300, 100) → 200"
void TestPlateau::testEnleverCapitalDiminueCapitalJoueur(void)
{
    int capitalAvant = mJoueur1->getCapital();
    mPlateau->enlevercapital(mJoueur1, 300);
    CPPUNIT_ASSERT(mJoueur1->getCapital() == capitalAvant - 300);
}

// [E] existant — montant > capital → conditionfinanciere() != RICHE
void TestPlateau::testEnleverCapitalDeclencheBanqueroute(void)
{
    mPlateau->enlevercapital(mJoueur2, 9999);
    CPPUNIT_ASSERT(mJoueur2->conditionfinanciere() != Condition::RICHE);
}

// [E] nouveau — joueur sans biens + montant > capital → FAILLITE
void TestPlateau::testEnleverCapital_JoueurPasseEnFaillite(void)
{
    mJoueur2->clearProprietes(); // pas de biens hypothécables
    mPlateau->enlevercapital(mJoueur2, 9999);
    CPPUNIT_ASSERT(mJoueur2->conditionfinanciere() == Condition::FAILLITE);
}

// ── transfertargent() ────────────────────────────────────────────────────────

// [N] existant + specs PDF §1 : "transfert(A:500, B:300, 100) → A=400, B=400"
void TestPlateau::testTransfertArgentDebitEtCredit(void)
{
    int cap1Avant = mJoueur1->getCapital();
    int cap2Avant = mJoueur2->getCapital();
    mPlateau->transfertargent(mJoueur1, mJoueur2, 100);
    CPPUNIT_ASSERT(mJoueur1->getCapital() == cap1Avant - 100);
    CPPUNIT_ASSERT(mJoueur2->getCapital() == cap2Avant + 100);
}

// [L] nouveau — transfert exactement égal au capital du débiteur → capital = 0
void TestPlateau::testTransfertArgent_CapitalExactementVide(void)
{
    int capitalJ2 = mJoueur2->getCapital(); // 500
    mPlateau->transfertargent(mJoueur2, mJoueur1, capitalJ2);
    CPPUNIT_ASSERT(mJoueur2->getCapital() == 0);
    CPPUNIT_ASSERT(mJoueur2->conditionfinanciere() == Condition::RICHE); // 0 == RICHE
}

// ── ordre() ──────────────────────────────────────────────────────────────────

// [N] existant — 2 joueurs, ordre après lancer de dés
void TestPlateau::testOrdreJoueursTrieParDe(void)
{
    mPlateau->ordre();
    CPPUNIT_ASSERT(mPlateau->getJoueur(0)->getDernierLancer()
                >= mPlateau->getJoueur(1)->getDernierLancer());
}

// [N] specs PDF §1 — 3 joueurs avec scores prédéfinis A=8, B=11, C=5 → [B, A, C]
void TestPlateau::testOrdreJoueurs3JoueursTrieParDe(void)
{
    mPlateau->ajouterJoueur(mJoueur3);
    // Forcer les scores de dés
    mJoueur1->setDernierLancer(8);   // Alice = 8
    mJoueur2->setDernierLancer(11);  // Bob   = 11
    mJoueur3->setDernierLancer(5);   // Charlie = 5
    mPlateau->trierParDernierLancer();
    // Ordre attendu : Bob (11) > Alice (8) > Charlie (5)
    CPPUNIT_ASSERT(mPlateau->getJoueur(0) == mJoueur2); // Bob
    CPPUNIT_ASSERT(mPlateau->getJoueur(1) == mJoueur1); // Alice
    CPPUNIT_ASSERT(mPlateau->getJoueur(2) == mJoueur3); // Charlie
}

// ── update() ─────────────────────────────────────────────────────────────────

// [N] specs PDF §1 — chaque joueur joue un tour dans l'ordre
void TestPlateau::testUpdateExecuteTousLesTours(void)
{
    // Réinitialise les compteurs de tours
    mJoueur1->resetCompteurTours();
    mJoueur2->resetCompteurTours();
    mPlateau->update();
    // Chaque joueur actif doit avoir joué exactement 1 tour
    CPPUNIT_ASSERT(mJoueur1->getCompteurTours() == 1);
    CPPUNIT_ASSERT(mJoueur2->getCompteurTours() == 1);
}

// ── gamelooping() ────────────────────────────────────────────────────────────

// [N] specs PDF §1 — la boucle s'arrête quand la partie est terminée
void TestPlateau::testGameloopingArreteSurConditionFin(void)
{
    // Mettre Bob en faillite pour déclencher la fin de partie rapidement
    mJoueur2->setCapital(-9999);
    mJoueur2->clearProprietes();
    mJoueur2->misebanqueroute(); // Bob → FAILLITE
    mPlateau->retirerJoueursEnFaillite();

    // La boucle doit se terminer (1 seul joueur actif)
    mPlateau->gamelooping();
    CPPUNIT_ASSERT(mPlateau->finDePartie() == true);
}

// ── fin() ─────────────────────────────────────────────────────────────────────

// [N] existant — Alice (1500) > Bob (500) → gagnant = Alice
void TestPlateau::testFinRetourneGagnantParCapitalPlusBiens(void)
{
    Joueur* gagnant = mPlateau->fin();
    CPPUNIT_ASSERT(gagnant == mJoueur1);
}

// [L] nouveau — 1 seul joueur actif → retourné comme gagnant par défaut
void TestPlateau::testFin_UnSeulJoueurActif(void)
{
    // Éliminer Bob
    mJoueur2->setCapital(-9999);
    mJoueur2->clearProprietes();
    mJoueur2->misebanqueroute();
    mPlateau->retirerJoueursEnFaillite();
    Joueur* gagnant = mPlateau->fin();
    CPPUNIT_ASSERT(gagnant == mJoueur1);
}

// ── enregistrement et main ────────────────────────────────────────────────────

CPPUNIT_TEST_SUITE_REGISTRATION(TestPlateau);


