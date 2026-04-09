// ============================================================
// isMovePassive() — Ludøl dommer-funksjon
// ============================================================
//   Parametere:
//     chosenPieceIndex  – indeksen (0-3) i current_player sin pieces-vektor
//     chosenSteps       – antall steg spilleren prøver å flytte
//
//   Returnerer en streng med grunnen til straff, eller "" om trekket er OK.
//
// 
#include "LudolWindow.h"

// Hjelpefunksjon: kan brikke 'piece' fra spiller 'playerIndex' slå ut en
// motstanderbrikke om den flytter 'steps' steg?
// Returnerer true om det finnes minst en fiendtlig brikke på landingsfeltet
// som ikke er beskyttet (ikke rokade, ikke på eget frifelt).
bool LudolWindow::canKnockOut(int playerIndex, const Piece& piece, int steps) {
    // Brikker i mål kan ikke flyttes
    if (piece.home_end) return false;

    // Simuler flyttet
    Piece simulated = flytt_brike_struct(piece, steps);
    auto landingPos = calculate_piece_board_position(playerIndex, simulated);

    for (const auto& enemy : players) {
        if (enemy.playernumber == playerIndex) continue;
        for (const auto& ep : enemy.pieces) {
            if (ep.home_start || ep.home_end) continue;

            auto enemyPos = calculate_piece_board_position(enemy.playernumber, ep);
            if (enemyPos != landingPos) continue;

            // Rokade — kan ikke slås
            if (ep.rokade) continue;

            // Frifelt — kan ikke slås
            const auto& enemyStartPos = BOARD_PATH.at(ep.start_index);
            if (enemyPos.first == enemyStartPos.first &&
                enemyPos.second == enemyStartPos.second) continue;

            // Fiendtlig brikke som KAN slås ut!
            return true;
        }
    }
    return false;
}


std::string LudolWindow::isMovePassive(int chosenPieceIndex, int chosenSteps) {

    const Player& player = players.at(current_player_index);
    const Piece& chosenPiece = player.pieces.at(chosenPieceIndex);

    // De to gyldige steg-verdiene for dette terningkastet
    // (røros-konversjonen: kast K gir enten K eller 7-K steg)
    const int stepsA = dice_result;            // f.eks. kastet 2 → 2 steg
    const int stepsB = 7 - dice_result;        // f.eks. kastet 2 → 5 steg
    const bool canMoveFromHome = (dice_result == 1 || dice_result == 6);

    
    // REGEL 1: Kunne spilleren slått ut en motstander, men lot være?
    //          (Alltid straffeslurk – høyeste prioritet)

    {
        bool chosenMoveKnocks = canKnockOut(current_player_index, chosenPiece, chosenSteps);

        // Sjekk om det finnes NOEN brikke+steg-kombinasjon som slår ut noen
        if (!chosenMoveKnocks) {
            for (const auto& p : player.pieces) {
                if (p.home_end) continue;

                // Brikker på home_start kan bare flytte 1 steg ut (og bare med 1 eller 6)
                if (p.home_start) {
                    if (canMoveFromHome && canKnockOut(current_player_index, p, 1)) {
                        return player.name + " spilte passivt! Kunne slått ut en motstander ved å sette ut en brikke. DRIKK!";
                    }
                    continue;
                }

                // Brikker på brettet kan flytte stepsA eller stepsB
                if (canKnockOut(current_player_index, p, stepsA)) {
                    return player.name + " spilte passivt! Kunne slått ut en motstander. DRIKK!";
                }
                if (stepsA != stepsB && canKnockOut(current_player_index, p, stepsB)) {
                    return player.name + " spilte passivt! Kunne slått ut en motstander. DRIKK!";
                }
            }
        }
    }

    // ------------------------------------------------------------------
    // REGEL 2: Kunne spilleren satt ut en brikke fra home_start, men lot være?
    // ------------------------------------------------------------------
    if (canMoveFromHome && !chosenPiece.home_start) {
    // Hvis det valgte trekket slår ut en motstander, er det alltid OK
        bool chosenMoveKnocks = canKnockOut(current_player_index, chosenPiece, chosenSteps);
            if (!chosenMoveKnocks) {
                for (const auto& p : player.pieces) {
                    if (p.home_start) {
                        return player.name + " spilte passivt! Kunne satt ut en brikke fra start. DRIKK!";
                    }
                }
            }
    }

    // ------------------------------------------------------------------
    // REGEL 3: Lager trekket en rokade?
    //          (Overstyres av regel 2 — rokade er tillatt om det var for
    //           å flytte ut en brikke fra home_start)
    // ------------------------------------------------------------------
    {
        // Simuler trekket
        Piece simulated = flytt_brike_struct(chosenPiece, chosenSteps);
        auto landingPos = calculate_piece_board_position(current_player_index, simulated);

        // Sjekk om en av egne brikker allerede står der vi lander
        for (const auto& p : player.pieces) {
            if (p.piece_number == chosenPiece.piece_number) continue;
            if (p.home_start || p.home_end) continue;

            auto otherPos = calculate_piece_board_position(current_player_index, p);
            if (otherPos == landingPos) {
                // Rokade oppstår — men det er tillatt om brikken kom fra home_start
                if (!chosenPiece.home_start) {
                    return player.name + " spilte passivt! Laget en unødvendig rokade. DRIKK!";
                }
            }
        }
    }

    // ------------------------------------------------------------------
    // REGEL 4: Tok ikke det høyeste antall steg mulig?
    //          (Overstyres av alle reglene over)
    // ------------------------------------------------------------------
    {
        const int maxSteps = std::max(stepsA, stepsB);

        // Bare relevant for brikker som allerede er på brettet (ikke home_start)
        if (!chosenPiece.home_start && chosenSteps < maxSteps) {
            // Sjekk at det høyeste antall steg faktisk er gyldig
            // (brikken lander ikke forbi mål, og veien er ikke blokkert av rokade)
            Piece simHigh = flytt_brike_struct(chosenPiece, maxSteps);

            // Sjekk at den ikke overskyter mål
            bool highStepsValid = true;

            if (simHigh.home_end) {
                int homeEndIdx = simHigh.steps_made - static_cast<int>(BOARD_PATH.size());
                if (homeEndIdx >= static_cast<int>(HOME_END.at(current_player_index).size())) {
                    highStepsValid = false; // overskyter mål
                }
            }

            // Sjekk at veien ikke er blokkert av fiendtlig rokade
            if (highStepsValid) {
                for (int s = 1; s <= maxSteps; s++) {
                    Piece simStep = flytt_brike_struct(chosenPiece, s);
                    auto stepPos = calculate_piece_board_position(current_player_index, simStep);

                    for (const auto& enemy : players) {
                        if (enemy.playernumber == current_player_index) continue;
                        for (const auto& ep : enemy.pieces) {
                            if (ep.rokade) {
                                auto rokadePos = calculate_piece_board_position(enemy.playernumber, ep);
                                if (rokadePos == stepPos) {
                                    highStepsValid = false;
                                }
                            }
                        }
                    }
                    if (!highStepsValid) break;
                }
            }

            if (highStepsValid) {
                return player.name + " spilte passivt! Tok ikke det høyeste antall steg. DRIKK!";
            }
        }
    }

    // Trekket er ikke passivt
    return "";
}


