#pragma once
#include <string>
#include <vector>
#include <utility>

// Types of cases supported in the board config file.
enum class CaseType {
    DEPART,
    EVENEMENT,
    PROPRIETE,
    PRISON,
    GOTO_PRISON,
    ALEATOIRE
};

struct CaseConfig {
    CaseType    type;
    std::string name;     // underscores are converted to spaces for display
    std::string texture;  // relative path or "none"
    int         param = 0; // bonus (DEPART) or price (PROPRIETE)
};

struct BoardConfig {
    int grid_size = 4;          // NxN square grid
    std::string board_image;    // background image path or "none"
    std::vector<CaseConfig> cases;

    // Total outer cells: 4 * (grid_size - 1)
    int nbCases() const { return 4 * (grid_size - 1); }

    // Returns the normalised (0..1) centre of case i, clockwise from
    // bottom-right.  Multiply by BOARD_SIZE to get pixel coordinates.
    std::pair<float, float> caseNormPos(int idx) const;

    // Returns the index of the first case with the given type, or -1.
    int findCaseType(CaseType t) const;

    // Load from a text file; returns a default config on failure.
    static BoardConfig loadFromFile(const std::string& path);
};
