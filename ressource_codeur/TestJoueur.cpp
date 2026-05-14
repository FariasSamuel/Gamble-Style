/**
 * @file TestJoueur.cpp
 * @brief Tests unitaires de la classe Joueur (CppUnit).
 *
 * Couverture : lancerde(), conditionfinanciere(), acheter(),
 *              hypotequer(), misebanqueroute(), bougerjoueur().
 *
 * Légende des types de tests :
 *   [N] = Nominal    — comportement attendu standard
 *   [L] = Limite     — valeur extrême ou cas frontière
 *   [E] = Erreur     — entrée invalide ou cas exceptionnel
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
#include "Joueur.hpp"
#include "CasePropriete.hpp"
#include "Case.hpp"

using namespace CppUnit;
using namespace std;

class TestJoueur : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestJoueur);

    // ── lancerde() ──────────────────────────────────────────────
    CPPUNIT_TEST(testLancerDeInRange);                   // [N] existant
    CPPUNIT_TEST(testLancerDeValeurMinAtteinte);         // [L] nouveau
    CPPUNIT_TEST(testLancerDeValeurMaxAtteinte);         // [L] nouveau

    // ── conditionfinanciere() ────────────────────────────────────
    CPPUNIT_TEST(testConditionFinanciereRiche);          // [N] existant (specs PDF)
    CPPUNIT_TEST(testConditionFinanciereBanqueroute);    // [N] existant (specs PDF)
    CPPUNIT_TEST(testConditionFinanciereFaillite);       // [E] existant (specs PDF)
    CPPUNIT_TEST(testConditionFinanciereCapitalZero);    // [L] nouveau

    // ── acheter() ────────────────────────────────────────────────
    CPPUNIT_TEST(testAcheterAjoutePropriete);            // [N] existant
    CPPUNIT_TEST(testAcheterDeduiteCapital);             // [N] existant
    CPPUNIT_TEST(testAcheterAugmenteValPropriete);       // [N] existant
    CPPUNIT_TEST(testAcheterCapitalExactementEgalPrix);  // [L] nouveau
    CPPUNIT_TEST(testAcheterCapitalInsuffisant);         // [E] nouveau

    // ── hypotequer() ─────────────────────────────────────────────
    CPPUNIT_TEST(testHypotequerReduitValPropriete);      // [N] existant
    CPPUNIT_TEST(testHypotequerRestitueCapital);         // [L] nouveau

    // ── misebanqueroute() ────────────────────────────────────────
    CPPUNIT_TEST(testMiseBanqueroutePasseFailliteSiImpossible); // [E] existant

    // ── bougerjoueur() ───────────────────────────────────────────
    CPPUNIT_TEST(testBougerJoueurChangeLaCase);          // [N] existant
    CPPUNIT_TEST(testBougerJoueurMemeCase);              // [L] nouveau

    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);
    void tearDown(void);

protected:
    // ── lancerde() ──────────────────────────────────────────────
    /** [N] Chaque appel renvoie une valeur dans [1,6]. */
    void testLancerDeInRange(void);
    /** [L] Sur 1000 tirages, la valeur 1 apparaît au moins une fois. */
    void testLancerDeValeurMinAtteinte(void);
    /** [L] Sur 1000 tirages, la valeur 6 apparaît au moins une fois. */
    void testLancerDeValeurMaxAtteinte(void);

    // ── conditionfinanciere() ────────────────────────────────────
    /** [N] capital > 0 → RICHE. */
    void testConditionFinanciereRiche(void);
    /** [N] capital < 0 et biens > 0 → BANQUEROUTE. */
    void testConditionFinanciereBanqueroute(void);
    /** [E] capital < 0 et biens = 0 après misebanqueroute() → FAILLITE. */
    void testConditionFinanciereFaillite(void);
    /** [L] capital == 0 exactement → RICHE (frontière). */
    void testConditionFinanciereCapitalZero(void);

    // ── acheter() ────────────────────────────────────────────────
    /** [N] La propriété est ajoutée à la liste du joueur. */
    void testAcheterAjoutePropriete(void);
    /** [N] Le capital diminue du prix d'achat. */
    void testAcheterDeduiteCapital(void);
    /** [N] valPropriete augmente du prix d'achat. */
    void testAcheterAugmenteValPropriete(void);
    /** [L] capital == prix_achat → achat réussi, capital = 0. */
    void testAcheterCapitalExactementEgalPrix(void);
    /** [E] capital < prix_achat → achat refusé, capital inchangé. */
    void testAcheterCapitalInsuffisant(void);

    // ── hypotequer() ─────────────────────────────────────────────
    /** [N] valPropriete diminue après hypothèque. */
    void testHypotequerReduitValPropriete(void);
    /** [L] Le capital remonte exactement du prix_hypotheque. */
    void testHypotequerRestitueCapital(void);

    // ── misebanqueroute() ────────────────────────────────────────
    /** [E] Sans bien hypothécable, le joueur passe en FAILLITE. */
    void testMiseBanqueroutePasseFailliteSiImpossible(void);

    // ── bougerjoueur() ───────────────────────────────────────────
    /** [N] getCaseActuelle() retourne la case cible après déplacement. */
    void testBougerJoueurChangeLaCase(void);
    /** [L] Déplacer le joueur vers sa case actuelle ne plante pas. */
    void testBougerJoueurMemeCase(void);

private:
    Joueur        *mJoueur;
    CasePropriete *mCasePropriete;
    CasePropriete *mCaseChere;   // prix > capital du joueur pauvre
    Case          *mCase;
};

// ── setUp / tearDown ────────────────────────────────────────────────────────

void TestJoueur::setUp(void)
{
    mJoueur        = new Joueur("TestPlayer", 1500);
    mCasePropriete = new CasePropriete("Rue de la Paix", 400);
    mCaseChere     = new CasePropriete("Avenue Mozart",  9999);
    mCase          = new Case(5);
}

void TestJoueur::tearDown(void)
{
    delete mJoueur;
    delete mCasePropriete;
    delete mCaseChere;
    delete mCase;
}

// ── lancerde() ──────────────────────────────────────────────────────────────

// [N] existant
void TestJoueur::testLancerDeInRange(void)
{
    for (int i = 0; i < 100; ++i)
    {
        int result = mJoueur->lancerde();
        CPPUNIT_ASSERT(result >= 1 && result <= 6);
    }
}

// [L] nouveau — la valeur minimale (1) doit être atteignable
void TestJoueur::testLancerDeValeurMinAtteinte(void)
{
    bool found1 = false;
    for (int i = 0; i < 1000 && !found1; ++i)
        if (mJoueur->lancerde() == 1) found1 = true;
    CPPUNIT_ASSERT(found1);
}

// [L] nouveau — la valeur maximale (6) doit être atteignable
void TestJoueur::testLancerDeValeurMaxAtteinte(void)
{
    bool found6 = false;
    for (int i = 0; i < 1000 && !found6; ++i)
        if (mJoueur->lancerde() == 6) found6 = true;
    CPPUNIT_ASSERT(found6);
}

// ── conditionfinanciere() ────────────────────────────────────────────────────

// [N] existant — specs PDF §2 : "capital = 100 → état normal"
void TestJoueur::testConditionFinanciereRiche(void)
{
    mJoueur->setCapital(1500);
    CPPUNIT_ASSERT(mJoueur->conditionfinanciere() == Condition::RICHE);
}

// [N] existant — specs PDF §2 : "capital négatif + propriétés → banqueroute"
void TestJoueur::testConditionFinanciereBanqueroute(void)
{
    mJoueur->setCapital(-100);
    // Le joueur possède mCasePropriete via setUp → valPropriete > 0
    CPPUNIT_ASSERT(mJoueur->conditionfinanciere() == Condition::BANQUEROUTE);
}

// [E] existant — specs PDF §2 : "capital négatif + sans propriété → faillite"
void TestJoueur::testConditionFinanciereFaillite(void)
{
    mJoueur->setCapital(-500);
    mJoueur->clearProprietes();
    mJoueur->misebanqueroute();
    CPPUNIT_ASSERT(mJoueur->conditionfinanciere() == Condition::FAILLITE);
}

// [L] nouveau — capital == 0 : frontière RICHE/BANQUEROUTE (0 est considéré RICHE)
void TestJoueur::testConditionFinanciereCapitalZero(void)
{
    mJoueur->setCapital(0);
    CPPUNIT_ASSERT(mJoueur->conditionfinanciere() == Condition::RICHE);
}

// ── acheter() ────────────────────────────────────────────────────────────────

// [N] existant
void TestJoueur::testAcheterAjoutePropriete(void)
{
    int nbAvant = mJoueur->getNbProprietes();
    mJoueur->acheter(mCasePropriete);
    CPPUNIT_ASSERT(mJoueur->getNbProprietes() == nbAvant + 1);
}

// [N] existant
void TestJoueur::testAcheterDeduiteCapital(void)
{
    int capitalAvant = mJoueur->getCapital();
    mJoueur->acheter(mCasePropriete);
    CPPUNIT_ASSERT(mJoueur->getCapital() == capitalAvant - mCasePropriete->getPrix());
}

// [N] existant
void TestJoueur::testAcheterAugmenteValPropriete(void)
{
    int valAvant = mJoueur->getValPropriete();
    mJoueur->acheter(mCasePropriete);
    CPPUNIT_ASSERT(mJoueur->getValPropriete() == valAvant + mCasePropriete->getPrix());
}

// [L] nouveau — capital exactement égal au prix → achat réussi, capital = 0
void TestJoueur::testAcheterCapitalExactementEgalPrix(void)
{
    int prix = mCasePropriete->getPrix(); // 400
    mJoueur->setCapital(prix);
    mJoueur->acheter(mCasePropriete);
    CPPUNIT_ASSERT(mJoueur->getCapital() == 0);
    CPPUNIT_ASSERT(mJoueur->getNbProprietes() >= 1);
}

// [E] nouveau — capital insuffisant → achat refusé, capital et liste inchangés
void TestJoueur::testAcheterCapitalInsuffisant(void)
{
    mJoueur->setCapital(100); // prix = 400 → impossible
    int capitalAvant = mJoueur->getCapital();
    int nbAvant      = mJoueur->getNbProprietes();
    mJoueur->acheter(mCasePropriete);
    // Ni le capital ni la liste ne doivent avoir changé
    CPPUNIT_ASSERT(mJoueur->getCapital()      == capitalAvant);
    CPPUNIT_ASSERT(mJoueur->getNbProprietes() == nbAvant);
}

// ── hypotequer() ─────────────────────────────────────────────────────────────

// [N] existant
void TestJoueur::testHypotequerReduitValPropriete(void)
{
    mJoueur->acheter(mCasePropriete);
    int valAvant = mJoueur->getValPropriete();
    mJoueur->hypotequer(mCasePropriete);
    CPPUNIT_ASSERT(mJoueur->getValPropriete() < valAvant);
}

// [L] nouveau — le capital remonte exactement du prix_hypotheque
void TestJoueur::testHypotequerRestitueCapital(void)
{
    mJoueur->acheter(mCasePropriete);
    int capitalApresAchat = mJoueur->getCapital();
    int prixHypo = mCasePropriete->getPrixHypotheque();
    mJoueur->hypotequer(mCasePropriete);
    CPPUNIT_ASSERT(mJoueur->getCapital() == capitalApresAchat + prixHypo);
}

// ── misebanqueroute() ─────────────────────────────────────────────────────────

// [E] existant
void TestJoueur::testMiseBanqueroutePasseFailliteSiImpossible(void)
{
    mJoueur->setCapital(-9999);
    mJoueur->clearProprietes();
    mJoueur->misebanqueroute();
    CPPUNIT_ASSERT(mJoueur->conditionfinanciere() == Condition::FAILLITE);
}

// ── bougerjoueur() ───────────────────────────────────────────────────────────

// [N] existant
void TestJoueur::testBougerJoueurChangeLaCase(void)
{
    mJoueur->bougerjoueur(mCase);
    CPPUNIT_ASSERT(mJoueur->getCaseActuelle() == mCase);
}

// [L] nouveau — déplacer vers la case où on se trouve déjà ne plante pas
void TestJoueur::testBougerJoueurMemeCase(void)
{
    mJoueur->bougerjoueur(mCase);
    Case* caseAvant = mJoueur->getCaseActuelle();
    mJoueur->bougerjoueur(mCase); // même case
    CPPUNIT_ASSERT(mJoueur->getCaseActuelle() == caseAvant);
}

// ── enregistrement et main ────────────────────────────────────────────────────

CPPUNIT_TEST_SUITE_REGISTRATION(TestJoueur);


