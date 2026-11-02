// src/tools/calculator_eval.cpp — YOU IMPLEMENT (spec §4.6).
//
// calc_evaluate: evaluate a well-formed token list with two stacks
// (one for numbers, one for operators and '('). See spec §4.6 and the
// worked trace in Appendix C. std::stack or std::vector are both fine.

#include "tools/calculator.h"
#include <vector>
#include <bits/codecvt.h>

// "malformed expression" checks throughout are to ensure that
// the code works; I understand that a truly malformed expression
// will never be passed in

static int precedence (CalcToken::Kind kind) {
    switch (kind) {
        case CalcToken::Kind::Times:  return 2;
        case CalcToken::Kind::Divide: return 2;
        case CalcToken::Kind::Plus:   return 1;
        case CalcToken::Kind::Minus:  return 1;
        default: return 0;
    }
}

// Return value is used to check if operation was successful
static bool apply_operator(std::vector<double>& values, std::vector<CalcToken>& ops,
                            std::string* error) {
    if (ops.empty()) {
        *error = "malformed expression";
        return false;
    }

    // pop back operation
    const CalcToken::Kind op = ops.back().kind;
    ops.pop_back();

    double result;
    const double rightval = values.back();
    values.pop_back();
    const double leftval = values.back();
    values.pop_back();

    switch (op) {
        case CalcToken::Kind::Plus:
            result = leftval + rightval;
            break;
        case CalcToken::Kind::Minus:
            result = leftval - rightval;
            break;
        case CalcToken::Kind::Times:
            result = leftval * rightval;
            break;
        case CalcToken::Kind::Divide:
            if (rightval == 0.0) {
                *error = "division by zero";
                return false;
            }
            result = leftval / rightval;
            break;
        default:
            *error = "malformed expression";
            return false;
    }
    values.push_back(result);
    return true;
}

bool calc_evaluate(const std::vector<CalcToken>& tokens, double* out,
                   std::string* error) {
    std::vector<double> values;
    std::vector<CalcToken> ops;
    for (const CalcToken& tok: tokens) {
        switch (tok.kind) {
            case CalcToken::Kind::Number:
                values.push_back(tok.value);
                break;
            case CalcToken::Kind::LParen:
                ops.push_back(tok);
                break;
            case CalcToken::Kind::RParen:
                while (!ops.empty() && ops.back().kind != CalcToken::Kind::LParen) {
                    if (!apply_operator(values, ops, error)) return false;
                }
                if (ops.empty()) {
                    *error = "malformed expression";
                    return false;
                }
                ops.pop_back(); // pop the '('
                break;
            case CalcToken::Kind::Plus:
            case CalcToken::Kind::Minus:
            case CalcToken::Kind::Times:
            case CalcToken::Kind::Divide:
                while (!ops.empty() && ops.back().kind != CalcToken::Kind::LParen &&
                       precedence(tok.kind) <= precedence(ops.back().kind)) {

                    if (!apply_operator(values, ops, error)) return false;

                }
                ops.push_back(tok);
                break;
        }
    }

    // End of tokens, apply everything left on the stack
    while (!ops.empty()) {
        if (!apply_operator(values, ops, error)) return false;
    }

    *out = values.back();
    return true;

}
