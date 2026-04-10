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
    CPPUNIT_TEST(testLancerDeInRange);
    CPPUNIT_TEST(testConditionFinanciereRiche);
    CPPUNIT_TEST(testConditionFinanciereBanqueroute);
    CPPUNIT_TEST(testConditionFinanciereFaillite);
    CPPUNIT_TEST(testAcheterAjoutePropriete);
    CPPUNIT_TEST(testAcheterDeduiteCapital);
    CPPUNIT_TEST(testAcheterAugmenteValPropriete);
    CPPUNIT_TEST(testHypotequerReduitValPropriete);
    CPPUNIT_TEST(testMiseBanqueroutePasseFailliteSiImpossible);
    CPPUNIT_TEST(testBougerJoueurChangeLaCase);
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp(void);
    void tearDown(void);

protected:
    // lancerde()
    void testLancerDeInRange(void);

    // conditionfinanciere()
    void testConditionFinanciereRiche(void);
    void testConditionFinanciereBanqueroute(void);
    void testConditionFinanciereFaillite(void);

    // acheter(CasePropriete)
    void testAcheterAjoutePropriete(void);
    void testAcheterDeduiteCapital(void);
    void testAcheterAugmenteValPropriete(void);

    // hypotequer(CasePropriete)
    void testHypotequerReduitValPropriete(void);

    // misebanqueroute()
    void testMiseBanqueroutePasseFailliteSiImpossible(void);

    // bougerjoueur(Case)
    void testBougerJoueurChangeLaCase(void);

private:
    Joueur        *mJoueur;
    CasePropriete *mCasePropriete;
    Case          *mCase;
};

// ---------------------------------------------------------------------------

void TestJoueur::setUp(void)
{
    mJoueur        = new Joueur("TestPlayer", 1500);
    mCasePropriete = new CasePropriete("Rue de la Paix", 400);
    mCase          = new Case(5); // case index 5
}

void TestJoueur::tearDown(void)
{
    delete mJoueur;
    delete mCasePropriete;
    delete mCase;
}

// ---------------------------------------------------------------------------
// lancerde() : doit renvoyer une valeur entre 1 et 6 inclus
void TestJoueur::testLancerDeInRange(void)
{
    for (int i = 0; i < 100; ++i)
    {
        int result = mJoueur->lancerde();
        CPPUNIT_ASSERT(result >= 1 && result <= 6);
    }
}

// ---------------------------------------------------------------------------
// conditionfinanciere() : joueur avec capital > 0 => riche
void TestJoueur::testConditionFinanciereRiche(void)
{
    mJoueur->setCapital(1500);
    CPPUNIT_ASSERT(mJoueur->conditionfinanciere() == Condition::RICHE);
}

// conditionfinanciere() : joueur avec capital < 0 => banqueroute
void TestJoueur::testConditionFinanciereBanqueroute(void)
{
    mJoueur->setCapital(-100);
    CPPUNIT_ASSERT(mJoueur->conditionfinanciere() == Condition::BANQUEROUTE);
}

// conditionfinanciere() : joueur en faillite (capital < 0 et plus de biens) => faillite
void TestJoueur::testConditionFinanciereFaillite(void)
{
    mJoueur->setCapital(-500);
    mJoueur->clearProprietes();
    // on force la faillite
    mJoueur->misebanqueroute();
    CPPUNIT_ASSERT(mJoueur->conditionfinanciere() == Condition::FAILLITE);
}

// ---------------------------------------------------------------------------
// acheter() : la propriete doit apparaitre dans la liste du joueur
void TestJoueur::testAcheterAjoutePropriete(void)
{
    int nbAvant = mJoueur->getNbProprietes();
    mJoueur->acheter(mCasePropriete);
    CPPUNIT_ASSERT(mJoueur->getNbProprietes() == nbAvant + 1);
}

// acheter() : le capital diminue du prix d'achat
void TestJoueur::testAcheterDeduiteCapital(void)
{
    int capitalAvant = mJoueur->getCapital();
    mJoueur->acheter(mCasePropriete);
    CPPUNIT_ASSERT(mJoueur->getCapital() == capitalAvant - mCasePropriete->getPrix());
}

// acheter() : valPropriete augmente du prix d'achat
void TestJoueur::testAcheterAugmenteValPropriete(void)
{
    int valAvant = mJoueur->getValPropriete();
    mJoueur->acheter(mCasePropriete);
    CPPUNIT_ASSERT(mJoueur->getValPropriete() == valAvant + mCasePropriete->getPrix());
}

// ---------------------------------------------------------------------------
// hypotequer() : la valeur des biens du joueur doit diminuer
void TestJoueur::testHypotequerReduitValPropriete(void)
{
    mJoueur->acheter(mCasePropriete);
    int valAvant = mJoueur->getValPropriete();
    mJoueur->hypotequer(mCasePropriete);
    CPPUNIT_ASSERT(mJoueur->getValPropriete() < valAvant);
}

// ---------------------------------------------------------------------------
// misebanqueroute() : sans bien hypothecable, le joueur passe en faillite
void TestJoueur::testMiseBanqueroutePasseFailliteSiImpossible(void)
{
    mJoueur->setCapital(-9999);
    mJoueur->clearProprietes();
    mJoueur->misebanqueroute();
    CPPUNIT_ASSERT(mJoueur->conditionfinanciere() == Condition::FAILLITE);
}

// ---------------------------------------------------------------------------
// bougerjoueur() : la position du joueur doit correspondre a la case cible
void TestJoueur::testBougerJoueurChangeLaCase(void)
{
    mJoueur->bougerjoueur(mCase);
    CPPUNIT_ASSERT(mJoueur->getCaseActuelle() == mCase);
}

// ---------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_REGISTRATION(TestJoueur);

int main(int argc, char* argv[])
{
    CPPUNIT_NS::TestResult testresult;

    CPPUNIT_NS::TestResultCollector collectedresults;
    testresult.addListener(&collectedresults);

    CPPUNIT_NS::BriefTestProgressListener progress;
    testresult.addListener(&progress);

    CPPUNIT_NS::TestRunner testrunner;
    testrunner.addTest(CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest());
    testrunner.run(testresult);

    CPPUNIT_NS::CompilerOutputter compileroutputter(&collectedresults, std::cerr);
    compileroutputter.write();

    ofstream xmlFileOut("cppTestJoueurResults.xml");
    XmlOutputter xmlOut(&collectedresults, xmlFileOut);
    xmlOut.write();

    return collectedresults.wasSuccessful() ? 0 : 1;
}
