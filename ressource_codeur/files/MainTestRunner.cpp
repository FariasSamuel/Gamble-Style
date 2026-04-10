#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <cppunit/TestCase.h>
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/TestFailure.h>
#include <cppunit/BriefTestProgressListener.h>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/XmlOutputter.h>

// --- Inclusion de tous les fichiers de test ---
#include "TestJoueur.cpp"
#include "TestPlateau.cpp"
#include "TestCarte.cpp"
#include "TestCaseAleatoire.cpp"
#include "TestCaseEvenement.cpp"
#include "TestCaseDepart.cpp"
#include "TestCasePropriete.cpp"
#include "TestCasePrison.cpp"
#include "TestMinijeu.cpp"

using namespace std;

// ---------------------------------------------------------------------------
// Listener personnalise : affiche OK / KO en temps reel pour chaque test
// ---------------------------------------------------------------------------
class OKKOListener : public CPPUNIT_NS::TestListener
{
public:
    OKKOListener() : m_currentFailed(false) {}

    void startTest(CPPUNIT_NS::Test* test) override
    {
        m_currentFailed = false;
        m_currentName   = test->getName();
    }

    void addFailure(const CPPUNIT_NS::TestFailure& failure) override
    {
        m_currentFailed  = true;
        m_failureMessage = failure.thrownException()->what();
    }

    void endTest(CPPUNIT_NS::Test* /*test*/) override
    {
        if (m_currentFailed)
        {
            cout << "  [ KO ] " << m_currentName << "\n";
            cout << "         => " << m_failureMessage << "\n";
        }
        else
        {
            cout << "  [ OK ] " << m_currentName << "\n";
        }
    }

    void startSuite(CPPUNIT_NS::Test* suite) override
    {
        // N'afficher que les suites de test (pas la suite racine)
        if (suite->getName() != "All Tests")
            cout << "\n=== " << suite->getName() << " ===\n";
    }

private:
    bool        m_currentFailed;
    string      m_currentName;
    string      m_failureMessage;
};

// ---------------------------------------------------------------------------
// Affichage du bilan final
// ---------------------------------------------------------------------------
static void afficherBilan(const CPPUNIT_NS::TestResultCollector& results)
{
    int total   = results.runTests();
    int echecs  = results.testFailures();
    int succes  = total - echecs;

    cout << "\n";
    cout << "==========================================\n";
    cout << "  BILAN FINAL\n";
    cout << "==========================================\n";
    cout << "  Total    : " << total   << " test(s)\n";
    cout << "  OK       : " << succes  << " test(s)\n";
    cout << "  KO       : " << echecs  << " test(s)\n";
    cout << "==========================================\n";

    if (results.wasSuccessful())
        cout << "  RESULTAT : TOUS LES TESTS SONT PASSES\n";
    else
        cout << "  RESULTAT : DES TESTS ONT ECHOUE\n";

    cout << "==========================================\n\n";
}

// ---------------------------------------------------------------------------
// Main
// ---------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    cout << "\n##########################################\n";
    cout << "#      EXECUTION DE TOUS LES TESTS       #\n";
    cout << "##########################################\n";

    // Gestionnaire de resultats
    CPPUNIT_NS::TestResult controller;

    // Collecteur (pour le bilan et le XML)
    CPPUNIT_NS::TestResultCollector collectedResults;
    controller.addListener(&collectedResults);

    // Notre listener OK/KO personnalise
    OKKOListener okkoListener;
    controller.addListener(&okkoListener);

    // Ajout et lancement de toutes les suites enregistrees
    CPPUNIT_NS::TestRunner runner;
    runner.addTest(CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest());
    runner.run(controller);

    // Bilan console
    afficherBilan(collectedResults);

    // Export XML global
    ofstream xmlFileOut("cppAllTestsResults.xml");
    CPPUNIT_NS::XmlOutputter xmlOut(&collectedResults, xmlFileOut);
    xmlOut.write();

    return collectedResults.wasSuccessful() ? 0 : 1;
}
